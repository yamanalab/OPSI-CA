#include "protocol.h"

BP::BP(long nThreads,  long global_nslots, int npacks,EncryptedArray* global_ea ):
	nThreads(nThreads), global_nslots(global_nslots), npacks(npacks), global_ea(global_ea)
{

}
std::vector<std::vector<Ctxt>> BP::execute(const FHEPubKey& publicKey,  std::vector<EnBloomFilter> largerEnBFs, std::vector<EnBloomFilter> smallerEnBFs ){
			

	//merge  larger bloom filters	
	LOG("merging bloom filters.....");
	EnBloomFilter enbloomSum=mergeEncryptedBFs(publicKey,largerEnBFs);
	LOG("finished merging bloom filters");

	
	long small_size=smallerEnBFs.size();
	Ctxt ctemp(publicKey);
	std::vector<std::vector<Ctxt>> results(small_size,std::vector<Ctxt>(npacks,ctemp));

	LOG("inclusion check....");	
	BasicThreadPool multiTask(nThreads);
	multiTask.exec_range(small_size,
		[&](long first, long last){
			for(long i=first;i<last;i++){
				std::vector<Ctxt> Cresult=inclusionCheck(publicKey,enbloomSum,smallerEnBFs[i]);
				for(long n=0;n<npacks;n++){
					results[i][n]=Cresult[n];
					
				}
				
			}
		});
	LOG("finished inclusion check");
	
	return results;
	
}
EnBloomFilter BP::mergeEncryptedBFs(const FHEPubKey& publicKey,  std::vector<EnBloomFilter> enBFs){


	if(enBFs.size()==1)
		return enBFs[0];
	
	
	
	BasicThreadPool multiTask(nThreads);
	multiTask.exec_range(npacks,
		[&](long first, long last){
		for(int n=first;n<last;n++){
			for(uint64_t i=1;i<enBFs.size();i*=2){
				for(uint64_t j=0;j<enBFs.size();j+=i*2){
					
					uint64_t k=j+i;
					if(k>=enBFs.size())
						break;
					enBFs[j].enbloomfilter[n]=myOR(enBFs[j].enbloomfilter[n],enBFs[k].enbloomfilter[n]);
				}
			}
		}
	});
	
	

	return enBFs[0];

}

std::vector<Ctxt> BP::inclusionCheck(const FHEPubKey& publicKey,EnBloomFilter bloomAll, EnBloomFilter bloom){
	
	//return 1 if the "bloom" is included in the "bloomAll", otherwise 0

	Ctxt Cresult(publicKey);
	std::vector<Ctxt> Cresults(npacks,Cresult);
	ZZX zzxOne=getZZXOne(global_nslots,global_ea);
	
	// -----------------------------------------
	//1.x=bloomAll & bloom
	//2.if(x==bloom)->1....1 
	//	else ->0..1...0


	std::vector<Ctxt> nbloomAll=bloomAll.enbloomfilter;
	std::vector<Ctxt> nbloom=bloom.enbloomfilter;
	BasicThreadPool multiTask(1);
	multiTask.exec_range(npacks,
		[&](long first,long last){
		for(int n=first;n<last;n++){
			Ctxt Cresult1=myAND(bloomAll.enbloomfilter[n],bloom.enbloomfilter[n]);
			
			Ctxt Cresult2=myNOT(myXOR(Cresult1,bloom.enbloomfilter[n]),zzxOne);//2.equality check		
			Cresults[n]=Cresult2;
		}
	});
	
	return Cresults;


}
QFP::QFP(long nThreads,  long global_nslots, int npacks,EncryptedArray* global_ea):
	BP(nThreads,global_nslots,npacks,global_ea)
{}
std::vector<Ctxt> QFP::execute(const FHEPubKey& publicKey,  std::vector<EnBloomFilter> largerEnBFs, std::vector<EnBloomFilter> smallerEnBFs ){

	std::vector<Ctxt> results;
	//merge  larger bloom filters	
	LOG("merging bloom filters.....");
	EnBloomFilter enbloomSum=mergeEncryptedBFs(publicKey,largerEnBFs);	
	LOG("finished merging bloom filters");
	 

	//ex.)[00001][00000][00001]
	//  [00001](shift)
	//->[00010](OR[00000])
	//->[00010](shift)
	//->[00100](OR[00001])
	//->[00101]
	

	
	long small_size=smallerEnBFs.size();
	long re_npacks=small_size/global_nslots;
	if((small_size%global_nslots)!=0)
		re_npacks++;

	Ctxt ctemp(publicKey);
	
	std::vector<std::vector<Ctxt> > tournamentORs(re_npacks,std::vector<Ctxt>(global_nslots,ctemp));


	LOG("inclusion check....");	 
	BasicThreadPool multiTask(nThreads);
	multiTask.exec_range(small_size,
		[&](long first, long last){
			for(long i=first;i<last;i++){
				std::vector<Ctxt> in_resultCtxts=inclusionCheck(publicKey,enbloomSum,smallerEnBFs[i]);
				
				//[001][000][000] -> [000][000][000]
				Ctxt in_resultCtxt=tournamentProductArray(in_resultCtxts);
							
				global_ea->rotate(in_resultCtxt,-1*(i%global_nslots));
				
				tournamentORs[i/global_nslots][i%global_nslots]=in_resultCtxt;
				
			}
		});
	LOG("finished inclusion check");

	
	LOG("aggregating....");
	results=aggregate(publicKey,tournamentORs);
	LOG("finished aggregating");
	
	return results;
	
}
std::vector<Ctxt> QFP::inclusionCheck(const FHEPubKey& publicKey,EnBloomFilter bloomAll, EnBloomFilter bloom){


	//-------------------------------
	//2^n roration
	//If n
	//[0,1,2,3,4,5,6,7]* (1bit rotate)
	//[7,0,1,2,3,4,5,6] (AND)
	//[0^7,0^1,1^2,2^3,3^4,4^5,5^6,6^7]**(2bit rotate)
	//[5^6,6^7,0^7,0^1,1^2,2^3,3^4,4^5](AND)
	//[0^5^6^7,0^1^6^7,0^1^2^7,0^1^2^3,1^2^3^4,2^3^4^5,3^4^5^6,4^5^6^7]***(4bit rotate)
	//[1^2^3^4,2^3^4^5,3^4^5^6,4^5^6^7,0^5^6^7,0^1^6^7,0^1^2^7,0^1^2^3](AND)
	//[0^1^2^3^4^5^6^7,0^1^2^3^4^5^6^7,0^1^2^3^4^5^6^7,0^1^2^3^4^5^6^7,0^1^2^3^4^5^6^7,0^1^2^3^4^5^6^7,0^1^2^3^4^5^6^7,0^1^2^3^4^5^6^7]

	//
	
	Ctxt Cresult(publicKey);
	std::vector<Ctxt> Cresults(npacks, Cresult);
	ZZX zzxOne=getZZXOne(global_nslots,global_ea);
	ZZX zzxOOne=getZZXOOne(global_nslots,global_ea);

	BasicThreadPool multiTask(1);
	multiTask.exec_range(npacks,
		[&](long first,long last){
		for(int n=first;n<last;n++){
			//1.x=bloomAll & bloom
			Ctxt Cresult1=myAND(bloomAll.enbloomfilter[n],bloom.enbloomfilter[n]);//1.
		
			//2.if(x==bloom)->1....1 
			//	else ->0..1...0
			Ctxt Cresult2=myNOT(myXOR(Cresult1,bloom.enbloomfilter[n]),zzxOne);//2.equality check
			
			//3. bitwise and operation
			//1......1 -> 1......1
			//0..1...0 -> 0......0
			Ctxt Cresult3=totalProduct(Cresult2,global_ea,global_nslots);
		
			//4.
			//[000000000]*[0000000001]=[000000000000]
			//[111111111]*[0000000001]=[000000000001]	
			Ctxt Cresult4=myANDConstant(Cresult3,zzxOOne);

			Cresults[n]=Cresult4;
		}
	});

	

	return  Cresults;

}
std::vector<Ctxt> QFP::aggregate(const FHEPubKey& publicKey, std::vector<std::vector<Ctxt> > bloom){

	std::vector<Ctxt> Cresults;
	long re_npacks=bloom.size();
	for(int i=0;i<re_npacks;i++){

		Ctxt result=tournamentORope(bloom[i]);
		Cresults.push_back(result);
	}
	
	return Cresults;

}



