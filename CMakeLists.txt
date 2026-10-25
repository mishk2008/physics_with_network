cmake_minimum_required(VERSION 3.20)
project(cpp-network-replication LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Threads REQUIRED)   
find_package(Qt6 REQUIRED COMPONENTS Core Network Widgets)

add_executable( server
                src/server/main.cpp
                src/server/server.cpp
                src/server/session.cpp
                src/server/physics.cpp
                src/common/serialize.cpp
)
target_include_directories( server PRIVATE 
                            /mingw64/include
                            src/server
                            src/common
)
target_link_libraries(  server PRIVATE
                        Threads::Threads
                        ws2_32       
                        mswsock
)

add_executable( client WIN32
                src/client/main.cpp
                src/client/main_window.cpp
                src/common/serialize.cpp
)
target_include_directories( client PRIVATE 
                            src/client
                            src/common
)
target_link_libraries(  client PRIVATE
                        Qt6::Core
                        Qt6::Network
                        Qt6::Widgets
)
