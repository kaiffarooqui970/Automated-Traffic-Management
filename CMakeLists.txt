cmake_minimum_required(VERSION 3.15)
project(traffic_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Point CMake to Homebrew's general folder (for Asio) AND the MySQL folders
include_directories(
    /opt/homebrew/include 
    /opt/homebrew/include/mysql 
    /opt/homebrew/opt/mysql-client/include/mysql
)
link_directories(
    /opt/homebrew/lib 
    /opt/homebrew/opt/mysql-client/lib
)

# Add your executable
add_executable(traffic_api main.cpp)

# Link the standard mysql client library
target_link_libraries(traffic_api mysqlclient)