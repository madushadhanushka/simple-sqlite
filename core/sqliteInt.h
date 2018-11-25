
#ifndef UINT32_TYPE
# define UINT32_TYPE unsigned int
#endif
#ifndef UINT16_TYPE
# define UINT16_TYPE unsigned short int
#endif
#ifndef UINT8_TYPE
# define UINT8_TYPE unsigned char
#endif
#ifndef INTPTR_TYPE
# define INTPTR_TYPE int
#endif
typedef UINT32_TYPE u32;           /* 4-byte unsigned integer */
typedef UINT16_TYPE u16;           /* 2-byte unsigned integer */
typedef UINT8_TYPE u8;             /* 1-byte unsigned integer */
typedef INTPTR_TYPE ptr;           /* Big enough to hold a pointer */
typedef unsigned INTPTR_TYPE uptr; /* Big enough to hold a pointer */
extern int sqlite_malloc_failed;
/*
** This macro casts a pointer to an integer.  Useful for doing
** pointer arithmetic.
*/
#define Addr(X)  ((uptr)X)
