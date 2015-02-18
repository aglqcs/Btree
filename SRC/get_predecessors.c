#include <stdio.h>
#include <string.h>
#include "def.h"

extern struct PageHdr *FetchPage(PAGENO Page);
extern int FreePage(struct PageHdr *PagePtr);

char current_key[MAXWORDSIZE];
int current_result_count = 0;
int max_result_count = 0;
char **ref = NULL;

void write_list(char *key){
	strcpy(ref[current_result_count],key);
	current_result_count ++;
}
void exit_function(struct PageHdr *PagePtr){
	/* on exit: sort the result list and clear all global variables*/
	for(int i = 0;i < current_result_count; i ++){
		for(int j = i + 1;j < current_result_count;j ++){
			if( strcmp(ref[i], ref[j]) > 0 ){
				char t[MAXWORDSIZE];
				strcpy(t,ref[i]);
				strcpy(ref[i],ref[j]);
				strcpy(ref[j],t);
			}
		}
	}
	current_result_count = 0;
	max_result_count = 0;
	ref = NULL;
	memset(current_key,0,MAXWORDSIZE);
	free(PagePtr);
}

void find(struct KeyRecord *record, struct PageHdr *PagePtr) {
	if (current_result_count  == max_result_count) {
		return;
	}
	/* if this is a leaf page
			if the record go end of the leaf, return
			recursive try next record
			write this record to result
			because first try right record(bigger record), after that write current record.
			The result list will always contain records that more close to the key.
	   if a non-leaf page
			try next leaf
			if end of the leaf, need to try a bigger record, so go to the right most child
			else need to try a smaller record, go to the PageNo saved in first record 
	*/
	struct PageHdr *next_search;
	if( PagePtr->PgTypeID == 'L'){
		if( record == NULL) return;
		else{
			 if(strcmp(current_key, record->StoredKey) > 0){
                       		find(record->Next, PagePtr);
                     		if (current_result_count == max_result_count) {
                               		return;
                       		}
				write_list(record->StoredKey);
               		 }
		}
	}
	else{
		if(record != NULL){	
                        if(strcmp(current_key, record->StoredKey) > 0){
                               	find(record->Next, PagePtr);
                               	if (current_result_count == max_result_count) {
                                       	return;
                               	 }
        	        }
			next_search = FetchPage(record->PgNum);		
		}
		else{
			next_search = FetchPage(PagePtr->PtrToFinalRtgPg);
		}
                find(next_search->KeyListPtr,next_search); 
	}
	return;
}
int get_predecessors(char *key, int k, char *result[]) {
	max_result_count = k;
	ref = result;
	strcpy(current_key,key);
	for(int i = 0;i < max_result_count;i ++){
		ref[i] = (char *)malloc(MAXWORDSIZE);
	}
	/* start from root */
	struct PageHdr *PagePtr = FetchPage(ROOT);	
	if (PagePtr != NULL) {
		struct KeyRecord *record = PagePtr->KeyListPtr;
		find(record, PagePtr);
	}
	/* save return value, then clear all global variables for next time */
	int ret_value = current_result_count;  
	exit_function(PagePtr);
	return ret_value;
}
