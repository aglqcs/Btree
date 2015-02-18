#include "def.h"
extern void strtolow(char *);
extern struct PageHdr *FetchPage(PAGENO Page);
extern PAGENO treesearch_page(PAGENO PageNo, char *key);
extern void printKey(struct KeyRecord *p);
extern int FreePage(struct PageHdr *PagePtr);
extern int FindInsertionPosition(struct KeyRecord *KeyListTraverser, char *Key,
                          int *Found, NUMKEYS NumKeys, int Count);
/*
	return values
		-1  : when enter a error page or invalid record
		k   : when get requested k records cloud be less than k
*/

int get_successors(char *key, int k, char *result[]) {
	struct PageHdr *PagePtr = NULL;
	struct KeyRecord *record = NULL;
	int Found;
	int Count = 0;
	int i;
	int print_results = 0;
	
	int pos;
	strtolow(key);

	PAGENO search_result = treesearch_page(ROOT, key);
	if(search_result < 0){
		/* is this correct? */
		FreePage(PagePtr);
		return -1;
	}
	PagePtr = FetchPage(search_result);
	record = PagePtr->KeyListPtr;
	if(PagePtr == NULL || record == NULL){
		/* is this correct? */
		FreePage(PagePtr);
		return -1;
	}
	pos = FindInsertionPosition(record, key, &Found,PagePtr->NumKeys,Count);
	int looptime = 0;
	while(print_results < k && print_results >= 0){
                  for(i = 0;i < pos ;i ++){
               	 /* after this loop the record will point to the next of record*/
                       	record = record->Next;
			if( record == NULL ){
			/* if the pos in the next leaf page */
				PagePtr = FetchPage(PagePtr->PgNumOfNxtLfPg);
				record = PagePtr->KeyListPtr;
				/* if still NULL means reach the end of the tree*/
				if(record == NULL ){
					break;
				}
			}
                   }
		if(record == NULL){
			printf("exit while loop at %d pos = %d numkeys = %d\n",++looptime,pos,PagePtr->NumKeys);
			break;	
		}
	        while(record != NULL){
			result[print_results] = (char *)malloc(strlen(record->StoredKey) + 1);
			strcpy(result[print_results],record->StoredKey);
			record = record->Next;
                        print_results ++;
                        if(k == print_results){ 
				break;
			}
                }
		if( k != print_results){
			search_result = PagePtr->PgNumOfNxtLfPg;
			if(search_result < 0){
				break;
			}
			PagePtr = FetchPage(search_result);
			record = PagePtr->KeyListPtr;
	       		pos = FindInsertionPosition(record, key, &Found,PagePtr->NumKeys,Count);
		}
	}	
	FreePage(PagePtr);
	return print_results;
}
