/*
All common elements are contained here, viz. Packet structure, common header files, Macros etc.
Any change here has to be updated accordingly in all other files.

By : 
Abhishek Sarkar
Ajay Kumar Kilaka
*/

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include <dirent.h>

#define ERR -1
#define READY 0
#define OK 1
#define DONE 2
#define GET 3
#define PUT 4
#define PWD 5
#define LS 6
#define CD 7
#define CPWD 8
#define CLS 9
#define CCD 10
#define QUIT 11
#define DATALEN 504

//PACKET structure
struct PACKET{
	char data[DATALEN];
	int flag;
	int len;
	int commid;
};

struct PACKET * ntohp(struct PACKET *);
struct PACKET * htonp(struct PACKET *);

void sendFile(struct PACKET *, int, FILE *);

void recvFile(struct PACKET *, struct PACKET *, int, FILE *);