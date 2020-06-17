#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#define Sleep(X) sleep(X)
#define wait() scanf("1")



#define PHT_SIZE 5

void* eat(void *args);

typedef struct philosopher_tag {
	const char *name;
	unsigned left_fork;
	unsigned right_fork;
} philosopher_t;


typedef struct table_tag {
	pthread_mutex_t forks[PHT_SIZE];
} table_t;

typedef struct philosopher_args_tag {
	const philosopher_t *philosopher;
	const table_t *table;
} philosopher_args_t;


void init_philosopher(philosopher_t *philosopher, const char *name, unsigned left_fork, unsigned right_fork) {
	philosopher->name = name;
	philosopher->left_fork = left_fork;
	philosopher->right_fork = right_fork;
}

void init_table(table_t *table) {
	size_t i;
	for (i = 0; i < PHT_SIZE; i++) {
		pthread_mutex_init(&table->forks[i], NULL);
	}
}


void* eat(void *args) 
{
	philosopher_args_t *arg = (philosopher_args_t*)args;
	const philosopher_t *philosopher = arg->philosopher;
	const table_t *table = arg->table;

	printf("%s started dinner\n", philosopher->name);

	pthread_mutex_lock(&table->forks[philosopher->left_fork]);
	pthread_mutex_lock(&table->forks[philosopher->right_fork]);

	printf("%s is eating\n", philosopher->name);
	Sleep(1000);

	pthread_mutex_unlock(&table->forks[philosopher->right_fork]);
	pthread_mutex_unlock(&table->forks[philosopher->left_fork]);

	printf("%s finished dinner\n", philosopher->name);
}





void main() {
	pthread_t threads[PHT_SIZE];

	philosopher_t philosophers[PHT_SIZE];
	philosopher_args_t arguments[PHT_SIZE];
	table_t table;

	size_t i;

	init_table(&table);

	init_philosopher(&philosophers[0], "Alice", 0, 1);
	init_philosopher(&philosophers[1], "Bob", 1, 2);
	init_philosopher(&philosophers[2], "Clark", 2, 3);
	init_philosopher(&philosophers[3], "Denis", 3, 4);
	init_philosopher(&philosophers[4], "Eugin", 4, 0);

	for (i = 0; i < PHT_SIZE; i++) {
		arguments[i].philosopher = &philosophers[i];
		arguments[i].table = &table;
	}

	for (i = 0; i < PHT_SIZE; i++) {
		pthread_create(&threads[i], NULL, eat, &arguments[i]);
	}

	for (i = 0; i < PHT_SIZE; i++) {
		pthread_join(threads[i], NULL);
	}

	wait();
}
