set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(scratch scratch.cpp)
target_link_libraries(scratch PRIVATE hades)