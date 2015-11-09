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
	return strcmp(b->token,a->token);
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
SortedListPtr getTokensForFile(char* file_name, SortedListPtr tokens)
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
	
	char * delimiter = " \n\t.-\\+)(][}{,!?$@#^&*=_";


	while(!feof(fp))
	{
		char *input=(char *)malloc(sizeof(char)*255);
		fgets(input, 255, fp); 
		// printf("Input is %s\n",input );

		char *output = strtok(input, delimiter);
		//THESE LINES GIVE A SEGFAULT
		/*int i;
		for(i = 0; output[i]; i++)
		{
			output[i]=tolower(output[i]);
		}*/	

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
				//need to remove it and reinsert to insert in order
				SLRemove(thisToken->sources,thisSource);
				thisSource->frequency++;
				SLInsert(thisToken->sources, thisSource);
				//head=insert(head, output);
			}	
			// printf("just inserted: %s\n", output);
			output = strtok(NULL, delimiter);
			 
		}
	}
	

	// printf("SORTED WORDS\n");	
	// SortedListIteratorPtr token_walker=SLCreateIterator(tokens);
	// Token* t;
	// while(t=SLGetItem(token_walker),t!=NULL)
	// {
	// 	printf("Token: %s\n",t->token );
	// 	displaySources(t);
	// 	SLNextItem(token_walker);
	// }


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
	return tokens;
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
		// printf("Successfully opened directory: %s \n",directory_name);
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
				// printf("Directory name = %s \n",dirdetails->d_name);
				// printf("Path is: %s\n",path);
				// printf("Path is: %s\n",path);
				tokens_ptr=getContents(path,tokens_ptr);
			}
			else
			{
				// printf("%s is a file in directory %s. Perform actions here! \n", dirdetails->d_name,directory_name	);
				tokens_ptr =getTokensForFile(path,tokens_ptr);
			}	
		}
	}
	return tokens_ptr;
}

int main(int argc, char **argv)
{
	//assuming that argument is the name of a directory
	SortedListPtr tokens=NULL;
	// printf("Starting the program\n");
	tokens=getContents(argv[1],tokens);
	if(tokens==NULL)
		printf("tokens is null\n");
	// printf("SORTED WORDS\n");	
	


	SortedListIteratorPtr token_walker=SLCreateIterator(tokens);
	Token* t;
		
	FILE *f = fopen("file.txt", "w");
	
	fprintf(f, "{\"list\" : [ \n");
	t=SLGetItem(token_walker);
	while(t!=NULL)
	{
		fprintf(f,"\t{\"%s\" : [\n", t->token);

		SortedListIteratorPtr source_walker= SLCreateIterator(t->sources);
		Source *s = SLGetItem(source_walker);
		while(s!=NULL)
		{
			fprintf(f,"\t\t{\"%s\" : %d", s->path, s->frequency);
			SLNextItem(source_walker);
			s = SLGetItem(source_walker);
			if(s!=NULL)
				fprintf(f,"},\n");
			else
				fprintf(f,"}\n");
		}
			
		SLNextItem(token_walker);
		t=SLGetItem(token_walker);
		if(t!=NULL)
			fprintf(f,"\t]},\n");
		else
			fprintf(f,"\t]}\n");
	}
	
	fprintf(f, "]}");	
	return 0;
}


