#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include "sorted-list.h"

typedef struct Token{
	char *token;
	SortedListPtr sources;
} Token;

typedef struct Source{
	char *path;
	int frequency;
} Source;

int compareTokens(void * x,void* y){
	Token* a= (Token*)x;
	Token* b= (Token*)y;
	return strcmp(a->token,b->token);
}

int compareSources(void * x,void * y){
	Source* a= (Source*) x;
	Source* b= (Source*) y;
	if(a->frequency<b->frequency)
		return -1;
	else if(a->frequency>b->frequency)
		return 1;
	else
		return strcmp(a->path,b->path);
}

void destroyToken(void* x){
	Token* a=(Token*)x;
	free(a);
}

void destroySource(void * x){
	Source * a=(Source*) x;
	free(a);
}

Token* lookup_token(char *token, SortedListIteratorPtr walker)
{
	if(SLGetItem(walker)==NULL)
		return NULL;
	if(strcmp(((Token*)(SLGetItem(walker)))->token,token)==0)
		return (Token*)(SLGetItem(walker));
	while(SLNextItem(walker)!=NULL){
		if(strcmp(((Token*)(SLGetItem(walker)))->token,token)==0)
			return (Token*)(SLGetItem(walker));
	}
	return NULL;
}

Source* lookup_source(char *file_name, SortedListIteratorPtr walker)
{
	if(SLGetItem(walker)==NULL)
		return NULL;
	if(strcmp(((Source*)(SLGetItem(walker)))->path,file_name)==0)
		return (Source*)(SLGetItem(walker));
	while(SLNextItem(walker)!=NULL){
		if(strcmp(((Source*)(SLGetItem(walker)))->path,file_name)==0)
			return (Source*)(SLGetItem(walker));
	}
	return NULL;
}

void displaySources(Token * t)
{
	SortedListIteratorPtr source_walker= SLCreateIterator(t->sources);
	Source *s;
	while(s=SLGetItem(source_walker),s!=NULL)
	{
		printf(" Source: %s frequency: %d \n",s->path, s->frequency );
		SLNextItem(source_walker);
	}
}

//assuming file_name is entire path of file
int getTokensForFile(char* file_name, SortedListPtr tokens)
{
	if(tokens==NULL)
	{
		tokens=SLCreate(&compareTokens,&destroyToken);
	}
	
	// Node *head = (Node *)malloc(sizeof(Node));
	// head->prev=NULL;
	// head->next=NULL;
	// head->word=NULL;
	// head->freq=1;

	FILE *fp = fopen(file_name, "r");

	if(fp==NULL)
	{
		printf("ERROR\n");
		return(0);
	}
	
	char * delimiter = " \n\t";


	while(!feof(fp))
	{
		char *input=(char *)malloc(sizeof(char)*255);
		fgets(input, 255, fp); 
		// printf("Input is %s\n",input );
		char *output = strtok(input, delimiter);
		while(output != NULL)
		{
			if(isalpha(output[0]))
			{
				// printf("token is %s \n",output); 
				SortedListIteratorPtr tokens_iterator=SLCreateIterator(tokens);
				// printf("Iterator for tokens created\n"); 
				Token* thisToken = lookup_token(output, tokens_iterator);
				//printf("Hi\n");
				if(thisToken==NULL)
				{
					//add new entry for token
					// printf("Adding new token to tokens SL\n");
					thisToken=(Token*)malloc(sizeof(Token));
					thisToken->token=(char*)malloc(strlen(output)*sizeof(char));
					strcpy(thisToken->token,output);
					// printf("String copied to token \n");
					SortedListPtr sources=SLCreate(&compareSources,destroySource);
					thisToken->sources=sources;
					SLInsert(tokens, thisToken);

				}
				//check if source is present in the token's sources
				SortedListIteratorPtr sources_iterator=SLCreateIterator(thisToken->sources); 
				Source* thisSource = lookup_source(file_name, sources_iterator);
				if(thisSource==NULL)
				{
					thisSource= (Source*)malloc(sizeof(Source));
					thisSource->path=file_name;
					thisSource->frequency=0;
					SLInsert(thisToken->sources, thisSource);
				}
				thisSource->frequency++;
				//head=insert(head, output);
			}	
			// printf("just inserted: %s\n", output);
			output = strtok(NULL, delimiter);
			 
		}
	}
	

	printf("SORTED WORDS\n");	
	SortedListIteratorPtr token_walker=SLCreateIterator(tokens);
	Token* t;
	while(t=SLGetItem(token_walker),t!=NULL)
	{
		printf("Token: %s\n",t->token );
		displaySources(t);
		SLNextItem(token_walker);
	}


	// Node *tmp=head;
	// while(tmp!=NULL)
	// {
		// printf("%s \tfreq: %d \n", tmp->word, tmp->freq);
		// tmp=tmp->next;
	// }	
	

	// lookup(head, "b");
	// lookup(head, "c");
	// lookup(head, "g");

	fclose(fp);		
	return(0);
}
SortedListPtr getContents(char * directory_name, SortedListPtr tokens_ptr)
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
				getContents(path,tokens_ptr);
			}
			else
			{
				// printf("%s is a file in directory %s. Perform actions here! \n", dirdetails->d_name,directory_name	);
				getTokensForFile(dirdetails->d_name,tokens_ptr);
			}	
		}
	}
	return tokens_ptr;
}

int main(int argc, char **argv)
{
	//assuming that argument is the name of a directory
	SortedListPtr tokens=NULL;
	printf("Starting the program\n");
	getContents(argv[1],tokens);
	return 0;
}

// struct Node_ {

// 	struct Node_ *next;
// 	struct Node_ *prev;
// 	char *word;	
// 	int freq;
// };

// typedef struct Node_ Node;

// Node *delete(Node *head, char *target)
// {
// 	Node *tmp=head;
// 	Node *prev=NULL;
	
// 	while(tmp!=NULL && strcmp(target, tmp->word)!=0)
// 	{
// 		prev=tmp;
// 		tmp=tmp->next;
// 	}

// 	if(tmp==NULL)
// 		return head;
// 	else if(tmp==head)
// 	{
// 		tmp = tmp->next;
// 		free(head);
// 		return tmp;
// 	}
// 	else
// 	{
// 		prev->next=tmp->next;
// 		if(tmp->next!=NULL)
// 			tmp->next = prev;
// 		free(tmp);
// 	}
	
// }

// Node *insert(Node *head, char *input)
// {
// 	input[0]=tolower(input[0]);

// 	if(head->word==NULL)
// 		head->word=input;
// 	else
// 	{
// 		Node *tmp = head;
// 		Node *prev = NULL;

// 		while(tmp->next!=NULL && tmp->freq>1)
// 		{
// 			if(strcmp(input, tmp->word)==0)
// 				goto same;
// 			tmp=tmp->next;
// 		}

// 		while(tmp->next!=NULL && strcmp(input, tmp->word)>0)
// 		{
// 			prev=tmp;
// 			tmp=tmp->next;
// 		}

// 		if(strcmp(input, tmp->word)<0)
// 		{
// 			Node *new = (Node *)malloc(sizeof(Node));
// 			new->word = input;
// 			new->next=tmp;
// 			new->prev=prev;
// 			new->freq=1;

// 			if(tmp==head)
// 			{
// 				tmp->prev=new;
// 				head=new;
// 			}
// 			else
// 			{
// 				prev->next=new;
// 				tmp->prev=new;
// 			}
// 		}
// 		else if (strcmp(input, tmp->word)==0)
// 		{
// 			same:

// 			tmp->freq++;

// 			if(tmp!=head && tmp->prev->freq < tmp->freq)
// 			{
// 				Node *freqcheck = head;
// 				//Node *holding = freqcheck->prev;

// 				while(freqcheck->freq > tmp->freq)
// 					freqcheck=freqcheck->next;

// 				if(freqcheck->freq==tmp->freq)
// 				{
// 					while(strcmp(input, freqcheck->word)>0 && freqcheck->freq==tmp->freq)
// 						freqcheck=freqcheck->next;

// 				}

// 				Node *holding = freqcheck->prev;

// 				if(freqcheck->prev!=NULL)
// 					freqcheck->prev->next=tmp;
// 				freqcheck->prev=tmp;

// 				if(tmp->prev!=NULL)
// 					tmp->prev->next=tmp->next;
// 				if(tmp->next!=NULL)
// 					tmp->next->prev=tmp->prev;
					
// 				tmp->prev=holding;
// 				tmp->next=freqcheck;

// 				if(freqcheck==head)
// 					head=tmp;
// 			}			
			
// 		}	
// 		else
// 		{
// 			Node *new = (Node *)malloc(sizeof(Node));
// 			new->word=input;
// 			tmp->next=new;
// 			new->prev=tmp;
// 			new->next=NULL;
// 			new->freq=1;
// 		}
// 	}
// 	return head;
// }	

// void lookup(Node *head, char *find)
// {

// 	Node *tmp = head;
// 	while(tmp!=NULL && strcmp(find, tmp->word)!=0)
// 		tmp=tmp->next;

// 	if(tmp!=NULL)
// 		printf("Word %s is in list.\n", tmp->word);
// 	else
// 		printf("Word %s is not in list.\n", find);

// }

// void getContents(char * directory_name)
// {
// 	DIR * directory;
// 	struct dirent* dirdetails;
// 	if(directory=opendir(directory_name),directory==NULL)
// 	{
// 		printf("Could not open directory %s\n",directory_name );
// 	}
// 	else 
// 	{
// 		printf("Successfully opened directory: %s \n",directory_name);
// 		while(dirdetails=readdir(directory))
// 		{
// 			char * path=(char*)malloc(sizeof(char)*(strlen(dirdetails->d_name)+strlen(directory_name)+2));
// 			strcpy(path, directory_name);
// 			strcat(path,"/");
// 			strcat(path,dirdetails->d_name);
// 			if(dirdetails->d_type==DT_DIR)
// 			{
// 				if(strcmp(dirdetails->d_name,".")==0||strcmp(dirdetails->d_name,"..")==0)
// 				{
// 					// printf("Skipping directory\n");
// 					continue;
// 				}
// 				printf("Directory name = %s \n",dirdetails->d_name);
// 				// printf("Path is: %s\n",path);
// 				// printf("Path is: %s\n",path);
// 				getContents(path);
// 			}
// 			else
// 			{
// 				printf("%s is a file in directory %s. Perform actions here! \n", dirdetails->d_name,directory_name	);
// 				if(dirdetails->d_type==DT_REG)
// 				{
// 					// printf("Its a regular file.\n");
// 					//getting contents of text file.
// 					int fd=open(path,O_RDONLY);
// 					if(fd!=-1)
// 					{
// 						int length = lseek(fd, 0L, SEEK_END) + 1;
// 						// printf("Length of file %s is %d\n",path, length );
// 						lseek(fd, 0L, SEEK_SET);
// 						char *file_text= (char*)malloc(length*sizeof(char));
// 						read(fd,file_text,length);
// 						// printf("file contents: %s \n",file_text);
// 						//call tokenizer and pass file_text to it!


// 					}
// 					else
// 					{
// 						printf("Could not open file %s\n", dirdetails->d_name);
// 					}
// 				}
// 			}	
// 			free(path);
// 		}
// 	}
// }
// void tokenizer(Node_* root, char *file_text)
// {

// }
// char * delimiter = " \n\t";
// 						char *output = strtok(file_text, delimiter);
// 						while(output != NULL)
// 						{
// 							if(isalpha(output[0]))
// 							head=insert(head, output);
// 							printf("just inserted: %s\n", output);
// 							output = strtok(NULL, delimiter);
// 						}

// int main(int argc, char **argv)
// {
// 	//assuming that argument is the name of a directory
// 	getContents(argv[1]);
// 	return 0;
// }

