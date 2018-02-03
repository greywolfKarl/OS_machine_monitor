#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "client.h"

int main()
{
	int sock = 0;
	struct sockaddr_in Server;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1) {
		puts("Socket Failed.");
		return 0;
	}
	puts("Socket Created...");

	Server.sin_family = AF_INET;
	Server.sin_port = htons(59487);
	Server.sin_addr.s_addr = inet_addr("127.0.0.1");
	if(connect(sock, (struct sockaddr *)&Server, sizeof(Server)) == -1) {
		puts("Connect Failed.");
		return 0;
	}
	puts("Connected.");

	char Receive[100] = {};
	char Message[100] = {"Watch Officer to Watch Officer: Not only that. Let them do double time! U-47"};
	char Buffer[2048] = {};
	char selection[2] = {};
	char pid[6] = {};

	send(sock, Message, sizeof(Message), 0);
	recv(sock, Receive, sizeof(Receive), 0);
	printf("%s\n", Receive);

	while(1) {
		memset(selection, 0, 2);
		memset(pid, 0, 6);
		memset(Buffer, 0, 2048);

		printf("=============================================\n");
		printf("(a) list all process ids\n");
		printf("(b) thread's PIDs\n");
		printf("(c) child's PIDs\n");
		printf("(d) process name\n");
		printf("(e) state of process(D,R,S,T,t,W,X,Z)\n");
		printf("(f) command line pf excuting process(cmdline)\n");
		printf("(g) parent's PID\n");
		printf("(h) all ancestors of PIDs\n");
		printf("(i) virtual memory size(VmSize)\n");
		printf("(j) physical memory size(VmRSS)\n");
		printf("(k) exit\n");
		printf("which? ");

		scanf("%s", selection);
		while(selection[0] > 'k' || selection[0] < 'a')
			scanf("%s", selection);
		send(sock, selection, sizeof(selection), 0);
		if(selection[0] != 'a' && selection[0] != 'k') {
			printf("pid? ");
			int countpid;
			scanf("%s", pid);
			while(countpid = atoi(pid) == 0)
				scanf("%s", pid);
			send(sock, pid, sizeof(pid), 0);
		}
		recv(sock, Buffer, sizeof(Buffer), 0);

		if(selection[0] == 'a')
			printf("[list] %s\n", Buffer);
		if(selection[0] == 'b')
			printf("[Tid] %s\n", Buffer);
		if(selection[0] == 'c')
			printf("[children] %s\n", Buffer);
		if(selection[0] == 'd')
			printf("[process name] %s\n", Buffer);
		if(selection[0] == 'e')
			printf("[State] %s\n", Buffer);
		if(selection[0] == 'f')
			printf("[cmdline] %s\n", Buffer);
		if(selection[0] == 'g')
			printf("[PPid] %s\n", Buffer);
		if(selection[0] == 'h')
			printf("[ancestor PIDs]\n%s\n", Buffer);
		if(selection[0] == 'i')
			printf("[VmSize] %s\n", Buffer);
		if(selection[0] == 'j')
			printf("[VmRSS] %s\n", Buffer);
		if(selection[0] == 'k') {
			printf("%s\n", Buffer);
			break;
		}
	}
	close(sock);
	return 0;
}
