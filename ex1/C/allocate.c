#include <stdio.h>
#include <stdlib.h>

int allocate(int value)
{
		int *ptr = malloc(1024*1024*sizeof(char));
		if (ptr == NULL)
		{
			perror("Error: ");
			return 0;
		}
		
		*ptr = value;
		printf("test of allocated memory: %i\n", *ptr);
		
		return 1;
}
