set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_executable(gameoflife gameoflife.cpp)
target_link_libraries(gameoflife PRIVATE hades)

add_executable(spinning_ovals spinning_ovals.cpp)
target_link_libraries(spinning_ovals PRIVATE hades)

add_executable(font_corrupt font_corrupt.cpp)
target_link_libraries(font_corrupt PRIVATE hades)

add_executable(music_player music_player.cpp)
target_link_libraries(music_player PRIVATE hades)

add_executable(testing testing.cpp)
target_link_libraries(testing PRIVATE hades)