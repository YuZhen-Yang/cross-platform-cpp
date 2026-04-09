#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Log4Qt::log4qt" for configuration "Release"
set_property(TARGET Log4Qt::log4qt APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(Log4Qt::log4qt PROPERTIES
  IMPORTED_IMPLIB_RELEASE "${_IMPORT_PREFIX}/lib/log4qt.lib"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/bin/log4qt.dll"
  )

list(APPEND _cmake_import_check_targets Log4Qt::log4qt )
list(APPEND _cmake_import_check_files_for_Log4Qt::log4qt "${_IMPORT_PREFIX}/lib/log4qt.lib" "${_IMPORT_PREFIX}/bin/log4qt.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
