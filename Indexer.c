#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
			char * path=(char*)malloc(sizeof(char)*(strlen(dirdetails->d_name)+strlen(directory_name)+2));
			strcpy(path, directory_name);
			strcat(path,"/");
			strcat(path,dirdetails->d_name);
			if(dirdetails->d_type==DT_DIR)
			{
				if(strcmp(dirdetails->d_name,".")==0||strcmp(dirdetails->d_name,"..")==0)
				{
					// printf("Skipping directory\n");
					continue;
				}
				printf("Directory name = %s \n",dirdetails->d_name);
				// printf("Path is: %s\n",path);
				// printf("Path is: %s\n",path);
				getContents(path);
			}
			else
			{
				printf("%s is a file in directory %s. Perform actions here! \n", dirdetails->d_name,directory_name	);
				if(dirdetails->d_type==DT_REG)
				{
					// printf("Its a regular file.\n");
					//getting contents of text file.
					int fd=open(path,O_RDONLY);
					if(fd!=-1)
					{
						int length = lseek(fd, 0L, SEEK_END) + 1;
						// printf("Length of file %s is %d\n",path, length );
						lseek(fd, 0L, SEEK_SET);
						char *file_text= (char*)malloc(length*sizeof(char));
						read(fd,file_text,length);
						// printf("file contents: %s \n",file_text);
						//call tokenizer and pass file_text to it!

					}
					else
					{
						printf("Could not open file %s\n", dirdetails->d_name);
					}
				}
			}	
			free(path);
		}
	}
}

int main(int argc, char **argv)
{
	//assuming that argument is the name of a directory
	getContents(argv[1]);
	return 0;
}

