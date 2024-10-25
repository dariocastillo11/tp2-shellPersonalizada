# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(unity_FRAMEWORKS_FOUND_DEBUG "") # Will be filled later
conan_find_apple_frameworks(unity_FRAMEWORKS_FOUND_DEBUG "${unity_FRAMEWORKS_DEBUG}" "${unity_FRAMEWORK_DIRS_DEBUG}")

set(unity_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET unity_DEPS_TARGET)
    add_library(unity_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET unity_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Debug>:${unity_FRAMEWORKS_FOUND_DEBUG}>
             $<$<CONFIG:Debug>:${unity_SYSTEM_LIBS_DEBUG}>
             $<$<CONFIG:Debug>:>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### unity_DEPS_TARGET to all of them
conan_package_library_targets("${unity_LIBS_DEBUG}"    # libraries
                              "${unity_LIB_DIRS_DEBUG}" # package_libdir
                              "${unity_BIN_DIRS_DEBUG}" # package_bindir
                              "${unity_LIBRARY_TYPE_DEBUG}"
                              "${unity_IS_HOST_WINDOWS_DEBUG}"
                              unity_DEPS_TARGET
                              unity_LIBRARIES_TARGETS  # out_libraries_targets
                              "_DEBUG"
                              "unity"    # package_name
                              "${unity_NO_SONAME_MODE_DEBUG}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${unity_BUILD_DIRS_DEBUG} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Debug ########################################
    set_property(TARGET unity::unity
                 APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Debug>:${unity_OBJECTS_DEBUG}>
                 $<$<CONFIG:Debug>:${unity_LIBRARIES_TARGETS}>
                 )

    if("${unity_LIBS_DEBUG}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET unity::unity
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     unity_DEPS_TARGET)
    endif()

    set_property(TARGET unity::unity
                 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Debug>:${unity_LINKER_FLAGS_DEBUG}>)
    set_property(TARGET unity::unity
                 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Debug>:${unity_INCLUDE_DIRS_DEBUG}>)
    # Necessary to find LINK shared libraries in Linux
    set_property(TARGET unity::unity
                 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                 $<$<CONFIG:Debug>:${unity_LIB_DIRS_DEBUG}>)
    set_property(TARGET unity::unity
                 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Debug>:${unity_COMPILE_DEFINITIONS_DEBUG}>)
    set_property(TARGET unity::unity
                 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Debug>:${unity_COMPILE_OPTIONS_DEBUG}>)

########## For the modules (FindXXX)
set(unity_LIBRARIES_DEBUG unity::unity)
