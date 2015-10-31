#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

void getContents(char * directory_name)
{
	DIR * directory;
	struct dirent* dirdetails;
	if(directory=opendir(directory_name),directory==NULL)
	{
		printf("Could not open directory %s\n",directory_name );
	}
	else 
	{
		printf("Successfully opened directory: %s \n",directory_name);
		while(dirdetails=readdir(directory))
		{
			if(dirdetails->d_type==DT_DIR)
			{
				if(strcmp(dirdetails->d_name,".")==0||strcmp(dirdetails->d_name,"..")==0)
				{
					// printf("Skipping directory\n");
					continue;
				}
				printf("Directory name = %s \n",dirdetails->d_name);
				char * path=(char*)malloc(sizeof(char)*(strlen(dirdetails->d_name)+strlen(directory_name)+2));
				strcpy(path, directory_name);
				// printf("Path is: %s\n",path);
				strcat(path,"/");
				strcat(path,dirdetails->d_name);
				// printf("Path is: %s\n",path);
				getContents(path);
			}
			else
			{
				printf("%s is a file in directory %s. Perform actions here! \n", dirdetails->d_name,directory_name);
			}	
		}
	}
}

int main(int argc, char **argv)
{
	//assuming that argument is the name of a directory
	getContents(argv[1]);
	// DIR * directory;
	// struct dirent* dirdetails;
	// if(directory=opendir(argv[1]),directory==NULL)
	// {
	// 	printf("Could not open directory %s\n",argv[1] );
	// }
	// else 
	// {
	// 	printf("Successfully opened directory!\n");
	// 	while(dirdetails=readdir(directory))
	// 	{
	// 		if(dirdetails->d_type==DT_DIR)
	// 		{
	// 			if(strcmp(dirdetails->d_name,".")==0||strcmp(dirdetails->d_name,"..")==0)
	// 			{
	// 				printf("Skipping directory\n");
	// 				continue;
	// 			}
	// 			printf("Directory name = %s \n",dirdetails->d_name);

	// 		}	
			
	// 		printf("%s are the contents \n", dirdetails->d_name);
	// 	}
	// }
	return 0;
}

