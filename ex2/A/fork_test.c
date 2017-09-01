#include <stdio.h>
#include <stdlib.h>

int some_int_global = 1;

int main(int argc, char* argv[])
{
		pid_t pid;
		int some_int_local = 0;
		some_int_global++;
		some_int_local++;
		
		
			
		pid = fork();
		if (pid == 0)
		{
			printf ("hello from the child\n");
			some_int_global++;
			some_int_local++;
			printf("global = %d, local = %d\n",some_int_local, some_int_global);
			_exit(EXIT_SUCCESS);
		}	
		else
		{
			int status;
			printf ("hello from the parent\n");
			some_int_global++;
			some_int_local++;
			printf("global = %d, local = %d\n",some_int_local, some_int_global);
			(void)waitpid(pid, &status, 0);
		}
		
		some_int_global++;
		some_int_local++;
		printf(" final global = %d, local = %d\n",some_int_local, some_int_global);
		
		return 0;
}
