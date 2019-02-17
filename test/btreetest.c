#include "btree.h"
#include "os.h"

void testBtree(char **fileLocation) {
    printf("\nStart running Btree tests\n");
    Btree *pBt;
    char zBuf[100];
    int rc;
    rc = sqliteBtreeOpen(*fileLocation, 0666, 10, &pBt);
    printf("Error while opening sqlite database %d\n", rc);
    if( rc!=0 ){
        return;
    }

    int iTable;
    sqliteBtreeBeginTrans(pBt);
    rc = sqliteBtreeCreateTable(pBt, &iTable);          // Create new table and return table id to iTable
    printf("Root Page id: %d\n",iTable);
    sqliteBtreeCommit(pBt);                         // Commit changes to file system

    BtCursor *pCur;
    int wrFlag = 1; // Read and write
    rc = sqliteBtreeCursor(pBt, iTable, wrFlag, &pCur);         // Get cursor to Btree

    char *keyToWrote = "key1";
    char *valToWrite = "value1";
    sqliteBtreeBeginTrans(pBt);
    sqliteBtreeInsert(pCur, keyToWrote, strlen(keyToWrote), valToWrite, strlen(valToWrite));    // Insert value to Btree
    sqliteBtreeCommit(pBt);

    char *keyToWrote2 = "key20345094830594830580398485739470583049504857237487329580438059840593875987239473850943850997593287493794879285794835394805980239";
    char *valToWrite2 = "value2a4444499000233234234324234234384023984092830498203948283749623849938423804598059834-059340-5934-0959-20394-203768723487248";
    sqliteBtreeBeginTrans(pBt);
    sqliteBtreeInsert(pCur, keyToWrote2, strlen(keyToWrote2), valToWrite2, strlen(valToWrite2)); // Insert value to Btree
    sqliteBtreeCommit(pBt);

    char *keyToWrote4 = "key4";
    char *valToWrite4 = "value4";
    sqliteBtreeBeginTrans(pBt);
    sqliteBtreeInsert(pCur, keyToWrote4, strlen(keyToWrote4), valToWrite4, strlen(valToWrite4));
    sqliteBtreeCommit(pBt);

    char *keyToWrote3 = "key3";
    char *valToWrite3 = "value3";
    sqliteBtreeBeginTrans(pBt);
    sqliteBtreeInsert(pCur, keyToWrote3, strlen(keyToWrote3), valToWrite3, strlen(valToWrite3));
    sqliteBtreeCommit(pBt);


    char *keyToWrote5 = "key5";
    char *valToWrite5 = "value5";
    sqliteBtreeBeginTrans(pBt);
    sqliteBtreeInsert(pCur, keyToWrote5, strlen(keyToWrote5), valToWrite5, strlen(valToWrite5));
    sqliteBtreeCommit(pBt);

    char *keyToWrote10 = "key3";
    int res;
    rc = sqliteBtreeMoveto(pCur, keyToWrote10, strlen(keyToWrote10), &res);


    int keySize;
    int dataSize;
    // Get key size of cursor is pointing to
    sqliteBtreeKeySize(pCur, &keySize);
    // Read the key and write it into the buffer
    rc = sqliteBtreeKey(pCur, 0, keySize, zBuf);
    printf("Pointed key is: %s\n", zBuf);

    // Get data size of cursor is pointing to
    sqliteBtreeDataSize(pCur, &dataSize);
    // Read the data and write it into the buffer
    rc = sqliteBtreeData(pCur, 0, dataSize, zBuf);
    printf("Pointed data is: %s\n", zBuf);

    rc = sqliteBtreeNext(pCur, 0);

    // Get key size of cursor is pointing to
    sqliteBtreeKeySize(pCur, &keySize);
    // Read the key and write it into the buffer
    rc = sqliteBtreeKey(pCur, 0, keySize, zBuf);
    printf("Next pointed key is: %s\n", zBuf);

    // Get data size of cursor is pointing to
    sqliteBtreeDataSize(pCur, &dataSize);
    // Read the data and write it into the buffer
    rc = sqliteBtreeData(pCur, 0, dataSize, zBuf);
    printf("Next pointed data is: %s\n", zBuf);

    int pRes = 0;
    // Move cursor to first record
    rc = sqliteBtreeFirst(pCur, &pRes);

    // Get key size of cursor is pointing to
    sqliteBtreeKeySize(pCur, &keySize);
    // Read the key and write it into the buffer
    rc = sqliteBtreeKey(pCur, 0, keySize, zBuf);
    printf("First key is: %s\n", zBuf);

    // Get data size of cursor is pointing to
    sqliteBtreeDataSize(pCur, &dataSize);
    // Read the data and write it into the buffer
    rc = sqliteBtreeData(pCur, 0, dataSize, zBuf);
    printf("First data is: %s\n", zBuf);

    int aMeta[SQLITE_N_BTREE_META];
    rc = sqliteBtreeGetMeta(pBt, aMeta);
    printf("meta data: %d\n" ,aMeta);

    rc = sqliteBtreeCloseCursor(pCur);
    rc = sqliteBtreeCommit(pBt);

    //sqliteOsDelete(*fileLocation);
}