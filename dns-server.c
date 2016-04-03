#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>

#include "dns.h"

#define BUFFER_SIZE 2048
// returns a file descriptor for the socket
int CreateSocket(int port){
	int fd;
	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){
		perror("Cannot create socket\n");
		return -1;
	}
	printf("Created socket: %d\n", fd);
	return fd;
}

struct node {
  char* addr;
  char* name; 
  struct node *next;
};

typedef struct node host;

int main(int argc, char **argv) {

	int i;
	int port;
	int recvlen; // # of bytes received
	int serverSocket; // the socket receiving data
	char *hostfile = NULL;
	FILE *host_fp; 
	
	struct sockaddr_in myaddr;
	struct sockaddr_in remaddr;
	socklen_t addrlen = sizeof(remaddr);
	unsigned char buf[BUFFER_SIZE]; // receiving buffer

	printf("argc = %d\n", argc);	
	/*for(i=0; i<argc; i++) {
		printf("arg[%d] = \"%s\"\n", i, argv[i]);
	}*/

	if(argc==1) {
		port = 12345;
		hostfile = "hosts.txt";
	}
	else if(argc==3) {
		if(strcmp(argv[1], "-p")==0) {
			port = atoi(argv[2]);
			hostfile = "hosts.txt";
		}
		else if(strcmp(argv[1], "-f")==0) {
			port = 12345;
			hostfile = argv[2];
		}
		else {
			printf("Proper cmd: dns-server [-p port#] [-f hostfile]\n");
			return 1;
		}
	}
	else if(argc==5) {
		if(strcmp(argv[1], "-p")==0 && strcmp(argv[3], "-f")==0) {
			port = atoi(argv[2]);
			hostfile = argv[4];
		}
		else if(strcmp(argv[1], "-f")==0 && strcmp(argv[3], "-p")==0) {
			port = atoi(argv[4]);
			hostfile = argv[2];
		}
		else {
			printf("Proper cmd: dns-server [-p port#] [-f hostfile]\n");
			return 1;
		}
	}
	else {
		printf("Proper cmd: dns-server [-p port#] [-f hostfile]\n");
		return 1;
	}
	if((serverSocket = CreateSocket(port)) < 0){
		return -1;
	} // returns a file descriptor
	memset((char *)&myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(port);
	if(bind(serverSocket, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0){
		printf("Created socket: %d\n", serverSocket);
		perror("Cannot bind socket\n");
		return 0;
	}
	
//parsing the host file	
	host * curr, * head;
	char * addr;
	char * name;
	head = NULL;
	host_fp=fopen(hostfile,"r");
    if (host_fp == 0)
    {
        //null pointer returned - failure
        perror("Canot open host file\n");
        exit(-1);
    }else{
    
    char line[256];

	//iterate through every line of hosts.txt
    while (fgets(line, sizeof(line), host_fp)) {
    	if(line[0] == '#' || line[0] == '\n' || strlen(line)==0)
        	continue; 
        addr = strtok(line, " \t\n");
        name = strtok(NULL, " \t\n");
        //printf("%s\n", addr);
        //printf("%s\n", host);

        curr = (host *)malloc(sizeof(host));
        curr->name = malloc(strlen(name)+1);
        strcpy(curr->name, name);
        curr->addr = malloc(strlen(addr)+1);
        strcpy(curr->addr, addr);
        curr->next = head;
        head = curr;
    }
    curr= head;
    
    
 	host * temp=head;
	while(temp!=NULL){
		printf("%s\n", temp->addr);
        printf("%s\n", temp->name);
    	temp=temp->next;
	}
    
    
    fclose(host_fp);
    }


	char* qname;
	dns_header* dns_h = NULL;
	dns_question* dns_q = NULL;
	dns_rrhdr* dns_a = NULL;

	while(1){
		printf("Waiting on port %d\n", port);
		recvlen = recvfrom(serverSocket, buf, BUFFER_SIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
		printf("Received %d bytes \n", recvlen);
		if(recvlen > 0){
			buf[recvlen] = '0';
		}
		dns_h = (dns_header*)buf;
		unsigned char* qname_buf = (unsigned char *)&buf[sizeof(dns_header)];
		char c = *qname_buf;
		char* qname_fin = malloc(256);
		int j = 0;
		int num = 0;
		int flag = 0;
//		printf("Query name: ");
		while(c != 0){
			if(flag == 0){
				num = c;
				flag = 1;
//				printf("%d",num);
				*(qname_fin + j) = num + '0';
			}
			else{
//				printf("%c",c);
				*(qname_fin + j) = c;
				num--;
				if(num==0){
					flag = 0;
				}
			}
				j++;
				c = *(qname_buf + j);
		}
	//	*(qname_buf + j) = '\0';
		j++;
		printf("J === %d\n",j);
		unsigned char* queryName = malloc(j+1);
		queryName = strncpy(queryName, qname_fin,j+1);
		*(queryName+j+1) = '\0';
	//	free(qname_fin);
		
		
		
// Started step 4 here
		char* test =strncpy(&buf[sizeof(dns_header) + j + sizeof(dns_question)],qname_buf, j+1);
//		*(qname_buf + j) = '0';
		dns_q = (dns_question*)&buf[sizeof(dns_header) + j];
		dns_a = (dns_rrhdr*)&buf[sizeof(dns_header) + j + j + sizeof(dns_question)];
		dns_a->type = htons(1);
		dns_a->class = htons(1);
		dns_a->data_len = htons(4);
		dns_a->ttl = 30;
		unsigned int answer = htons(0);
		
		int g = sizeof(dns_header) + j + j + sizeof(dns_question) + sizeof(dns_rrhdr);
		buf[g] = answer;
//		buf[g+4]='0';
		dns_h->qr = 1;
		dns_h->aa = 1;
		dns_h->tc = 0;
		dns_h->ra = 0;
		dns_h->rcode = 0; // 0 for no error, or 3 for name error (not found in hostsfile.txt
		dns_h->an_count = htons(1); // Set an_count to 1 if having an answer. Else, 0 if it's not in host file.

		
		printf("qname_fin: %s\n", queryName);
		printf("test: %s\n", test);
		printf("question_header: %d\n", dns_q->qtype);
		printf("question_header: %d\n", dns_q->qclass);
		
		printf("DNS access test: \n");
		printf("---------------------\n");
		printf("ID: %d\n", dns_h->id);
		printf("qr_code:  %d\n", htons(dns_h->qr));
		printf("qd_count:  %d\n", htons(dns_h->qd_count));
		printf("rd:  %d\n", dns_h->rd);
		sendto(serverSocket, buf, g+sizeof(answer), 0, (struct sockaddr *)&remaddr, addrlen);
		printf("an_count:  %d\n", htons(dns_h->an_count));
		printf("ns_count:  %d\n", htons(dns_h->ns_count));
		printf("ar_count:  %d\n", htons(dns_h->ar_count));
	//	printf("qtype:  %d\n", dns_q->qtype);
	//	printf("dns_q qtype: %s\n", dns_q->qtype);




//		sendto(serverSocket, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, addrlen);
	}
	printf("Port #: %d\n", port);
	printf("Host filename: %s\n", hostfile);
	close(serverSocket);
	return 0;
}
