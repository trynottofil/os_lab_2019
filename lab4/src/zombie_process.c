#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	pid_t child_process;
	/* Создание дочернего процесса. */
	child_process = fork();
	if (child_process > 0)
	{
		/* Hодительский процесс — делаем паузу. */
		sleep(45);
	}
	else 
	{
		/* Дочерний процесс — завершаем работу. */
		exit(0);
	}
	return 0;

}
