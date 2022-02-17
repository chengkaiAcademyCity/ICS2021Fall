/* Kai Cheng 2000013172 */
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include "cachelab.h"
#define INF 0x3fffffff
int hits = 0, misses = 0, evictions = 0;
int verbose = 0;
struct s_cache
{
    size_t S;       // Number of set index bits (S = 2^s is the number of sets)
    size_t E;       // Associativity (number of lines per set)
    size_t B;       // Number of block bits (B = 2^b is the block size)
    size_t ***addr; // Array address of cache
} cache;
/* Get valid bit */
size_t GetValid(size_t _S, size_t _E)
{
    return cache.addr[_S][_E][0];
}
/* Get tag bits */
size_t GetTag(size_t _S, size_t _E)
{
    return cache.addr[_S][_E][1];
}
/* Get recently used number, to check which was accessed the furthest in the past,
in order to use the LRU (least-recently used) replacement policy
 when choosing which cache line to evict */
size_t GetRU(size_t _S, size_t _E)
{
    return cache.addr[_S][_E][2];
}
/* Set cache parameters */
void SetCache(size_t _S, size_t _E, size_t _B)
{
    cache.S = _S;
    cache.E = _E;
    cache.B = _B;
}
/* Allocate cache */
void AllocCache()
{
    cache.addr = (size_t ***)malloc(cache.S * sizeof(size_t **));
    for (size_t i = 0; i < cache.S; i++)
        cache.addr[i] = (size_t **)malloc(cache.E * sizeof(size_t *));
    for (size_t i = 0; i < cache.S; i++)
    {
        for (size_t j = 0; j < cache.E; j++)
        {
            cache.addr[i][j] = (size_t *)malloc(3 * sizeof(size_t));
            cache.addr[i][j][0] = 0;
            cache.addr[i][j][1] = 0;
            cache.addr[i][j][2] = INF;
        }
    }
}
/* Free Cache Memory */
void FreeCache()
{
    for (size_t i = 0; i < cache.S; i++)
    {
        for (size_t j = 0; j < cache.E; j++)
        {
            free(cache.addr[i][j]);
        }
    }
    for (size_t i = 0; i < cache.S; i++)
        free(cache.addr[i]);
    free(cache.addr);
}
/* Print Help Page */
void PrintHelp()
{
    printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n\n");
    printf("Examples:\n");
    printf("  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}
/* Get the hit line number */
int isHit(size_t _S, size_t _tag)
{
    for (size_t i = 0; i < cache.E; i++)
    {
        if (cache.addr[_S][i][0] == 1 && cache.addr[_S][i][1] == _tag)
        {
            return i;
        }
    }
    return -1;
}
/* Get the least recently used line */
size_t GetLRU(size_t _S)
{
    size_t ru = INF, pos = 0;
    for (size_t i = 0; i < cache.E; i++)
    {
        if (!GetValid(_S, i))
            return i;
        if (ru > GetRU(_S, i))
        {
            ru = GetRU(_S, i);
            pos = i;
        }
    }
    return pos;
}
/* Replace the least recently used line */
void Replace(size_t _S, size_t _E, size_t _tag)
{
    if (GetValid(_S, _E) == 1 && GetTag(_S, _E) != _tag)
    {
        evictions++;
        if (verbose)
            printf(" eviction\n");
    }
    cache.addr[_S][_E][0] = 1;
    cache.addr[_S][_E][1] = _tag;
    for (size_t i = 0; i < cache.E; i++)
    {
        cache.addr[_S][i][2]--; // used length
    }
    cache.addr[_S][_E][2] = INF;
}
/* Check whether to hit or to miss */
void Chk(size_t _S, size_t _tag)
{
    int _ishit = isHit(_S, _tag);
    if (_ishit == -1)
    {
        misses++;
        if (verbose)
            printf(" miss\n");
        Replace(_S, GetLRU(_S), _tag);
    }
    else
    {
        hits++;
        if (verbose)
            printf(" hit\n");
        Replace(_S, _ishit, _tag);
    }
}
int main(int argc, char *argv[])
{
    char opt;
    FILE *path = NULL;
    int _s = -1, _E = -1, _b = -1;
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1)
    {
        switch (opt)
        {
        case 'h':
            // PrintHelp();
            break;
        case 'v':
            verbose = 1;
            break;
        case 's':
            _s = atoi(optarg);
            break;
        case 'E':
            _E = atoi(optarg);
            break;
        case 'b':
            _b = atoi(optarg);
            break;
        case 't':
            path = fopen(optarg, "r");
            break;
        default:
            PrintHelp();
            break;
        }
    }
    if (_s <= 0 || _E <= 0 || _b <= 0 || path == NULL)
    {
        PrintHelp();
        return 0;
    }
    SetCache(1 << _s, _E, 1 << _b);
    AllocCache();
    char cmd;
    size_t address;
    size_t nsize1;
    while (fscanf(path, " %c %lx,%ld", &cmd, &address, &nsize1) != EOF)
    {
        if (cmd == 'I')
            continue;
        size_t tag = address >> (_s + _b);                     // Get tag bits
        size_t S = (address & ((1lu << (_s + _b)) - 1)) >> _b; // Get S bits
        if (verbose)
            printf("%c %lx,%ld", cmd, address, nsize1);
        if (cmd == 'L' || cmd == 'S')
            Chk(S, tag);
        else if (cmd == 'M')
        {
            Chk(S, tag);
            Chk(S, tag);
        }
    }
    fclose(path);
    printSummary(hits, misses, evictions);
    FreeCache();
    return 0;
}