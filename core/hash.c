/*
** 2001 September 22
**
** The author disclaims copyright to this source code.  In place of
** a legal notice, here is a blessing:
**
**    May you do good and not evil.
**    May you find forgiveness for yourself and forgive others.
**    May you share freely, never taking more than you give.
**
*************************************************************************
** This is the implementation of generic hash-tables
** used in SQLite.
**
** $Id: hash.c,v 1.8 2002/05/21 23:44:30 drh Exp $
*/
#include "sqliteInt.h"
#include <assert.h>
#include "hash.h"
#include "util.h"

/* Turn bulk memory into a hash table object by initializing the
** fields of the Hash structure.
**
** "new" is a pointer to the hash table that is to be initialized.
** keyClass is one of the constants SQLITE_HASH_INT, SQLITE_HASH_POINTER,
** SQLITE_HASH_BINARY, or SQLITE_HASH_STRING.  The value of keyClass
** determines what kind of key the hash table will use.  "copyKey" is
** true if the hash table should make its own private copy of keys and
** false if it should just use the supplied pointer.  CopyKey only makes
** sense for SQLITE_HASH_STRING and SQLITE_HASH_BINARY and is ignored
** for other key classes.
*/
void sqliteHashInit(Hash *new, int keyClass, int copyKey){
    assert( new!=0 );
    assert( keyClass>=SQLITE_HASH_INT && keyClass<=SQLITE_HASH_BINARY );
    new->keyClass = keyClass;
    new->copyKey = copyKey &&
                   (keyClass==SQLITE_HASH_STRING || keyClass==SQLITE_HASH_BINARY);
    new->first = 0;
    new->count = 0;
    new->htsize = 0;
    new->ht = 0;
}

/* Remove all entries from a hash table.  Reclaim all memory.
** Call this routine to delete a hash table or to reset a hash table
** to the empty state.
*/
void sqliteHashClear(Hash *pH){
    HashElem *elem;         /* For looping over all elements of the table */

    assert( pH!=0 );
    elem = pH->first;
    pH->first = 0;
    if( pH->ht ) sqliteFree(pH->ht);
    pH->ht = 0;
    pH->htsize = 0;
    while( elem ){
        HashElem *next_elem = elem->next;
        if( pH->copyKey && elem->pKey ){
            sqliteFree(elem->pKey);
        }
        sqliteFree(elem);
        elem = next_elem;
    }
    pH->count = 0;
}

/*
** Hash and comparison functions when the mode is SQLITE_HASH_INT
*/
static int intHash(const void *pKey, int nKey){
    return nKey ^ (nKey<<8) ^ (nKey>>8);
}
static int intCompare(const void *pKey1, int n1, const void *pKey2, int n2){
    return n2 - n1;
}

/*
** Hash and comparison functions when the mode is SQLITE_HASH_POINTER
*/
static int ptrHash(const void *pKey, int nKey){
    uptr x = Addr(pKey);
    return x ^ (x<<8) ^ (x>>8);
}
static int ptrCompare(const void *pKey1, int n1, const void *pKey2, int n2){
    if( pKey1==pKey2 ) return 0;
    if( pKey1<pKey2 ) return -1;
    return 1;
}

/*
** Hash and comparison functions when the mode is SQLITE_HASH_STRING
*/
static int strHash(const void *pKey, int nKey){
    return sqliteHashNoCase((const char*)pKey, nKey);
}
static int strCompare(const void *pKey1, int n1, const void *pKey2, int n2){
    if( n1!=n2 ) return n2-n1;
    return sqliteStrNICmp((const char*)pKey1,(const char*)pKey2,n1);
}

/*
** Hash and comparison functions when the mode is SQLITE_HASH_BINARY
*/
static int binHash(const void *pKey, int nKey){
    int h = 0;
    const char *z = (const char *)pKey;
    while( nKey-- > 0 ){
        h = (h<<3) ^ h ^ *(z++);
    }
    if( h<0 ) h = -h;
    return h;
}
static int binCompare(const void *pKey1, int n1, const void *pKey2, int n2){
    if( n1!=n2 ) return n2-n1;
    return memcmp(pKey1,pKey2,n1);
}

/*
** Return a pointer to the appropriate hash function given the key class.
**
** The C syntax in this function definition may be unfamilar to some
** programmers, so we provide the following additional explanation:
**
** The name of the function is "hashFunction".  The function takes a
** single parameter "keyClass".  The return value of hashFunction()
** is a pointer to another function.  Specifically, the return value
** of hashFunction() is a pointer to a function that takes two parameters
** with types "const void*" and "int" and returns an "int".
*/
static int (*hashFunction(int keyClass))(const void*,int){
    switch( keyClass ){
        case SQLITE_HASH_INT:     return &intHash;
        case SQLITE_HASH_POINTER: return &ptrHash;
        case SQLITE_HASH_STRING:  return &strHash;
        case SQLITE_HASH_BINARY:  return &binHash;;
        default: break;
    }
    return 0;
}

/*
** Return a pointer to the appropriate hash function given the key class.
**
** For help in interpreted the obscure C code in the function definition,
** see the header comment on the previous function.
*/
static int (*compareFunction(int keyClass))(const void*,int,const void*,int){
    switch( keyClass ){
        case SQLITE_HASH_INT:     return &intCompare;
        case SQLITE_HASH_POINTER: return &ptrCompare;
        case SQLITE_HASH_STRING:  return &strCompare;
        case SQLITE_HASH_BINARY:  return &binCompare;
        default: break;
    }
    return 0;
}


/* Resize the hash table so that it cantains "new_size" buckets.
** "new_size" must be a power of 2.  The hash table might fail
** to resize if sqliteMalloc() fails.
*/
static void rehash(Hash *pH, int new_size){
    struct _ht *new_ht;            /* The new hash table */
    HashElem *elem, *next_elem;    /* For looping over existing elements */
    HashElem *x;                   /* Element being copied to new hash table */
    int (*xHash)(const void*,int); /* The hash function */

    assert( (new_size & (new_size-1))==0 );
    new_ht = (struct _ht *)sqliteMalloc( new_size*sizeof(struct _ht) );
    if( new_ht==0 ) return;
    if( pH->ht ) sqliteFree(pH->ht);
    pH->ht = new_ht;
    pH->htsize = new_size;
    xHash = hashFunction(pH->keyClass);
    for(elem=pH->first, pH->first=0; elem; elem = next_elem){
        int h = (*xHash)(elem->pKey, elem->nKey) & (new_size-1);
        next_elem = elem->next;
        x = new_ht[h].chain;
        if( x ){
            elem->next = x;
            elem->prev = x->prev;
            if( x->prev ) x->prev->next = elem;
            else          pH->first = elem;
            x->prev = elem;
        }else{
            elem->next = pH->first;
            if( pH->first ) pH->first->prev = elem;
            elem->prev = 0;
            pH->first = elem;
        }
        new_ht[h].chain = elem;
        new_ht[h].count++;
    }
}

/* This function (for internal use only) locates an element in an
** hash table that matches the given key.  The hash for this key has
** already been computed and is passed as the 4th parameter.
*/
static HashElem *findElementGivenHash(
        const Hash *pH,     /* The pH to be searched */
        const void *pKey,   /* The key we are searching for */
        int nKey,
        int h               /* The hash for this key. */
){
    HashElem *elem;                /* Used to loop thru the element list */
    int count;                     /* Number of elements left to test */
    int (*xCompare)(const void*,int,const void*,int);  /* comparison function */

    if( pH->ht ){
        elem = pH->ht[h].chain;
        count = pH->ht[h].count;
        xCompare = compareFunction(pH->keyClass);
        while( count-- && elem ){
            if( (*xCompare)(elem->pKey,elem->nKey,pKey,nKey)==0 ){
                return elem;
            }
            elem = elem->next;
        }
    }
    return 0;
}

/* Remove a single entry from the hash table given a pointer to that
** element and a hash on the element's key.
*/
static void removeElementGivenHash(
        Hash *pH,         /* The pH containing "elem" */
        HashElem* elem,   /* The element to be removed from the pH */
        int h             /* Hash value for the element */
){
    if( elem->prev ){
        elem->prev->next = elem->next;
    }else{
        pH->first = elem->next;
    }
    if( elem->next ){
        elem->next->prev = elem->prev;
    }
    if( pH->ht[h].chain==elem ){
        pH->ht[h].chain = elem->next;
    }
    pH->ht[h].count--;
    if( pH->ht[h].count<=0 ){
        pH->ht[h].chain = 0;
    }
    if( pH->copyKey && elem->pKey ){
        sqliteFree(elem->pKey);
    }
    sqliteFree( elem );
    pH->count--;
}

/* Attempt to locate an element of the hash table pH with a key
** that matches pKey,nKey.  Return the data for this element if it is
** found, or NULL if there is no match.
*/
void *sqliteHashFind(const Hash *pH, const void *pKey, int nKey){
    int h;             /* A hash on key */
    HashElem *elem;    /* The element that matches key */
    int (*xHash)(const void*,int);  /* The hash function */

    if( pH==0 || pH->ht==0 ) return 0;
    xHash = hashFunction(pH->keyClass);
    assert( xHash!=0 );
    h = (*xHash)(pKey,nKey);
    assert( (pH->htsize & (pH->htsize-1))==0 );
    elem = findElementGivenHash(pH,pKey,nKey, h & (pH->htsize-1));
    return elem ? elem->data : 0;
}

/* Insert an element into the hash table pH.  The key is pKey,nKey
** and the data is "data".
**
** If no element exists with a matching key, then a new
** element is created.  A copy of the key is made if the copyKey
** flag is set.  NULL is returned.
**
** If another element already exists with the same key, then the
** new data replaces the old data and the old data is returned.
** The key is not copied in this instance.  If a malloc fails, then
** the new data is returned and the hash table is unchanged.
**
** If the "data" parameter to this function is NULL, then the
** element corresponding to "key" is removed from the hash table.
*/
void *sqliteHashInsert(Hash *pH, const void *pKey, int nKey, void *data){
    int hraw;             /* Raw hash value of the key */
    int h;                /* the hash of the key modulo hash table size */
    HashElem *elem;       /* Used to loop thru the element list */
    HashElem *new_elem;   /* New element added to the pH */
    int (*xHash)(const void*,int);  /* The hash function */

    assert( pH!=0 );
    xHash = hashFunction(pH->keyClass);
    assert( xHash!=0 );
    hraw = (*xHash)(pKey, nKey);
    assert( (pH->htsize & (pH->htsize-1))==0 );
    h = hraw & (pH->htsize-1);
    elem = findElementGivenHash(pH,pKey,nKey,h);
    if( elem ){
        void *old_data = elem->data;
        if( data==0 ){
            removeElementGivenHash(pH,elem,h);
        }else{
            elem->data = data;
        }
        return old_data;
    }
    if( data==0 ) return 0;
    new_elem = (HashElem*)sqliteMalloc( sizeof(HashElem) );
    if( new_elem==0 ) return data;
    if( pH->copyKey && pKey!=0 ){
        new_elem->pKey = sqliteMalloc( nKey );
        if( new_elem->pKey==0 ){
            sqliteFree(new_elem);
            return data;
        }
        memcpy((void*)new_elem->pKey, pKey, nKey);
    }else{
        new_elem->pKey = (void*)pKey;
    }
    new_elem->nKey = nKey;
    pH->count++;
    if( pH->htsize==0 ) rehash(pH,8);
    if( pH->htsize==0 ){
        pH->count = 0;
        sqliteFree(new_elem);
        return data;
    }
    if( pH->count > pH->htsize ){
        rehash(pH,pH->htsize*2);
    }
    assert( (pH->htsize & (pH->htsize-1))==0 );
    h = hraw & (pH->htsize-1);
    elem = pH->ht[h].chain;
    if( elem ){
        new_elem->next = elem;
        new_elem->prev = elem->prev;
        if( elem->prev ){ elem->prev->next = new_elem; }
        else            { pH->first = new_elem; }
        elem->prev = new_elem;
    }else{
        new_elem->next = pH->first;
        new_elem->prev = 0;
        if( pH->first ){ pH->first->prev = new_elem; }
        pH->first = new_elem;
    }
    pH->ht[h].count++;
    pH->ht[h].chain = new_elem;
    new_elem->data = data;
    return 0;
}
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
int sqliteStrICmp(const char *zLeft, const char *zRight){
    register unsigned char *a, *b;
    a = (unsigned char *)zLeft;
    b = (unsigned char *)zRight;
    while( *a!=0 && UpperToLower[*a]==UpperToLower[*b]){ a++; b++; }
    return *a - *b;
}
int sqliteStrNICmp(const char *zLeft, const char *zRight, int N){
    register unsigned char *a, *b;
    a = (unsigned char *)zLeft;
    b = (unsigned char *)zRight;
    while( N-- > 0 && *a!=0 && UpperToLower[*a]==UpperToLower[*b]){ a++; b++; }
    return N<0 ? 0 : *a - *b;
}