#include <stdio.h>
#include <stdlib.h>

int main()
{

	FILE *f = fopen("file.txt", "w");
	if(f==NULL)
	{
		printf("Error opening file. \n");
		exit(1);
	}

	const char *text = "Testing, testing.";
	fprintf(f, "Some text: %s\n", text);

	fprintf(f, "{\"list\": [ \n");

	//for(Struct ptr= head; while ptr != null; ptr=ptr->next)
	//{
		//fprintf(f, "\t{ \" %s \" : [ \n", ptr->token)
		//fileptr = ptr->fileList
		//for( fileptr; while fileptr != null; fileptr=fileptr->next)
		//{
		//	fprintf(f, "\t\t{\"%s\" : %d}, \n", fileptr->filename, fileptr->freq)
		//}
		//fprintf(f, "\t]}, \n")
		//
	//}
	//
	//

	fprintf(f, "]}");
		

	return(0);

}
