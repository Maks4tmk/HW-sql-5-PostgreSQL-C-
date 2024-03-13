cmake_minimum_required(VERSION 3.20)

set(PosthreSQL_ADDITIONAL_VERSION "16")

project(DataBase01)

add_executable(DataBase01 main.cpp)

add_subdirectory("D:\\Netology\\source\\repos\\libpqxx7.9.0" libpqxx_build)

target_compile_features(DataBase01 PRIVATE cxx_std_20)

target_link_libraries(DataBase01 PRIVATE pqxx)

