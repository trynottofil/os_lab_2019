#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>   //Äëÿ signal

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

pid_t* child_pids = NULL;
int len = -1;
bool DeadChild = false;
void ProcessKiller(int sig)
{
	int pid;
	int status;
	while (1)
	{
		pid = waitpid(WAIT_ANY, &status, WNOHANG);
		if (pid < 0)
		{
			perror("waitpid");
			break;
		}
		if (pid == 0) break;
		for (int i = 0; i < len; i++) //ïðîõîäèìñÿ ïî âñåì äî÷åðíèì ïðîö-àì è ïðèíóäèòåëüíî çàâåðøàåì èõ.
		{
			kill(child_pids[i], SIGKILL);
		}
		break;
	}
	printf("\nChild are dead!\n");
	DeadChild = true;

}

int main(int argc, char **argv) {
	int seed = -1;
	int array_size = -1;
	int pnum = -1;
	int time = 0;
	bool with_files = false;
	//
	printf("-1\n");
	//
	while (true)
	{
		int current_optind = optind ? optind : 1;

		static struct option options[] = { {"seed", required_argument, 0, 0}, // êîðåíü, èìååò çíà÷åíèå, áåç ôëàãà, âîçâðàùàåò 0
										  {"array_size", required_argument, 0, 0}, //ðàçìåð ìàññèâà, èìååò çíà÷åíèå
										  {"pnum", required_argument, 0, 0},
										  {"by_files", no_argument, 0, 'f'},
										  {"timeout", required_argument, 0,0},
										  {0, 0, 0, 0} }; // áåç àðãóìåíòîâ
		// äëÿ ðàáîòû getopt_long.

		int option_index = 0;
		int c = getopt_long(argc, argv, "f", options, &option_index); // ïîëó÷àåò êîðîòêèå(f) è äëèííûå ïàðàìåòðû

		if (c == -1) break; // åñëè âñå àðãóìåíòû çàêîí÷èëèñü - öèêë çàêàí÷èâàåòñÿ.

		switch (c)
		{
		case 0: // åñëè ïîëó÷åí äëèííûé ïàðàìåòð(îïöèÿ) òî :
			switch (option_index) // èíäåêñ òåêóùåé îïöèè
			{
			case 0:
				seed = atoi(optarg);
				// your code here
				// error handling
				if (seed < 0)
				{
					printf("Seed=%d < 0!\n", seed);
					return 1;
				}
				break;
			case 1:
				array_size = atoi(optarg); // ðàçìåð ìàññèâà (to int çíà÷åíèå òåêóùåãî ïàðàìåòðà)
				// your code here
				// error handling

				if (array_size < 0)
				{
					printf("Array size=%d < 0!\n", array_size);
					return 1;
				}
				break;
			case 2:
				pnum = atoi(optarg); // êîëè÷åñòâî ïðîöåññîâ
				// your code here
				// error handling

				if (pnum <= 0)
				{
					printf("Number of processes =%d <= 0!\n", pnum);
					return 1;
				}
				break;
			case 3:
				with_files = true;
				break;
			case 4:
				printf("\nkek\n");
				time = atoi(optarg);
				if (time <= 0)
				{
					printf("Time of alarm =%d <= 0!\n", time);
					return 1;
				}
				break;

			defalut:
				printf("Index %d is out of options\n", option_index);
			}
			break;
		case 'f': // ïîëó÷åí êîðîòêèé ïàðàìåòð
			with_files = true;
			break;

		case '?':
			break;

		default:
			printf("getopt returned character code 0%o?\n", c);
		}
	}
	//printf("\n%d %d\n", optind, argc);
	if (optind < argc)// åñëè îïöèé ìåíüøå ÷åì âñåãî ïàðàìåòðîâ, ïðîã-ìà çàêàí÷èâàåòñÿ
	{
		printf("Has at least one no option argument\n");
		return 1;
	}

	if (seed == -1 || array_size == -1 || pnum == -1)
	{
		printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\"  \n",
			argv[0]);
		return 1;
	}
	//
	//printf("0\n");
	//

	int* array = malloc(sizeof(int) * array_size);
	GenerateArray(array, array_size, seed);

	//printf("\n \n");
	//for (int i = 0; i < array_size; i++)
	//{
	//	printf("%d\n", array[i]);
	//}

	int active_child_processes = 0;

	struct timeval start_time;
	gettimeofday(&start_time, NULL);

	//
	int file_pipes[2];
	pipe(file_pipes);
	//
	//
	//printf("\n \n");
	//
	pid_t* child_pids = (pid_t*)malloc(pnum * sizeof(pid_t));                    // ìàññèâ pid äî÷åðíèõ ïðîöåññîâ
	for (int i = 0; i < pnum; i++)
	{
		pid_t child_pid = fork(); // ñîçäàåòñÿ ïðîöåññ
		child_pids[i] = child_pid; // ñîõðàíÿåì pid äî÷åðíåãî ïðîöåññà
		if (child_pid >= 0)       // åñëè ðîäèòåëü èëè ïîòîìîê(óñïåøíîå ñîçäàíèå ïðîöåññà)
		{
			// successful fork
			active_child_processes += 1;
			if (child_pid == 0)   //ýòîò ïðîöåññ - ïîòîìîê
			{

				//ïîäåëèì ìåæäó ïðîöåññàìè-ïîòîìêàìè ìàññèâ íà ÷àñòè, ñ êîòîðûìè êàæäûé èç íèõ áóäåò ðàáîòàòü. 
				struct MinMax min_max;
				int Part_Size = array_size / pnum;
				if (i == pnum - 1) // åñëè ïîñëåäíèé ïðîöåññ
				{
					min_max = GetMinMax(array, i*Part_Size, array_size);
					//	printf("%d - minmax %d %d\n", i, min_max.min, min_max.max);
				}
				else
				{
					min_max = GetMinMax(array, i*Part_Size, (i + 1)*Part_Size);
					//	printf("%d - minmax %d %d\n", i, min_max.min, min_max.max);
				}



				if (with_files) // ìñïîëüçîâàòü ôàéëû
				{
					// use files here

					FILE *f;
					if (i == 0)f = fopen("data.txt", "w"); // î÷èùàåì ôàéë ïåðåä ïåðâûì èñïîëüçîâàíèåì.
					else f = fopen("data.txt", "a+");      // äîçàïèñûâàåì â ñëó÷àå ïîâòîðíîãî èñïîëüçîâàíèÿ.

					fprintf(f, "%d\n", min_max.min);
					fprintf(f, "%d\n", min_max.max);
					fclose(f);
				}
				else
				{
					// use pipe here

					write(file_pipes[1], &min_max, sizeof(struct MinMax));

				}
				return 0;
			}

		}
		else
		{
			printf("Fork failed!\n");
			return 1;
		}
	}
	//ïîñëå ñîçäàíèÿ ïðîöåññîâ, çàïóñòèì "òàéìåð" äî óíè÷òîæåíèÿ äî÷åðíèõ ïðîöåññîâ
	signal(SIGALRM, ProcessKiller);
	alarm(time);
	//
	//while (!DeadChild && time != 0) { printf("while\n"); } // áåñêîíå÷íûé öèêë äëÿ ïðîâåðêè òàéìåðà
	
	while (active_child_processes > 0)
	{
		// your code here
		int status;
		wait(&status); //æäåì çàâåðøåíèÿ äî÷åðíèõ ïðîö-îâ.

		active_child_processes -= 1;
	}
	//
	//printf("3\n");
	//
	struct MinMax min_max;
	min_max.min = INT_MAX;
	min_max.max = INT_MIN;

	for (int i = 0; i < pnum; i++) // ïðîõîä ïî ïðîöåññàì è ïîèñê ñðåäè íèõ max and min value
	{
		int min = INT_MAX;
		int max = INT_MIN;

		if (with_files)
		{
			// read from files
			struct MinMax NewMinMax;
			FILE *f;
			f = fopen("data.txt", "r");
			for (int j = 0; j < i; j++)
			{
				fscanf(f, "%d", &NewMinMax.min);
				fscanf(f, "%d", &NewMinMax.min);
			}
			fscanf(f, "%d", &NewMinMax.min);
			fscanf(f, "%d", &NewMinMax.max);
			fclose(f);
			//printf("%d - minmaxf %d %d\n", i, NewMinMax.min, NewMinMax.max);
			min = NewMinMax.min;
			max = NewMinMax.max;

		}
		else
		{
			// read from pipes
			struct MinMax NewMinMax;
			read(file_pipes[0], &NewMinMax, sizeof(struct MinMax));

			//printf("%d - minmax %d %d\n", i, NewMinMax.min, NewMinMax.max);
			min = NewMinMax.min;
			max = NewMinMax.max;

		}

		if (min < min_max.min) min_max.min = min;
		if (max > min_max.max) min_max.max = max;
	}
	//
	//printf("4\n");
	//
	struct timeval finish_time;
	gettimeofday(&finish_time, NULL);

	double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
	elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

	free(array);

	printf("Min: %d\n", min_max.min);
	printf("Max: %d\n", min_max.max);
	printf("Elapsed time: %fms\n", elapsed_time);
	free(child_pids);
	fflush(NULL);
	return 0;
}
