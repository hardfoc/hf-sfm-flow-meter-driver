# Consumer of an installed hf_sfm package: find_package() must give a working hf::sfm target.
cmake_minimum_required(VERSION 3.16)
project(hf_sfm_install_consumer LANGUAGES CXX)
find_package(hf_sfm 1.0 REQUIRED CONFIG)
add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE hf::sfm)
