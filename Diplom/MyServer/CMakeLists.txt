cmake_minimum_required(VERSION 3.22.0)

project(MyServer)

set(WINDOWS_EXPORT_ALL_SYMBOLS ON)

add_executable(MyServer MyServer.cpp)

set (PostgreSQL_ADDITIONAL_VERSIONS "16")

target_compile_features(MyServer PRIVATE cxx_std_17)

add_subdirectory ("L:\\Prog\\LibPqxx\\libpqxx-7.9.0" pqxx-build)

target_link_libraries (MyServer pqxx)

set(BOOST_ROOT "C:\\Source\\boost_1_87_0")

set (Boost_NO_SYSTEM_PATHS ON)
set (Boost_USE_MULTITHREADED ON)
set (Boost_USE_STATIC_LIBS ON)
set (Boost_USE_STATIC_RUNTIME OFF)
set (BOOST_ALL_DYN_LINK OFF)

find_package(Boost REQUIRED COMPONENTS system thread)

if (Boost_FOUND)
    include_directories (SYSTEM ${Boost_INCLUDE_DIR}) # подключаем заголовочные файлы    
    target_link_libraries (MyServer ${Boost_LIBRARIES}) # подключаем библиотеку
endif ()