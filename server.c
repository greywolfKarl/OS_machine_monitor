#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "server.h"
#define READL 2048
#define PATHL 64

void *messenger(void*);

int main()
{
	int sock = 0, clientsock = 0;
	struct sockaddr_in Server, Client;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock == -1) {
		puts("Socket Failed.");
		return 0;
	}
	puts("Socket Created...");

	int timewait = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &timewait, sizeof(timewait));
	Server.sin_family = AF_INET;
	Server.sin_port = htons(59487);
	Server.sin_addr.s_addr = INADDR_ANY;

	if(bind(sock, (struct sockaddr*)&Server, sizeof(Server)) == -1) {
		puts("Bind Failed.");
		return 0;
	}
	puts("Binded...");

	if(listen(sock, 20) == -1) {
		puts("Listen Failed.");
	}
	puts("Listening...");

	int length = sizeof(Client);
	int p = 0, *newp = NULL;

	int count = 0;
	pthread_t *threads = (pthread_t*)malloc(64*sizeof(pthread_t));

	while(clientsock = accept(sock, (struct sockaddr*)&Client,
	                          (socklen_t*)&length)) {
		if(clientsock == -1) {
			puts("Connect Failed.");
			return 0;
		}
		puts("Connected.");

		newp = malloc(sizeof(clientsock));
		*newp = clientsock;
		p = pthread_create(&threads[count++], NULL, messenger, (void*)newp);
		if(p != 0) {
			puts("Pthread Failed.");
			return 0;
		}
	}
	while(--count)
		pthread_join(threads[count], NULL);
	free(threads);
	return 0;
}

void *messenger(void *sock)
{
	int csock = *(int*)sock;
	free(sock);
	char selection[2] = {};
	char pid[6] = {};
	char Exit[7] = {"[Exit]"};
	char Err[20] = {"Pid Not Exist."};
	char Receive[100] = {};
	char Message[100] = {"Watch Officer to Watch Officer-Let them all go to Hell! U-46"};
	char Path[PATHL] = {};
	char Readfile[READL] = {};
	char List[READL] = {};
	char *flag = "";
	char *sp = " ";
	FILE* file = NULL;
	DIR* direction = NULL;
	struct dirent* name;

	send(csock, Message, sizeof(Message), 0);
	recv(csock, Receive, sizeof(Receive), 0);
	printf("%s\n", Receive);

	while(1) {
		memset(selection, 0, 2);
		memset(pid, 0, 6);
		memset(Path, 0, PATHL);
		memset(Readfile, 0, READL);
		memset(List, 0, READL);

		recv(csock, selection, sizeof(selection), 0);
		printf("do: %s\n", selection);
		if(selection[0] != 'a' && selection[0] != 'k') {
			recv(csock, pid, sizeof(pid), 0);
			printf("pid: %s\n", pid);
		}

		if(selection[0] == 'a') {
			direction = opendir("/proc");
			while(name = readdir(direction)) {
				flag = name->d_name;
				if (flag[0] >= '0' && flag[0] <= '9') {
					strcat(List, flag);
					strcat(List, sp);
				}
			}
			printf("%s\n", List);
			closedir(direction);
			send(csock, List, sizeof(List), 0);
		}

		if(selection[0] == 'b') {
			snprintf(Path, PATHL, "/proc/%s/task", pid);
			direction = opendir(Path);
			if(direction == NULL) {
				puts("PID Not exist.");
				send(csock, Err, sizeof(Err), 0);
			} else {
				while(name = readdir(direction)) {
					flag = name->d_name;
					if (flag[0] != '.') {
						strcat(List, flag);
						strcat(List, sp);
					}
				}
				printf("%s\n", List);
				closedir(direction);
				send(csock, List, sizeof(List), 0);
			}
		}

		if(selection[0] == 'c') {
			snprintf(Path, PATHL, "/proc/%s/task/%s/children", pid, pid);
			file = fopen(Path, "r");
			if(file == NULL) {
				puts("PID Not exist.");
				send(csock, Err, sizeof(Err), 0);
			} else {
				if(fgets(List, READL, file) != NULL)
					printf("%s\n", List);
				fclose(file);
				send(csock, List, sizeof(List), 0);
			}
		}

		if(selection[0] == 'd') {
			snprintf(Path, PATHL, "/proc/%s/status", pid);
			file = fopen(Path, "r");
			if(file == NULL) {
				puts("PID Not exist.");
				send(csock, Err, sizeof(Err), 0);
			} else {
				while(fgets(Readfile, READL, file)) {
					if(strncmp(Readfile, "Name:", 5) != 0)
						continue;
					flag = Readfile + 6;
					while(isspace(*flag) != 0)
						flag++;
					printf("%s", flag);
					break;
				}
				fclose(file);
				send(csock, flag, sizeof(List), 0);
			}
		}

		if(selection[0] == 'e') {
			snprintf(Path, PATHL, "/proc/%s/status", pid);
			file = fopen(Path, "r");
			if(file == NULL) {
				puts("PID Not exist.");
				send(csock, Err, sizeof(Err), 0);
			} else {
				while(fgets(Readfile, READL, file)) {
					if(strncmp(Readfile, "State:", 6) != 0)
						continue;
					flag = Readfile + 7;
					while(isspace(*flag) != 0)
						flag++;
					printf("%s", flag);
					break;
				}
				fclose(file);
				send(csock, flag, sizeof(List), 0);
			}
		}

		if(selection[0] == 'f') {
			snprintf(Path, PATHL, "/proc/%s/cmdline", pid);
			file = fopen(Path, "r");
			if(file == NULL) {
				puts("PID Not exist.");
				send(csock, Err, sizeof(Err), 0);
			} else {
				if(fgets(List, READL, file) != NULL)
					printf("%s\n", List);
				fclose(file);
				send(csock, List, sizeof(List), 0);
			}
		}

		if(selection[0] == 'g') {
			snprintf(Path, PATHL, "/proc/%s/status", pid);
			file = fopen(Path, "r");
			if(file == NULL) {
				puts("PID Not exist.");
				send(csock, Err, sizeof(Err), 0);
			} else {
				while(fgets(Readfile, READL, file)) {
					if(strncmp(Readfile, "PPid:", 5) != 0)
						continue;
					flag = Readfile + 6;
					while(isspace(*flag) != 0)
						flag++;
					printf("%s", flag);
					break;
				}
				fclose(file);
				send(csock, flag, sizeof(List), 0);
			}
		}

		if(selection[0] == 'h') {
			int tempid = atoi(pid);
			char tempidc[5] = {};
			snprintf(Path, PATHL, "/proc/%s/status", pid);
			file = fopen(Path, "r");
			if(file == NULL) {
				puts("PID Not exist.");
				send(csock, Err, sizeof(Err), 0);
			} else {
				while(tempid != 0) {
					snprintf(tempidc, 5, "%d", tempid);
					snprintf(Path, PATHL, "/proc/%s/status", tempidc);
					file = fopen(Path, "r");
					while(fgets(Readfile, READL, file)) {
						if(strncmp(Readfile, "PPid:", 5) != 0)
							continue;
						flag = Readfile + 6;
						while(isspace(*flag) != 0)
							flag++;
						printf("%s", flag);
						break;
					}
					tempid = atoi(flag);
					strcat(List, flag);
				}
				fclose(file);
				send(csock, List, sizeof(List), 0);
			}
		}

		if(selection[0] == 'i') {
			snprintf(Path, PATHL, "/proc/%s/status", pid);
			file = fopen(Path, "r");
			if(file == NULL) {
				puts("PID Not exist.");
				send(csock, Err, sizeof(Err), 0);
			} else {
				while(fgets(Readfile, READL, file)) {
					if(strncmp(Readfile, "VmSize:", 7) != 0)
						continue;
					flag = Readfile + 8;
					while(isspace(*flag) != 0)
						flag++;
					printf("%s", flag);
					break;
				}
				fclose(file);
				send(csock, flag, sizeof(List), 0);
			}
		}

		if(selection[0] == 'j') {
			snprintf(Path, PATHL, "/proc/%s/status", pid);
			file = fopen(Path, "r");
			if(file == NULL) {
				puts("PID Not exist.");
				send(csock, Err, sizeof(Err), 0);
			} else {
				while(fgets(Readfile, READL, file)) {
					if(strncmp(Readfile, "VmRSS:", 6) != 0)
						continue;
					flag = Readfile + 7;
					while(isspace(*flag) != 0)
						flag++;
					printf("%s", flag);
					break;
				}
				fclose(file);
				send(csock, flag, sizeof(List), 0);
			}
		}

		if(selection[0] == 'k') {
			puts("Client Exit.");
			send(csock, Exit, sizeof(Exit), 0);
			pthread_exit(NULL);
		}
	}
	close(csock);
	return NULL;
}
