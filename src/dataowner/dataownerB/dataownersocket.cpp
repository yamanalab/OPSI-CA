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

// void test(){

// 	//Simple Test Program
// 	int s;
// 	struct sockaddr_in sa;
// 	const char* message="Hello\n";
// 	char buf[1024];

// 	/*create socket*/
// 	s=Socket(AF_INET,SOCK_STREAM,0);
	
// 	/*assign  names to the socket*/
// 	memset(&sa, 0, sizeof(sa));
// 	sa.sin_len = sizeof(sa);//if sin_len exists
// 	sa.sin_family = AF_INET;
// 	sa.sin_port = htons(SERVER_PORT);
// 	sa.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	
// 	/*connect to server*/
// 	std::cerr<<"Connecting to the server..."<<std::endl;
// 	Connect(s,(struct sockaddr *)&sa, sizeof(sa));
// 	std::cerr<<"Connected"<<std::endl;

// 	/*send message*/
// 	Write(s,message,strlen(message));
// 	std::cout<<"sent message:"<<message<<std::endl;

// 	//receive message
// 	Read(s,buf,sizeof(buf));	
// 	std::cout<<buf<<std::endl;	


// 	Write(s,EXIT_MSG,strlen(EXIT_MSG));

// 	std::cerr<<"finished receiving"<<std::endl;

// 	//stop connection
// 	Shutdown(s,SHUT_RDWR);

// 	//close socket
// 	Close(s);

// }

