cmake_minimum_required(VERSION 3.15)

project(SysyLex)
set(CMAKE_CXX_STANDARD 17)

# add antlr4
include_directories(/usr/local/include/antlr4-runtime)
link_directories(/usr/local/lib)

# add source files like CBaseListener.cpp, CBaseVisitor.cpp, etc.
file(GLOB SRC_LIST_SYSY ${PROJECT_SOURCE_DIR}/Sysy*.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST_SYSY} IRBuilder.cpp IRBuilder.h IRSen.cpp IRSen.h)
target_include_directories(${PROJECT_NAME} PUBLIC /usr/local/include/antlr4-runtime)
target_link_libraries(${PROJECT_NAME} antlr4-runtime)
