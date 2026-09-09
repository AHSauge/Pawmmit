add_library(pawmmitUtil Path.cpp Debug.h Debug.cpp)

target_link_libraries(pawmmitUtil Qt6::Core)
target_include_directories(pawmmitUtil INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(pawmmitUtil PROPERTIES AUTOMOC ON)
