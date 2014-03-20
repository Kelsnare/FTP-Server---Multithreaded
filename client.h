/*
Contains client specific declarations and function prototypes..

By : 
Abhishek Sarkar
Ajay Kumar Kilaka
*/

#include "commons.h"

#define IPSERVER "127.0.0.1"  //This value decides the IP to which the client will attempt connection with
#define PORTSERVER 5555		//Port at which the server is running at above IP. Must be pre-known.
#define U_INPUTLEN 1024		//User input length

//User command structure
struct command{
	int id;
	char path[DATALEN];
	char fileName[DATALEN];
};

size_t size_sockaddr = sizeof(struct sockaddr), size_packet = sizeof(struct PACKET);

struct command * getUserCommand(char *);

void getCurrentWorkingDir();
void listContentsDir();

void server_pwd(int);
void server_ls(int);
void server_cd(struct command *, int);
void server_get(struct command *, int);
void server_put(struct command *, int);
//void client_QUIT(struct command *, int);