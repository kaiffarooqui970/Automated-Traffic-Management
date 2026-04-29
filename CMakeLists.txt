cmake_minimum_required(VERSION 3.15)
project(traffic_api)

set(CMAKE_CXX_STANDARD 17)

# 1. Include Crow
include_directories(${CMAKE_SOURCE_DIR})

# 2. Tell CMake where Homebrew installed ASIO and MySQL on your Mac
include_directories(/opt/homebrew/include)  # <-- Added this line for ASIO!
include_directories(/opt/homebrew/opt/mysql-client/include/mysql)
link_directories(/opt/homebrew/opt/mysql-client/lib)

# 3. Add your main executable
add_executable(traffic_server main.cpp)

# 4. Link the libraries
target_link_libraries(traffic_server mysqlclient pthread)