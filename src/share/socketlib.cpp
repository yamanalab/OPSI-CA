#include "socketlib.h"
int Socket(int domain,int type,int protocol){

	int fd;
	if((fd=socket(domain,type,protocol))==-1){
		std::cerr<<"socket "<<strerror(errno);
		exit(1);
	}

	return fd;

}

void Setsockopt(int fd, int level, int optname, const void* optval, socklen_t optlen){
	
	if(setsockopt(fd,level,optname, optval, optlen)==-1){
		std::cerr<<"setsockopt "<<strerror(errno);
		exit(1);
	}

}

void Connect(int fd, const struct sockaddr *sa, socklen_t salen){

	if(connect(fd,sa, salen) == -1){
		std::cerr<<"connect "<<strerror(errno);
		exit(1);
	}

}

void Bind(int fd, const struct sockaddr *sa, socklen_t salen){
	
	if(bind(fd,sa, salen) == -1){
		std::cerr<<"bind "<<strerror(errno);
		exit(1);
	}

}

void Listen(int fd,int backlog){
	
	if(listen(fd,backlog)){
		std::cerr<<"listen "<<strerror(errno);
		exit(1);
	}

}

int Accept(int fd, struct sockaddr *sa, socklen_t *salen){

	int n;
	while(1){
		if((n=accept(fd,(struct sockaddr *)&sa, salen))==-1){
			std::cerr<<"accept "<<strerror(errno);
			continue;
		}
		break;
	}
	return n;

}

void Shutdown(int fd, int how){

	if(shutdown(fd, how)==-1){
		std::cerr<<"shutdown "<<strerror(errno);
		exit(1);
	}

}

void Close(int fd){

	if(close(fd)==-1){
		std::cerr<<"close "<<strerror(errno);
		exit(1);
	}

}



ssize_t Receive(int fd, std::string& packet){
	ssize_t received_size=0;

	//received data size
	char packetsize_buf[SIZELENGTH];
	uint64_t packetsize;
	int pre_size=recv(fd,packetsize_buf,SIZELENGTH,0);
	sscanf(packetsize_buf,"%zu",&packetsize);
	
	//received whole data
	uint64_t received_max_size=1500;
	
	char buf[received_max_size];
	memset(buf, 0, received_max_size);
	

	uint64_t remain=packetsize;
	while(remain>0){
		memset(buf, 0, received_max_size);
		ssize_t n=recv(fd,buf,sizeof(buf),0);
		received_size+=n;
		if(n == -1){
			std::cerr<<"receive "<<strerror(errno);
			exit(1);
		}else if(n==0){
			break;
		}

		for(int i=0;i<n;i++){
			packet.push_back(buf[i]);
		}
		

		remain-=n;
	}

	received_size+=pre_size;

	return received_size;

}
int classifyPacket(const std::string& packet,std::string& header, std::string& data){
	
	
	int si=0;

	//extract type
	std::string s_type;
	int type;
	for(int i=0;i<TYPEMAXLENGTH;i++){
		if(packet[si+i]=='*')
			break;
		s_type.push_back(packet[si+i]);
	}
	
	type=std::stoi(s_type);


	//extract header size
	si=TYPEMAXLENGTH;
	std::string s_headersize;
	int headersize;
	for(int i=0;i<HEADERMAXLENGTH;i++){
		if(packet[si+i]=='*')
			break;
		s_headersize.push_back(packet[si+i]);
	}
	headersize=std::stoi(s_headersize);


	//extract header
	si=TYPEMAXLENGTH+HEADERMAXLENGTH;
	for(int i=0;i<headersize;i++){
		header.push_back(packet[si+i]);		
	}
	

	//extract data size
	si=TYPEMAXLENGTH+HEADERMAXLENGTH+headersize;
	/*
	std::string s_datasize;
	uint64_t datasize=0;
	for(int i=0;i<DATAMAXLENGTH;i++){
		if(packet[si+i]=='*')
			break;
		s_datasize.push_back(packet[si+i]);
	}
	datasize=std::stoull(s_datasize);
	*/

	//extract data
	si=TYPEMAXLENGTH+HEADERMAXLENGTH+headersize+DATAMAXLENGTH;	
	data=packet;
	data.erase(0,si);
	return type;
	
}
ssize_t Send(int fd, const std::string& str_buf){

	ssize_t sent_size=0;

	const char* buf=str_buf.c_str();
	ssize_t count=strlen(buf);

	char size_buf[SIZELENGTH];
	snprintf(size_buf,SIZELENGTH,"%zu",count);
	int pre_size=send(fd,size_buf,SIZELENGTH,0);

	

	while(sent_size < count){

		ssize_t n = send(fd, buf + sent_size, count - sent_size, 0);
		if(n==-1){
			std::cerr<<"send "<<strerror(errno);
			exit(1);
		}
		sent_size+=n;
	}



	sent_size+=pre_size;


	return sent_size;
}

std::string  createPacket(int type, const std::string header, const std::string filedataname){

	uint64_t datasize;
	int headersize;
	std::ifstream sendFile;
	

	if(!filedataname.empty()){
		sendFile.open(filedataname,std::fstream::in);
		
		sendFile.seekg(0,std::fstream::end);
		uint64_t eofPos=sendFile.tellg();

		sendFile.clear();

		sendFile.seekg(0,std::fstream::beg);
		uint64_t begPos=sendFile.tellg();

		datasize=eofPos-begPos;
	}else{
		datasize=0;
	}
	
	if(!header.empty()){
		headersize=header.size();
	}else{
		headersize=0;
	}

	uint64_t packetsize=TYPEMAXLENGTH+HEADERMAXLENGTH+headersize+DATAMAXLENGTH+datasize;
	std::string buf(packetsize,'*');


	//set type
	int si=0;
	std::string stype=std::to_string(type);
	for(int i=0;i<(int)stype.size();i++){
		buf[si+i]=stype[i];
	}

	//set header length
	si=TYPEMAXLENGTH;
	std::string s_headersize=std::to_string(headersize);
	for(int i=0;i<(int)s_headersize.size();i++){
		buf[si+i]=s_headersize[i];
	}
	
	//set header 
	si=TYPEMAXLENGTH+HEADERMAXLENGTH;
	for(int i=0;i<(int)header.size();i++){
		buf[si+i]=header[i];
	}
	//set data length
	si=TYPEMAXLENGTH+HEADERMAXLENGTH+headersize;
	std::string s_datasize=std::to_string(datasize);
	for(uint64_t i=0;i<(uint64_t)s_datasize.size();i++){
		buf[si+i]=s_datasize[i];
	}
	//set data
	if(datasize!=0){
		std::string data(datasize,'0');
		sendFile.read(&data[0],datasize);
		sendFile.close();
		si=TYPEMAXLENGTH+HEADERMAXLENGTH+headersize+DATAMAXLENGTH;
		for(uint64_t i=0;i<(uint64_t)datasize;i++){
			buf[si+i]=data[i];
		}
	
	}
	
	return buf;

}

ssize_t sendAccept(int fd){

	std::string  packet;
	std::string null;
	packet=createPacket(ACCEPT,null,null);
	ssize_t size=Send(fd,packet);
	
	return size;

}
ssize_t sendReject(int fd){

	std::string  packet;
	std::string null;
	packet=createPacket(REJECT,null,null);
	ssize_t size=Send(fd,packet);
	
	return size;

}
ssize_t sendExit(int fd){
	
	std::string packet;
	std::string null;
	packet=createPacket(EXIT,null,null);
	ssize_t size=Send(fd,packet);
	
	return size;

}

