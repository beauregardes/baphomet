add_executable(messages messages.cpp)
target_compile_features(messages PUBLIC cxx_std_20)
target_link_libraries(messages PRIVATE baphomet)