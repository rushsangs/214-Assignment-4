all: index

index: Indexer.c sorted-list.c
	gcc -o index Indexer.c sorted-list.c -I.

clean:
	rm index 
