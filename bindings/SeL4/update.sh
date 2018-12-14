#!/bin/bash


# SeL4 kernel repo
if [ -d "kernel" ]; then
	git -C kernel/ pull
else 
	git clone https://github.com/seL4/seL4.git kernel
fi

# SeL4 build tools
if [ -d "tools" ]; then
        git -C tools/ pull
else 
	git clone https://github.com/seL4/seL4_tools.git tools
fi


# create projects folder if needed
#if [ ! -d "projects" ]; then
#	mkdir projects
#fi


if [ -d "projects/util_libs/" ]; then
	git -C projects/util_libs/ pull
else
	git clone https://github.com/seL4/util_libs.git projects/util_libs
fi

if [ -d "projects/musllibc/" ]; then
        git -C projects/musllibc/ pull
else 
	git clone https://github.com/seL4/musllibc.git projects/musllibc
fi

if [ -d "projects/seL4_libs/" ]; then
        git -C projects/seL4_libs/ pull
else 
	git clone https://github.com/seL4/seL4_libs.git projects/seL4_libs
fi




if [ ! -f "init-build.sh" ]; then
	ln -s tools/cmake-tool/init-build.sh init-build.sh
fi
