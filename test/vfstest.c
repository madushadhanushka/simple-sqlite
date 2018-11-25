#include "../core/os.h"
#include "vfstest.h"
#include <stdio.h>
#include <assert.h>

/**
 * Test VFS implimetation by writing and reading back data into a file specified by fileLocation.
 * Step 1 : Open a new file
 * Step 2 : Seek for the offset 0 and write "hello this is vfs test"
 * Step 3 : Seek for the offset 6 and read 4 bytes
 * Step 4 : Overwrite offset 11 with ""is override""
 * Step 5 : Close the connection and delete files
 *
 * @param fileLocation
 */
void testVFS(char **fileLocation) {
    printf("\nStart running VFS tests\n");
    OsFile fd;
    int readOnly = 0;

    /** Step 1 **/
    printf("Open file from %s\n", *fileLocation);
    int rc = sqliteOsOpenReadWrite(*fileLocation, &fd, &readOnly);
    printf("File open status %d\n" ,rc);
    int fileExist = sqliteOsFileExists(*fileLocation);
    printf("Does file exist %d\n", fileExist);

    // File should be exist when open a file.
    assert(fileExist == 1);

    /** Step 2 **/
    // Seek offset 0 on the database fle
    sqliteOsSeek(&fd, 0);
    char *valueToWrite = "hello this is vfs test";
    // write string to start offset 0 with the length of 22 byte
    sqliteOsWrite(&fd, valueToWrite, 22);

    /** step 3 **/
    // Seek offset 6 on the database file
    sqliteOsSeek(&fd, 6);
    char readBuffer[16];
    // Read 4 bytes from offset 6 and write it into readBuffer
    sqliteOsRead(&fd, &readBuffer, 4);
    printf("Read from offset: %s\n", readBuffer);

    // Extracted data should be "this"
    assert(strncmp(readBuffer, "this", 4) == 0);

    /** Step 4 **/
    // Seek offset 11 on database file
    sqliteOsSeek(&fd, 11);
    char *valueToOverride = "is override";
    // Write string to offset 11
    sqliteOsWrite(&fd, valueToOverride,11);

    // Seek offset 0 on database file
    sqliteOsSeek(&fd, 0);
    char finalResult[22];
    // Read the string from offset 0 and write the result into finalResult
    sqliteOsRead(&fd, &finalResult,22);
    printf("Final result: %s\n", finalResult);

    // After override the initial string
    assert(strncmp(finalResult, "hello this is override", 22) == 0);

    /** Step 5 **/
    // Close the file connection
    rc = sqliteOsClose(&fd);
    printf("File close status: %d\n", rc);

    // Delete the file
    sqliteOsDelete(*fileLocation);

    // Check if file exist
    fileExist = sqliteOsFileExists(*fileLocation);
    printf("Does file exist after delete %d\n", fileExist);

    assert(fileExist == 0);

}
