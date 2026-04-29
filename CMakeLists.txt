cmake_minimum_required(VERSION 3.10)
project(traffic_management_backend)

set(CMAKE_CXX_STANDARD 17)

# 1. Find ASIO (required for Crow)
find_path(ASIO_INCLUDE_DIR asio.hpp
    HINTS /opt/homebrew/include /usr/local/include
)

# 2. Find libpqxx (The C++ PostgreSQL connector)
find_path(PQXX_INCLUDE_DIR pqxx/pqxx
    HINTS /opt/homebrew/include /usr/local/include
)
find_library(PQXX_LIBRARY pqxx
    HINTS /opt/homebrew/lib /usr/local/lib
)

# 3. Find libpq (The C PostgreSQL connector - required by libpqxx)
# Note: Homebrew installs this as "keg-only", so it lives in a special 'opt' folder
find_path(PQ_INCLUDE_DIR libpq-fe.h
    HINTS /opt/homebrew/opt/libpq/include /usr/local/opt/libpq/include
)
find_library(PQ_LIBRARY pq
    HINTS /opt/homebrew/opt/libpq/lib /usr/local/opt/libpq/lib
)

# Create the executable
add_executable(traffic_backend main.cpp)

# Give VS Code and the compiler the include directories
target_include_directories(traffic_backend PUBLIC 
    ${ASIO_INCLUDE_DIR}
    ${PQXX_INCLUDE_DIR}
    ${PQ_INCLUDE_DIR}
)

# Link the actual binary library files to your program
target_link_libraries(traffic_backend PUBLIC 
    ${PQXX_LIBRARY}
    ${PQ_LIBRARY}
)