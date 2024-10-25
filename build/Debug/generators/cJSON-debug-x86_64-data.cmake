########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

list(APPEND cjson_COMPONENT_NAMES cjson)
list(REMOVE_DUPLICATES cjson_COMPONENT_NAMES)
if(DEFINED cjson_FIND_DEPENDENCY_NAMES)
  list(APPEND cjson_FIND_DEPENDENCY_NAMES )
  list(REMOVE_DUPLICATES cjson_FIND_DEPENDENCY_NAMES)
else()
  set(cjson_FIND_DEPENDENCY_NAMES )
endif()

########### VARIABLES #######################################################################
#############################################################################################
set(cjson_PACKAGE_FOLDER_DEBUG "/home/dario/.conan2/p/b/cjson9e28834bfcace/p")
set(cjson_BUILD_MODULES_PATHS_DEBUG )


set(cjson_INCLUDE_DIRS_DEBUG "${cjson_PACKAGE_FOLDER_DEBUG}/include")
set(cjson_RES_DIRS_DEBUG )
set(cjson_DEFINITIONS_DEBUG )
set(cjson_SHARED_LINK_FLAGS_DEBUG )
set(cjson_EXE_LINK_FLAGS_DEBUG )
set(cjson_OBJECTS_DEBUG )
set(cjson_COMPILE_DEFINITIONS_DEBUG )
set(cjson_COMPILE_OPTIONS_C_DEBUG )
set(cjson_COMPILE_OPTIONS_CXX_DEBUG )
set(cjson_LIB_DIRS_DEBUG "${cjson_PACKAGE_FOLDER_DEBUG}/lib")
set(cjson_BIN_DIRS_DEBUG )
set(cjson_LIBRARY_TYPE_DEBUG STATIC)
set(cjson_IS_HOST_WINDOWS_DEBUG 0)
set(cjson_LIBS_DEBUG cjson)
set(cjson_SYSTEM_LIBS_DEBUG m)
set(cjson_FRAMEWORK_DIRS_DEBUG )
set(cjson_FRAMEWORKS_DEBUG )
set(cjson_BUILD_DIRS_DEBUG )
set(cjson_NO_SONAME_MODE_DEBUG FALSE)


# COMPOUND VARIABLES
set(cjson_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${cjson_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${cjson_COMPILE_OPTIONS_C_DEBUG}>")
set(cjson_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${cjson_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${cjson_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${cjson_EXE_LINK_FLAGS_DEBUG}>")


set(cjson_COMPONENTS_DEBUG cjson)
########### COMPONENT cjson VARIABLES ############################################

set(cjson_cjson_INCLUDE_DIRS_DEBUG "${cjson_PACKAGE_FOLDER_DEBUG}/include")
set(cjson_cjson_LIB_DIRS_DEBUG "${cjson_PACKAGE_FOLDER_DEBUG}/lib")
set(cjson_cjson_BIN_DIRS_DEBUG )
set(cjson_cjson_LIBRARY_TYPE_DEBUG STATIC)
set(cjson_cjson_IS_HOST_WINDOWS_DEBUG 0)
set(cjson_cjson_RES_DIRS_DEBUG )
set(cjson_cjson_DEFINITIONS_DEBUG )
set(cjson_cjson_OBJECTS_DEBUG )
set(cjson_cjson_COMPILE_DEFINITIONS_DEBUG )
set(cjson_cjson_COMPILE_OPTIONS_C_DEBUG "")
set(cjson_cjson_COMPILE_OPTIONS_CXX_DEBUG "")
set(cjson_cjson_LIBS_DEBUG cjson)
set(cjson_cjson_SYSTEM_LIBS_DEBUG m)
set(cjson_cjson_FRAMEWORK_DIRS_DEBUG )
set(cjson_cjson_FRAMEWORKS_DEBUG )
set(cjson_cjson_DEPENDENCIES_DEBUG )
set(cjson_cjson_SHARED_LINK_FLAGS_DEBUG )
set(cjson_cjson_EXE_LINK_FLAGS_DEBUG )
set(cjson_cjson_NO_SONAME_MODE_DEBUG FALSE)

# COMPOUND VARIABLES
set(cjson_cjson_LINKER_FLAGS_DEBUG
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${cjson_cjson_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${cjson_cjson_SHARED_LINK_FLAGS_DEBUG}>
        $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${cjson_cjson_EXE_LINK_FLAGS_DEBUG}>
)
set(cjson_cjson_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${cjson_cjson_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${cjson_cjson_COMPILE_OPTIONS_C_DEBUG}>")