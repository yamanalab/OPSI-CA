#ifndef FUNCTION_H
#define FUNCTION_H

#include "FHE.h"
#include "EncryptedArray.h"

std::vector<std::string> split(const std::string &s, char delim) ;
void decryptAndPrint(const Ctxt& ct, const FHESecKey& sk,long nslots,EncryptedArray* ea);
Ctxt myXOR(const Ctxt& a, const Ctxt& b);
Ctxt myAND(const Ctxt& a, const Ctxt& b);
Ctxt myNOT(const Ctxt& a,const ZZX& zzxOne);
Ctxt myOR(const Ctxt& a, const Ctxt& b);
Ctxt myXORConstant(const Ctxt& a, const ZZX& zzx);
Ctxt myANDConstant(const Ctxt& a, const ZZX& zzx);
Ctxt myORConstant(const Ctxt& a, const ZZX& zzx);
Ctxt getCOne(const FHEPubKey& pk,const long nslots,EncryptedArray* ea);
Ctxt getCZero(const FHEPubKey& pk,const long nslots,EncryptedArray* ea);
Ctxt getCOOne(const FHEPubKey& pk,const long nslots,EncryptedArray* ea);
ZZX getZZXOne(const long nslots,EncryptedArray* ea);
ZZX getZZXZero(const long nslots,EncryptedArray* ea);
ZZX getZZXOOne(const long nslots,EncryptedArray* ea);
std::vector<int> decomposeIntoBinary(int x);
std::vector<Ctxt> mulPowerOfTwoShiftList(const Ctxt& v0,int n,EncryptedArray* ea);
Ctxt tournamentProductArray(std::vector<Ctxt> array);
Ctxt tournamentORope(std::vector<Ctxt> array);
Ctxt totalProduct(const Ctxt& ctxt,EncryptedArray* ea,long nslots);
#endif