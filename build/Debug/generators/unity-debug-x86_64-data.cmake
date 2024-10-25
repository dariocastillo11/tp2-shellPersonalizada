########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(unity_COMPONENT_NAMES "")
if(DEFINED unity_FIND_DEPENDENCY_NAMES)
  list(APPEND unity_FIND_DEPENDENCY_NAMES )
  list(REMOVE_DUPLICATES unity_FIND_DEPENDENCY_NAMES)
else()
  set(unity_FIND_DEPENDENCY_NAMES )
endif()

########### VARIABLES #######################################################################
#############################################################################################
set(unity_PACKAGE_FOLDER_DEBUG "/home/dario/.conan2/p/b/unityd897ac2a90767/p")
set(unity_BUILD_MODULES_PATHS_DEBUG )


set(unity_INCLUDE_DIRS_DEBUG "${unity_PACKAGE_FOLDER_DEBUG}/include"
			"${unity_PACKAGE_FOLDER_DEBUG}/include/unity")
set(unity_RES_DIRS_DEBUG )
set(unity_DEFINITIONS_DEBUG )
set(unity_SHARED_LINK_FLAGS_DEBUG )
set(unity_EXE_LINK_FLAGS_DEBUG )
set(unity_OBJECTS_DEBUG )
set(unity_COMPILE_DEFINITIONS_DEBUG )
set(unity_COMPILE_OPTIONS_C_DEBUG )
set(unity_COMPILE_OPTIONS_CXX_DEBUG )
set(unity_LIB_DIRS_DEBUG "${unity_PACKAGE_FOLDER_DEBUG}/lib")
set(unity_BIN_DIRS_DEBUG )
set(unity_LIBRARY_TYPE_DEBUG STATIC)
set(unity_IS_HOST_WINDOWS_DEBUG 0)
set(unity_LIBS_DEBUG unity)
set(unity_SYSTEM_LIBS_DEBUG )
set(unity_FRAMEWORK_DIRS_DEBUG )
set(unity_FRAMEWORKS_DEBUG )
set(unity_BUILD_DIRS_DEBUG )
set(unity_NO_SONAME_MODE_DEBUG FALSE)


# COMPOUND VARIABLES
set(unity_COMPILE_OPTIONS_DEBUG
    "$<$<COMPILE_LANGUAGE:CXX>:${unity_COMPILE_OPTIONS_CXX_DEBUG}>"
    "$<$<COMPILE_LANGUAGE:C>:${unity_COMPILE_OPTIONS_C_DEBUG}>")
set(unity_LINKER_FLAGS_DEBUG
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${unity_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${unity_SHARED_LINK_FLAGS_DEBUG}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${unity_EXE_LINK_FLAGS_DEBUG}>")


set(unity_COMPONENTS_DEBUG )