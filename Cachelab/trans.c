/* Kai Cheng 2000013172
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */
#include <stdio.h>
#include "cachelab.h"
#include "contracts.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/*
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. The REQUIRES and ENSURES from 15-122 are included
 *     for your convenience. They can be removed if you like.
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    REQUIRES(M > 0);
    REQUIRES(N > 0);
    int i, j, k, t0, t1, t2, t3, t4, t5, t6, t7;
    /* Every block contains 32 bytes = 8 ints,
    every A line uses 32 ints = 4 blocks,
    every 8 A lines uses 32 blocks = the entire cache. */
    if (M == 32)
    {
        for (i = 0; i < 32; i += 8)
            for (j = 0; j < 32; j += 8) // To catch 8*8
                for (k = i; k < i + 8; k++)
                {
                    t0 = A[k][j];
                    t1 = A[k][j + 1];
                    t2 = A[k][j + 2];
                    t3 = A[k][j + 3];
                    t4 = A[k][j + 4];
                    t5 = A[k][j + 5];
                    t6 = A[k][j + 6];
                    t7 = A[k][j + 7];
                    B[j][k] = t0;
                    B[j + 1][k] = t1;
                    B[j + 2][k] = t2;
                    B[j + 3][k] = t3;
                    B[j + 4][k] = t4;
                    B[j + 5][k] = t5;
                    B[j + 6][k] = t6;
                    B[j + 7][k] = t7;
                }
    }
    /* Every block contains 32 bytes = 8 ints,
    every A line uses 64 ints = 8 blocks,
    every 4 A lines uses 32 blocks = the entire cache. */
    else if (M == 64)
    {
        for (i = 0; i < 64; i += 8)
            for (j = 0; j < 64; j += 8) // To catch 8*8
            {
                for (k = i; k < i + 4; k++) // The above 4 lines
                {
                    t0 = A[k][j];
                    t1 = A[k][j + 1];
                    t2 = A[k][j + 2];
                    t3 = A[k][j + 3];
                    t4 = A[k][j + 4];
                    t5 = A[k][j + 5];
                    t6 = A[k][j + 6];
                    t7 = A[k][j + 7];
                    /* Reverse 4*4 normally. */
                    B[j][k] = t0;
                    B[j + 1][k] = t1;
                    B[j + 2][k] = t2;
                    B[j + 3][k] = t3;
                    /* Reverse 4*4 to unused space. */
                    B[j][k + 4] = t4;
                    B[j + 1][k + 4] = t5;
                    B[j + 2][k + 4] = t6;
                    B[j + 3][k + 4] = t7;
                }
                for (k = j; k < j + 4; k++) // The left 4 rows
                {
                    t0 = A[i + 4][k];
                    t1 = A[i + 5][k];
                    t2 = A[i + 6][k];
                    t3 = A[i + 7][k];
                    /* Rearrange the 4*4 space. */
                    t4 = B[k][i + 4];
                    t5 = B[k][i + 5];
                    t6 = B[k][i + 6];
                    t7 = B[k][i + 7];
                    /* Reverse 4*4 normally. */
                    B[k][i + 4] = t0;
                    B[k][i + 5] = t1;
                    B[k][i + 6] = t2;
                    B[k][i + 7] = t3;
                    /* Recover to the original 4*4 place. */
                    B[k + 4][i] = t4;
                    B[k + 4][i + 1] = t5;
                    B[k + 4][i + 2] = t6;
                    B[k + 4][i + 3] = t7;
                }
                for (k = i + 4; k < i + 8; k++) // The 4*4 leftover
                {
                    t4 = A[k][j + 4];
                    t5 = A[k][j + 5];
                    t6 = A[k][j + 6];
                    t7 = A[k][j + 7];
                    B[j + 4][k] = t4;
                    B[j + 5][k] = t5;
                    B[j + 6][k] = t6;
                    B[j + 7][k] = t7;
                }
            }
    }
    else
    {
        /* Original 4*4 version, reach 1641 misses.
        for(j=0;j<60;j+=4)//To catch 4*4
                for(i=0;i<68;i+=4)
                    for(k=i;k<i+4&&k<68;k++)
                    {
                        t0=A[k][j];
                        t1=A[k][j+1];
                        t2=A[k][j+2];
                        t3=A[k][j+3];
                        B[j][k]=t0;
                        B[j+1][k]=t1;
                        B[j+2][k]=t2;
                        B[j+3][k]=t3;
                    }
        */
        /* Combine 8*8 and 4*4 versions, reach 1587 misses. */
        for (i = 0; i < 56; i += 8)
            for (j = 0; j < 56; j += 8) // To catch 8*8
            {
                for (k = i; k < i + 4; k++) // The above 4 lines
                {
                    t0 = A[k][j];
                    t1 = A[k][j + 1];
                    t2 = A[k][j + 2];
                    t3 = A[k][j + 3];
                    t4 = A[k][j + 4];
                    t5 = A[k][j + 5];
                    t6 = A[k][j + 6];
                    t7 = A[k][j + 7];
                    /* Reverse 4*4 normally. */
                    B[j][k] = t0;
                    B[j + 1][k] = t1;
                    B[j + 2][k] = t2;
                    B[j + 3][k] = t3;
                    /* Reverse 4*4 to unused space. */
                    B[j][k + 4] = t4;
                    B[j + 1][k + 4] = t5;
                    B[j + 2][k + 4] = t6;
                    B[j + 3][k + 4] = t7;
                }
                for (k = j; k < j + 4; k++) // The left 4 rows
                {
                    t0 = A[i + 4][k];
                    t1 = A[i + 5][k];
                    t2 = A[i + 6][k];
                    t3 = A[i + 7][k];
                    /* Rearrange the 4*4 space. */
                    t4 = B[k][i + 4];
                    t5 = B[k][i + 5];
                    t6 = B[k][i + 6];
                    t7 = B[k][i + 7];
                    /* Reverse 4*4 normally. */
                    B[k][i + 4] = t0;
                    B[k][i + 5] = t1;
                    B[k][i + 6] = t2;
                    B[k][i + 7] = t3;
                    /* Recover to the original 4*4 place. */
                    B[k + 4][i] = t4;
                    B[k + 4][i + 1] = t5;
                    B[k + 4][i + 2] = t6;
                    B[k + 4][i + 3] = t7;
                }
                for (k = i + 4; k < i + 8; k++) // The 4*4 leftover
                {
                    t4 = A[k][j + 4];
                    t5 = A[k][j + 5];
                    t6 = A[k][j + 6];
                    t7 = A[k][j + 7];
                    B[j + 4][k] = t4;
                    B[j + 5][k] = t5;
                    B[j + 6][k] = t6;
                    B[j + 7][k] = t7;
                }
            }
        for (j = 56; j < 60; j += 4) // To catch 4*4
            for (i = 0; i < 68; i += 4)
                for (k = i; k < i + 4 && k < 68; k++)
                {
                    t0 = A[k][j];
                    t1 = A[k][j + 1];
                    t2 = A[k][j + 2];
                    t3 = A[k][j + 3];
                    B[j][k] = t0;
                    B[j + 1][k] = t1;
                    B[j + 2][k] = t2;
                    B[j + 3][k] = t3;
                }
        for (j = 0; j < 60; j += 4) // To catch 4*4
            for (i = 56; i < 68; i += 4)
            {
                if (j >= 56)
                    continue;
                for (k = i; k < i + 4 && k < 68; k++)
                {
                    t0 = A[k][j];
                    t1 = A[k][j + 1];
                    t2 = A[k][j + 2];
                    t3 = A[k][j + 3];
                    B[j][k] = t0;
                    B[j + 1][k] = t1;
                    B[j + 2][k] = t2;
                    B[j + 3][k] = t3;
                }
            }
    }
    ENSURES(is_transpose(M, N, A, B));
}

/*
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started.
 */

/*
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    REQUIRES(M > 0);
    REQUIRES(N > 0);

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; j++)
        {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }

    ENSURES(is_transpose(M, N, A, B));
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc);

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc);
}

/*
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < M; ++j)
        {
            if (A[i][j] != B[j][i])
            {
                return 0;
            }
        }
    }
    return 1;
}
