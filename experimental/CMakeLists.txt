cmake_minimum_required(VERSION 2.8)
project(test1)
find_package(qibuild)


# Create a executable named test1
# with the source file: main.cpp
qi_create_lib(test1 SHARED main.cpp helloworld.cpp SUBFOLDER naoqi)

