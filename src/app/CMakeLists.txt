set(CONF_DIR ${CMAKE_SOURCE_DIR}/conf)
set(RSRC_DIR ${CMAKE_SOURCE_DIR}/rsrc)

option(GENERATE_APPDATA "Create appdata/metadata file" OFF)

if(APPLE)
  set(ICON ${RSRC_DIR}/Pawmmit.icns)
  set(APP_IMPL_FILE Application_mac.mm)
  set(THEME_IMPL_FILE CustomTheme_mac.mm)
elseif(WIN32)
  set(RC_FILE ${RSRC_DIR}/Pawmmit.rc)
endif()

set(EMOJI ${CONF_DIR}/emoji.json)
set(MERGETOOLS ${CONF_DIR}/mergetools)

file(GLOB CONFIG_FILES ${CONF_DIR}/*.lua)
file(GLOB DICTIONARIES ${CONF_DIR}/dictionaries/*.aff
     ${CONF_DIR}/dictionaries/*.dic ${CONF_DIR}/dictionaries/*.txt)
file(GLOB THEMES ${CONF_DIR}/themes/*.lua)
file(GLOB SCINTILLUA_LEXERS ${SRC_SCINTILLUA_LEXERS_DIR}/*.lua)
file(GLOB LUA_PLUGINS ${CONF_DIR}/plugins/*.lua ${CONF_DIR}/plugins/*.txt)

# Build resources.
qt6_add_resources(RESOURCES ${RSRC_DIR}/resources.qrc)

# Generate acknowledgments.
set(DOC_SOURCE_DIR ${CMAKE_SOURCE_DIR}/docs)
set(DOC_BINARY_DIR ${CMAKE_BINARY_DIR}/docs)
set(CHANGELOG_MD ${DOC_SOURCE_DIR}/changelog.md)
set(ACKNOWLEDGMENTS_MD ${DOC_SOURCE_DIR}/acknowledgments.md)
set(PRIVACY_MD ${DOC_SOURCE_DIR}/privacy.md)
set(PLUGIN_MD ${DOC_SOURCE_DIR}/plugin.md)
set(CHANGELOG_HTML ${DOC_BINARY_DIR}/changelog.html)
set(ACKNOWLEDGMENTS_HTML ${DOC_BINARY_DIR}/acknowledgments.html)
set(PRIVACY_HTML ${DOC_BINARY_DIR}/privacy.html)
set(PLUGIN_HTML ${DOC_BINARY_DIR}/plugin.html)
set(APPDATA_CONF
    ${CMAKE_SOURCE_DIR}/rsrc/linux/com.github.Pawmmit.Pawmmit.appdata.xml.in)
set(APPDATA
    ${CMAKE_BINARY_DIR}/rsrc/linux/com.github.Pawmmit.Pawmmit.appdata.xml)
add_custom_command(
  OUTPUT ${CHANGELOG_HTML} ${ACKNOWLEDGMENTS_HTML} ${PRIVACY_HTML}
         ${PLUGIN_HTML}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${DOC_BINARY_DIR}
  COMMAND cmark_exe ${CHANGELOG_MD} > ${CHANGELOG_HTML}
  COMMAND cmark_exe ${ACKNOWLEDGMENTS_MD} > ${ACKNOWLEDGMENTS_HTML}
  COMMAND cmark_exe ${PRIVACY_MD} > ${PRIVACY_HTML}
  COMMAND cmark_exe ${PLUGIN_MD} > ${PLUGIN_HTML}
  DEPENDS ${CHANGELOG_MD} ${ACKNOWLEDGMENTS_MD} ${PRIVACY_MD} ${PLUGIN_MD})

# Add application library.
add_library(app Application.cpp CustomTheme.cpp Theme.cpp ${RESOURCES}
                ${APP_IMPL_FILE} ${THEME_IMPL_FILE})

if(UNIX AND NOT APPLE)
  set_target_properties(app PROPERTIES AUTOMOC ON)
endif()

target_compile_definitions(
  app
  PUBLIC PAWMMIT_NAME="${PAWMMIT_NAME}"
         PAWMMIT_VERSION="${PAWMMIT_VERSION}"
         PAWMMIT_IDENTIFIER="${PAWMMIT_IDENTIFIER}"
         PAWMMIT_ORGANIZATION_DOMAIN="${PAWMMIT_ORGANIZATION_DOMAIN}"
         PAWMMIT_DBUS_INTERFACE_NAME="${PAWMMIT_IDENTIFIER}.Application"
         PAWMMIT_DBUS_OBJECT_PATH="/com/github/Pawmmit/Pawmmit/Application"
  PRIVATE QT_TRANSLATIONS_DIR="${QT_TRANSLATIONS_DIR}"
          BUILD_DESCRIPTION="${BUILD_DESCRIPTION}")

target_link_libraries(
  app
  translation
  conf
  git
  ui
  update
  Qt6::Widgets)

set_target_properties(app PROPERTIES AUTOMOC ON)

if(UNIX AND NOT APPLE)
  target_link_libraries(app Qt6::DBus)
endif()

# Add main executable.
add_executable(
  pawmmit WIN32 MACOSX_BUNDLE
  Pawmmit.cpp
  ${ICON}
  ${RC_FILE}
  ${CONFIG_FILES}
  ${DICTIONARIES}
  ${EMOJI}
  ${MERGETOOLS}
  ${THEMES}
  ${LUA_PLUGINS}
  ${SCINTILLUA_LEXERS}
  ${CHANGELOG_HTML}
  ${ACKNOWLEDGMENTS_HTML}
  ${PRIVACY_HTML}
  ${PLUGIN_HTML})

if(GENERATE_APPDATA)
  add_custom_target(
    appdata ALL
    COMMAND
      ${CMAKE_COMMAND} -DCHANGELOG_HTML:STRING=${CHANGELOG_HTML}
      -DAPPDATA_CONF:STRING=${APPDATA_CONF} -DAPPDATA:STRING=${APPDATA}
      -DPAWMMIT_IDENTIFIER:STRING=${PAWMMIT_IDENTIFIER} -P
      "${CMAKE_SOURCE_DIR}/cmake/generate_appdata.cmake"
    DEPENDS ${CHANGELOG_HTML}
    COMMENT "Create appdata file")
endif()

target_link_libraries(pawmmit app)

set_target_properties(
  pawmmit
  PROPERTIES OUTPUT_NAME ${PAWMMIT_EXECUTABLE_NAME}
             MACOSX_BUNDLE_BUNDLE_NAME ${PAWMMIT_NAME}
             RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(WIN32)
  target_link_libraries(app Dbghelp.lib)
elseif(APPLE)
  set_target_properties(
    pawmmit
    PROPERTIES INSTALL_RPATH "/usr/local/qt;@loader_path/../Frameworks"
               MACOSX_BUNDLE_INFO_PLIST ${RSRC_DIR}/Info.plist.in
               MACOSX_BUNDLE_ICON_FILE ${PAWMMIT_NAME}
               MACOSX_BUNDLE_BUNDLE_NAME ${PAWMMIT_NAME}
               MACOSX_BUNDLE_BUNDLE_VERSION ${PAWMMIT_VERSION}
               MACOSX_BUNDLE_LONG_VERSION_STRING ${PAWMMIT_VERSION}
               MACOSX_BUNDLE_SHORT_VERSION_STRING ${PAWMMIT_VERSION}
               MACOSX_BUNDLE_GUI_IDENTIFIER ${PAWMMIT_IDENTIFIER})

  # Copy config files into bundle.
  set_source_files_properties(
    ${ICON}
    ${EMOJI}
    ${MERGETOOLS}
    ${CONFIG_FILES}
    ${CHANGELOG_HTML}
    ${ACKNOWLEDGMENTS_HTML}
    ${PRIVACY_HTML}
    ${PLUGIN_HTML}
    PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

  set_source_files_properties(${DICTIONARIES} PROPERTIES MACOSX_PACKAGE_LOCATION
                                                         Resources/dictionaries)

  set_source_files_properties(${THEMES} PROPERTIES MACOSX_PACKAGE_LOCATION
                                                   Resources/themes)

  set_source_files_properties(${LUA_PLUGINS} PROPERTIES MACOSX_PACKAGE_LOCATION
                                                        Resources/plugins)

  set_source_files_properties(
    ${SCINTILLUA_LEXERS} PROPERTIES MACOSX_PACKAGE_LOCATION Resources/lexers)
else()

  if(NOT FLATPAK)
    set_target_properties(pawmmit PROPERTIES INSTALL_RPATH "$ORIGIN")
  endif()
endif()

add_dependencies(pawmmit pawmmit-indexer pawmmit-relauncher)

install(
  TARGETS pawmmit
  DESTINATION $<IF:$<PLATFORM_ID:Darwin>,.,${CMAKE_INSTALL_BINDIR}>
  COMPONENT ${PAWMMIT_NAME})

if(APPLE)
  get_target_property(QT_LIBRARY Qt6::Core LOCATION)
  get_filename_component(QT_FRAMEWORK ${QT_LIBRARY} DIRECTORY)
  get_filename_component(RPATH ${QT_FRAMEWORK} DIRECTORY)

  # Fixup relauncher RPATH.
  set(RELAUNCHER ${CONTENTS_DIR}/MacOS/pawmmit-relauncher)
  install(
    CODE "execute_process(COMMAND
		${CMAKE_INSTALL_NAME_TOOL} -delete_rpath \"${RPATH}\"
	  \"\${CMAKE_INSTALL_PREFIX}/${RELAUNCHER}\"
  )
execute_process(COMMAND
	${CMAKE_INSTALL_NAME_TOOL} -add_rpath \"/usr/local/qt\"
	  \"\${CMAKE_INSTALL_PREFIX}/${RELAUNCHER}\"
  )
execute_process(COMMAND
	${CMAKE_INSTALL_NAME_TOOL} -add_rpath \"@loader_path/../Frameworks\"
	  \"\${CMAKE_INSTALL_PREFIX}/${RELAUNCHER}\"
  )"
    COMPONENT ${PAWMMIT_NAME})

  # Fixup indexer RPATH.
  set(INDEXER ${CONTENTS_DIR}/MacOS/pawmmit-indexer)
  install(
    CODE "execute_process(COMMAND
		${CMAKE_INSTALL_NAME_TOOL} -delete_rpath \"${RPATH}\"
	  \"\${CMAKE_INSTALL_PREFIX}/${INDEXER}\"
  )
execute_process(COMMAND
	${CMAKE_INSTALL_NAME_TOOL} -add_rpath \"/usr/local/qt\"
	  \"\${CMAKE_INSTALL_PREFIX}/${INDEXER}\"
  )
execute_process(COMMAND
	${CMAKE_INSTALL_NAME_TOOL} -add_rpath \"@loader_path/../Frameworks\"
	  \"\${CMAKE_INSTALL_PREFIX}/${INDEXER}\"
  )"
    COMPONENT ${PAWMMIT_NAME})
else()
  # Windows and UNIX must be ./ otherwise it is interpreted as absolute path
  if(UNIX)
    install(
      FILES ${CMAKE_SOURCE_DIR}/rsrc/linux/com.github.Pawmmit.Pawmmit.desktop
      DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    if(${GENERATE_APPDATA})
      install(
        FILES ${APPDATA}
        DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)
    endif()
  endif()

  # Install config files.
  if(CHANGELOG_HTML
     AND ACKNOWLEDGMENTS_HTML
     AND PRIVACY_HTML
     AND PLUGIN_HTML)
    install(
      FILES ${CHANGELOG_HTML} ${ACKNOWLEDGMENTS_HTML} ${PRIVACY_HTML}
            ${PLUGIN_HTML}
      DESTINATION ${RESOURCES_DIR}
      COMPONENT ${PAWMMIT_NAME})
  endif()

  install(
    FILES ${EMOJI} ${MERGETOOLS}
    DESTINATION ${RESOURCES_DIR}
    COMPONENT ${PAWMMIT_NAME})

  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${EMOJI}
                          ${CMAKE_BINARY_DIR}/Resources/emoji.json)
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MERGETOOLS}
                          ${CMAKE_BINARY_DIR}/Resources/mergetools)

  foreach(CONFIG_FILE ${CONFIG_FILES})
    install(
      FILES ${CONFIG_FILE}
      DESTINATION ${RESOURCES_DIR}
      COMPONENT ${PAWMMIT_NAME})

    file(RELATIVE_PATH CONFIG_FILE_DEST ${CONF_DIR} ${CONFIG_FILE})
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CONFIG_FILE}
              ${CMAKE_BINARY_DIR}/Resources/${CONFIG_FILE_DEST})
  endforeach()

  install(
    FILES ${DICTIONARIES}
    DESTINATION ${RESOURCES_DIR}/dictionaries
    COMPONENT ${PAWMMIT_NAME})

  foreach(DICTIONARY ${DICTIONARIES})
    file(RELATIVE_PATH DICTIONARY_DEST ${CONF_DIR} ${DICTIONARY})
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${DICTIONARY}
                            ${CMAKE_BINARY_DIR}/Resources/${DICTIONARY_DEST})
  endforeach()

  install(
    FILES ${THEMES}
    DESTINATION ${RESOURCES_DIR}/themes
    COMPONENT ${PAWMMIT_NAME})

  foreach(THEME ${THEMES})
    file(RELATIVE_PATH THEME_DEST ${CONF_DIR} ${THEME})
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${THEME}
                            ${CMAKE_BINARY_DIR}/Resources/${THEME_DEST})
  endforeach()

  install(
    FILES ${LUA_PLUGINS}
    DESTINATION ${RESOURCES_DIR}/plugins
    COMPONENT ${PAWMMIT_NAME})

  foreach(LUA_PLUGIN ${LUA_PLUGINS})
    file(RELATIVE_PATH LUA_PLUGIN_DEST ${CONF_DIR} ${LUA_PLUGIN})
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${LUA_PLUGIN}
                            ${CMAKE_BINARY_DIR}/Resources/${LUA_PLUGIN_DEST})
  endforeach()

  foreach(SCINTILLUA_LEXER ${SCINTILLUA_LEXERS})
    install(
      FILES ${SCINTILLUA_LEXER}
      DESTINATION ${RESOURCES_DIR}/lexers
      COMPONENT ${PAWMMIT_NAME})

    get_filename_component(SCINTILLUA_LEXER_DEST ${SCINTILLUA_LEXER} NAME)
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SCINTILLUA_LEXER}
              ${CMAKE_BINARY_DIR}/Resources/lexers/${SCINTILLUA_LEXER_DEST})
  endforeach()
endif()
