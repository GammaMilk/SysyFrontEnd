cmake_minimum_required(VERSION 3.15)

project(SysyLex)
set(CMAKE_CXX_STANDARD 17)
set(THREADS_PREFER_PTHREAD_FLAG ON)
#SET(CMAKE_BUILD_TYPE Debug)

find_package(Threads REQUIRED)

# add antlr4
include_directories(/usr/local/include/antlr4-runtime)
link_directories(/usr/local/lib)
# multi-platform dependency
if (WIN32)
    include_directories("C:\\Program Files (x86)\\LIBANTLR4\\include\\antlr4-runtime")
    link_directories("F:\\Downloads\\antlr4-cpp-runtime-4.13.0-source\\cmake-build-debug-gcc112\\runtime")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MT")
endif (WIN32)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
# -Wl,--stack,<stack_size_in_bytes>
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--stack,4000000")
# add source files like CBaseListener.cpp, CBaseVisitor.cpp, etc.
file(GLOB SRC_LIST_SYSY ${PROJECT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST_SYSY} SysyVisitor_Cond.cpp SysyVisitor_Arithmetic.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC /usr/local/include/antlr4-runtime)
target_link_libraries(${PROJECT_NAME} antlr4-runtime)
#target_link_libraries(${PROJECT_NAME} Threads::Threads)
if(WIN32)
#    target_link_libraries(${PROJECT_NAME} antlr4-runtime.lib)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "F:/Downloads/antlr4-cpp-runtime-4.13.0-source/cmake-build-debug-gcc112/runtime/libantlr4-runtime.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMENT "Copying libantlr4-runtime.dll to executable directory"
            )
endif()