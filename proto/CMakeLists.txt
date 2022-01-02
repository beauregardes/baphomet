set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(draw_line draw_line.cpp)
target_link_libraries(draw_line PRIVATE hades)