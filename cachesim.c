#define AMAX 10	 /* Maximum (square) array size, either 2 or 10 */
#define CWRD 256  /* Cache size: Either 16 or 256 words */
#define ASS  4   /* Cache associativity: 1, 2, or 4 way */

#include <stdio.h>

/*	memory management, code density, Cache emulation - statistics generation */
/*	Generated for CSC 315 Lab 5 */

//typedef struct cache_entry {
//    int* location[ASS]; /* Memory addresses of cache entries */
//    int next_evict; /* Next n-way cache entry to evict for round robin implementation */
//} cache_entry;

//cache_entry cache[CWRD];

/* Statically define the arrays a, b, and mult, where mult will become the cross product of a and b, i.e., a x b. */
static int a[AMAX][AMAX], b[AMAX][AMAX], mult[AMAX][AMAX];
static int CACHE[CWRD]; 
static size_t HITS;
static size_t MISS;
static int READS;
static int WRITES;

int *cache_addrs[ASS]; /* Cache is one block, so all we need to know to simulate it is
    the starting address and block size.  Associativity is number of blocks. */
int next_evict = 0; /* Rount robin cache eviction */

/* This function gets called with each "read" reference to memory */
void mem_read(int *mp)
{
    READS++;
    (void) mp;
    int foundCache = 0;
    /* Check if address is in range of current cache */
    for (int i = 0; i < ASS; i++){ // Check each "way" of cache associativity
        if (mp >= cache_addrs[i] && mp < (cache_addrs[i] + ASS)){
            foundCache = 1;
        }
    }
    if(foundCache){ // Hit
        HITS++;
    }else{ // Miss
        MISS++;
        cache_addrs[next_evict] = mp; /* Start block at requested address */
        if(++next_evict >= ASS){ // Increment next_evict
            next_evict = 0;
        }
    }
    /* printf("Memory read from location %p\n", mp);  */
    
}

/* This function gets called with each "write" reference to memory */
void mem_write(int *mp)
{
    WRITES++;
    (void) mp;
    MISS++; /* Write-through */
    int foundCache = 0;
    /* Check if address is in range of current cache */
    for (int i = 0; i < ASS; i++){ // Check each "way" of cache associativity
        if (mp >= cache_addrs[i] && mp < (cache_addrs[i] + ASS)){
            foundCache = 1;
        }
    }
    if(!foundCache){ // Miss
        MISS++;
        cache_addrs[next_evict] = mp; /* Start block at requested address */
        if(++next_evict >= ASS){ // Increment next_evict
            next_evict = 0;
        }
    }
    /* printf("Memory write to location %p\n", mp); */

}

void matmul(int r1, int c1, int c2 )
{
    int i,j,k;
    int *mp1, *mp2, *mp3;

    /* Initializing elements of matrix mult to 0.*/
    for(i=0; i<r1; ++i)
    {
        for(j=0; j<c2; ++j)
        {
            mult[i][j]=0;
        }
    }

    /* Multiplying matrix a and b and storing in array mult. */

    for(i=0; i<r1; ++i)
    {
        for(j=0; j<c2; ++j)
        {
            for(k=0; k<c1; ++k)
            {
                mp1 = &mult[i][j];
                mp2 = &a[i][k];
                mp3 = &b[k][j];   
                mem_read(mp1);
                mem_read(mp2);
                mem_read(mp3);
                mem_write(mp1); 

                mult[i][j]+=a[i][k]*b[k][j];
            }
        }
    }
    printf("Ratio of read to write accesses: %d\n\n", READS/WRITES);
    printf("Hit rate for cache: %lu\n\n", (HITS*100/(HITS+MISS)));
}

int main(void)
{
    int r1, c1, r2, c2, i, j;

    r1 = AMAX;
    r2 = AMAX;
    c1 = AMAX;
    c2 = AMAX;

    /* Storing elements of first matrix. */
    for(i=0; i<r1; ++i)
    {
        for(j=0; j<c1; ++j)
        {
            a[i][j] = 49*(i+j); // build sample data

        }
    }

    /* Storing elements of second matrix. */
    for(i=0; i<r2; ++i)
    {
        for(j=0; j<c2; ++j)
        {
            b[i][j] = 11 - 2*i + j;
        }
    }

    matmul(r1, c1, c2);  	/* Invoke matrix multiply function */	


    /* Displaying the multiplication of two matrix. */
    printf("\nOutput Matrix:\n");
    for(i=0; i<r1; ++i)
    {
        for(j=0; j<c2; ++j)
        {
            printf("%d  ",mult[i][j]);
            if(j==c2-1)
                printf("\n\n");
        }
    }
    return 0;
}
