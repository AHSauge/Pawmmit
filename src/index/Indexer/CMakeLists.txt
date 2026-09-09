add_executable(
  pawmmit-indexer
  main.cpp
  indexer.cpp
  diffgrabber.cpp
  map.cpp
  reduce.cpp
  resultwriter.cpp
  common.cpp
  lexerpool.cpp
  idstorage.cpp)
target_link_libraries(pawmmit-indexer index)
target_compile_definitions(pawmmit-indexer
                           PRIVATE PAWMMIT_VERSION="${PAWMMIT_VERSION}")

set_target_properties(pawmmit-indexer PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                                 $<TARGET_FILE_DIR:pawmmit>)

if(APPLE)
  set_target_properties(
    pawmmit-indexer PROPERTIES INSTALL_RPATH
                               "/usr/local/qt;@loader_path/../Frameworks")
else()
  if(WIN32)
    target_link_libraries(pawmmit-indexer Dbghelp.lib)
  elseif(NOT FLATAPK)
    set_target_properties(pawmmit-indexer PROPERTIES INSTALL_RPATH "$ORIGIN")
  endif()

  install(
    TARGETS pawmmit-indexer
    DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT ${PAWMMIT_NAME})
endif()
