#include <stdio.h>
#include <string.h>
#include "def.h"
extern PAGENO FindNumPagesInTree(void);
extern PAGENO find_father(PAGENO current, char *key);
extern struct PageHdr *FetchPage(PAGENO Page);
extern PAGENO treesearch_page(PAGENO PageNo, char *key);
extern void printKey(struct KeyRecord *p);
extern int FreePage(struct PageHdr *PagePtr);
extern int FindInsertionPosition(struct KeyRecord *KeyListTraverser, char *Key,
                          int *Found, NUMKEYS NumKeys, int Count);
extern PAGENO FindPageNumOfChild(struct PageHdr *PagePtr, struct KeyRecord *KeyListPtr, char *key,
                                              NUMKEYS NumKey);

char **temp; //global variables to store search result
char current_key[MAXWORDSIZE];
PAGENO current_page = 0;
int result_count = 0;
int result_max_count;


void exit_function(){
	current_page = 0;
	result_count = 0;
	memset(current_key,0,MAXWORDSIZE);
}

void print_page(struct PageHdr *PagePtr){
	printf("-----PAGE START-----\n");
	struct KeyRecord *record = PagePtr->KeyListPtr;
	printf("%c :",PagePtr->PgTypeID);
	while(record != NULL){
		printf("%s  ",record->StoredKey);
		record = record->Next;
	}
	printf("\n--------END--------\n");
}
int write_list(int max_length,char **dest, struct PageHdr *PagePtr, char *endwords){
	if(PagePtr->PgTypeID == 'N'){
		printf("error: should not read a non-leaf page\n");
		return -1;
	}
	if(current_page == PagePtr->PgNum)
		return 0;
	int current_length = 0;
	struct KeyRecord *start = PagePtr->KeyListPtr;
	temp = (char **)malloc(max_length * sizeof(char *));
	for(int i = 0;i < max_length;i ++){
		temp[i] = (char *)malloc(MAXWORDSIZE);
	}
	struct KeyRecord *p = start;
	int index = 0;
	while( strcmp(p->StoredKey,endwords) != 0){
		strcpy(temp[index],p->StoredKey);
		index = (index + 1) % max_length;
		current_length = (current_length == max_length) ? max_length : (current_length + 1);
		p = p->Next;
		if(p == NULL){
			PAGENO next = PagePtr->PgNumOfNxtLfPg;
			PagePtr = FetchPage(next);
			p = PagePtr->KeyListPtr;
		}
	}
	        strcpy(current_key,start->StoredKey);
        for(int i = 0; i < current_length; i++){
                strcpy(dest[result_count + i],temp[current_length - 1 - i]);
        }
        result_count += current_length;
	for(int i = 0;i < max_length;i ++){
		free(temp[i]);
	}
	free(temp);
	return current_length;
}
PAGENO my_treesearch_page(PAGENO *fatherpage,PAGENO PageNo, char *key) {
    PAGENO result;
    struct PageHdr *PagePtr = FetchPage(PageNo);
    if (IsLeaf(PagePtr)) { /* found leaf */
        result = PageNo;
    } else if ((IsNonLeaf(PagePtr)) && (PagePtr->NumKeys == 0)) {
        result = my_treesearch_page(fatherpage,FIRSTLEAFPG, key);
    } else if ((IsNonLeaf(PagePtr)) && (PagePtr->NumKeys > 0)) {
        PAGENO ChildPage = FindPageNumOfChild(PagePtr, PagePtr->KeyListPtr, key,
                                              PagePtr->NumKeys);
	*fatherpage = PagePtr->PgNum;
        result = my_treesearch_page(fatherpage,ChildPage, key);
    } else {
        assert(0 && "this should never happen");
    }
    FreePage(PagePtr);
    return result;
}


int get_predecessors(char *key, int k, char *result[]) {
	struct PageHdr *PagePtr = NULL;
        struct KeyRecord *record = NULL;
	PAGENO father = 0;
	int length = 0;
	result_max_count = k;
	int ret;
	PAGENO bound = FindNumPagesInTree();
	/* get the first leaf of result */
	PAGENO search_result = my_treesearch_page(&father,ROOT,key);
	if(search_result < 1 && search_result > bound){
		 FreePage(PagePtr);
	       	 ret = result_count;
       		 exit_function();
       		 return ret;
	}
	PagePtr = FetchPage(search_result);
	record = PagePtr->KeyListPtr;	
	length = write_list(k,result,PagePtr,key);
	if(length == 0) goto ret;
	current_page = FetchPage(father)->PgNum;
	while(result_count < k ){
		father = find_father(current_page,current_key);
		/* this is a non-leaf page */
		if(father < 1 && father > bound){
                	 FreePage(PagePtr);
               		 ret = result_count;
                	 exit_function();
                	 return ret;
       		}
		PagePtr = FetchPage(father);
		current_page = PagePtr->PgNum;
		record = PagePtr->KeyListPtr;
		/* will get the leaf which stores a word smaller than key*/
		while( PagePtr->PgTypeID != 'L'){
			PagePtr = FetchPage(record->PgNum);
			record = PagePtr->KeyListPtr;
		}
		record = PagePtr->KeyListPtr;
		length = write_list(k - result_count,result, PagePtr, current_key);
		if(length == 0) goto ret;
	}

ret:
       	FreePage(PagePtr);
	ret = result_count;
	exit_function();
        return ret;
}
