include_directories(${CMAKE_SOURCE_DIR}/app-framework/include)
include_directories(${CMAKE_SOURCE_DIR}/app-framework-base/include)

# set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads REQUIRED)
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")


add_executable(simple_test_app simple_test_app.cc)
target_link_libraries(simple_test_app app-framework Threads::Threads)