#include "server.h"

Server::Server():
	server_port(SERVER_PORT)
	{
		setUp();
	}
Server::Server(const int port):
	server_port(port)
	{
		setUp();
	}
void Server::setUp(){

	//initialization
	nclients=0;
	for(int i=0;i<MAXNCLIENTS;i++){
		clients[i]=0;
	}

	int soval;
	struct sockaddr_in sa;
	/*create socket*/
	socket=Socket(AF_INET,SOCK_STREAM,0);

	/*set address for reuse*/
	soval = 1;
	Setsockopt(socket,SOL_SOCKET,SO_REUSEADDR, &soval, sizeof(soval));

	/*assign  names to the socket*/
	memset(&sa, 0, sizeof(sa));
	//sa.sin_len = sizeof(sa);//if sin_len exists
	sa.sin_family = AF_INET;
	sa.sin_port = htons(server_port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	Bind(socket,(struct sockaddr *)&sa, sizeof(sa));

	Listen(socket,NQUEUESIZE);
	std::cerr<<"Ready"<<std::endl;	


}

void Server::receive(int& fd,int& type,std::string& packet, std::string& header, std::string& data){

	//for(;;){

		int maxfd;
		fd_set readfds;

		/*create set of file discripters for select*/
		FD_ZERO(&readfds);//initialize the set to the null set
		FD_SET(socket,&readfds);//observe the socket waiting for being connected
		maxfd=socket;
		
		/*observe if there are any actions from connected clients*/
		for(int i=0;i<nclients;i++){
			FD_SET(clients[i],&readfds);
			if(clients[i] >maxfd)
				maxfd = clients[i];
		}	
		
		/*wate till any file discripters get ready*/
		if(select(maxfd+1,&readfds,NULL,NULL,NULL)<0){
			std::cerr<<"select"<<strerror(errno);
			exit(1);
		}
	
		/*check whether this is a new connection*/
		if(FD_ISSET(socket,&readfds)){

			struct sockaddr_in ca;
			socklen_t ca_len;
			int ws;
			ca_len=sizeof(ca);
			ws=Accept(socket,(struct sockaddr *)&ca, &ca_len);

			if(nclients >= MAXNCLIENTS){
				//already full
				Shutdown(ws,SHUT_RDWR);
				Close(ws);
				std::cerr<<"Refused a new connection"<<std::endl;
			}else{
				for(int i=0;i<MAXNCLIENTS;i++){
					if(clients[i]==0){
						clients[i]=ws;//add the list of the file discripters
						nclients++;
						std::cerr<<"Accepted a connection on descriptor "<<ws<<std::endl;
						break;
					}
				}
			}
		}		
	
		/*if there are any requests from the clients*/		
		for(int i=0;i<MAXNCLIENTS;i++){
		
			if(FD_ISSET(clients[i], &readfds)){
		
				int cc=Receive(clients[i],packet);
				std::cout<<"....received"<<std::endl;				
				if(cc==0){
					std::cerr<<"close..."<<std::endl;
					Close(clients[i]);
					clients[i]=0;
					continue;
				}

				type=classifyPacket(packet,header,data);
				
				if(type==EXIT){
					std::cout<<"close exit..."<<std::endl;
					Close(clients[i]);
					clients[i]=0;

					continue;
				}else{
					fd=clients[i];		
					return;
				}
				
				
			

			}
		}
		

	

}


