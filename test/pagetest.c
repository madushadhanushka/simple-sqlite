#include "pager.h"
#include "os.h"
#include <stdio.h>
#include <assert.h>

/**
 * Test Pager implemetation by reading and writing into a page
 * Step 1 : Open new file and create three pages
 * Step 2 : Write data into three different pages and save it on the file
 * Step 3 : Read pages to make sure changes commited
 * Step 4 : Write data into the third page and before commit the changes, rollback to the previous state
 *
 * @param fileLocation
 */
void testPager(char **fileLocation) {
    printf("\nStart running Pager tests\n");
    char zBuf[100];
    int rc;
    Pager *pPager;
    void *pPage;

    /** Step 1 : Open new file and create three pages **/
    // Open new database file in given location with 10 max in memory cache and 0 extra byte append to each
    // in memory page
    rc = sqlitepager_open(&pPager, *fileLocation, 10, 0);

    // Create page with given page numbers
    // page numbers should be starts from one
    rc = sqlitepager_get(pPager, 1, &pPage);
    rc = sqlitepager_get(pPager, 2, &pPage);
    rc = sqlitepager_get(pPager, 3, &pPage);


    /** Step 2 : Write data into three different pages and save it on the file **/
    // Look for the first page
    pPage = sqlitepager_lookup(pPager, 1);
    // Start write page
    rc = sqlitepager_write(pPage);
    // Add string into the page
    strncpy((char*)pPage, "Page One", SQLITE_PAGE_SIZE-1);
    // Commit page data into the file
    rc = sqlitepager_commit(pPager);

    pPage = sqlitepager_lookup(pPager, 2);
    rc = sqlitepager_write(pPage);
    strncpy((char*)pPage, "Page Two", SQLITE_PAGE_SIZE-1);
    rc = sqlitepager_commit(pPager);

    pPage = sqlitepager_lookup(pPager, 3);
    rc = sqlitepager_write(pPage);
    strncpy((char*)pPage, "Page Three", SQLITE_PAGE_SIZE-1);
    rc = sqlitepager_commit(pPager);

    /** Step 3 : Read pages to make sure changes commited **/
    sqlitepager_get(pPager, 1, &pPage);
    memcpy(zBuf, pPage, sizeof(zBuf));
    printf("Read page result1: %s\n", zBuf);
    assert(strncmp(zBuf, "Page One", 8) == 0);

    sqlitepager_get(pPager, 2, &pPage);
    memcpy(zBuf, pPage, sizeof(zBuf));
    printf("Read page result2: %s\n", zBuf);
    assert(strncmp(zBuf, "Page Two", 8) == 0);

    sqlitepager_get(pPager, 3, &pPage);
    memcpy(zBuf, pPage, sizeof(zBuf));
    printf("Read page result3: %s\n", zBuf);
    assert(strncmp(zBuf, "Page Three", 10) == 0);

    /** Step 4 : Write data into the third page and before commit the changes, rollback to the previous state **/
    pPage = sqlitepager_lookup(pPager, 3);
    rc = sqlitepager_write(pPage);
    strncpy((char*)pPage, "Page test rollback", SQLITE_PAGE_SIZE-1);
    // Rallback changes to the previous state
    sqlitepager_rollback(pPager);
    rc = sqlitepager_commit(pPager);

    sqlitepager_get(pPager, 3, &pPage);
    memcpy(zBuf, pPage, sizeof(zBuf));
    printf("Read page result4: %s\n", zBuf);
    assert(strncmp(zBuf, "Page Three", 10) == 0);


    printf("Page count: %d\n",sqlitepager_pagecount(pPager));
    assert(sqlitepager_pagecount(pPager) == 3);

    sqlitepager_close(pPager);
    sqliteOsDelete(*fileLocation);
}