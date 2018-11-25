
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

    testVFS(&fileLocation);
    testPager(&fileLocation);
    testBtree(&fileLocation);
    return 0;
}