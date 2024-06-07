cmake_minimum_required(VERSION 3.7)
project(ecs)
set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(
   columnist
   GIT_REPOSITORY https://github.com/rollbear/columnist
   GIT_TAG 746d68701ca1b60199e0157ba989d477e5413c21)
FetchContent_MakeAvailable(columnist)

add_executable(gen gen.cpp)

add_custom_command(
COMMAND gen
DEPENDS gen
OUTPUT input_data.cpp input_data.hpp 
)
add_library(input_data OBJECT input_data.cpp input_data.hpp)
include_directories(${CMAKE_BINARY_DIR})

add_executable(struct_of_vectors struct_of_vectors.cpp)
target_link_libraries(struct_of_vectors input_data)

add_executable(filter_struct_of_vectors filter_struct_of_vectors.cpp)
target_link_libraries(filter_struct_of_vectors input_data)

add_executable(vector_of_structs vector_of_structs.cpp)
target_link_libraries(vector_of_structs input_data)

add_executable(filter_vector_of_structs filter_vector_of_structs.cpp)
target_link_libraries(filter_vector_of_structs input_data)

add_executable(templated templated.cpp)
target_link_libraries(templated input_data)

add_executable(filter_templated filter_templated.cpp)
target_link_libraries(filter_templated input_data)

add_executable(iterators iterators.cpp)
target_link_libraries(iterators input_data)

add_executable(filter_iterators filter_iterators.cpp)
target_link_libraries(filter_iterators input_data)

add_executable(iterators_row iterators_row.cpp)
target_link_libraries(iterators_row input_data)

add_executable(filter_iterators_row filter_iterators_row.cpp)
target_link_libraries(filter_iterators_row input_data)

add_executable(row_select row_select.cpp)
target_link_libraries(row_select input_data)

add_executable(filter_row_select filter_row_select.cpp)
target_link_libraries(filter_row_select input_data)

add_executable(function_select function_select.cpp)
target_link_libraries(function_select input_data)

add_executable(filter_function_select filter_function_select.cpp)
target_link_libraries(filter_function_select input_data)

add_executable(columnistt columnist.cpp)
target_link_libraries(columnistt input_data columnist::columnist)

add_executable(filter_columnistt filter_columnist.cpp)
target_link_libraries(filter_columnistt input_data columnist::columnist)

