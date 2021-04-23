
# CMake 最低版本号要求
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# 项目名
PROJECT(demo_cmake)
# 添加第三方的库
SET(INC_DIR "/usr/local/include/")
SET(LINK_DIR "/usr/local/lib")
set(CMAKE_CXX_COMPILER "/usr/bin/aarch64-linux-gnu-g++")
set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -g -O3 -pthread ${CMAKE_CXX_FLAGS}")
SET(SRC_LIST "${PROJECT_SOURCE_DIR}/src")
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)    #指定要链接的库文件的路径，一般不需要
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)           #设置库文件的输出目录
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)       #设置可执行文件的输出目录

SET(LIB_USE "OFF") 			##################生成库或使用库
if(${LIB_USE} STREQUAL "OFF")

#===================生成库文件和执行文件===========================
# 指定源文件
aux_source_directory(${SRC_LIST} DIR_SRCS)

############################条件判断，是否生成库
set(LIB_OPEN "ON")

if(${LIB_OPEN} STREQUAL "ON")
#生成静态库
add_library(HB_DTU ${DIR_SRCS} )
#生成动态库
add_library(HB_DTU_SO SHARED ${DIR_SRCS} )
endif()

# 指定生成目标
add_executable(${PROJECT_NAME} ${DIR_SRCS})
#=================================================================

elseif(${LIB_USE} STREQUAL "ON")


#==============动态、静态库使用================================
#set(CMAKE_CXX_STANDARD 11)

add_executable(${PROJECT_NAME} ${SRC_LIST}/main.cpp)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBRARY_OUTPUT_PATH}/libHB_DTU.a)
#TARGET_LINK_LIBRARIES(${PROJECT_NAME} libfun.so)
#==============================================================

endif(${LIB_USE} STREQUAL "OFF")

#[[
#LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/lib)
]]
#INCLUDE_DIRECTORIES(${INC_DIR})    # 设置头文件搜索路径
#LINK_DIRECTORIES(${LINK_DIR})    #指定要链接的库文件的路径，一般不需要
# 将第三方的库链接到可执行文件，静态链接
#LINK_LIBRARIES(fun)

# 指定源文件
#aux_source_directory(${SRC_LIST} DIR_SRCS)
#SET(SRC_LIST "main.cpp")
# MESSAGE(STATUS "SRC_LIST :" ${SRC_LIST})

#生成静态库
#add_library( test ${DIR_SRCS} )
#生成动态库
#add_library( test_shared SHARED ${DIR_SRCS} )

# 指定生成目标
# ADD_EXECUTABLE(${PROJECT_NAME} ${SRC_LIST})
#add_executable(${PROJECT_NAME} ${DIR_SRCS})
# 将第三方的库链接到可执行文件，动态链接
#TARGET_LINK_LIBRARIES(${PROJECT_NAME} log4cplus)

#add_executable(project1 ${src})                        #编译为可执行程序
#add_library(project1 ${src})                           #编译为静态库
#add_library(project1 SHARED ${src})                    #编译为动态链接库

#add_executable(project1 MACOSX_BUNDLE ${src})          #编译为可执行程序 *.app

#add_library(project1 MODULE ${src})                    #编译为程序资源包 *.bundle
#set_target_properties(project1 PROPERTIES BUNDLE TRUE)

#add_library(project1 SHARED ${src})                     #编译为程序资源包 *.framework
#set_target_properties(project1 PROPERTIES FRAMEWORK TRUE)

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")  # Debug模式下的编译指令
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")         # Release模式下的编译指令

#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../bin)       #设置可执行文件的输出目录
#SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../lib)           #设置库文件的输出目录
