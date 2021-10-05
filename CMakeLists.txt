cmake_minimum_required(VERSION 3.12)
project(simple_sqlite C)

set(CMAKE_C_STANDARD 11)

# issue #1
if(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
        set(CMAKE_C_FLAGS "-Wno-implicit-function-declaration")
endif()

include_directories(core)

add_executable(simple_sqlite
        core/hash.c
        core/hash.h
        core/os.c
        core/os.h
        core/random.c
        core/random.h
        core/sqlite.h
        core/sqliteInt.h
        core/util.c
        core/util.h
        core/pager.c
        core/pager.h
        core/btree.c
        core/btree.h
        main.c test/vfstest.c test/vfstest.h test/pagetest.c test/pagetest.h test/btreetest.c test/btreetest.h)
