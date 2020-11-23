#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "block.h"
#include "page.h"

struct page pages[NUM_OF_PAGES];

size_t align(size_t size) {
    return (int) pow(2, ceil(log2(size)));
}

size_t multiblock_align(size_t size) {
    int x = 1;
    while (size % (size_t) (PAGE_SIZE * x) != size) {
        x++;
    }
    return PAGE_SIZE * x;
}

void initializer() {
    ptr = malloc(NUM_OF_PAGES*PAGE_SIZE);
    for (int i = 0; i < NUM_OF_PAGES; ++i) {
        pages[i].class = 0;
        pages[i].classSize = 0;
        pages[i].pageStart = ptr + (i * PAGE_SIZE);
        pages[i].status = FREE;
        pages[i].numOfBlocks = 0;
        pages[i].freeBlocks = 0;
        pages[i].freeBlock = pages[i].pageStart;
    }
}

void *mem_alloc(size_t size) {
    if (size == 0) {
        return NULL;
    }
    if (size < PAGE_SIZE / 2) {
        size = align(size);
    } else {
        size = multiblock_align(size);
    }
    int class;
    for (int i = 0; i < NUM_OF_PAGES; ++i) {
        if (pages[i].status == FREE) {
            if (size < PAGE_SIZE / 2) {
                class = (int) log2(size);
                int num_of_blocks = PAGE_SIZE / size;
                pages[i].status = DIVIDED;
                pages[i].freeBlock = pages[i].freeBlock + size;
                pages[i].class = class;
                pages[i].classSize = size;
                pages[i].numOfBlocks = num_of_blocks;
                pages[i].freeBlocks = num_of_blocks - 1;
                return pages[i].pageStart;
            } else {
                class = size / PAGE_SIZE;
                pages[i].freeBlock = pages[i].freeBlock
                                     + class * PAGE_SIZE;
                for (int j = i; j < i + class; j++) {
                    pages[j].status = MULTIBLOCK;
                    pages[j].freeBlock = pages[i].freeBlock;
                    pages[j].class = class;
                    pages[j].classSize = size;
                    pages[i].numOfBlocks = 1;
                }
                return pages[i].pageStart;
            }
        } else if (pages[i].status == DIVIDED &&
        pages[i].freeBlocks > 0 && pages[i].classSize == size) {
            if (pages[i].freeBlock == pages[i].pageStart + PAGE_SIZE) {
                return NULL;
            }
            pages[i].freeBlock = pages[i].freeBlock + size;
            pages[i].freeBlocks = pages[i].freeBlocks - 1;
            return pages[i].freeBlock - pages[i].classSize;
        }
    }
    return NULL;
}

void mem_free(void* addr) {
    int page_number = 0;
    for (int i = 0; i < NUM_OF_PAGES; i++) {
        if (addr >= pages[i].pageStart &&
                            addr < pages[i].freeBlock) {
            page_number = i;
            break;
        }
    }
    int num_of_pages = pages[page_number].class;
    switch (pages[page_number].status) {
        case FREE: break;
        case MULTIBLOCK:
            for (int j = page_number;
                            num_of_pages > 0; j++) {
                pages[j].status = FREE;
                pages[j].freeBlock = pages[j].pageStart;
                pages[j].class = 0;
                pages[j].classSize = 0;
                pages[j].numOfBlocks = 0;
                num_of_pages--;
            }
            break;
        case DIVIDED: pages[page_number].freeBlock
                            = pages[page_number].freeBlock - pages[page_number].classSize;
            pages[page_number].freeBlocks
                    = pages[page_number].freeBlocks + 1;
        if (pages[page_number].freeBlocks ==
                                      pages[page_number].numOfBlocks) {
            pages[page_number].status = FREE;
            pages[page_number].class = 0;
            pages[page_number].classSize = 0;
            pages[page_number].numOfBlocks = 0;
            pages[page_number].freeBlocks = 0;
        }
            break;
    }
}

void* mem_realloc(void* addr, size_t size) {

}


void mem_dump() {
//    printf("Стан пам'яті: \n\n");
    for (int i = 0; i < NUM_OF_PAGES; i++) {
        printf("Сторінка %d\n", i + 1);
        printf("Початок сторінки: %p\n", pages[i].pageStart);
        if (pages[i].numOfBlocks != 0) {
            void *blockStart = pages[i].pageStart;
            printf("-----------------------------------------\n");
            for (int j = 1; j <= pages[i].numOfBlocks
            - pages[i].freeBlocks; j++) {
                printf("    Блок %d\n", j);
                printf("    Адреса: %p\n", blockStart);
                blockStart = blockStart + pages[i].classSize;
                printf("-----------------------------------------\n");
            }
            printf("Загальна кількість блоків: %d\n", pages[i].numOfBlocks);
            printf("     серед яких вільних: %d\n", pages[i].freeBlocks);
        }
        printf("Адреса вільного блоку: %p\n", pages[i].freeBlock);
        printf("Клас блоку: %d\n", pages[i].class);
        printf("Розмір блока: %zu\n", pages[i].classSize);
        switch (pages[i].status) {
            case FREE: printf("Стан сторінки: %s\n", "вільна");
                break;
            case DIVIDED: printf("Стан сторінки: %s\n", "розділена на блоки");
                break;
            case MULTIBLOCK: printf("Стан сторінки: %s\n", "мультиблок");
                break;
        }
        printf("----------------------------------------------------------------------\n");
    }

}


int main() {
    initializer();
    printf("Початковий стан пам'яті:\n");
    mem_dump();
    printf("\n\n\n");
    printf("Принцип роботи malloc:\n");
    void* block1 = mem_alloc(53);
    void* block2_1 = mem_alloc(15);
    void* block2_2 = mem_alloc(16);
    void* block2_3 =mem_alloc(10);
    void* block3 = mem_alloc(512);
    void* multi = mem_alloc(13000);
    mem_dump();
    printf("Принцип роботи mem_free:\n\n");
    printf("   для мультиблоку:\n");
    mem_free(multi);
    mem_dump();
    printf("   для звичайного блоку:\n");
    mem_free(block2_2);
    mem_free(block1);
    mem_dump();
}
