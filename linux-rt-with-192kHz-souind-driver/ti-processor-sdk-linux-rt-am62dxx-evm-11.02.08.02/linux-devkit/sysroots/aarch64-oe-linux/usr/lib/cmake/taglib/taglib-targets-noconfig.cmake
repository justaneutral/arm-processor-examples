#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "TagLib::tag" for configuration ""
set_property(TARGET TagLib::tag APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(TagLib::tag PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libtag.so.2.0.1"
  IMPORTED_SONAME_NOCONFIG "libtag.so.2"
  )

list(APPEND _cmake_import_check_targets TagLib::tag )
list(APPEND _cmake_import_check_files_for_TagLib::tag "${_IMPORT_PREFIX}/lib/libtag.so.2.0.1" )

# Import target "TagLib::tag_c" for configuration ""
set_property(TARGET TagLib::tag_c APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(TagLib::tag_c PROPERTIES
  IMPORTED_LINK_DEPENDENT_LIBRARIES_NOCONFIG "TagLib::tag"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libtag_c.so.2.0.1"
  IMPORTED_SONAME_NOCONFIG "libtag_c.so.2"
  )

list(APPEND _cmake_import_check_targets TagLib::tag_c )
list(APPEND _cmake_import_check_files_for_TagLib::tag_c "${_IMPORT_PREFIX}/lib/libtag_c.so.2.0.1" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
