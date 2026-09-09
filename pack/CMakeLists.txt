set(CONF_DIR ${CMAKE_SOURCE_DIR}/conf)
set(RSRC_DIR ${CMAKE_SOURCE_DIR}/rsrc)

set(MAC $<PLATFORM_ID:Darwin>)

# Windows packaging is x64-only. (CMake's WIN32 is true for any Windows ABI,
# including obsolete 32-bit toolchains.)
set(PAWMMIT_PACK_WIN64 OFF)
if(WIN32)
  if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Pawmmit packaging: Windows is 64-bit (x64) only.")
  endif()
  set(PAWMMIT_PACK_WIN64 ON)
endif()

# Install Qt plugins.
set(QT_PLUGINS QJpegPlugin)

if(APPLE)
  set(QT_PLUGINS ${QT_PLUGINS} QCocoaIntegrationPlugin QMacStylePlugin)
elseif(PAWMMIT_PACK_WIN64)
  set(QT_PLUGINS ${QT_PLUGINS} QWindowsIntegrationPlugin
                 QModernWindowsStylePlugin)
else()
  set(QT_PLUGINS ${QT_PLUGINS} QXcbIntegrationPlugin
                 QComposePlatformInputContextPlugin)
endif()

if(PAWMMIT_PACK_WIN64)
  set(INSTALL_LIBDIR ${CMAKE_INSTALL_BINDIR})
elseif(NOT APPLE)
  set(INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR})
else()
  set(INSTALL_LIBDIR ${CONTENTS_DIR})
endif()

if(FLATPAK)
  qt_import_plugins(pawmmit INCLUDE ${QT_PLUGINS})
elseif(NOT USE_SYSTEM_QT)
  foreach(QT_PLUGIN ${QT_PLUGINS})
    if(NOT TARGET Qt6::${QT_PLUGIN})
      continue()
    endif()

    get_target_property(PLUGIN Qt6::${QT_PLUGIN} LOCATION_${CMAKE_BUILD_TYPE})
    get_filename_component(FILE_NAME ${PLUGIN} NAME)
    get_filename_component(DIR ${PLUGIN} DIRECTORY)
    get_filename_component(DIR_NAME ${DIR} NAME)
    set(PLUGIN_PATH Plugins/${DIR_NAME}/${FILE_NAME})

    install(
      FILES ${PLUGIN}
      DESTINATION ${INSTALL_LIBDIR}/Plugins/${DIR_NAME}
      PERMISSIONS
        OWNER_READ
        OWNER_WRITE
        OWNER_EXECUTE
        GROUP_READ
        GROUP_EXECUTE
        WORLD_READ
        WORLD_EXECUTE
      COMPONENT ${PAWMMIT_NAME})

    if(PAWMMIT_PACK_WIN64)
      # Copy into build dir.
      execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PLUGIN}
                              ${CMAKE_BINARY_DIR}/${PLUGIN_PATH})
    elseif(APPLE)
      # Sign plugin.
      if(CODESIGN_IDENTITY)
        install(
          CODE "execute_process(COMMAND
            codesign --timestamp --options runtime
            -s \"${CODESIGN_IDENTITY}\"
            \${CMAKE_INSTALL_PREFIX}/${CONTENTS_DIR}/${PLUGIN_PATH}
          )"
          COMPONENT ${PAWMMIT_NAME})
      endif()
    else()
      # Delete runpath.
      install(CODE "execute_process(COMMAND chrpath --delete
          \${CMAKE_INSTALL_PREFIX}/${PLUGIN_PATH}
        )" COMPONENT ${PAWMMIT_NAME})
    endif()
  endforeach()

  # Install Qt libraries.
  foreach(QT_MODULE ${QT_MODULES})
    get_target_property(QT_LIBRARY Qt6::${QT_MODULE}
                        LOCATION_${CMAKE_BUILD_TYPE})

    if(APPLE)
      get_filename_component(QT_FRAMEWORK ${QT_LIBRARY} DIRECTORY)

      # FIXME: QT_LIBRARY points directly to the library bin instead of the
      # symlink 2 directories above (since qt6)
      string(REGEX REPLACE "/Versions/.*" "" QT_FRAMEWORK "${QT_FRAMEWORK}")

      set(DEST_DIR ${CONTENTS_DIR}/Frameworks)
      install(
        DIRECTORY ${QT_FRAMEWORK}
        DESTINATION ${DEST_DIR}
        COMPONENT ${PAWMMIT_NAME}
        PATTERN "*.prl" EXCLUDE
        PATTERN "*_debug" EXCLUDE
        PATTERN "Headers" EXCLUDE
        PATTERN ".DS_Store" EXCLUDE)
    else()
      if(PAWMMIT_PACK_WIN64)
        get_filename_component(TARGET_NAME ${QT_LIBRARY} NAME)

        # Copy into build dir.
        execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different
                                ${QT_LIBRARY} ${CMAKE_BINARY_DIR})
      else()
        # Strip minor and patch version extensions.
        get_filename_component(TARGET_NAME_WE ${QT_LIBRARY} NAME_WE)
        set(TARGET_NAME ${TARGET_NAME_WE}.so.6)
      endif()

      install(
        FILES ${QT_LIBRARY}
        DESTINATION ${INSTALL_LIBDIR}
        PERMISSIONS
          OWNER_READ
          OWNER_WRITE
          OWNER_EXECUTE
          GROUP_READ
          GROUP_EXECUTE
          WORLD_READ
          WORLD_EXECUTE
        COMPONENT ${PAWMMIT_NAME}
        RENAME ${TARGET_NAME})

      if(NOT PAWMMIT_PACK_WIN64)
        # Delete runpath.
        install(CODE "execute_process(COMMAND chrpath --delete
            \${CMAKE_INSTALL_PREFIX}/${TARGET_NAME}
          )" COMPONENT ${PAWMMIT_NAME})
      endif()
    endif()
  endforeach()
endif()

# Install SSL libraries.
if(NOT APPLE)
  if(NOT USE_SYSTEM_OPENSSL)
    if(PAWMMIT_PACK_WIN64)
      set(SSL_LIB_SUFFIX "-3-x64.dll")
    else()
      set(SSL_LIB_SUFFIX ".so.3")
    endif()

    foreach(SSL_LIB_NAME ssl crypto)
      set(SSL_LIB "${OPENSSL_DIR}/lib${SSL_LIB_NAME}${SSL_LIB_SUFFIX}")

      # Copy into build dir.
      execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SSL_LIB}
                              ${CMAKE_BINARY_DIR})

      install(
        FILES ${SSL_LIB}
        DESTINATION ${INSTALL_LIBDIR}
        PERMISSIONS
          OWNER_READ
          OWNER_WRITE
          OWNER_EXECUTE
          GROUP_READ
          GROUP_EXECUTE
          WORLD_READ
          WORLD_EXECUTE
        COMPONENT ${PAWMMIT_NAME})
    endforeach()
  endif()

  # libssh2 (submodule) installs its DLL without COMPONENT ${PAWMMIT_NAME};
  # CPack only installs that component, so ship the DLL from Pawmmit's pack
  # rules.
  if(PAWMMIT_PACK_WIN64
     AND NOT USE_SYSTEM_LIBSSH2
     AND TARGET libssh2_shared)
    install(
      FILES $<TARGET_FILE:libssh2_shared>
      DESTINATION ${INSTALL_LIBDIR}
      PERMISSIONS
        OWNER_READ
        OWNER_WRITE
        OWNER_EXECUTE
        GROUP_READ
        GROUP_EXECUTE
        WORLD_READ
        WORLD_EXECUTE
      COMPONENT ${PAWMMIT_NAME})
  endif()

  if(PAWMMIT_PACK_WIN64 OR APPLE)
    # Install config files.
    set(QT_CONF ${CONF_DIR}/qt.conf)
    install(
      FILES ${QT_CONF}
      DESTINATION
        $<${MAC}:${RESOURCES_DIR}>$<$<PLATFORM_ID:Windows>:${INSTALL_LIBDIR}>
      COMPONENT ${PAWMMIT_NAME})
  endif()
endif()

if(UNIX AND NOT APPLE)
  foreach(icon 16 32 64 128 256 512)
    install(
      FILES ${RSRC_DIR}/Pawmmit.iconset/icon_${icon}x${icon}.png
      DESTINATION share/icons/hicolor/${icon}x${icon}/apps
      COMPONENT ${PAWMMIT_NAME}
      RENAME ${PAWMMIT_EXECUTABLE_NAME}.png)
  endforeach()
  install(
    FILES ${RSRC_DIR}/Pawmmit.iconset/pawmmit_logo.svg
    DESTINATION share/icons/hicolor/scalable/apps
    COMPONENT ${PAWMMIT_NAME}
    RENAME ${PAWMMIT_EXECUTABLE_NAME}.svg)
endif()

# Sign bundle on macOS.
if(APPLE AND CODESIGN_IDENTITY)
  install(
    CODE "execute_process(COMMAND
      codesign --deep --timestamp --options runtime
        -s \"${CODESIGN_IDENTITY}\"
        \${CMAKE_INSTALL_PREFIX}/${PAWMMIT_EXECUTABLE_NAME}.app
    )"
    COMPONENT ${PAWMMIT_NAME})
endif()

# Add package rule.
if(APPLE)
  set(CPACK_GENERATOR DragNDrop)
  set(CPACK_DMG_VOLUME_NAME ${PAWMMIT_NAME})
  set(CPACK_DMG_BACKGROUND_IMAGE ${RSRC_DIR}/dmg-background.png)
  set(CPACK_DMG_DS_STORE_SETUP_SCRIPT ${RSRC_DIR}/dmg-setup.applescript)
elseif(PAWMMIT_PACK_WIN64)
  set(CPACK_GENERATOR NSIS)
  set(CPACK_NSIS_CREATE_ICONS_EXTRA
      "CreateShortCut \
    \\\"$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\${PAWMMIT_NAME}.lnk\\\" \
    \\\"$INSTDIR\\\\${CMAKE_INSTALL_BINDIR}\\\\${PAWMMIT_EXECUTABLE_NAME}.exe\\\""
  )
  set(CPACK_NSIS_EXECUTABLES_DIRECTORY ${CMAKE_INSTALL_BINDIR})
  set(CPACK_NSIS_MUI_FINISHPAGE_RUN ${PAWMMIT_EXECUTABLE_NAME}.exe)

  install(
    FILES ${RSRC_DIR}/vcredist_x64.exe
    DESTINATION .
    COMPONENT ${PAWMMIT_NAME})
  set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS
      "ExecWait '\\\"$INSTDIR\\\\vcredist_x64.exe\\\" /q /norestart'")
  set(PLATFORM "-win64")
else()
  set(CPACK_GENERATOR TGZ)
endif()

if(DEV_BUILD)
  set(BUILD_TAG "-dev")
else()
  set(BUILD_TAG "")
endif()
set(PACKAGE_FILE_NAME
    "${PAWMMIT_NAME}${PLATFORM}-${PAWMMIT_VERSION}${BUILD_TAG}")

set(CPACK_MONOLITHIC_INSTALL true)
set(CPACK_INSTALL_CMAKE_PROJECTS ${CMAKE_BINARY_DIR} ${PAWMMIT_NAME}
                                 ${PAWMMIT_NAME} /)

# Add Context Menu Shortcuts
if(PAWMMIT_PACK_WIN64)
  set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS
      "${CPACK_NSIS_EXTRA_INSTALL_COMMANDS}
    WriteRegStr HKCR 'Directory\\\\Background\\\\shell\\\\Pawmmit' '' 'Open with Pawmmit'
    WriteRegStr HKCR 'Directory\\\\Background\\\\shell\\\\Pawmmit' 'Icon' '\\\"$INSTDIR\\\\${CMAKE_INSTALL_BINDIR}\\\\${PAWMMIT_EXECUTABLE_NAME}.exe\\\"'
    WriteRegStr HKCR 'Directory\\\\Background\\\\shell\\\\Pawmmit\\\\command' '' '\\\"$INSTDIR\\\\${CMAKE_INSTALL_BINDIR}\\\\${PAWMMIT_EXECUTABLE_NAME}.exe\\\" \\\"%V\\\"'
    WriteRegStr HKCR 'Directory\\\\shell\\\\Pawmmit' '' 'Open with Pawmmit'
    WriteRegStr HKCR 'Directory\\\\shell\\\\Pawmmit' 'Icon' '\\\"$INSTDIR\\\\${CMAKE_INSTALL_BINDIR}\\\\${PAWMMIT_EXECUTABLE_NAME}.exe\\\"'
    WriteRegStr HKCR 'Directory\\\\shell\\\\Pawmmit\\\\command' '' '\\\"$INSTDIR\\\\${CMAKE_INSTALL_BINDIR}\\\\${PAWMMIT_EXECUTABLE_NAME}.exe\\\" \\\"%L\\\"'"
  )
  set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS
      "${CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS}
    DeleteRegKey HKCR 'Directory\\\\Background\\\\shell\\\\Pawmmit'
    DeleteRegKey HKCR 'Directory\\\\shell\\\\Pawmmit'")
endif()

set(CPACK_PACKAGE_FILE_NAME ${PACKAGE_FILE_NAME})
set(CPACK_PACKAGE_DIRECTORY ${CMAKE_BINARY_DIR}/pack)
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PAWMMIT_NAME})
set(CPACK_PACKAGE_VENDOR "Pawmmit Contributors")
set(CPACK_PACKAGE_VERSION_MAJOR ${PAWMMIT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PAWMMIT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PAWMMIT_VERSION_PATCH})

if(PAWMMIT_PACK_WIN64)
  set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE.md)
else()
  install(
    FILES ${CMAKE_SOURCE_DIR}/LICENSE.md
    DESTINATION ${CMAKE_INSTALL_DATADIR}/licenses/Pawmmit
    RENAME LICENSE)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include(CPack)

if(PAWMMIT_PACK_WIN64)
  set(PACKAGE_DIR ${CMAKE_BINARY_DIR}/${PACKAGE_FILE_NAME})

  # Add target to package and sign executable.
  add_custom_target(
    deploy
    signtool
    sign
    /v
    /f
    ${CERTIFICATE_FILE}
    /p
    ${CERTIFICATE_PASSWORD}
    ${CMAKE_BINARY_DIR}/${PACKAGE_FILE_NAME}.exe
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PACKAGE_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/Pawmmit.exe
            ${PACKAGE_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/Pawmmit.pdb
            ${PACKAGE_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/pawmmit-indexer.exe
            ${PACKAGE_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/pawmmit-indexer.pdb
            ${PACKAGE_DIR})

  add_dependencies(deploy package)
endif()
