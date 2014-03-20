/*
Contains all the common implementations. Any changes here needs to be done keeping in mind its effect on both client.c
and server.c as both use this extensively.

By : 
Abhishek Sarkar
Ajay Kumar Kilaka
*/

#include "commons.h"

//Correcting packet bit storage for usage in host
struct PACKET * ntohp(struct PACKET *np){
	struct PACKET * hp = (struct PACKET *)malloc(sizeof (struct PACKET));

	hp->len = ntohs(np->len);
	hp->flag = ntohs(np->flag);
	hp->commid = ntohs(np->commid);
	memcpy(hp->data, np->data, DATALEN);

	return hp;
}

//Correcting packet bit storage for usage in network
struct PACKET * htonp(struct PACKET *hp){
	struct PACKET * np = (struct PACKET *)malloc(sizeof (struct PACKET));

	np->len = htons(hp->len);
	np->flag = htons(hp->flag);
	np->commid = htons(hp->commid);
	memcpy(np->data, hp->data, DATALEN);

	return np;
}

//Common function for sending file
void sendFile(struct PACKET *hp, int sfd, FILE *fp){
	//size_t read;
	struct PACKET *np;
	//char fName[DATALEN];
	//strcpy(fName, hp->data);
	int sent, total_sent =0,error = 0, j = 0;
	while(!feof(fp)){
		
		memset(hp->data, '\0', sizeof(char)*DATALEN);
		hp->len = fread(hp->data, 1, DATALEN-1, fp);
		total_sent += hp->len;
		hp->flag = OK;
		np = htonp(hp);
		sent = send(sfd, np, sizeof(struct PACKET), 0);
		if(sent != sizeof(struct PACKET)){
			error++;
			break;
		}
		
		j++;
	}
	if(error){
		hp->flag = ERR;
		strcpy(hp->data, "");
		hp->len = 0;
		np = htonp(hp);
		sent = send(sfd, np, sizeof(struct PACKET), 0);
	}else{
		hp->flag = DONE;
		strcpy(hp->data, "");
		hp->len = 0;
		np = htonp(hp);
		sent = send(sfd, np, sizeof(struct PACKET), 0);
	}
	fprintf(stderr, "%d bytes read.\n%d packets sent\n", total_sent, j);
	fflush(stderr);
}


//Common function for receiving file
void recvFile(struct PACKET *hp, struct PACKET *np, int sfd, FILE *out){
	int bytes, total_bytes = 0, j = 0;

	if((bytes = recv(sfd, np, sizeof(struct PACKET), 0)) <= 0){
		fprintf(stderr, "Error receiving file !!\n");
		return;
	}
	j++;
	hp = ntohp(np);
	while(hp->flag == OK){
		if(hp->len > 0){
			total_bytes += fwrite(hp->data, 1, hp->len, out);
		}
		if((bytes = recv(sfd, np, sizeof(struct PACKET), 0)) <= 0){
			fprintf(stderr, "Error receiving file !!\n");
			return;
		}
		j++;
		hp = ntohp(np);
	}
	

	if(hp->flag == ERR){
		fprintf(stderr, "Error sending file !!\n");
	}
	fprintf(stderr, "%d bytes written\n%d packets received\n",total_bytes, j );
	fflush(stderr);
}