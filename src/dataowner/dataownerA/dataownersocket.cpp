#include "dataownersocket.h"

DataOwnerSocket::DataOwnerSocket()
	:Client()
	{

	}

DataOwnerSocket::DataOwnerSocket(const std::string addr)
	:Client(addr)
	{
		
	}

DataOwnerSocket::DataOwnerSocket(const int port)
	:Client(port)
	{
	
	}
DataOwnerSocket::DataOwnerSocket(const std::string addr,const int port)
	:Client(addr,port)
	{
	}

ssize_t DataOwnerSocket::sendData(const std::string header,const std::string filedataname){

	
	std::string packet;
	packet=createPacket(STOREREQUEST_MSG,header,filedataname);
	ssize_t size=Send(socket,packet);

	//receive acceptance or refuse
	int type;
	std::string data;
	std::string re_packet;
	std::string re_header;
	Receive(socket,re_packet);
	type=classifyPacket(re_packet,re_header,data);
	if(type!=ACCEPT)
		return -1;


	return size;
}
ssize_t DataOwnerSocket::downloadNmax(const std::string header,std::string& data){

        std::string packet;
        std::string null;
        packet=createPacket(GETNMAX,header,null);
        Send(socket,packet);

        int type=-1;
        std::string re_packet;
        std::string re_header;
        ssize_t size=Receive(socket,re_packet);
        type=classifyPacket(re_packet,re_header,data);

        if(type==GETNMAX)
                return size;
        else
                return -1;

}


