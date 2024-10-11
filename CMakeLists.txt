# CMake 最小版本
cmake_minimum_required(VERSION 3.12)
# CMake 定义项目名称
project(zhou)               


# --------------------------------------------------------


# CMake 过程中输出 Makefile 执行过程中详细的命令
set(CMAKE_VERBOSE_MAKEFILE ON)
# 自定义 Make 编译参数
# set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -g -ggdb -pg -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -g -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")


# --------------------------------------------------------


# 添加编译时 include 路径
include_directories(.)


# --------------------------------------------------------


# 将源码文件设置到变量 LIB_SRC
set(LIB_SRC
    zhou/log/log.cc                     # 日志模块
    zhou/utils/util.cc
    zhou/thread/thread.cc               # 线程模块
    zhou/thread/semaphore.cc                # 同步
    # zhou/thread/lock.cc                   # 互斥
    zhou/fiber/fiber.cc                 # 协程模块
    zhou/fiber/stack_allocator.cc           # 协程栈分配器
    zhou/fiber/scheduler.cc                 # 协程调度器（线程池）
    zhou/fiber/iomanager.cc                 # IO 协程调度器 - 协程 + epoll 配合
    zhou/fiber/iomanager_fd_ctx.cc          # IO 协程调度器 - event 上下文
    zhou/fiber/iomanager_timer.cc
    zhou/timer/timer.cc                 # 定时器
    zhou/timer/timer_manager.cc             # 定时器管理器
    zhou/hook/hook.cc                   # IO hook 模块
    zhou/hook/hook_sleep.cc                 # sleep hook: sleep usleep nanosleep
    zhou/hook/fd_ctx.cc                     # socket etc ... IO 管理
    zhou/hook/fd_manager.cc                 
    zhou/hook/hook_socket.cc                # socket hook: socket connect accept. 关于 bind 和 listen 会在 Socket 中封装
    zhou/hook/hook_read.cc                  # read hook:  read  readv  recv recvfrom recvmsg
    zhou/hook/hook_write.cc                 # write hook: write writev send sendto   sendmsg
    zhou/hook/hook_fdctl.cc                 # fcntl hook: fcntl
    zhou/net/address.cc                 # socket 地址封装
    zhou/net/ipv4_address.cc                # IPv4 socket 地址封装
    zhou/net/ipv6_address.cc                # IPv6 socket 地址封装
    zhou/net/unix_address.cc                # Unix socket 地址封装
    zhou/net/socket.cc                  # socket 封装
    zhou/http/http_parser/http.cc
    zhou/http/http_parser/http_parser.cc
    zhou/http/http_parser/http_request/http_request.cc
    zhou/http/http_parser/http_request/ragel/http11_parser.rl.cc
    zhou/http/http_parser/http_request/http_request_parser.cc
    zhou/http/http_parser/http_request/http_request_callback.cc
    zhou/http/http_parser/http_response/http_response.cc
    zhou/http/http_parser/http_response/ragel/httpclient_parser.rl.cc
    zhou/http/http_parser/http_response/http_response_parser.cc
    )


# --------------------------------------------------------

include (cmake/utils.cmake)





# 添加 .so 共享库 zhou ， 其使用 LIB_SRC 变量中的源文件构建而成
add_library(zhou SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(zhou) #__FILE__


# --- 静态库 --- 

# 生成 .a 静态库 zhou_static ， 其使用 LIB_SRC 变量中的源文件构建而成
# add_library(zhou_static STATIC ${LIB_SRC})
# 将静态库 zhou_static 输出为 zhou.a
# set_target_properties(zhou_static PROPERTIES OUTPUT_NAME "zhou")




# --- 动态库 --- 
set(BOOST_ROOT "/home/zhou/workspace/web_server/library/boost")
set(BOOST_INCLUDEDIR "/home/zhou/workspace/web_server/library/boost/boost")
set(BOOST_LIBRARYDIR "/home/zhou/workspace/web_server/library/boost/stage/lib")
set(Boost_NO_SYSTEM_PATHS ON)

find_package(Boost REQUIRED COMPONENTS filesystem system)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    target_link_libraries(zhou ${Boost_LIBRARIES})
else()
    message(FATAL_ERROR "Could not find Boost")
endif()


# --------------------------------------------------------


# 生成测试文件 test                    
set(TEST_EXEC test)
add_executable(${TEST_EXEC} tests/${TEST_EXEC}.cc)
# 测试文件 test 依赖于我们生成的共享库 zhou.so  
add_dependencies(${TEST_EXEC} zhou)      
# __FILE__ 重定义
force_redefine_file_macro_for_sources(${TEST_EXEC}) #__FILE__
# 其链接时需要链接到 zhou.so 共享库          
target_link_libraries(${TEST_EXEC} zhou) 


# 生成测试文件 test                    
set(TEST_EXEC test_thread)
add_executable(${TEST_EXEC} tests/${TEST_EXEC}.cc)
# 测试文件 test 依赖于我们生成的共享库 zhou.so  
add_dependencies(${TEST_EXEC} zhou)      
# __FILE__ 重定义
force_redefine_file_macro_for_sources(${TEST_EXEC}) #__FILE__
# 其链接时需要链接到 zhou.so 共享库          
target_link_libraries(${TEST_EXEC} zhou) 

# 生成测试文件 test                    
set(TEST_EXEC test_rwlock)
add_executable(${TEST_EXEC} tests/${TEST_EXEC}.cc)
# 测试文件 test 依赖于我们生成的共享库 zhou.so  
add_dependencies(${TEST_EXEC} zhou)      
# __FILE__ 重定义
force_redefine_file_macro_for_sources(${TEST_EXEC}) #__FILE__
# 其链接时需要链接到 zhou.so 共享库          
target_link_libraries(${TEST_EXEC} zhou) 

# 生成测试文件 test                    
set(TEST_EXEC test_lock)
add_executable(${TEST_EXEC} tests/${TEST_EXEC}.cc)
# 测试文件 test 依赖于我们生成的共享库 zhou.so  
add_dependencies(${TEST_EXEC} zhou)      
# __FILE__ 重定义
force_redefine_file_macro_for_sources(${TEST_EXEC}) #__FILE__
# 其链接时需要链接到 zhou.so 共享库          
target_link_libraries(${TEST_EXEC} zhou) 

# 生成测试文件 test                    
set(TEST_EXEC test_thread_log)
add_executable(${TEST_EXEC} tests/${TEST_EXEC}.cc)
# 测试文件 test 依赖于我们生成的共享库 zhou.so  
add_dependencies(${TEST_EXEC} zhou)      
# __FILE__ 重定义
force_redefine_file_macro_for_sources(${TEST_EXEC}) #__FILE__
# 其链接时需要链接到 zhou.so 共享库          
target_link_libraries(${TEST_EXEC} zhou) 

# 生成测试文件 test                    
set(TEST_EXEC test_util)
add_executable(${TEST_EXEC} tests/${TEST_EXEC}.cc)
# 测试文件 test 依赖于我们生成的共享库 zhou.so  
add_dependencies(${TEST_EXEC} zhou)      
# __FILE__ 重定义
force_redefine_file_macro_for_sources(${TEST_EXEC}) #__FILE__
# 其链接时需要链接到 zhou.so 共享库          
target_link_libraries(${TEST_EXEC} zhou) 

# 生成测试文件 test                    
set(TEST_EXEC test_fiber)
add_executable(${TEST_EXEC} tests/${TEST_EXEC}.cc)
# 测试文件 test 依赖于我们生成的共享库 zhou.so  
add_dependencies(${TEST_EXEC} zhou)      
# __FILE__ 重定义
force_redefine_file_macro_for_sources(${TEST_EXEC}) #__FILE__
# 其链接时需要链接到 zhou.so 共享库          
target_link_libraries(${TEST_EXEC} zhou) 

# 生成测试文件 test                    
set(TEST_EXEC test_thread_fiber)
add_executable(${TEST_EXEC} tests/${TEST_EXEC}.cc)
# 测试文件 test 依赖于我们生成的共享库 zhou.so  
add_dependencies(${TEST_EXEC} zhou)      
# __FILE__ 重定义
force_redefine_file_macro_for_sources(${TEST_EXEC}) #__FILE__
# 其链接时需要链接到 zhou.so 共享库          
target_link_libraries(${TEST_EXEC} zhou) 

# 生成测试文件 test                    
set(TEST_EXEC test_scheduler)
add_executable(${TEST_EXEC} tests/${TEST_EXEC}.cc)
# 测试文件 test 依赖于我们生成的共享库 zhou.so  
add_dependencies(${TEST_EXEC} zhou)      
# __FILE__ 重定义
force_redefine_file_macro_for_sources(${TEST_EXEC}) #__FILE__
# 其链接时需要链接到 zhou.so 共享库          
target_link_libraries(${TEST_EXEC} zhou) 

# 生成测试文件 test                    
set(TEST_EXEC test_iomanager)
add_executable(${TEST_EXEC} tests/${TEST_EXEC}.cc)
# 测试文件 test 依赖于我们生成的共享库 zhou.so  
add_dependencies(${TEST_EXEC} zhou)      
# __FILE__ 重定义
force_redefine_file_macro_for_sources(${TEST_EXEC}) #__FILE__
# 其链接时需要链接到 zhou.so 共享库          
target_link_libraries(${TEST_EXEC} zhou) 

# 生成测试文件 test                    
set(TEST_EXEC test_timer)
add_executable(${TEST_EXEC} tests/${TEST_EXEC}.cc)
# 测试文件 test 依赖于我们生成的共享库 zhou.so  
add_dependencies(${TEST_EXEC} zhou)      
# __FILE__ 重定义
force_redefine_file_macro_for_sources(${TEST_EXEC}) #__FILE__
# 其链接时需要链接到 zhou.so 共享库          
target_link_libraries(${TEST_EXEC} zhou) 

# 生成测试文件 test                    
set(TEST_EXEC test_hook)
add_executable(${TEST_EXEC} tests/${TEST_EXEC}.cc)
# 测试文件 test 依赖于我们生成的共享库 zhou.so  
add_dependencies(${TEST_EXEC} zhou)      
# __FILE__ 重定义
force_redefine_file_macro_for_sources(${TEST_EXEC}) #__FILE__
# 其链接时需要链接到 zhou.so 共享库          
target_link_libraries(${TEST_EXEC} zhou) 

# 生成测试文件 test                    
set(TEST_EXEC test_address)
add_executable(${TEST_EXEC} tests/${TEST_EXEC}.cc)
# 测试文件 test 依赖于我们生成的共享库 zhou.so  
add_dependencies(${TEST_EXEC} zhou)      
# __FILE__ 重定义
force_redefine_file_macro_for_sources(${TEST_EXEC}) #__FILE__
# 其链接时需要链接到 zhou.so 共享库          
target_link_libraries(${TEST_EXEC} zhou) 


# --------------------------------------------------------



# 可执行（测试）文件生成路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# 库文件生成路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

