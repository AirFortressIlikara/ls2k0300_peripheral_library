# 包含交叉编译工具链
include(cross.cmake)

# 设置CMake的最低版本要求
cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pthread -Wall")  # 对于 C++ 编译器
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -Wall")      # 对于 C 编译器

# 定义项目名称和版本，并指定使用C和C++语言
project(test VERSION 0.1.0 LANGUAGES C CXX)

# 包含项目的自定义库路径
include_directories(lib)

# 查找源文件所在的目录
aux_source_directory(src DIR_SRCS)

# 创建可执行文件，使用项目名称作为目标名称
add_executable(${PROJECT_NAME} ${DIR_SRCS})
