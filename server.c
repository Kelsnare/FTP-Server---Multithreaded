/*
Contains server specific function implementations..

By : 
Abhishek Sarkar
Ajay Kumar Kilaka
*/

#include "server.h"

char addr_str[INET_ADDRSTRLEN];

int main(int argc, char *argv[]){
	struct sockaddr_in serv_addr, d_serv_addr, client_addr;
	int sin_size;

	/* initialize linked list */
    list_init(&client_list);
     
    /* initiate mutex */
    pthread_mutex_init(&clientlist_mutex, NULL);

	//Creating control socket
	if((c_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		printf("error creating control socket : Returning Error No:%d\n", errno);
		return errno;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(C_PORT);
	serv_addr.sin_addr.s_addr = inet_addr(IP);
	memset(&(serv_addr.sin_zero), 0, 8);

	if(bind(c_sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1){
		printf("Error binding control socket to port %d : Returning error No:%d\n", C_PORT, errno);
		return errno;
	}

	if(listen(c_sockfd, BACKLOG) == -1){
		printf("Error listening at control port : Returning Error No:%d\n", errno);
		return errno;
	}

	printf("\nServer started at port:%d\n", C_PORT);
	sin_size = sizeof(struct sockaddr_in);
	while(1){
		if((con_csfd = accept(c_sockfd, (struct sockaddr *)&client_addr, (socklen_t *)&sin_size)) == -1){
			printf("Error accepting connections : Returning Error No:%d\n\n", errno);
			return errno;
		}
		if(client_list.size == CLIENTS){
			fprintf(stderr, "Connection full. Request rejected !!\n");
			continue;
		}	

		void *addr;
		addr = &(client_addr.sin_addr);
		inet_ntop(AF_INET, addr, addr_str, sizeof(addr_str));
		printf("Connection request received from:[%d]%s\n", con_csfd, addr_str);
		fflush(stdout);

		struct THREADINFO threadinfo;
		threadinfo.sockfd = con_csfd;
		char cwd[DATALEN];
		getcwd(cwd, DATALEN);
		strcpy(threadinfo.curr_dir, cwd);
		pthread_mutex_lock(&clientlist_mutex);
		list_insert(&client_list, &threadinfo);
		pthread_mutex_unlock(&clientlist_mutex);

		pthread_create(&threadinfo.thread_ID, NULL, client_handler, (void *)&threadinfo);

		
		
	}

	return 0;
}

void client_handler(void *fd){
	int bytes, option;
	struct PACKET *np = (struct PACKET *)malloc(sizeof(struct PACKET)), *hp;
	
	struct THREADINFO threadinfo = *(struct THREADINFO *)fd;

	while(1){
		bytes = recv(threadinfo.sockfd, np, sizeof(struct PACKET), 0);
		if(bytes == 0){
			printf("Connection closed by client at:[%d]%s\n",threadinfo.sockfd, addr_str);
			pthread_mutex_lock(&clientlist_mutex);
			list_delete(&client_list, &threadinfo);
			pthread_mutex_unlock(&clientlist_mutex);
			break;
		}

		hp = ntohp(np);
		printf("[%d]command #%d\n",threadinfo.sockfd, hp->commid);
		switch(hp->commid){
			case GET:
				client_get(hp, threadinfo);
				break;
			case PUT:
				client_put(hp, threadinfo);
				break;
			case LS:
				client_ls(hp, threadinfo);
				break;
			case CD:
				client_cd(hp, &threadinfo);
				break;
			case PWD:
				client_pwd(hp, threadinfo);
				break;
			/*case QUIT:
				client_quit(hp, threadinfo);
				break;*/
			default:
				//fprintf(stderr,"Corrupt packet received..\n\n");
				break;
		}
	}	
}

//pwd Command...............................................................................................
void client_pwd(struct PACKET *hp, struct THREADINFO t){
	
	int sent;
	struct PACKET *np;
	
	strcpy(hp->data, t.curr_dir);
	hp->flag = OK;
	hp->len = strlen(hp->data);
	np = htonp(hp);
	sent = send(t.sockfd, np, sizeof(struct PACKET), 0);	
}

//ls Command.................................................................................................
void client_ls(struct PACKET *hp, struct THREADINFO t){
	
	struct PACKET *np;
	int sent;
	//char cwd[DATALEN];
	DIR *dir;
	struct dirent *e;

	if((dir = opendir(t.curr_dir)) != NULL){
		while((e = readdir(dir)) != NULL){
			strcpy(hp->data, e->d_name);
			//strcat(hp->data, "\n");
			hp->flag = OK;
			hp->len = strlen(hp->data);
			np = htonp(hp);
			sent = send(t.sockfd, np, sizeof(struct PACKET), 0);
		}
		hp->flag = DONE;
		strcpy(hp->data, "");
		hp->len = strlen(hp->data);
		np = htonp(hp);
		sent = send(t.sockfd, np, sizeof(struct PACKET), 0);
	}else
		hp->flag = ERR;

	if(hp->flag == ERR){
		strcpy(hp->data, "Error processing 'ls' command at server..!!\n\n");
		hp->len = strlen(hp->data);
		np = htonp(hp);
		sent = send(t.sockfd, np, sizeof(struct PACKET), 0);
		//exit(1);
	}
	
}

//cd Command.........................................................................................
void client_cd(struct PACKET *hp, struct THREADINFO *t){
	struct PACKET *np;
	int sent;

	char temp[DATALEN];
	getcwd(temp, DATALEN);

	chdir(t->curr_dir);

	if(chdir(hp->data) < 0){
		hp->flag = ERR;
		strcpy(hp->data, "Error changing directory, probably non-existent\n");
		hp->len = strlen(hp->data);
		np = htonp(hp);
		sent = send(t->sockfd, np, sizeof(struct PACKET), 0);
	}

	else{
		getcwd(t->curr_dir, DATALEN);
		chdir(temp);
		client_pwd(hp, *t);
	}

}

//get Command..................................................................................................
void client_get(struct PACKET *hp, struct THREADINFO t){
	struct PACKET *np;
	int sent, total_sent = 0;
	size_t read;
	FILE *in;
	char path[496];
	strcpy(path,t.curr_dir);
	strcat(path, "/");
	strcat(path,hp->data);
	printf("File:%s\n", path);
	in = fopen(path, "rb");

	if(!in){
		
				hp->flag = ERR;
				strcpy(hp->data, "Error opening file in server!\n\n");
				hp->len = strlen(hp->data);
				np = htonp(hp);
				sent = send(t.sockfd, np, sizeof(struct PACKET), 0);
								
		
		fprintf(stderr, "Error opening file:%s\n\n", hp->data);
		return;
	}

	
			sendFile(hp, t.sockfd, in);
	
	fclose(in);
}

//put Command...................................................................................................
void client_put(struct PACKET *hp, struct THREADINFO t){
	struct PACKET *np = (struct PACKET *)malloc(sizeof(struct PACKET));
	int bytes, total_recv = 0;
	//getFileNameFromPath(hp->data);
	FILE *out;
	char path[496];
	strcpy(path,t.curr_dir);
	strcat(path, "/");
	strcat(path,hp->data);
	printf("File:%s\n", path);
	out = fopen(path, "wb");
	if(!out){
		
				hp->flag = ERR;
				strcpy(hp->data, "Error creating file in server!\n\n");
				hp->len = strlen(hp->data);
				np = htonp(hp);
				bytes = send(t.sockfd, np, sizeof(struct PACKET), 0);
		
		fprintf(stderr, "Error creating file:%s\n\n", hp->data);
		//return;
	}else{

		hp->flag = READY;
		//strcpy(hp->data,"");
		hp->len = strlen(hp->data);
		np = htonp(hp);

		
				bytes = send(t.sockfd, np, sizeof(struct PACKET), 0);

				//Once file is created at server
				recvFile(hp, np,t.sockfd, out);
		
	}	
	
	fclose(out);	
}

//For comparing if two threads in arguments are the same..........................
int compare(struct THREADINFO *a, struct THREADINFO *b){
	return a->sockfd - b->sockfd;
} 

//Initializing given list.........................................................
void list_init(struct LLIST *ll){
	ll->head = ll->tail = NULL;
    ll->size = 0;
}

//Insert operation on list.........................................................
int list_insert(struct LLIST *ll, struct THREADINFO *thr_info) {
    if(ll->size == CLIENTS) return -1;
    if(ll->head == NULL) {
    ll->head = (struct LLNODE *)malloc(sizeof(struct LLNODE));
    ll->head->threadinfo = *thr_info;
    ll->head->next = NULL;
    ll->tail = ll->head;
    }
    else {
    ll->tail->next = (struct LLNODE *)malloc(sizeof(struct LLNODE));
    ll->tail->next->threadinfo = *thr_info;
    ll->tail->next->next = NULL;
    ll->tail = ll->tail->next;
    }
    ll->size++;
    return 0;
}

//Delete operation on list...........................................................
int list_delete(struct LLIST *ll, struct THREADINFO *thr_info) {
    struct LLNODE *curr, *temp;
    if(ll->head == NULL) return -1;
    if(compare(thr_info, &ll->head->threadinfo) == 0) {
    temp = ll->head;
    ll->head = ll->head->next;
    if(ll->head == NULL) ll->tail = ll->head;
    free(temp);
    ll->size--;
    return 0;
    }
    for(curr = ll->head; curr->next != NULL; curr = curr->next) {
    if(compare(thr_info, &curr->next->threadinfo) == 0) {
    temp = curr->next;
    if(temp == ll->tail) ll->tail = curr;
    curr->next = curr->next->next;
    free(temp);
    ll->size--;
    return 0;
    }
    }
    return -1;
}

