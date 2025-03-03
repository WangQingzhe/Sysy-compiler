# Install script for directory: /home/lyq/Desktop/Sysy/sysY_backend/sysy/antlr/antlr4-runtime

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/lyq/Desktop/Sysy/sysY_backend/sysy/build/antlr/antlr4-runtime/runtime/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/antlr4-runtime/antlr4-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/antlr4-runtime/antlr4-targets.cmake"
         "/home/lyq/Desktop/Sysy/sysY_backend/sysy/build/antlr/antlr4-runtime/CMakeFiles/Export/b687d5edce7d360f0dcc0a9573266ef6/antlr4-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/antlr4-runtime/antlr4-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/antlr4-runtime/antlr4-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/antlr4-runtime" TYPE FILE FILES "/home/lyq/Desktop/Sysy/sysY_backend/sysy/build/antlr/antlr4-runtime/CMakeFiles/Export/b687d5edce7d360f0dcc0a9573266ef6/antlr4-targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/antlr4-runtime" TYPE FILE FILES "/home/lyq/Desktop/Sysy/sysY_backend/sysy/build/antlr/antlr4-runtime/CMakeFiles/Export/b687d5edce7d360f0dcc0a9573266ef6/antlr4-targets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/antlr4-runtime" TYPE FILE FILES
    "/home/lyq/Desktop/Sysy/sysY_backend/sysy/build/antlr/antlr4-runtime/antlr4-runtime-config.cmake"
    "/home/lyq/Desktop/Sysy/sysY_backend/sysy/build/antlr/antlr4-runtime/antlr4-runtime-config-version.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/antlr4-generator" TYPE FILE FILES
    "/home/lyq/Desktop/Sysy/sysY_backend/sysy/build/antlr/antlr4-runtime/antlr4-generator-config.cmake"
    "/home/lyq/Desktop/Sysy/sysY_backend/sysy/build/antlr/antlr4-runtime/antlr4-generator-config-version.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/doc/libantlr4" TYPE FILE FILES
    "/home/lyq/Desktop/Sysy/sysY_backend/sysy/antlr/antlr4-runtime/README.md"
    "/home/lyq/Desktop/Sysy/sysY_backend/sysy/antlr/antlr4-runtime/VERSION"
    )
endif()

