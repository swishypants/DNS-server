#include<stdio.h>

int main(int argc, char **argv) {

	int i;
	int port;
	char *hostfile;
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
			printf("%s", "Proper cmd: dns-server [-p port#] [-f hostfile]\n");
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
			printf("%s", "Proper cmd: dns-server [-p port#] [-f hostfile]\n");
			return 1;
		}
	}
	else {
		printf("%s", "Proper cmd: dns-server [-p port#] [-f hostfile]\n");
		return 1;
	}

	printf("Port #: %d\n", port);
	printf("Host filename: %s\n", hostfile);

	return 0;
}
