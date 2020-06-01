
file(GLOB source_files *.cc)
add_library(app-framework_DAQModules SHARED ${source_files} )
target_link_libraries(app-framework_DAQModules ers::ers)
