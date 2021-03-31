cmake_minimum_required(VERSION 3.17)
project(BTPClion)

set(CMAKE_CXX_STANDARD 14)
find_package (Threads)

set(
        SOURCE_FILES
        main.cpp
        Peer.cpp
        cluster.cc
        Subtracker.cpp
)
add_executable(BTPClion ${SOURCE_FILES})
target_link_libraries(BTPClion PRIVATE Threads::Threads)



#set(GML ./readgml)
#include_directories(${GML})



#set(
#        SOURCE_FILES
#        ${GML}/readgml.c
#        comparison_deletion.cpp
#
#)
#add_executable(Clion ${SOURCE_FILES})
