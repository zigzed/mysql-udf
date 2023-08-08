cmake_minimum_required(VERSION 3.22.1)

project(mysql-udf)

set(CMAKE_CXX_STANDARD 14)

include_directories(/usr/local/include/mysql)

add_executable(unittest
    test.cpp 
    parse_url.h parse_url.cpp
)

add_library(querystring SHARED 
    parse_url.h parse_url.cpp
)
target_compile_definitions(querystring PUBLIC EXPORT_MYSQL_UDF)