#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


main()
{
	printf("RUID = %d\nEUID = %d\n", getuid(), geteuid());
	FILE* f = fopen("Textfile", "r");
	if (f != NULL)
	{
        	perror("Failed to open file\n");
	}
	else
	{
		fclose(f);
	}

	printf("RUID set to = %d\n", geteuid());
	setuid(geteuid());

	f = fopen("Textfile", "r");
	if (f != NULL)
	{
		perror("Failed to open file\n");
	}
	else
	{
		fclose(f);
	}

	return 0;
}
