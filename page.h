#ifndef UNTITLED3_PAGE_H
#define UNTITLED3_PAGE_H
enum pg{FREE, DIVIDED, MULTIBLOCK};
typedef enum pg status;

typedef struct page{
    status status;
    void *pageStart;
    void *freeBlock;
    int class;
    size_t classSize;
    int numOfBlocks;
    int freeBlocks;
} page_t;

void *ptr;

#endif //UNTITLED3_PAGE_H
