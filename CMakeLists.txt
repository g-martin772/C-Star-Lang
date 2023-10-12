cmake_minimum_required(VERSION 3.20)

project(C-Star-Compiler)

add_executable(csc
        src/main.cpp
        src/main.cpp
        src/Compiler/Lexer.cpp
        src/Compiler/Lexer.h
        src/IO/File.cpp
        src/IO/File.h
)