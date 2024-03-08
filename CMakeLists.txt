# CMake 最小版本
cmake_minimum_required(VERSION 2.8)
# CMake 定义项目名称
project(zhou)               


# --------------------------------------------------------


# CMake 过程中输出 Makefile 执行过程中详细的命令
set(CMAKE_VERBOSE_MAKEFILE ON)
# 自定义 Make 编译参数
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -g -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")


# --------------------------------------------------------


# 添加编译时 include 路径
include_directories(.)


# --------------------------------------------------------


# 将源码文件设置到变量 LIB_SRC
set(LIB_SRC
    zhou/log/log.cc
    zhou/utils/util.cc
    zhou/thread/thread.cc
    zhou/thread/semaphore.cc    # 同步
    # zhou/thread/lock.cc         # 互斥
    )


# --------------------------------------------------------


# --- 动态库 --- 

# 添加 .so 共享库 zhou ， 其使用 LIB_SRC 变量中的源文件构建而成
add_library(zhou SHARED ${LIB_SRC})


# --- 静态库 --- 

# 生成 .a 静态库 zhou_static ， 其使用 LIB_SRC 变量中的源文件构建而成
# add_library(zhou_static STATIC ${LIB_SRC})
# 将静态库 zhou_static 输出为 zhou.a
# set_target_properties(zhou_static PROPERTIES OUTPUT_NAME "zhou")


# --------------------------------------------------------


# 生成测试文件 test                    
add_executable(test tests/test.cc)
# 测试文件 test 依赖于我们生成的共享库 zhou.so  
add_dependencies(test zhou)      
# 其链接时需要链接到 zhou.so 共享库          
target_link_libraries(test zhou) 

# 生成测试文件 test                    
set(TEST_EXEC test_thread)
add_executable(${TEST_EXEC} tests/test_thread.cc)
# 测试文件 test 依赖于我们生成的共享库 zhou.so  
add_dependencies(${TEST_EXEC} zhou)      
# 其链接时需要链接到 zhou.so 共享库          
target_link_libraries(${TEST_EXEC} zhou) 


# --------------------------------------------------------

# 可执行（测试）文件生成路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 库文件生成路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

