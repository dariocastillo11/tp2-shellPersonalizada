#----------------------------------------------------------------
# Generated CMake target import file for configuration "Relea".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "unity::framework" for configuration "Relea"
set_property(TARGET unity::framework APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEA)
set_target_properties(unity::framework PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEA "C"
  IMPORTED_LOCATION_RELEA "${_IMPORT_PREFIX}/lib/libunity.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS unity::framework )
list(APPEND _IMPORT_CHECK_FILES_FOR_unity::framework "${_IMPORT_PREFIX}/lib/libunity.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
