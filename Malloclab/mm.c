/*
 * mm.c
 *
 * Kai Cheng 2000013172
 *
 * Modified from textbook code, most of the macro and functions reused.
 * Seg free list complement
 * Store pointers to head & tail of free lists in the heap
 * Divide blocks into list of [2^n,2^(n+1)) size.
 * To improve utility, the footer is only used in free blocks.
 * Allocated blocks is tagged on 1 bit of the header of next block.
 * Best fit in each list to malloc.
 * Naive realloc: if new size is greater than old size, simply copy the data to a new place.
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mm.h"
#include "memlib.h"

/* If you want debugging output, use the following macro.  When you hand
 * in, remove the #define DEBUG line. */
#define DEBUGx
#ifdef DEBUG
#define dbg_printf(...) printf(__VA_ARGS__)
#else
#define dbg_printf(...)
#endif

/* do not change the following! */
#ifdef DRIVER
/* create aliases for driver tests */
#define malloc mm_malloc
#define free mm_free
#define realloc mm_realloc
#define calloc mm_calloc
#endif /* def DRIVER */

/*
 * If NEXT_FIT defined use next fit search, else use first-fit search
 */
#define NEXT_FITx

/* Basic constants and macros */
#define WSIZE 4             /* Word and header/footer size (bytes) */
#define DSIZE 8             /* Double word size (bytes) */
#define BLOCKSIZE 16        /* Min block size */
#define INFOSIZE 4          /* Info part size in a block */
#define CHUNKSIZE (1 << 11) /* Extend heap by this amount (bytes) */
#define MAXLIST 16          /* Number of free lists*/

#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(p) (((size_t)(p) + (ALIGNMENT - 1)) & ~0x7)

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_PREF(p) (GET(p) & 0x2)
#define GET_ALLOC(p) (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp)-WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp)-WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp)-GET_SIZE(((char *)(bp)-DSIZE)))

#define PREV(bp) ((char *)(bp)-GET(bp))
#define SUCC(bp) ((char *)(bp) + GET((char *)(bp) + WSIZE))
#define PUT_PREV(bp, pre) PUT(bp, (unsigned int)((char *)(bp) - (char *)(pre)))
#define PUT_SUCC(bp, suc) PUT((char *)(bp) + WSIZE, (unsigned int)((char *)(suc) - (char *)(bp)))

/* Block type, to distinguish neighbours */
#define ONLYB 0
#define HEADB 1
#define ENDB 2
#define MIDB 3

/* Global variables */
static char *heap_listp = 0; /* Pointer to heap */
static size_t *free_headp;   /* Point to free list head */
static size_t *free_tailp;   /* Point to free list tail */

#ifdef NEXT_FIT
static char *rover; /* Next fit rover */
#endif

/* Function prototypes for internal helper routines */
static void *extend_heap(size_t words);
static void *place(void *bp, size_t asize);
static void *find_fit(size_t asize);
static void *coalesce(void *bp);
static int get_index(size_t size);
static void *insert_node(void *bp);
static int get_type(void *bp);
static void delete_node(void *bp);

/*
 * mm_init - Initialize the memory manager
 * Initialize (4+MAXLIST*4) WSIZE, 4 to set foreword, others to set pointer to free lists.
 */
int mm_init(void)
{
    /* Create the initial empty heap */
    if ((heap_listp = mem_sbrk((4 + MAXLIST * 4) * WSIZE)) == (void *)-1)
        return -1;
    free_headp = (size_t *)heap_listp;
    free_tailp = (size_t *)(heap_listp + MAXLIST * DSIZE);
    int i;
    for (i = 0; i < MAXLIST; ++i) /* Seg free lists*/
    {
        free_headp[i] = (size_t)NULL;
        free_tailp[i] = (size_t)NULL;
    }
    heap_listp += (MAXLIST * 4) * WSIZE;
    PUT(heap_listp, 0);                            /* Alignment padding */
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1)); /* Prologue header */
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
    PUT(heap_listp + (3 * WSIZE), PACK(0, 3));     /* Epilogue header */
    heap_listp += (2 * WSIZE);

#ifdef NEXT_FIT
    rover = heap_listp;
#endif

    /* Extend the empty heap with a free block of CHUNKSIZE bytes */
    if (extend_heap(CHUNKSIZE) == NULL)
        return -1;
    return 0;
}

/*
 * malloc - Allocate a block with at least size bytes of payload
 */
void *malloc(size_t size)
{
    size_t asize;      /* Adjusted block size */
    size_t extendsize; /* Amount to extend heap if no fit */
    char *bp;

    if (heap_listp == 0)
    {
        mm_init();
    }
    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

    asize = size + INFOSIZE;
    /* Adjust block size to include overhead and alignment reqs. */
    if (asize <= BLOCKSIZE)
        asize = BLOCKSIZE;
    else
        asize = DSIZE * ((asize + (DSIZE - 1)) / DSIZE);

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL)
    {
        bp = place(bp, asize);
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize)) == NULL)
        return NULL;
    place(bp, asize);
    return bp;
}

/*
 * free - Free a block, change next block's head tag
 */
void free(void *bp)
{
    if (bp == 0)
        return;

    size_t size = GET_SIZE(HDRP(bp));
    if (heap_listp == 0)
    {
        mm_init();
    }
    char pref = GET_PREF(HDRP(bp));
    PUT(HDRP(bp), PACK(size, pref | 0));
    PUT(FTRP(bp), PACK(size, 0));
    void *nextbp = NEXT_BLKP(bp);
    size_t nsize = GET_SIZE(HDRP(nextbp));
    char nalloc = GET_ALLOC(HDRP(nextbp));
    PUT(HDRP(nextbp), PACK(nsize, nalloc));
    bp = insert_node(bp);
    coalesce(bp);
}

/*
 * realloc - if the oldsize is enough to store newsize, simply change the size of oldblock
 * else memcpy the block to a new place.
 */
void *realloc(void *ptr, size_t size)
{
    size_t oldsize, asize;
    void *newptr;

    /* If size == 0 then this is just free, and we return NULL. */
    if (size == 0)
    {
        mm_free(ptr);
        return 0;
    }

    /* If oldptr is NULL, then this is just malloc. */
    if (ptr == NULL)
    {
        return mm_malloc(size);
    }

    /* Copy the old data. */
    oldsize = GET_SIZE(HDRP(ptr));
    asize = size + INFOSIZE;
    /* Adjust block size to include overhead and alignment reqs. */
    if (asize <= BLOCKSIZE)
        asize = BLOCKSIZE;
    else
        asize = DSIZE * ((asize + (DSIZE - 1)) / DSIZE);

    if (oldsize >= asize)
    {
        char pref = GET_PREF(HDRP(ptr));
        PUT(HDRP(ptr), PACK(oldsize, pref | 1));

        return ptr;
    }
    else
    {
        newptr = mm_malloc(size);

        /* If realloc() fails the original block is left untouched  */
        if (!newptr)
        {
            return 0;
        }

        memcpy(newptr, ptr, size);

        /* Free the old block. */
        mm_free(ptr);

        return newptr;
    }
}

/*
 * calloc - Allocate the block and set it to zero.
 */
void *calloc(size_t nmemb, size_t size)
{
    size_t bytes = nmemb * size;
    void *newptr;

    newptr = malloc(bytes);
    memset(newptr, 0, bytes);

    return newptr;
}

/*
 * mm_checkheap - Check the heap and free lists for correctness. Helpful hint: You
 *                can call this function using mm_checkheap(__LINE__);
 *                to identify the line number of the call site.
 */
void mm_checkheap(int lineno)
{
    char *curBp = heap_listp;
    if (lineno)
        printf("Heap from (%p):\n", heap_listp);
    int i = 0;
    for (; i < MAXLIST; ++i) /* Check free lists */
    {
        if (lineno)
            if (!free_headp[i])
            {
                printf("Free list No.%d is empty\n", i);
                continue;
            }
        for (curBp = (char *)free_headp[i]; curBp != (char *)free_tailp[i]; curBp = SUCC(curBp))
        {
            if (GET_ALLOC(HDRP(curBp)))
                printf("list No.%d ,there is SIZE :%u , ALLOC: %d NOT FREE!\n", i, GET_SIZE(HDRP(curBp)), GET_ALLOC(HDRP(curBp)));
        }
        if (GET_ALLOC(HDRP(free_tailp[i])))
            printf("list No.%d ,SIZE :%u , ALLOC: %d NOT FREE!\n\n", i, GET_SIZE(HDRP(free_tailp[i])), GET_ALLOC(HDRP(free_tailp[i])));
    }
    return;
}

/*
 * The remaining routines are internal helper routines
 */

/*
 * extend_heap - Extend heap with free block and return its block pointer
 */
static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignment */
    size = (words % 2) ? (words + 1) : words;
    if ((long)(bp = mem_sbrk(size)) == -1)
        return NULL;

    /* Initialize free block header/footer and the epilogue header */
    char pref = GET_PREF(HDRP(bp));
    PUT(HDRP(bp), PACK(size, pref | 0));  /* Free block header */
    PUT(FTRP(bp), PACK(size, 0));         /* Free block footer */
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */

    /* Coalesce if the previous block was free */
    return insert_node(bp);
}

/*
 * coalesce - Boundary tag coalescing. Return ptr to coalesced block
 */
static void *coalesce(void *bp)
{

    size_t prev_alloc = GET_PREF(HDRP(bp));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc)
    { /* Case 1 */
        return bp;
    }

    else if (prev_alloc && !next_alloc)
    { /* Case 2 */
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        void *nnextbp = NEXT_BLKP(NEXT_BLKP(bp));
        size_t nnsize = GET_SIZE(HDRP(nnextbp));
        char nnalloc = GET_ALLOC(HDRP(nnextbp));
        PUT(HDRP(nnextbp), PACK(nnsize, nnalloc));
        delete_node(NEXT_BLKP(bp));
        delete_node(bp);
        char pref = GET_PREF(HDRP(bp));
        PUT(HDRP(bp), PACK(size, pref | 0));
        PUT(FTRP(bp), PACK(size, 0));
        insert_node(bp);
    }

    else if (!prev_alloc && next_alloc)
    { /* Case 3 */
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        void *nextbp = NEXT_BLKP(bp);
        size_t nsize = GET_SIZE(HDRP(nextbp));
        char nalloc = GET_ALLOC(HDRP(nextbp));
        PUT(HDRP(nextbp), PACK(nsize, nalloc));
        delete_node(PREV_BLKP(bp));
        delete_node(bp);
        char pref = GET_PREF(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, pref | 0));
        bp = PREV_BLKP(bp);
        insert_node(bp);
    }

    else
    { /* Case 4 */
        size += GET_SIZE(HDRP(PREV_BLKP(bp))) +
                GET_SIZE(FTRP(NEXT_BLKP(bp)));
        delete_node(bp);
        delete_node(NEXT_BLKP(bp));
        delete_node(PREV_BLKP(bp));
        void *nnextbp = NEXT_BLKP(NEXT_BLKP(bp));
        size_t nnsize = GET_SIZE(HDRP(nnextbp));
        char nnalloc = GET_ALLOC(HDRP(nnextbp));
        PUT(HDRP(nnextbp), PACK(nnsize, nnalloc));
        char pref = GET_PREF(HDRP(PREV_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, pref | 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
        bp = PREV_BLKP(bp);
        insert_node(bp);
    }
#ifdef NEXT_FIT
    /* Make sure the rover isn't pointing into the free block */
    /* that we just coalesced */
    if ((rover > (char *)bp) && (rover < NEXT_BLKP(bp)))
        rover = bp;
#endif
    return bp;
}

/*
 * place - Place block of asize bytes at start of free block bp
 *         and split if remainder would be at least minimum block size
 */
static void *place(void *bp, size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));
    delete_node(bp);
    if ((csize - asize) >= (2 * DSIZE))
    {
        char pref = GET_PREF(HDRP(bp));
        PUT(HDRP(bp), PACK(asize, pref | 1));
        // PUT(FTRP(bp), PACK(asize, 1));
        void *nextbp = NEXT_BLKP(bp);
        PUT(HDRP(nextbp), PACK(csize - asize, 2));
        PUT(FTRP(nextbp), PACK(csize - asize, 0));
        insert_node(nextbp);
    }
    else
    {
        char pref = GET_PREF(HDRP(bp));
        PUT(HDRP(bp), PACK(csize, pref | 1));
        // PUT(FTRP(bp), PACK(csize, 1));
        void *nextbp = NEXT_BLKP(bp);
        size_t nsize = GET_SIZE(HDRP(nextbp));
        char nalloc = GET_ALLOC(HDRP(nextbp));
        PUT(HDRP(nextbp), PACK(nsize, nalloc | 2));
    }
    return bp;
}

/*
 * find_fit - Find a block that can hold the new block
 * Search from the size index
 */
static void *find_fit(size_t asize)
{
#ifdef NEXT_FIT
    /* Next fit search */
    char *oldrover = rover;

    /* Search from the rover to the end of list */
    for (; GET_SIZE(HDRP(rover)) > 0; rover = NEXT_BLKP(rover))
        if (!GET_ALLOC(HDRP(rover)) && (asize <= GET_SIZE(HDRP(rover))))
            return rover;

    /* search from start of list to old rover */
    for (rover = heap_listp; rover < oldrover; rover = NEXT_BLKP(rover))
        if (!GET_ALLOC(HDRP(rover)) && (asize <= GET_SIZE(HDRP(rover))))
            return rover;

    return NULL; /* no fit found */
#else
    /* Best-fit search */
    void *bp;
    int i = get_index(asize);
    for (; i < MAXLIST; ++i)
    {
        if ((void *)free_headp[i] == NULL)
            continue;
        void *bestbp = (void *)free_tailp[i];
        size_t bestsize = 1 << 30;
        for (bp = (char *)free_headp[i]; bp != (void *)free_tailp[i]; bp = SUCC(bp))
        {
            size_t bsize = GET_SIZE(HDRP(bp));
            if (bsize >= asize) /* Best fit */
            {
                if (bsize < bestsize)
                {
                    bestsize = bsize;
                    bestbp = bp;
                    if (bestsize == asize)
                        return bestbp;
                }
            }
        }
        if (bestsize != 1 << 30)
            return bestbp;
        if (GET_SIZE(HDRP(bp)) >= asize)
            return bp;
    }
    return NULL; /* No fit */
#endif
}

/*
 * getindex - Find target list with proper size
 */
static inline int get_index(size_t size)
{
    if (size <= (1 << 2))
        return 0;
    else if (size > (1 << (MAXLIST)))
        return MAXLIST - 1;
    int i = 2;
    unsigned int nowSize = (1 << 2);
    for (i = 3; i <= (MAXLIST + 1); ++i)
    {
        nowSize <<= 1;
        if (size <= nowSize)
        {
            return i - 2;
        }
    }
    return MAXLIST - 1;
}

/*
 * insert_node - Insert a block to proper list and keep relative address order
 */
static inline void *insert_node(void *bp)
{
    int index = get_index(GET_SIZE(HDRP(bp)));
    char *curBp = (char *)free_headp[index];
    if ((void *)free_headp[index] == NULL)
        free_headp[index] = free_tailp[index] = (size_t)bp;
    else
    {
        if (free_headp[index] > (size_t)bp)
        {
            PUT_SUCC(bp, free_headp[index]);
            PUT_PREV(free_headp[index], bp);
            free_headp[index] = (size_t)bp;
        }
        else if (free_tailp[index] < (size_t)bp)
        {
            PUT_SUCC(free_tailp[index], bp);
            PUT_PREV(bp, free_tailp[index]);
            free_tailp[index] = (size_t)bp;
        }
        else
        {
            while (SUCC(curBp) < (char *)bp)
            {
                curBp = SUCC(curBp);
            }
            char *nextBp = SUCC(curBp);
            PUT_PREV(nextBp, bp);
            PUT_SUCC(curBp, bp);
            PUT_SUCC(bp, nextBp);
            PUT_PREV(bp, curBp);
        }
    }
    return bp;
}

/*
 * get_type - Determine a block's type
 */
static inline int get_type(void *bp)
{
    int index = get_index(GET_SIZE(HDRP(bp)));
    if (free_headp[index] == free_tailp[index])
        return ONLYB;
    else if (free_headp[index] == (size_t)(bp))
        return HEADB;
    else if (free_tailp[index] == (size_t)(bp))
        return ENDB;
    else
        return MIDB;
}

/*
 * delete_node - Delete a block from list
 */
static inline void delete_node(void *bp)
{
    int type = get_type(bp);
    int i = get_index(GET_SIZE(HDRP(bp)));
    switch (type)
    {
    case ONLYB:
        free_headp[i] = free_tailp[i] = (size_t)NULL;
        break;
    case HEADB:
        free_headp[i] = (size_t)SUCC(bp);
        break;
    case ENDB:
        free_tailp[i] = (size_t)PREV(bp);
        break;
    case MIDB:
        PUT_PREV(SUCC(bp), PREV(bp));
        PUT_SUCC(PREV(bp), SUCC(bp));
        break;
    }
}
