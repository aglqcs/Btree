#include "def.h"
                        
extern PAGENO FindNumPagesInTree(void);
extern PAGENO FindPageNumOfChild(struct PageHdr *PagePtr, struct KeyRecord *KeyListPtr, char *key,
                                              NUMKEYS NumKey);
extern struct PageHdr *FetchPage(PAGENO Page);
extern PAGENO treesearch_page(PAGENO PageNo, char *key);
extern int FreePage(struct PageHdr *PagePtr);


/* just modify the tree_search function to get current page's father*/

void find_father_rec(PAGENO *fatherpage,PAGENO PageNo, char *key,PAGENO current) {
	if(PageNo < 1 && PageNo > FindNumPagesInTree()){
                *fatherpage = -1;        
		 return;
	}
    struct PageHdr *PagePtr = FetchPage(PageNo);
    if (PageNo == current) { /* found leaf */
        return;
    } else if ( PagePtr->NumKeys == 0) {
        /* keys, if any, will be stored in Page# 2
           THESE PIECE OF CODE SHOULD GO soon! **/
        return find_father_rec(fatherpage,FIRSTLEAFPG, key,current);
    } else if ((PagePtr->NumKeys > 0)) {
        PAGENO ChildPage = FindPageNumOfChild(PagePtr, PagePtr->KeyListPtr, key,
                                              PagePtr->NumKeys);
        *fatherpage = PagePtr->PgNum;
        return find_father_rec(fatherpage,ChildPage, key,current);
    } else {
        assert(0 && "this should never happen");
    }
    FreePage(PagePtr);
    return;
}

PAGENO find_father(PAGENO current, char *key){
        PAGENO result;
        find_father_rec(&result,ROOT,key,current);
        return result;
}
