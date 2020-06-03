if(Boost_FOUND)

    include_directories(${Boost_INCLUDE_DIRS})

    add_executable(StdDeQueue_test StdDeQueue_test.cc)
    add_executable(FollyQueue_test FollyQueue_test.cc)
    target_link_libraries(FollyQueue_test Folly::folly)
    add_test(NAME StdDeQueue_test COMMAND StdDeQueue_test)
    add_test(NAME FollyQueue_test COMMAND FollyQueue_test)
    add_subdirectory(DAQModules)
endif()

