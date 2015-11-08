//This is the tokenizer, with a main method just for testing purposes. It reads from a given file and creates a LL of tokens
//ordered by their frequency and then by alphabetical. Also has basic delete and lookup functions. 
//It's not 100% done however, some bigger input seems to break it so I'll look into that. -Ethan 12:48AM

//Updated again with some small fixes. Still stomping out bugs though. -Ethan 9:45AM

//A few more bugs squashed, note the way that I read input from the file with fgets and tokenize it with strtok(), 
// that's somewhat important. It ensures my functions are only recieving strings, no punctuation or spaces or any of that.
//Gonna take a break for a little bit, be back later. -Ethan 10:46
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


struct Node_ {

	struct Node_ *next;
	struct Node_ *prev;
	char *word;	
	int freq;
};

typedef struct Node_ Node;

Node *delete(Node *head, char *target)
{
	Node *tmp=head;
	Node *prev=NULL;
	
	while(tmp!=NULL && strcmp(target, tmp->word)!=0)
	{
		prev=tmp;
		tmp=tmp->next;
	}

	if(tmp==NULL)
		return head;
	else if(tmp==head)
	{
		tmp = tmp->next;
		free(head);
		return tmp;
	}
	else
	{
		prev->next=tmp->next;
		if(tmp->next!=NULL)
			tmp->next = prev;
		free(tmp);
	}
	
}

Node *insert(Node *head, char *input)
{
	input[0]=tolower(input[0]);

	if(head->word==NULL)
		head->word=input;
	else
	{
		Node *tmp = head;
		Node *prev = NULL;

		while(tmp->next!=NULL && tmp->freq>1)
		{
			if(strcmp(input, tmp->word)==0)
				goto same;
			tmp=tmp->next;
		}

		while(tmp->next!=NULL && strcmp(input, tmp->word)>0)
		{
			prev=tmp;
			tmp=tmp->next;
		}

		if(strcmp(input, tmp->word)<0)
		{
			Node *new = (Node *)malloc(sizeof(Node));
			new->word = input;
			new->next=tmp;
			new->prev=prev;
			new->freq=1;

			if(tmp==head)
			{
				tmp->prev=new;
				head=new;
			}
			else
			{
				prev->next=new;
				tmp->prev=new;
			}
		}
		else if (strcmp(input, tmp->word)==0)
		{
			same:

			tmp->freq++;

			if(tmp!=head && tmp->prev->freq <= tmp->freq)
			{
				Node *freqcheck = head;
				//Node *holding = freqcheck->prev;

				while(freqcheck->freq > tmp->freq)
					freqcheck=freqcheck->next;

				if(freqcheck->freq==tmp->freq)
				{
					while(strcmp(input, freqcheck->word)>0 && freqcheck->freq==tmp->freq)
						freqcheck=freqcheck->next;

				}

				Node *holding = freqcheck->prev;

				if(freqcheck->prev!=NULL)
					freqcheck->prev->next=tmp;
				freqcheck->prev=tmp;

				if(tmp->prev!=NULL)
					tmp->prev->next=tmp->next;
				if(tmp->next!=NULL)
					tmp->next->prev=tmp->prev;
					
				tmp->prev=holding;
				tmp->next=freqcheck;

				if(freqcheck==head)
					head=tmp;
			}			
			
		}	
		else
		{
			Node *new = (Node *)malloc(sizeof(Node));
			new->word=input;
			tmp->next=new;
			new->prev=tmp;
			new->next=NULL;
			new->freq=1;
		}
	}
	return head;
}	

void lookup(Node *head, char *find)
{

	Node *tmp = head;
	while(tmp!=NULL && strcmp(find, tmp->word)!=0)
		tmp=tmp->next;

	if(tmp!=NULL)
		printf("Word %s is in list.\n", tmp->word);
	else
		printf("Word %s is not in list.\n", find);

}


int main(int argc, char **argv)
{

	Node *head = (Node *)malloc(sizeof(Node));
	head->prev=NULL;
	head->next=NULL;
	head->word=NULL;
	head->freq=1;

	FILE *fp = fopen(argv[1], "r");

	if(fp==NULL)
	{
		printf("ERROR\n");
		return(0);
	}
	
	char * delimiter = " \n\t,'-:.";


	while(!feof(fp))
	{
		char *input=(char *)malloc(sizeof(char)*255);
		fgets(input, 255, fp); 

		char *output = strtok(input, delimiter);
		while(output != NULL)
		{
			if(isalpha(output[0]))
				head=insert(head, output);
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

