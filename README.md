[![CMake](https://github.com/madushadhanushka/simple-sqlite/actions/workflows/cmake.yml/badge.svg)](https://github.com/madushadhanushka/simple-sqlite/actions/workflows/cmake.yml)

Simple SQLite Backend
=========
This repository is to understand the basic backend implementation of SQLite database. Codes are extracted from SQLite 2.5.0 version and initial implementation may have changed. Here, testing codes are added into the test directory and all SQLite implementation are included into the core directory. Tests included to test VFS, Pager and Btree functionality.

## Reading materials
[Introduction to the indexing and Btree](https://github.com/madushadhanushka/simple-sqlite/blob/master/blog/introduction_to_btree_and_bplustree.md)

[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/bOFlJ0oUjU4/0.jpg)](https://www.youtube.com/watch?v=bOFlJ0oUjU4)

Compile and running instructions for Unix
======

1. Take a clone of this project into your local folder 
<br/>`git clone https://github.com/madushadhanushka/simple-sqlite.git`
2. Move inside the repository
<br/>`cd simple-sqlite`
3. Build the project with cmake
<br/>`cmake -DCMAKE_BUILD_TYPE=Debug`
4. Build with make
<br/>`make`
5. Run test 
<br/>`./simple_sqlite`

Use as your own Paging library
========
You can use paging functionality to create your own file accessing implementation. You can find out sample usecase in [pager.c](https://github.com/madushadhanushka/simple-sqlite/blob/master/test/pagetest.c).

Open pager with the given file name<br/>
`int sqlitepager_open(Pager **ppPager,const char *zFilename,int nPage,int nEx);`

Get page specified by the page number<br/>
`int sqlitepager_get(Pager *pPager, Pgno pgno, void **ppPage);`


Start to write data into a page specified in pData<br/>
`int sqlitepager_write(void *pData);`

Commit page changes into the file<br/>
`int sqlitepager_commit(Pager*);`

Close the connection to the file<br/>
`int sqlitepager_close(Pager *pPager);`


Contribute!
===========
If you see somthing wrong, Inclomplete content or improvement, you are welcome to create issue or send pull request to this repository in order to improve the code quality and the readability.

Following are the top contributors of this repository. Keep coding! 👨🏻‍💻👩🏻‍💻

<a href = "https://github.com/madushadhanushka/simple-sqlite/graphs/contributors">
  <img src = "https://contrib.rocks/image?repo=madushadhanushka/simple-sqlite"/>
</a>

