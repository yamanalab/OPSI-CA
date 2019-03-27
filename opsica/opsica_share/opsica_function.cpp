/*
 * Copyright 2018 Yamana Laboratory, Waseda University
 * Supported by JST CREST Grant Number JPMJCR1503, Japan.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE‐2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <sstream>
#include <vector>
#include <iostream>

#include "FHE.h"
#include "EncryptedArray.h"

#include <opsica_share/opsica_function.hpp>

namespace opsica_share
{

std::vector<std::string> split(const std::string& s, char delim)
{

    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> tokens;
    while (std::getline(ss, item, delim))
    {
        tokens.push_back(item);
    }
    return tokens;
}

void decryptAndPrint(const Ctxt& ct, const FHESecKey& sk, long nslots,
                     EncryptedArray* ea)
{

    std::vector<long> res;
    ea->decrypt(ct, sk, res);
    std::cout << "[";

    for (int i = 0; i < nslots; i++)
    {

        std::cout << (int)res[i] << std::flush;
    }
    std::cout << "]" << std::endl;
}

Ctxt myXOR(const Ctxt& a, const Ctxt& b)
{
    Ctxt cSum(a.getPubKey());
    cSum = a;
    cSum.addCtxt(b);
    return cSum;
}

Ctxt myAND(const Ctxt& a, const Ctxt& b)
{

    Ctxt cRlt(a.getPubKey());
    cRlt = a;
    cRlt.multiplyBy(b);
    return cRlt;
}

Ctxt myNOT(const Ctxt& a, const NTL::ZZX& zzxOne)
{
    Ctxt cRlt(a.getPubKey());
    cRlt = a;
    cRlt.addConstant(zzxOne);
    return cRlt;
}

Ctxt myOR(const Ctxt& a, const Ctxt& b)
{
    Ctxt ctxt1 = myAND(a, b);
    Ctxt ctxt2 = myXOR(a, b);
    Ctxt ctxt3 = myXOR(ctxt1, ctxt2);
    return ctxt3;
}

Ctxt myXORConstant(const Ctxt& a, const NTL::ZZX& zzx)
{
    Ctxt cSum(a.getPubKey());
    cSum = a;
    cSum.addConstant(zzx);
    return cSum;
}

Ctxt myANDConstant(const Ctxt& a, const NTL::ZZX& zzx)
{
    Ctxt cRlt(a.getPubKey());
    cRlt = a;
    cRlt.multByConstant(zzx);
    return cRlt;
}

Ctxt myORConstant(const Ctxt& a, const NTL::ZZX& zzx)
{
    Ctxt ctxt1 = myANDConstant(a, zzx);
    Ctxt ctxt2 = myXORConstant(a, zzx);
    Ctxt ctxt3 = myXOR(ctxt1, ctxt2);
    return ctxt3;
}

Ctxt getCOne(const FHEPubKey& pk, const long nslots, EncryptedArray* ea)
{
    Ctxt cOne(pk);
    std::vector<long> vOne(nslots, 1);
    ea->encrypt(cOne, pk, vOne);
    return cOne;
}

Ctxt getCZero(const FHEPubKey& pk, const long nslots, EncryptedArray* ea)
{

    Ctxt cZero(pk);
    std::vector<long> vOne(nslots, 0);
    ea->encrypt(cZero, pk, vOne);
    return cZero;
}
Ctxt getCOOne(const FHEPubKey& pk, const long nslots, EncryptedArray* ea)
{
    Ctxt cOOne(pk);
    std::vector<long> vOOne(nslots - 1, 0);
    vOOne.push_back(1);
    ea->encrypt(cOOne, pk, vOOne);
    return cOOne;
}

NTL::ZZX getZZXOne(const long nslots, EncryptedArray* ea)
{
    std::vector<long> vOne(nslots, 1);
    NTL::ZZX encX;
    ea->encode(encX, vOne);
    return encX;
}

NTL::ZZX getZZXZero(const long nslots, EncryptedArray* ea)
{
    std::vector<long> vZero(nslots, 0);
    NTL::ZZX encX;
    ea->encode(encX, vZero);
    return encX;
}

NTL::ZZX getZZXOOne(const long nslots, EncryptedArray* ea)
{
    std::vector<long> vOOne(nslots - 1, 0);
    vOOne.push_back(1);
    NTL::ZZX encX;
    ea->encode(encX, vOOne);
    return encX;
}

std::vector<int> decomposeIntoBinary(int x)
{
    std::vector<int> binary;
    int a = x;
    int b, r;
    while (1)
    {
        b = a / 2;
        r = a % 2;
        binary.push_back(r);
        if (b == 0)
        {
            break;
        }
        else
        {
            a = b;
        }
    }
    return binary;
}

std::vector<Ctxt> mulPowerOfTwoShiftList(const Ctxt& v0, int n,
                                         EncryptedArray* ea)
{
    std::vector<Ctxt> vk;
    vk.push_back(v0);
    for (int i = 0; i < n; i++)
    {
        Ctxt temp = vk[i];
        ea->rotate(temp, 1 << i);
        vk.push_back(myAND(vk[i], temp));
    }
    return vk;
}

Ctxt tournamentProductArray(std::vector<Ctxt> array)
{
    int size = array.size();

    for (int i = 1; i < size; i *= 2)
    {
        for (int j = 0; j < size; j += i * 2)
        {
            int k = j + i;
            if (k >= size)
            {
                break;
            }
            array[j] = myAND(array[j], array[k]);
        }
    }

    return array[0];
}
Ctxt tournamentORope(std::vector<Ctxt> array)
{
    int size = array.size();

    for (int i = 1; i < size; i *= 2)
    {
        for (int j = 0; j < size; j += i * 2)
        {
            int k = j + i;
            if (k >= size)
            {
                break;
            }
            array[j] = myOR(array[j], array[k]);
        }
    }

    return array[0];
}
Ctxt totalProduct(const Ctxt& ctxt, EncryptedArray* ea, long nslots)
{
    int length = nslots;
    std::vector<int> binary = decomposeIntoBinary(length);

    std::vector<Ctxt> mulPowerOfTwoShiftVector =
      mulPowerOfTwoShiftList(ctxt, binary.size() - 1, ea);

    int count = 1;
    std::vector<Ctxt> productSet;
    productSet.push_back(
      mulPowerOfTwoShiftVector[mulPowerOfTwoShiftVector.size() - 1]);
    for (int i = binary.size() - 2; i >= 0; i--)
    {
        if (binary[i] == 1)
        {
            count++;
            int nshift = (1 << (binary.size() - 1 - 1)) * count;
            Ctxt temp = mulPowerOfTwoShiftVector[i];
            ea->rotate(temp, nshift);
            productSet.push_back(temp);
        }
    }

    Ctxt result = tournamentProductArray(productSet);

    return result;
}

} /* namespace opsica_share */
