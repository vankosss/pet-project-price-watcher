#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "TgBot::TgBot" for configuration "Debug"
set_property(TARGET TgBot::TgBot APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(TgBot::TgBot PROPERTIES
  IMPORTED_IMPLIB_DEBUG "${_IMPORT_PREFIX}/debug/lib/TgBot.lib"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/bin/TgBot.dll"
  )

list(APPEND _cmake_import_check_targets TgBot::TgBot )
list(APPEND _cmake_import_check_files_for_TgBot::TgBot "${_IMPORT_PREFIX}/debug/lib/TgBot.lib" "${_IMPORT_PREFIX}/debug/bin/TgBot.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
