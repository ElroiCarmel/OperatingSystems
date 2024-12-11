#include <stdio.h>
#include <stdlib.h>
#include <glob.h>

int main()
{
	char **found;
	glob_t glob_struct;
	int result;

	result = glob("*.c", 0 , NULL, &glob_struct);
	/* check for errors */
	if( result!=0 )
	{
		if( result==GLOB_NOMATCH )
			fprintf(stderr,"No matches\n");
		else
			fprintf(stderr,"Some error\n");
		exit(1);
	}
	
	/* success, output found filenames */
	printf("Found %d filename matches\n",glob_struct.gl_pathc);
	found = glob_struct.gl_pathv;
	while(*found)
	{
		printf("%s\n",*found);
		found++;
	}

	return(0);
}
