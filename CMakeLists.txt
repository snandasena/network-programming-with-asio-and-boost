cmake_minimum_required(VERSION 3.10)
project(asio_samples)
set(CMAKE_CXX_STANDARD 14)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -I ${CMAKE_CURRENT_SOURCE_DIR}/asio-1.18.0/include")

add_executable(asio_samples main.cpp)
target_link_libraries(asio_samples Threads::Threads)
