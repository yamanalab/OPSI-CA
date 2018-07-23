#include "client.h"

Client::Client():
	server_addr(SERVER_ADDR),
	server_port(SERVER_PORT)
{

	setUp();

}

Client::Client(const std::string addr):
	server_addr(addr),
	server_port(SERVER_PORT)
	{
		setUp();
	}

Client::Client(const int port):
	server_addr(SERVER_ADDR),
	server_port(port)
	{
		setUp();
	}
Client::Client(const std::string addr, const int port):
	server_addr(addr),
	server_port(port)
	{
		setUp();
	}
void Client::setUp(){

	/*create socket*/
	socket=Socket(AF_INET,SOCK_STREAM,0);
	
	/*assign  names to the socket*/
	memset(&sa, 0, sizeof(sa));
	//sa.sin_len = sizeof(sa);//if sin_len exists
	sa.sin_family = AF_INET;
	sa.sin_port = htons(server_port);
	sa.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	
}

void Client::connect(){

	/*connect to server*/
	std::cerr<<"Connecting to the server..."<<std::endl;
	Connect(socket,(struct sockaddr *)&sa, sizeof(sa));
	std::cerr<<"Connected"<<std::endl;


}

void Client::disconnect(){

	
	sendExit(socket);
	//stop connection
	Shutdown(socket,SHUT_RDWR);

	//close socket
	Close(socket);

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

