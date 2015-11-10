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
		return strcmp(b->path,a->path);
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
	
	

	FILE *fp = fopen(file_name, "r");

	if(fp==NULL)
	{
		printf("ERROR\n");
		return(0);
	}
	
	char * delimiter = " \n\t\r\f\v.-\\+)(][}{,!?$@#^&*=_%%^/";


	while(!feof(fp))
	{
		char *input=(char *)malloc(sizeof(char)*255);
		fgets(input, 255, fp); 
		

		char *output2 = strtok(input, delimiter);
		//THESE LINES GIVE A SEGFAULT? Supposed to convert to lowercase.
		// char *output=(char*)malloc((strlen(output2)+1)*sizeof(char));
		// int i;
		// for(i = 0; output2[i]; i++)
		// {
		// 	output[i]=tolower(output2[i]);
		// }
		// output[i]='\0';
		// printf("Converted to lower case. %s \n",output);	
		// char* output;
		while(output2 != NULL)
		{
			if(strlen(output2)==0)
				continue;
			char *output=(char*)malloc((strlen(output2)+1)*sizeof(char));
			int i;
			for(i = 0; output2[i]; i++)
			{
				output[i]=tolower(output2[i]);
			}
			output[i]='\0';
			printf("Converted to lower case. %s \n",output);
			if(isalpha(output[0]))
			{
				
				SortedListIteratorPtr tokens_iterator=SLCreateIterator(tokens);
				
				Token* thisToken = lookup_token(output, tokens_iterator);
				
				if(thisToken==NULL)
				{
					
					thisToken=(Token*)malloc(sizeof(Token));
					thisToken->token=(char*)malloc(strlen(output)*sizeof(char));
					strcpy(thisToken->token,output);
					
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
				
			}	
			output2 = strtok(NULL, delimiter);
			 
		}
	}
	

	

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
					
					continue;
				}
				
				tokens_ptr=getContents(path,tokens_ptr);
			}
			else
			{
				
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
	char * filename = argv[1];
	
	tokens=getContents(argv[2],tokens);
	if(tokens==NULL)
		printf("tokens is null\n");
		
	


	SortedListIteratorPtr token_walker=SLCreateIterator(tokens);
	Token* t;
		
	FILE *f = fopen(filename, "w");
	
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



