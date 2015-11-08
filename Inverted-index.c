#include "sorted-list.h"
#include<stdio.h>

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
	TOken* b= (Token*)y;
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
		return strmp(a->path,b->path);
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
	if(strcmp((Token*)(SLGetItem(walker))->token,token)==0)
		return (Token*)(SLGetItem(walker));
	while(SLNextItem(walker)!=NULL){
		if(strcmp((Token*)(SLGetItem(walker))->token,token)==0)
			return (Token*)(SLGetItem(walker));
	}
	return NULL;
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

	FILE *fp = fopen(argv[1], "r");

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

		char *output = strtok(input, delimiter);
		while(output != NULL)
		{
			if(isalpha(output[0]))
			{
				SortedListIteratorPtr tokens_iterator=SLCreateIterator(tokens); 
				Token* thisToken = lookup_token(tokens_iterator);
				if(thisToken==NULL)
				{
					//add new entry for token
					thisToken=(Token*)malloc(sizeof(Token));
					thisToken->token=output;
					SortedListPtr sources=SLCreate(&compareSources,destroySource);
					thisToken->sources=sources;
				}
				//check if source is present in the token's sources
				SortedListIteratorPtr sources_iterator=SLCreateIterator(sources); 
				head=insert(head, output);
			}	
			printf("just inserted: %s\n", output);
			output = strtok(NULL, delimiter);
			 
		}
	}
	

	printf("SORTED WORDS\n");	
	Node *tmp=head;
	while(tmp!=NULL)
	{
		printf("%s \tfreq: %d \n", tmp->word, tmp->freq);
		tmp=tmp->next;
	}	
	

	lookup(head, "b");
	lookup(head, "c");
	lookup(head, "g");

	fclose(fp);		
	return(0);
}




////////////////////////////////////////////////////// EMPLOYEe EXAMPLE FOR REFERENCE //////////////////////////////////////////
// int main()
// {
// 	Employee *a=(Employee*)malloc(sizeof(Employee));
// 	a->name="Albert";
// 	a->salary=48000;

// 	Employee *b=(Employee*)malloc(sizeof(Employee));
// 	b->name="Bob";
// 	b->salary=47000;
	
// 	Employee *c=(Employee*)malloc(sizeof(Employee));
// 	c->name="Carter";
// 	c->salary=46000;
	
// 	Employee *d=(Employee*)malloc(sizeof(Employee));
// 	d->name="Dudley";
// 	d->salary=45000;

// 	//////////////////////// INITIALISING THE SORTED LIST ///////////////////////////
// 	SortedListPtr ptr=SLCreate(&compareEmployees, &destroyEmployee);
// 	printf("Creating SortedList...\n");
// 	if(SLInsert(ptr, (void *)b))
// 		printf("Inserted employee with name %s and salary %d \n",b->name,b->salary);
// 	if(SLInsert(ptr, (void *)a))
// 		printf("Inserted employee with name %s and salary %d \n",a->name,a->salary);
// 	if(SLInsert(ptr, (void *)c))
// 		printf("Inserted employee with name %s and salary %d \n",c->name,c->salary);
// 	if(SLInsert(ptr, (void *)d))
// 		printf("Inserted employee with name %s and salary %d \n",d->name,d->salary);
// 	printf("Finished inserting.\n\n");

// 	Employee* firstEmp=(Employee*)(ptr->head->data);
//   	printf("%s is the first employee in the sorted list. \n\n",firstEmp->name);
  	
//   	////////////////////// CREATING ITERATORS //////////////////////////////////////
//   	printf("Creating iterator 1 and 2\n");
// 	SortedListIteratorPtr walker = SLCreateIterator(ptr);
// 	SortedListIteratorPtr walker2 = SLCreateIterator(ptr);


// 	printf("Testing SLGetItem for Iterator 1: %s \n", ((Employee*)(SLGetItem(walker)))->name);

// 	//the walker1 will traverse through the list to the last element
// 	while(walker->current->next!=NULL){
// 		SLNextItem(walker);	
// 		printf("Testing Iterator 1 NextItem: %s \n\n", ((Employee *)(SLGetItem(walker)))->name);
// 	}

// 	///////////////// OPERATIONS ON SORTED LISTS //////////////////////////
// 	printf("Attempting to remove Bob from the list.\n");
// 	if(SLRemove(ptr, (void *)b))
// 		printf("Element removed successfully.\n");
	
// 	//trying to remove from list when iterator points to it
// 	printf("Attempting to remove Dudley from the SortedList (iterator 1 points to it).\n");
// 	SLRemove(ptr, (void *)d);
	
  	
//   	//destroyed the list but an iterator still points to one of the elements.
//   	printf("Attempting to destroy the list. (Note that iterators 1 and 2 are still active.\n\n");
//   	SLDestroy(ptr);
//   	// testing iterators
//   	printf("Iterator 1 is pointing to: %s \n", ((Employee *)(SLGetItem(walker)))->name);
// 	printf("Iterator 2 is pointing to: %s\n",((Employee *)(SLGetItem(walker2)))->name );
// 	printf("CHecking next element for each iterator\n");
// 	if(SLNextItem(walker)==NULL)
// 		printf("Iterator 1 reaches null\n");
// 	if(SLNextItem(walker2)==NULL)
// 		printf("Iterator 2 reaches null\n");
	
// 	SLDestroyIterator(walker);
// 	SLDestroyIterator(walker2);

//   	return 0;
// }





