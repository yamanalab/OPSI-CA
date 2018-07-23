#include "function.h"

std::vector<std::string> split(const std::string &s, char delim) {
   
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;

}


void decryptAndPrint(const Ctxt& ct, const FHESecKey& sk,long nslots,EncryptedArray* ea){


	vector<long> res;
	ea->decrypt(ct,sk,res);
	std::cout<<"[";

	for(int i=0 ;i < nslots;i++){
	
		std::cout<<(int)res[i]<<std::flush;
	}
	std::cout<<"]"<<std::endl;

}

Ctxt myXOR(const Ctxt& a, const Ctxt& b){

	/*
  	addCtxt:0+0=0,0+1=1+0=1,1+1=0 SIMD
	*/

	Ctxt cSum(a.getPubKey());
	cSum=a;
	//cout << "level before myXOR = " << cSum.findBaseLevel() << endl;
	cSum.addCtxt(b);	
	//cout << "level after myXOR = " << cSum.findBaseLevel() << endl;
	return cSum;

}
Ctxt myAND(const Ctxt& a, const Ctxt& b){

	Ctxt cRlt(a.getPubKey());
	cRlt=a;
	//cout << "level before myAND = " << cRlt.findBaseLevel() << endl;
	cRlt.multiplyBy(b);
	//cout << "level after myAND = " << cRlt.findBaseLevel() << endl;
	return  cRlt;

}
Ctxt myNOT(const Ctxt& a,const ZZX& zzxOne){

	//return xor(a,1);
	
	Ctxt cRlt(a.getPubKey());
	cRlt=a;
	//cout << "level before myNOT = " << cRlt.findBaseLevel() << endl;
	cRlt.addConstant(zzxOne);
	//cout << "level after myNOT = " << cRlt.findBaseLevel() << endl;
	return  cRlt;

}

Ctxt myOR(const Ctxt& a, const Ctxt& b){

	Ctxt ctxt1=myAND(a,b);
	Ctxt ctxt2=myXOR(a,b);
	Ctxt ctxt3=myXOR(ctxt1,ctxt2);

	return ctxt3;

}
Ctxt myXORConstant(const Ctxt& a, const ZZX& zzx){

	Ctxt cSum(a.getPubKey());
	cSum=a;
	//cout << "level before myXOR = " << cSum.findBaseLevel() << endl;
	cSum.addConstant(zzx);	
	//cout << "level after myXOR = " << cSum.findBaseLevel() << endl;
	return cSum;

}
Ctxt myANDConstant(const Ctxt& a, const ZZX& zzx){

	Ctxt cRlt(a.getPubKey());
	cRlt=a;
	//cout << "level before myAND = " << cRlt.findBaseLevel() << endl;
	cRlt.multByConstant(zzx);
	//cout << "level after myAND = " << cRlt.findBaseLevel() << endl;
	return  cRlt;

}
Ctxt myORConstant(const Ctxt& a, const ZZX& zzx){

	Ctxt ctxt1=myANDConstant(a,zzx);
	Ctxt ctxt2=myXORConstant(a,zzx);
	Ctxt ctxt3=myXOR(ctxt1,ctxt2);

	return ctxt3;

}
Ctxt getCOne(const FHEPubKey& pk,const long nslots,EncryptedArray* ea){

	Ctxt cOne(pk);
	vector<long> vOne(nslots,1);
	ea->encrypt(cOne,pk,vOne);
	return cOne;

}

Ctxt getCZero(const FHEPubKey& pk,const long nslots,EncryptedArray* ea){

	Ctxt cZero(pk);
	vector<long> vOne(nslots,0);
	ea->encrypt(cZero,pk,vOne);
	return cZero;
	
}
Ctxt getCOOne(const FHEPubKey& pk,const long nslots,EncryptedArray* ea){
	//[000000001]
	Ctxt cOOne(pk);
	vector<long> vOOne(nslots-1,0);
	vOOne.push_back(1);
	ea->encrypt(cOOne,pk,vOOne);
	return cOOne;
}

ZZX getZZXOne(const long nslots,EncryptedArray* ea){

	vector<long> vOne(nslots,1);
	ZZX encX;
  	ea->encode(encX, vOne);

	return encX;

}
ZZX getZZXZero(const long nslots,EncryptedArray* ea){

	
	vector<long> vZero(nslots,0);
	ZZX encX;
  	ea->encode(encX, vZero);

	return encX;
	
}
ZZX getZZXOOne(const long nslots,EncryptedArray* ea){

	//[000000001]
	vector<long> vOOne(nslots-1,0);
	vOOne.push_back(1);
	ZZX encX;
  	ea->encode(encX, vOOne);

	return encX;
}
std::vector<int> decomposeIntoBinary(int x){

	//6 = [0,1,1]
	//10= [0,1,0,1]

	std::vector<int> binary;
	int a=x;
	int b,r;
	//a=2*b+r
	while(1){
		b=a/2;
		r=a%2;
		//std::cout<<a<<","<<b<<","<<r<<std::endl;
		binary.push_back(r);
		if(b==0)
			break;
		else
			a=b;
	}
	return binary;


}
std::vector<Ctxt> mulPowerOfTwoShiftList(const Ctxt& v0,int n,EncryptedArray* ea){

	//return if n=2
	//[0,1,2,3,4](0)
	//[0^4,0^1,1^2,2^3,3^4](1)
	//[0^2^3^4,0^1^3^4,0^1^2^4,0^1^2^3,1^2^3^4](2)

	std::vector<Ctxt > vk;
	
	vk.push_back(v0);
	for(int i=0;i<n;i++){
		Ctxt temp=vk[i];
		ea->rotate(temp,1<<i);
		vk.push_back(myAND(vk[i],temp));

	}

	return vk;

}
Ctxt tournamentProductArray(std::vector<Ctxt> array){

	int size=array.size();
	
	//cout << "level before tournamentProductArray = " << array[0].findBaseLevel() << endl;
	
	for(int i=1;i<size;i*=2){
		for(int j=0;j<size;j+=i*2){
			
			int k=j+i;
			if(k>=size)
				break;
			//std::cout<<"#"<<j<<"+"<<k<<".........."<<std::endl;
			array[j]=myAND(array[j],array[k]);
		}
	}

	//cout << "level after tournamentProductArray = " << array[0].findBaseLevel() << endl;
	return array[0];
	
}
Ctxt tournamentORope(std::vector<Ctxt> array){

	int size=array.size();
	
	//cout << "level before tournamentORope = " << array[0].findBaseLevel() << endl;
	
	for(int i=1;i<size;i*=2){
		for(int j=0;j<size;j+=i*2){
			
			int k=j+i;
			if(k>=size)
				break;
			//std::cout<<"#"<<j<<"+"<<k<<".........."<<std::endl;
			array[j]=myOR(array[j],array[k]);
		}
	}

	//cout << "level after tournamentOrOpe = " << array[0].findBaseLevel() << endl;
	return array[0];
	
}
Ctxt totalProduct(const Ctxt& ctxt,EncryptedArray* ea,long nslots){
	//return [000000] OR [1111111]

	//if length=5
	//5 = 2^2+2^0
	//
	//[0^2^3^4,0^1^3^4,0^1^2^4,0^1^2^3,1^2^3^4](2)
	//				AND
	//[1	  ,2	  ,3	  ,4	  ,0	  ] (0's 4bit shift)
	//=> [0^1^2^3^4^5,0^1^2^3^4^5,0^1^2^3^4^5,0^1^2^3^4^5]


	int length=nslots;
	std::vector<int> binary=decomposeIntoBinary(length);


	std::vector<Ctxt > mulPowerOfTwoShiftVector=mulPowerOfTwoShiftList(ctxt,binary.size()-1,ea);
	

	int count=1;
	std::vector<Ctxt> productSet;
	productSet.push_back(mulPowerOfTwoShiftVector[mulPowerOfTwoShiftVector.size()-1]);
	for(int i=binary.size()-2;i>=0;i--){
		if(binary[i]==1){
			count++;
			int nshift=(1<<(binary.size()-1-1))*count;
			Ctxt temp=mulPowerOfTwoShiftVector[i];
			ea->rotate(temp,nshift);
			productSet.push_back(temp);
		}
	}
	


	Ctxt result=tournamentProductArray(productSet);


	return result;

}
