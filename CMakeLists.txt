# 设置CMake的最低版本要求
cmake_minimum_required(VERSION 3.5)

# 包含交叉编译工具链（放在项目定义前）
include(cross.cmake)

# 定义项目名称和版本，并指定使用C和C++语言
project(test
    VERSION 0.1.0
    LANGUAGES C CXX
    DESCRIPTION "Test Project"
)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找线程库
find_package(Threads REQUIRED)

# 全局编译选项
add_compile_options(
    "$<$<COMPILE_LANGUAGE:CXX>:-Wall;-O3;-pthread>"
    "$<$<COMPILE_LANGUAGE:C>:-Wall;-O3>"
)

# 设置OpenCV的安装路径
set(OpenCV_DIR "/home/ilikara/loongson/opencv-4.11.0/loongson")

# 查找OpenCV库
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV Include Directories: ${OpenCV_INCLUDE_DIRS}")

# 创建公共库目标
add_library(common_lib STATIC)
target_include_directories(common_lib 
    PUBLIC 
        lib
        ${OpenCV_INCLUDE_DIRS}
)

# 收集源文件
file(GLOB_RECURSE COMMON_SOURCES "src/*.cpp" "src/*.c")
target_sources(common_lib PRIVATE ${COMMON_SOURCES})

# 添加所有子目录
set(SUBDIRS
    main
)

foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
endforeach()
