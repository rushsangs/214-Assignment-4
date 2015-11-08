#include <stdlib.h>
#include <stdio.h>
#include "sorted-list.h"
/*
 * SLCreate creates a new, empty sorted list.  The caller must provide
 * a comparator function that can be used to order objects that will be
 * kept in the list, and a destruct function that gets rid of the objects
 * once they are no longer in the list or referred to in an iterator.
 * 
 * If the function succeeds, it returns a (non-NULL) SortedListT object,
 * otherwise, it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */

SortedListPtr SLCreate(CompareFuncT cf, DestructFuncT df){
	SortedListPtr list=(SortedListPtr)malloc(sizeof(struct SortedList));
	list->comparef=cf;
	list->destructf=df;
	list->head=NULL;
	return list;
}

/*
 * SLDestroy destroys a list, freeing all dynamically allocated memory.
 *
 * You need to fill in this function as part of your implementation.
 */
void SLDestroy(SortedListPtr list)
{
	//traverse through every node in the list, freeing the node
	Node* p=list->head;
	Node* tmp;
	while(p!=NULL)
	{
		tmp=p;
		p=p->next;
		if(tmp->refctr==1)
		{
		//destroy the element
			list->destructf(tmp->data);
			free(tmp);
			printf("Deleted one node.\n");
		}
		else
		{
			//do not destroy it, it has an iterator on it! just make it's next point to null
			tmp->next=NULL;
		}
	}
	//now free the list itself
	free(list);
	printf("List destroyed successfully. \n");
}


/*
 * SLInsert inserts a given object into a sorted list, maintaining sorted
 * order of all objects in the list.  If the new object is equal to an object
 * already in the list (according to the comparator function), then the operation should fail.
 *
 * If the function succeeds, it returns 1, otherwise it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

int SLInsert(SortedListPtr list, void *newObj){
	if(list->head==NULL)
	{
		//insert first element into list
		Node* newnode=(Node*)malloc(sizeof(Node));
		list->head=newnode;
		newnode->data=newObj;
		newnode->refctr=1;
		newnode->next=NULL;
		return 1;
	}
	else
	{
		//traverse through linked list and keep checking order
		Node *p=list->head;
		int result;
		Node* prev;
		while(p!=NULL){
			result=list->comparef(p->data,newObj);
			if(result==0)
			{
				printf("Elements are equal hence not inserted. \n");
				return 0;
			}
			else if(result>0)
			{
				prev=p;
				p=p->next;
			}
			else
			{
				// insert HERE, i.e. link prev->next to newnode and link newnode->next to p 
				Node* newnode=(Node*)malloc(sizeof(Node));
				newnode->data=newObj;
				//if first element in list
				if(p==list->head)
				{
					newnode->refctr=1;
					newnode->next=p;
					list->head=newnode;
				}
				else	//not first hence needs some swapping- ERROR DOES NOT WORK
				{	
					newnode->refctr=1;
					newnode->next=prev->next;
					prev->next=newnode;
				}
				return 1;
			}
		}
		//inserting at the end of linked list, and prev is the last node.
		Node* newnode= (Node*)malloc(sizeof(Node));
		newnode->data=newObj;
		newnode->refctr=1;
		newnode->next=prev->next;
		prev->next=newnode;
		return 1;
	}
	return 0;
}


/*
 * SLRemove removes a given object from a sorted list.  Sorted ordering
 * should be maintained.  SLRemove may not change the object whose
 * pointer is passed as the second argument.  This allows you to pass
 * a pointer to a temp object equal to the object in the sorted list you
 * want to remove.
 *
 * If the function succeeds, it returns 1, otherwise it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

int SLRemove(SortedListPtr list, void *newObj)
{
	if(list->head==NULL)
	{
		printf("Error: List is empty. Cannot remove anything. \n");
		return 0;
	}
			
	Node *tmp = list->head;
	Node *prev = list->head;	
	if(list->comparef(tmp->data, newObj)==0)
	{
		list->head=list->head->next;
		if(tmp->refctr>1)
		{
			printf("You are trying to remove a node which has an iterator pointing to it. Node removed from list but still exists in memory.\n");
			tmp->next=NULL;
		}
		else
		{
			list->destructf(tmp->data);
			free(tmp);
		}
		return 1;
	}	
	tmp=tmp->next;
	while(tmp!=NULL &&list->comparef(tmp->data,newObj)>-1)
	{
		if(list->comparef(tmp->data, newObj)==0)
		{
			prev->next=tmp->next;
			if(tmp->refctr>1)
			{
				printf("You are trying to remove a node which has an iterator pointing to it. Node removed from list but still exists in memory.\n");
				tmp->next=NULL;
			}
			else
			{
				list->destructf(tmp->data);
				free(tmp);
			}
			return 1;
		}
		tmp=tmp->next;
		prev=prev->next;		
	}
	printf("Element not found in list, hence could not be removed from the list.\n");
	return 0;
}


/*
 * SLCreateIterator creates an iterator object that will allow the caller
 * to "walk" through the list from beginning to the end using SLNextItem.
 *
 * If the function succeeds, it returns a non-NULL pointer to a
 * SortedListIterT object, otherwise it returns NULL.  The SortedListT
 * object should point to the first item in the sorted list, if the sorted
 * list is not empty.  If the sorted list object is empty, then the iterator
 * should have a null pointer.
 *
 * You need to fill in this function as part of your implementation.
 */

SortedListIteratorPtr SLCreateIterator(SortedListPtr list)
{
	SortedListIteratorPtr helper = (SortedListIteratorPtr)(malloc(sizeof(struct SortedListIterator)));
	helper->destructf=list->destructf;
	helper->current=list->head;
	helper->current->refctr++;
	return helper;
}


/*
 * SLDestroyIterator destroys an iterator object that was created using
 * SLCreateIterator().  Note that this function should destroy the
 * iterator but should NOT affect the original list used to create
 * the iterator in any way.
 *
 * You need to fill in this function as part of your implementation.
 */

void SLDestroyIterator(SortedListIteratorPtr iter)
{
	if(iter->current!=NULL)
		iter->current->refctr--;
	else if(iter->current!=NULL && iter->current->refctr==0)
	{
		//destroy the node
		iter->destructf(iter->current->data);
		free(iter->current);
	}	
	free(iter);		
	printf("Iterator destroyed\n");
}
/*
 * SLGetItem returns the pointer to the data associated with the
 * SortedListIteratorPtr.  It should return 0 if the iterator
 * advances past the end of the sorted list.
 * 
 * You need to fill in this function as part of your implementation.
*/

void * SLGetItem( SortedListIteratorPtr iter )
{
	if(iter->current==NULL)
	{
		printf("Iterator reached the end.\n");
		return NULL;
	}
	return iter->current->data;

}

/*
 * SLNextItem returns the pointer to the data associated with the
 * next object in the list associated with the given iterator.
 * It should return a NULL when the end of the list has been reached.
 *
 * One complication you MUST consider/address is what happens if a
 * sorted list associated with an iterator is modified while that
 * iterator is active.  For example, what if an iterator is "pointing"
 * to some object in the list as the next one to be returned but that
 * object is removed from the list using SLRemove() before SLNextItem()
 * is called.
 *
 * You need to fill in this function as part of your implementation.
 */

void * SLNextItem(SortedListIteratorPtr iter)
{
	if(iter->current->next!=NULL)
	{	
		iter->current->refctr--;
		if(iter->current->refctr==0)
		{
			//destroy the node
			Node* tmp=iter->current->next;
			iter->destructf(iter->current->data);
			free(iter->current);
			iter->current=tmp;
		}
		else
		{
			iter->current=iter->current->next;
		}
		iter->current->refctr++;
	}
	else
	{
		iter->current->refctr--;
		if(iter->current->refctr==0)
		{	
			//destroy the node
			iter->destructf(iter->current->data);
			free(iter->current);
		}
		iter->current=NULL;
		return NULL;
	}

	return iter->current->data;	
}