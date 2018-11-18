cmake_minimum_required(VERSION 3.12)
project(simple_sqlite C)

set(CMAKE_C_STANDARD 11)

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
        main.c)
