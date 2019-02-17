
#include "core/os.h"
#include "test/vfstest.h"
#include "test/pagetest.h"
#include "test/btreetest.h"

int main() {
    /*
     * Since file location is different for Unix and Windows, Here fileLocation set according to the OS
     */
    #if OS_UNIX
    char *fileLocation = "/tmp/test.db";
    #endif
    #if OS_WIN
    char *fileLocation = "C://test.db";
    #endif

    /* Tests for VFS functionality */
    testVFS(&fileLocation);

    /* Tests for Pager functionality */
    testPager(&fileLocation);

    /* Tests for Btree functionality */
    testBtree(&fileLocation);
    return 0;
}