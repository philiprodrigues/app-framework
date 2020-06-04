
#file(GLOB source_files *.cc)
#add_library(app-framework_DAQModules SHARED ${source_files} )

add_library(app-framework_DebugLoggingDAQModule_duneDAQModule DebugLoggingDAQModule.cc)
target_link_libraries(app-framework_DebugLoggingDAQModule_duneDAQModule ers::ers Folly::folly)
add_library(app-framework_FakeDataConsumerDAQModule_duneDAQModule FakeDataConsumerDAQModule.cc)
target_link_libraries(app-framework_FakeDataConsumerDAQModule_duneDAQModule ers::ers Folly::folly)
add_library(app-framework_FakeDataProducerDAQModule_duneDAQModule FakeDataProducerDAQModule.cc)
target_link_libraries(app-framework_FakeDataProducerDAQModule_duneDAQModule ers::ers Folly::folly)
