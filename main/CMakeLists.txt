# 主程序
add_executable(test main.cpp)

# 链接 OpenCV 库
target_link_libraries(test
    PRIVATE
        Threads::Threads
        common_lib
        ${OpenCV_LIBS}
)