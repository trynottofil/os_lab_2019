#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>
#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "lib.h"

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
uint64_t global_res = 1;
struct Server {
	char ip[255];
	int port;
};

//////////////////////////////////////////////////////////
//uint64_t MultModulo(uint64_t a, uint64_t b, uint64_t mod) {
//	uint64_t result = 0;
//	a = a % mod;
//	while (b > 0) {
//		if (b % 2 == 1)
//			result = (result + a) % mod;
//		a = (a * 2) % mod;
//		b /= 2;
//	}
//
//	return result % mod;
//}

bool ConvertStringToUI64(const char *str, uint64_t *val) {
	char *end = NULL;
	unsigned long long i = strtoull(str, &end, 10);
	if (errno == ERANGE) {
		fprintf(stderr, "Out of uint64_t range: %s\n", str);
		return false;
	}

	if (errno != 0)
		return false;

	*val = i;
	return true;
}

void* ListenerHost(void*sck_) {
	int sck = (int)sck_;
	char response[sizeof(uint64_t)];
	int a = recv(sck, response, sizeof(uint64_t), 0);
	if (a < 0) {
		fprintf(stderr, "Recieve failed\n");
		fprintf(stderr, "recv: %s (%d)\n", strerror(errno), errno);
		exit(1);
	}
	close(sck);
	//printf("\n%s-reñ\n", response);
	uint64_t answer = 0;
	memcpy(&answer, response, sizeof(response));
	pthread_mutex_lock(&mut);
	global_res *= answer;
	pthread_mutex_unlock(&mut);
	
}

int main(int argc, char **argv) {
	uint64_t k = -1;
	uint64_t mod = -1;
	char servers[255] = { '\0' }; // TODO: explain why 255 - ýòî îãðàíè÷åíèå íà äëèíó ïóòè â 255 áàéò, êîòîðîå íàêëàäûâàåòñÿ ôàéëîâîé ñèñòåìîé.
	FILE* file;
	//ïîëó÷àåì ïàðàìåòðû(îïöèè) è ïðîâåðÿåì èõ íà êîððåêòíîñòü.
	while (true) {
		int current_optind = optind ? optind : 1;

		static struct option options[] = { {"k", required_argument, 0, 0},
										  {"mod", required_argument, 0, 0},
										  {"servers", required_argument, 0, 0},
										  {0, 0, 0, 0} };

		int option_index = 0;
		int c = getopt_long(argc, argv, "", options, &option_index);

		if (c == -1)
			break;

		switch (c) {
		case 0: {
			switch (option_index) {
			case 0:
				ConvertStringToUI64(optarg, &k);
				//TODO: your code here
				if (k < 0) {
					printf("K %llu < 0!\n", k);
					exit(1);
				}
				break;
			case 1:
				ConvertStringToUI64(optarg, &mod);
				//TODO: your code here
				if (mod < 0) {
					printf("Mod %llu < 0!\n", mod);
					exit(1);
				}

				break;
			case 2:
				// TODO: your code here
				memcpy(servers, optarg, strlen(optarg));
				if ((file = fopen(servers, "r")) == NULL) {
					printf("Error with file :%s \n", servers);
					exit(1);
				}
				break;
			default:
				printf("Index %d is out of options\n", option_index);
			}
		} break;

		case '?':
			printf("Arguments error\n");
			break;
		default:
			printf(stderr, "getopt returned character code 0%o?\n", c);
		}
	}

	if (k == -1 || mod == -1 || !strlen(servers)) {
		fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
			argv[0]);
		return 1;
	}
	// TODO: for one server here, rewrite with servers from file
	unsigned int servers_num = 0;
	fseek(file, 0, SEEK_SET);
	char chr = getc(file);
	if (chr > '0' && chr < '9') { servers_num++; }
	while (chr != EOF)
	{
		if (chr == '\n')
		{
			servers_num = servers_num + 1;
		}
		chr = getc(file);

	}
	fseek(file, 0, SEEK_SET);

	struct Server *to = malloc(sizeof(struct Server) * servers_num);
	// TODO: delete this and parallel work between servers
	char line[100];
	for (size_t i = 0; i < servers_num; i++) {
		fgets(&line, sizeof(line), file);
		char *istr;
		char sep[2] = ":";
		istr = strtok(line, sep);
		memcpy(to[i].ip, istr, strlen(istr));
		istr = strtok(NULL, sep);
		to[i].port = atoi(istr);
		printf("ip:%s\nport:%d\n", to[i].ip, to[i].port);

	}
	

	// TODO: work continiously, rewrite to make parallel
	pthread_t* array = malloc(sizeof(pthread_t) * servers_num);

	for (int i = 0; i < servers_num; i++) {
		struct hostent *hostname = gethostbyname(to[i].ip);
		if (hostname == NULL) {
			fprintf(stderr, "gethostbyname failed with %s\n", to[i].ip);
			exit(1);
		}


		struct sockaddr_in server;
		server.sin_family = AF_INET;
		server.sin_port = htons(to[i].port);
		server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

		
		//ñîçäàåòñÿ ñîêåò è ïðîâåðÿåòñÿ íà îøèáêó

		int sck = socket(AF_INET, SOCK_STREAM, 0);
		if (sck < 0) {
			fprintf(stderr, "Socket creation failed!\n");
			exit(1);
		}

		//óñòàíîâëåíèå ñâÿçè ñ ñåðâåðîì 
		if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
			fprintf(stderr, "Connection failed\n");
			exit(1);
		}

		// TODO: for one server
		// parallel between servers
		int part = k / servers_num;
		uint64_t begin = 0;
		uint64_t end = 0;
		if (i < servers_num - 1) {
			if (i == 0) {
				begin = 1;
			}
			else {
				begin = i * part + 1;
			}
			end = (i + 1)*part;
		}
		else {
			begin = i * part + 1;
			end = k;
		}

		char task[sizeof(uint64_t) * 3];
		memcpy(task, &begin, sizeof(uint64_t));
		memcpy(task + sizeof(uint64_t), &end, sizeof(uint64_t));
		memcpy(task + 2 * sizeof(uint64_t), &mod, sizeof(uint64_t));

		if (send(sck, task, sizeof(task), 0) < 0) {
			fprintf(stderr, "Send failed\n");
			exit(1);
		}

		if (pthread_create(&array[i], NULL, (void *)ListenerHost, (void*)sck) != 0)
		{
			perror("pthread_create");
			exit(1);
		}
	}

	for (size_t i = 0; i < servers_num; i++)
	{
		if (pthread_join(array[i], NULL) != 0) {
			perror("pthread_join");
			exit(1);
		}
	}
	free(array);
	printf("Result: %llu\n", global_res);
	free(to);

	return 0;
}
