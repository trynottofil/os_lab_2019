#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>  // optind
#include <getopt.h>// option
#include <pthread.h>
#include <sys/time.h>


/*
*  Íàïèñàòü ïðîãðàììó äëÿ ïàðàëåëëüíîãî âû÷èñëåíèÿ ôàêòîðèàëà ïî ìîäóëþ mod(k!),
*  êîòîðàÿ áóäåò ïðèíèìàòü íà âõîä ñëåäóþùèå ïàðàìåòðû(ïðèìåð: -k 10 --pnum = 4 --mod = 10):
*  1.	k - ÷èñëî, ôàêòîðèàë êîòîðîãî íåîáõîäèìî âû÷èñëèòü.
*  -2.	pnum - êîëè÷åñòâî ïîòîêîâ.
*  3.	mod - ìîäóëü ôàêòîðèàëà
*	Äëÿ ñèíõðîíèçàöèè ðåçóëüòàòîâ íåîáõîäèìî èñïîëüçîâàòü ìüþòåêñû.
*/
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
int factmod(void *args);

int result = 1;




typedef struct someArgs {
	int start;
	int k;
	int mod;
	int *result;

} someArgs_t;

int main(int argc, char **argv)
{
	int pnum_num = -1;
	int mod = -1;
	int k = -1;



	while (true)
	{
		int current_optind = optind ? optind : 1;

		static struct option options[] = { {"pnum", required_argument, 0, 0},
										  {"mod", required_argument, 0, 0},
										  {0, 0, 0, 0} };

		int option_index = 0;
		int c = getopt_long(argc, argv, "k:", options, &option_index);

		if (c == -1) break;

		switch (c)
		{
		case 0:
			switch (option_index)
			{
			case 0:
				pnum_num = atoi(optarg);
				if (pnum_num < 0)
				{
					printf("pnum_num=%d < 0!\n", pnum_num);
					return 1;
				}
				break;
			case 1:
				mod = atoi(optarg); // ðàçìåð ìàññèâà (to int çíà÷åíèå òåêóùåãî ïàðàìåòðà)
				// your code here
				// error handling

				if (mod < 0)
				{
					printf("mod=%d < 0!\n", mod);
					return 1;
				}
				break;

			defalut:
				printf("Index %d is out of options\n", option_index);
			}
			break;

		case 'k':
			k = atoi(optarg);
			if (k <= 0)
			{
				printf("k =%d <= 0!\n", k);
				return 1;
			}
			break;

		default:
			printf("getopt returned character code 0%o?\n", c);
		}
	}
	
	if (optind < argc)
	{
		printf("Has at least one no option argument\n");
		return 1;
	}
	if (pnum_num == -1 || k == -1 || mod == -1)
	{
		printf("Usage: %s -k \"num\" --pnum \"num\" --mod \"num\"  \n", argv[0]);
		return 1;
	}

	if (k < pnum_num)
	{
		printf("\nError! Use k > pnum!\n");
		return 1;
	}


	
	int part = k / pnum_num;
	pthread_t* array = malloc(sizeof(pthread_t) * pnum_num);
	someArgs_t* args = malloc(sizeof(someArgs_t) * pnum_num);

	struct timeval start_time;
	gettimeofday(&start_time, NULL);

	for (size_t i = 0; i < pnum_num; i++)
	{
		if (i < pnum_num - 1)
		{

			if (i == 0) 
			{
				args[i].start = 1;
			}
			else
			{
				args[i].start = i * part;
			}
			//args[i].start = i * part;
			args[i].k = (i + 1)*part;
			args[i].result = &result;
		}
		else
		{
			args[i].start = i * part;
			args[i].k = k;
		}
		args[i].result = &result;
		args[i].mod = mod;
		//printf("\ni=%ld , start=%d, k=%d\n", i, args[i].start, args[i].k);
		if (pthread_create(&array[i], NULL, (void *)factmod, (void *)&args[i]) != 0)
		{
			perror("pthread_create");
			exit(1);
		}
		//printf("\n%lld=re\n", result);
	}

	for (size_t i = 0; i < pnum_num; i++)
	{
		if (pthread_join(array[i], NULL) != 0) {
			perror("pthread_join");
			exit(1);
		}
	}

	struct timeval finish_time;
	gettimeofday(&finish_time, NULL);

	double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
	elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
	//printf("\ntime =%fms\n", result,elapsed_time);
	printf("Result = %d\n", result);
	
	return 0;
}





// 
int factmod(void *args)
{
	someArgs_t *arg = (someArgs_t*)args;
	int res = 1;
	while (arg->k > 1) {
		res = (res * ((arg->k / arg->mod) % 2 ? arg->mod - 1 : 1)) % arg->mod;
		for (int i = arg->start; i <= (arg->k) % arg->mod; ++i)
			res = (res * i) % arg->mod;
		arg->k /= arg->mod;
	}
	res = res % arg->mod;
	pthread_mutex_lock(&mut);
	*(arg->result) *= res;
	pthread_mutex_unlock(&mut);
}
