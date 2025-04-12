# 交叉编译工具链配置
# 设置为 ON 表示交叉编译，OFF 表示本地编译
option(CROSS_COMPILE "Enable cross-compilation" ON)

if(CROSS_COMPILE)
    # 基本系统信息
    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR loongarch64)
    
    # 工具链路径
    set(TOOLCHAIN_DIR "/opt/loongson-gnu-toolchain-8.3-x86_64-loongarch64-linux-gnu-rc1.6")
    
    # 编译器路径
    set(CMAKE_CXX_COMPILER "${TOOLCHAIN_DIR}/bin/loongarch64-linux-gnu-g++")
    set(CMAKE_C_COMPILER "${TOOLCHAIN_DIR}/bin/loongarch64-linux-gnu-gcc")
    
    # 系统根目录设置
    set(CMAKE_FIND_ROOT_PATH "${TOOLCHAIN_DIR}/loongarch64-linux-gnu/sysroot" "${TOOLCHAIN_DIR}")
    
    # 查找规则设置
    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
endif()
