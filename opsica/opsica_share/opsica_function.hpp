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

#ifndef OPSICA_FUNCTION_HPP
#define OPSICA_FUNCTION_HPP

namespace opsica_share
{

std::vector<std::string> split(const std::string& s, char delim);
void decryptAndPrint(const Ctxt& ct, const FHESecKey& sk, long nslots,
                     EncryptedArray* ea);
Ctxt myXOR(const Ctxt& a, const Ctxt& b);
Ctxt myAND(const Ctxt& a, const Ctxt& b);
Ctxt myNOT(const Ctxt& a, const NTL::ZZX& zzxOne);
Ctxt myOR(const Ctxt& a, const Ctxt& b);
Ctxt myXORConstant(const Ctxt& a, const NTL::ZZX& zzx);
Ctxt myANDConstant(const Ctxt& a, const NTL::ZZX& zzx);
Ctxt myORConstant(const Ctxt& a, const NTL::ZZX& zzx);
Ctxt getCOne(const FHEPubKey& pk, const long nslots, EncryptedArray* ea);
Ctxt getCZero(const FHEPubKey& pk, const long nslots, EncryptedArray* ea);
Ctxt getCOOne(const FHEPubKey& pk, const long nslots, EncryptedArray* ea);
NTL::ZZX getZZXOne(const long nslots, EncryptedArray* ea);
NTL::ZZX getZZXZero(const long nslots, EncryptedArray* ea);
NTL::ZZX getZZXOOne(const long nslots, EncryptedArray* ea);
std::vector<int> decomposeIntoBinary(int x);
std::vector<Ctxt> mulPowerOfTwoShiftList(const Ctxt& v0, int n,
                                         EncryptedArray* ea);
Ctxt tournamentProductArray(std::vector<Ctxt> array);
Ctxt tournamentORope(std::vector<Ctxt> array);
Ctxt totalProduct(const Ctxt& ctxt, EncryptedArray* ea, long nslots);

} /* namespace opsica_share */

#endif /* OPSICA_FUNCTION_HPP */
