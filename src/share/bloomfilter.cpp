#include "bloomfilter.h"

BloomParams::BloomParams(uint64_t num_ele):
	num_elements(num_ele),
	false_positive_probability(0.001)
	{
		createParams();
	}
BloomParams::BloomParams(uint64_t num_ele, double error):
	num_elements(num_ele),
	false_positive_probability(error)
	{
		createParams();
	}
void BloomParams::createParams(){
	filter_size= -1.0*num_elements*log(false_positive_probability)/pow(log(2.0),2.0);
	num_hashes=filter_size*log(2.0)/num_elements;
}
BloomParams& BloomParams::operator=(const BloomParams& other) {
	if(this!=&other){
		num_elements=other.num_elements;
		num_hashes=other.num_hashes;
		filter_size=other.filter_size;
		false_positive_probability=other.false_positive_probability;
	}
	return *this;
}

/*------------------------Bloom Filer -------------------------*/
BloomFilter::BloomFilter(){

}
BloomFilter::BloomFilter(const BloomParams& params):
	num_elements(params.num_elements), 
	num_hashes(params.num_hashes),
	filter_size(params.filter_size),
	false_positive_probability(params.false_positive_probability)
	{
		//set all bits to  0
		for(uint64_t i=0;i<filter_size;i++){
			bloomfilter.push_back(0);
		}
	}
BloomFilter::BloomFilter(const BloomFilter& filter):
	num_elements(filter.num_elements),
	num_hashes(filter.num_hashes),
	filter_size(filter.filter_size),
	false_positive_probability(filter.false_positive_probability),
	bloomfilter(filter.bloomfilter)
	{}
uint64_t BloomFilter::nthHash(uint8_t n, uint64_t hashA, uint64_t hashB){

	return (hashA+n*hashB)%filter_size;	
}
std::array<uint64_t,2> BloomFilter::hash(const uint8_t* data, std::size_t len){
	std::array<uint64_t,2> hashValue;
	MurmurHash3_x64_128(data,len,0,hashValue.data());
	return hashValue;
}

void BloomFilter::add(const uint8_t *data, std::size_t len){	
	std::array<uint64_t,2> hashValues=hash(data,len);
	for(int i=0;i<num_hashes;i++){
		bloomfilter[nthHash(i,hashValues[0],hashValues[1])]=true;
	}
}
bool BloomFilter::constains(const uint8_t *data, std::size_t len){ 
	std::array<uint64_t,2> hashValues=hash(data,len);
	for(int i=0;i<num_hashes;i++){ 
		if(!bloomfilter[nthHash(i,hashValues[0],hashValues[1])]){ 
			return false;
		}
	}
	 
	return true;
}
bool BloomFilter::contains(const BloomFilter& filter){
	//^((^bloomAll) && bloom) 
	uint64_t result=1; 
	for(uint64_t i=0;i<bloomfilter.size();i++){
		result=result&(!((!bloomfilter[i]) & filter.bloomfilter[i]));
	}
	if(result==1)
		return true;
	else
		return false;
}
void BloomFilter::bloomJoin(const BloomFilter& filter){
	//bloomfilter<-filter
	for(uint64_t i=0;i<bloomfilter.size();i++){
		bloomfilter[i]=bloomfilter[i]|filter.bloomfilter[i];
	}
}
void BloomFilter::print(){

	std::cout<<"[";
	for(uint64_t i=0;i<bloomfilter.size();i++){
		std::cout<<std::to_string(bloomfilter[i]);
	}
	std::cout<<"]"<<std::endl;;
}
void BloomFilter::clear(){
	for(uint64_t i=0;i<bloomfilter.size();i++){
		bloomfilter[i]=false;
	}
}


/*------------------------Encrypted Bloom Filer -------------------------*/
EnBloomFilter::EnBloomFilter(const FHEPubKey& publicKey,long nslots, EncryptedArray* ea, const std::vector<bool> bloomfilter):
	nslots(nslots),
	ea(ea)

{
	

	int size=bloomfilter.size();

	npacks=calculateNPacks(size,nslots);

	//split the bloom filter into npacks bloom filters and encrypt
	std::vector<std::vector<long>> npacks_bf;
	for(int i=0;i<npacks;i++){
		std::vector<bool> bf;
		for(int j=0;j<nslots;j++){
			if(i*nslots+j>=size)
				break;
			bf.push_back(bloomfilter[i*nslots+j]);
		}
		//bool -> long
		std::vector<long> vec=convertToVector(bf);
		

		//encrypt
		Ctxt ctxt(publicKey);
		ea->encrypt(ctxt,publicKey,vec);
		enbloomfilter.push_back(ctxt);

	}
	

	

}
EnBloomFilter::EnBloomFilter(){

}

EnBloomFilter::EnBloomFilter(long nslots,EncryptedArray* ea, int npacks,const std::vector<Ctxt> enbf):
	nslots(nslots),
	ea(ea),
	npacks(npacks),
	enbloomfilter(enbf)
{

}
int EnBloomFilter::calculateNPacks(int size, long nslots){

	if(size>nslots){
		if(size%nslots==0)
			npacks=size/nslots;
		else
			npacks=size/nslots+1;
	}else{
		npacks=1;
	}

	return npacks;

}
std::vector<long> EnBloomFilter::convertToVector(const std::vector<bool> bf){

	std::vector<long> vec;
	int size=bf.size();
	for(int i=0;i<nslots-size;i++){
		vec.push_back(0);
	}
	for(int i=0;i<size;i++){
		vec.push_back((long)bf[i]);
	}
	return vec;

}
EnBloomFilter& EnBloomFilter::operator=(const EnBloomFilter& other) {
	if(this!=&other){
		nslots=other.nslots;
		ea=other.ea;
		npacks=other.npacks;
		
		enbloomfilter=other.enbloomfilter;
	}
	return *this;
}


