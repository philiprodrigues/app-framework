
file(GLOB source_files *.cc)
add_library(app-framework SHARED ${source_files} )
target_link_libraries(app-framework ers::ers)

add_subdirectory(DAQModules)

