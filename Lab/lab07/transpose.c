#include "transpose.h"

/* The naive transpose function as a reference. */
void transpose_naive(int n, int blocksize, int *dst, int *src)
{
    for (int x = 0; x < n; x++)
    {
        for (int y = 0; y < n; y++)
        {
            dst[y + x * n] = src[x + y * n];
        }
    }
}

/* Implement cache blocking below. You should NOT assume that n is a
 * multiple of the block size. */
void transpose_blocking(int n, int blocksize, int *dst, int *src)
{
    // YOUR CODE HERE
    // hint: you need 4 loops to implement blocking.
    for (int y = 0; y < n; y += blocksize)
    {
        int y_end = y + blocksize < n ? y + blocksize : n;
        for (int x = 0; x < n; x += blocksize)
        {
            int x_end = x + blocksize < n ? x + blocksize : n;
            for (int j = y; j < y_end; j++)
            {
                for (int i = x; i < x_end; i++)
                {
                    dst[j + i * n] = src[i + j * n];
                }
            }
        }
    }
}
