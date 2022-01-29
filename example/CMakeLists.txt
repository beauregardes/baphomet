add_executable(testing testing.cpp)
target_compile_features(testing PUBLIC cxx_std_20)
target_link_libraries(testing PRIVATE baphomet)