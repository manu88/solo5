cmake_minimum_required(VERSION 3.7.2)

project(root C) 


file(GLOB deps src/*.c )
list(SORT deps)

add_executable(root ${deps})

#add_executable(root src/main.c src/solo5.c)

target_link_libraries(root sel4muslcsys  muslc sel4allocman)

target_include_directories(root PUBLIC ../../include/solo5/)
 


# Set this image as the rootserver
DeclareRootserver(root)
