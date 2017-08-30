#include <stdio.h>
#include <stdlib.h>

int some_int_global = 1;

int main(int argc, char* argv[])
{
		int pid;
	
		if ((pid = vfork()) == 0)
			printf ("hello from the child\n");
		else
			printf ("hello from the parent\n");
			
		int some_int_local = 11;
		
		some_int_global++;
		some_int_local++;
		
		printf("int_g = %d, int_l = %d \n",
			some_int_global,
			some_int_local);
		
		
			
		return 0;
}
