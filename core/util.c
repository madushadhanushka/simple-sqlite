#include <stdarg.h>
#include <string.h>
#include <mm_malloc.h>

#include "sqliteInt.h"
int sqlite_malloc_failed = 0;

/* An array to map all upper-case characters into their corresponding
** lower-case character.
*/
static unsigned char UpperToLower[] = {
        0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17,
        18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
        36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53,
        54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 97, 98, 99,100,101,102,103,
        104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,
        122, 91, 92, 93, 94, 95, 96, 97, 98, 99,100,101,102,103,104,105,106,107,
        108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,
        126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
        144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,160,161,
        162,163,164,165,166,167,168,169,170,171,172,173,174,175,176,177,178,179,
        180,181,182,183,184,185,186,187,188,189,190,191,192,193,194,195,196,197,
        198,199,200,201,202,203,204,205,206,207,208,209,210,211,212,213,214,215,
        216,217,218,219,220,221,222,223,224,225,226,227,228,229,230,231,232,233,
        234,235,236,237,238,239,240,241,242,243,244,245,246,247,248,249,250,251,
        252,253,254,255
};
void sqliteFree(void *p){
    if( p ){
        free(p);
    }
}
/*
** This function computes a hash on the name of a keyword.
** Case is not significant.
*/
int sqliteHashNoCase(const char *z, int n){
    int h = 0;
    if( n<=0 ) n = strlen(z);
    while( n > 0  ){
        h = (h<<3) ^ h ^ UpperToLower[(unsigned char)*z++];
        n--;
    }
    if( h<0 ) h = -h;
    return h;
}

/*
** Allocate new memory and set it to zero.  Return NULL if
** no memory is available.
*/
void *sqliteMalloc(int n){
    void *p = malloc(n);
    if( p==0 ){
        sqlite_malloc_failed++;
        return 0;
    }
    memset(p, 0, n);
    return p;
}

/*
** Create a string from the 2nd and subsequent arguments (up to the
** first NULL argument), store the string in memory obtained from
** sqliteMalloc() and make the pointer indicated by the 1st argument
** point to that string.
*/
void sqliteSetString(char **pz, const char *zFirst, ...){
    va_list ap;
    int nByte;
    const char *z;
    char *zResult;

    if( pz==0 ) return;
    nByte = strlen(zFirst) + 1;
    va_start(ap, zFirst);
    while( (z = va_arg(ap, const char*))!=0 ){
        nByte += strlen(z);
    }
    va_end(ap);
    sqliteFree(*pz);
    *pz = zResult = sqliteMalloc( nByte );
    if( zResult==0 ){
        return;
    }
    strcpy(zResult, zFirst);
    zResult += strlen(zResult);
    va_start(ap, zFirst);
    while( (z = va_arg(ap, const char*))!=0 ){
        strcpy(zResult, z);
        zResult += strlen(zResult);
    }
    va_end(ap);
#ifdef MEMORY_DEBUG
    #if MEMORY_DEBUG>1
  fprintf(stderr,"string at 0x%x is %s\n", (int)*pz, *pz);
#endif
#endif
}
/*
** Make a copy of a string in memory obtained from sqliteMalloc()
*/
char *sqliteStrDup(const char *z){
    char *zNew;
    if( z==0 ) return 0;
    zNew = sqliteMalloc(strlen(z)+1);
    if( zNew ) strcpy(zNew, z);
    return zNew;
}