
#include <stdio.h>
#include "core/os.h"


int main() {

    #if OS_UNIX
    char *fileLocation = "/tmp/test.db";
    #endif
    #if OS_WIN
    char *fileLocation = "C://test.db";
    #endif

    OsFile fd;
    int readOnly = 0;

    printf("Open file from %s\n", fileLocation);
    int rc = sqliteOsOpenReadWrite(fileLocation, &fd, &readOnly);
    printf("File open status %d\n" ,rc);
    int fileExist = sqliteOsFileExists(fileLocation);
    printf("Does file exist %d\n", fileExist);

    sqliteOsSeek(&fd, 0);
    char *valueToWrite = "hello this is vfs test";
    sqliteOsWrite(&fd, valueToWrite, 22);

    sqliteOsSeek(&fd, 6);
    char readBuffer[16];
    sqliteOsRead(&fd, &readBuffer, 4);
    printf("Read from offset: %s\n", readBuffer);

    sqliteOsSeek(&fd, 11);
    char *valueToOverride = "is override";
    sqliteOsWrite(&fd, valueToOverride,11);

    sqliteOsSeek(&fd, 0);
    char finalResult[22];
    sqliteOsRead(&fd, &finalResult,22);
    printf("Final result: %s\n", finalResult);

    rc = sqliteOsClose(&fd);
    printf("File close status: %d\n", rc);

    sqliteOsDelete(fileLocation);

    fileExist = sqliteOsFileExists(fileLocation);
    printf("Does file exist after delete %d\n", fileExist);

    return 0;
}