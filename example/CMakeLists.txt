set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(spinning_ovals spinning_ovals.cpp)
target_link_libraries(spinning_ovals PRIVATE hades)

add_executable(testing testing.cpp)
target_link_libraries(testing PRIVATE hades)