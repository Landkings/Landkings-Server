cmake_minimum_required(VERSION 2.8)
project(Landkings-GameServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_EPOLL -g -std=c++17" )

file(GLOB headers "include/*.h")
include_directories("include/" "lib/lua/include/")
file(GLOB sources "src/*.cpp")
#file(GLOB lua "lib/lua/include/*")
#file(GLOB lua_wrapper "lib/lua-wrapper/include/*")
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib/lua/lib/)
add_executable(${PROJECT_NAME} "main.cpp" ${headers} ${sources} ${lua} ${lua_wrapper})
target_link_libraries(${PROJECT_NAME} "liblua.a" "uWS" "ssl" "pthread" "z" "lua" "dl")
