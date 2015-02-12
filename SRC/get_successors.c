#include "def.h"
extern struct PageHdr *FetchPage(PAGENO Page);
extern PAGENO treesearch_page(PAGENO PageNo, char *key);
extern void printKey(struct KeyRecord *p);
extern int FreePage(struct PageHdr *PagePtr);
extern int FindInsertionPosition(struct KeyRecord *KeyListTraverser, char *Key,
                          int *Found, NUMKEYS NumKeys, int Count);
/*
	return values
		-1  : when enter a error page or invalid record
		k   : when get exactly requested k records
		k+1 : when get less than requested k records
*/

int get_successors(char *key, int k, char *result[]) {
    	result = NULL; //TODO what is this result mean?
	struct PageHdr *PagePtr = NULL;
	struct KeyRecord *record = NULL;
	int Found;
	int Count = 0;
	int i;
	int print_results = 0;

	int pos;

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
	while(print_results < k && print_results >= 0){
		if(Found == TRUE){
                 	 for(i = 0;i < pos && record != NULL;i ++){
               		 /* after this loop the record will point to the next of record*/
                       		 record = record->Next;
                  	 }
                }
		if(record == NULL){
			print_results = k + 1;
			break;	
		}
		//TODO else Found == FALSE , if there is no this record.
	         while(record != NULL){
                        printKey(record);
                        record = record->Next;
                        print_results ++;
                        if(k == print_results){ 
				break;
			}
                }
		if( k != print_results){
			search_result = PagePtr->PgNumOfNxtLfPg;
			if(search_result < 0){
				print_results = k + 1;
				break;
			}
			PagePtr = FetchPage(search_result);
			record = PagePtr->KeyListPtr;
	       		pos = FindInsertionPosition(record, key, &Found,PagePtr->NumKeys,Count);
		}
	}	
	FreePage(PagePtr);
	return 0;
}
