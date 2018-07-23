#ifndef SOCKETLIB_H
#define SOCKETLIB_H

#include <iostream>
#include <sys/types.h> /* connect, inet_addr, read, socket, write */ 
#include <sys/socket.h> /* connect, inet_addr, shutdown, socket */
#include <stdlib.h>/*exit*/
#include <string.h>/*memset*/
#include <netinet/in.h> /* inet_addr */
#include <arpa/inet.h> /* htons, inet_addr */
#include <sys/uio.h> /* read, write */ 
#include <unistd.h> /* close, read, write */
#include <fstream>
#include <limits.h>

#define PUBKEYSTORE_MSG 0
#define JOINREQUEST_MSG 1
#define STOREREQUEST_MSG 2
#define RESULTREQUEST_MSG 3
#define ENRESULT 4
#define ACCEPT 5
#define REJECT 6
#define EXIT 7
#define GETNMAX 8
#define FPMAX_MSG 9

/*
--------------PACKET STRUCTURE----------------------
[TYPE][HEADER LENGTH][HEADER][DATA LENGTH][DATA]
   1       4                      20         ?
[1*][14**][inputdata.txt][5***********][hello]

Type:PUBKEYSTORE_MSG -> public key 
	querier sends public key to the cloud
	[0*][14**][encrypter.txt][123452******][[123]5?????????????????]

Type:JOINREQUEST_MSG -> join request
	querier asks cloud to join two datas
	[1*][36**][inputdataA1_1 inputdataB1_1 encrypter.txt resultA1_1-B1_1.txt][0********][]

Type:STOREREQUEST_MSG -> data storage request
	dataowner uploads data on the cloud
	[2*][14**][inputdataA1_1.txt][123452******][[123]5?????????????????]

Type:RESULTREQUEST_MSG -> result request
	querier downloads the result of joining data
	[3*][14**][resultA1_1-B1_1.txt][0*********][]

Type:ENRESULT -> return encrypted result
	cloud returns encrypted result to queirer
	[4*][14**][resultA1_1-B1_1.txt][123312****][[123]dafe????????????????????]

Type:ACCEPT -> accepted
	cloud accepts querier/dataowner's request
	[5*][0***][][0**********][]

Type:REJECT -> rejected
	cloud rejects querier/dataowner's request
	[6*][0***][][0**********][]

Type:Exit -> close socket
	dataowner/querier close sockets
	[7*][0***][][0**********][]

Type:GETNMAX -> return value of nmax
	cloud returns nmax to dataowners/querier
	[8*][7***][nmax.txt][3***][500]	

Type:FPMAX_MSG -> return value of fpmax
	querier sends fpmax to dataowners
	[9*][0***][][4***][0.001]
----------------------------------------------------
*/

#define TYPEMAXLENGTH 2//[BYTE]
#define HEADERMAXLENGTH 4//[BYTE]
#define DATAMAXLENGTH 20//[BYTE]
#define SIZELENGTH 20

int Socket(int domain,int type,int protocol);

void Setsockopt(int fd, int level, int optname, const void* optval, socklen_t optlen);

void Connect(int fd, const struct sockaddr *sa, socklen_t salen);

void Bind(int fd, const struct sockaddr *sa, socklen_t salen);

void Listen(int fd,int backlog);

int Accept(int fd, struct sockaddr *sa, socklen_t *salen);

void Shutdown(int fd, int how);

void Close(int fd);

//ssize_t Read(int fd, void* buf, size_t count);

//ssize_t Write(int fd, const char* buf, size_t count);

//ssize_t Receive(int fd,const std::string filename);
ssize_t Receive(int fd, std::string& packet);
int classifyPacket(const std::string& packet,std::string& header, std::string& data);
//ssize_t Send(int fd,const std::string filename);
ssize_t Send(int fd, const std::string&  buf);

/*
	createPacket(PUBKEYSTORE_MSG,"encrypter.txt","./encrypter.txt");
	createPacket(JOINREQUEST_MSG,"inputdataA1_1 inputdataB1_1 encrypter.txt resultA1_1-B1_1.txt",NULL);
	createPacket(STOREREQUEST_MSG,"inputdataA1_1.txt","./storage/inputdataA1_1.txt");
	createPacket(RESULTREQUEST_MSG,"resultA1_1-B1_1.txt", NULL)
	createPacket(ENRESULT,"resultA1_1-B1_1.txt","./result/resultA1_1-B1_1.txt")
	createPacket(ACCEPT,NULL,NULL)
	createPacket(REJECT,NULL,NULL)
	createPacket(EXIT,NULL,NULL)
*/
std::string createPacket(int type, const std::string header, const std::string filedataname);


// ssize_t sendPubKey(int fd,const std::string header,const std::string filedataname);
// ssize_t sendJoinRequest(int fd,const std::string header);
// ssize_t sendData(int fd,const std::string header,const std::string filedataname);
// ssize_t sendResultRequest(int fd,const std::string header);
// ssize_t sendEncryptedResult(int fd,const std::string header,const std::string filedataname);
ssize_t sendAccept(int fd);
ssize_t sendReject(int fd);
ssize_t sendExit(int fd);
#endif
