cmake_minimum_required(VERSION 3.9)
project(Foo VERSION 1.0.0)

if(MSVC)
    set(pdb_output_dir "${CMAKE_CURRENT_BINARY_DIR}/pdb-files")

    set(CMAKE_PDB_OUTPUT_DIRECTORY "${pdb_output_dir}")
    set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY "${pdb_output_dir}")

    get_cmake_property(is_multi GENERATOR_IS_MULTI_CONFIG)
    if(is_multi)
        set(config_suffix "$<CONFIG>")
    else()
        set(config_suffix "")
    endif()

    # Introduce variables:
    # * CMAKE_INSTALL_LIBDIR
    # * CMAKE_INSTALL_BINDIR
    include(GNUInstallDirs)

    if(BUILD_SHARED_LIBS)
        set(pdb_dst ${CMAKE_INSTALL_BINDIR})
    else()
        set(pdb_dst ${CMAKE_INSTALL_LIBDIR})
    endif()

    install(
        DIRECTORY "${pdb_output_dir}/${config_suffix}/"
        DESTINATION ${pdb_dst})
endif()

#
#  target configuration
#
set(sources_common "src/add.cpp")
set(sources_shared)
set(sources_static)
set(sources_runtime "src/foo.cpp")

add_library(shared SHARED ${sources_common} ${sources_shared})
add_library(static STATIC ${sources_common} ${sources_static})
add_executable(runtime ${sources_common} ${sources_runtime})

#
#  target properties
#
set_property(TARGET shared static runtime
             APPEND PROPERTY COMPILE_DEFINITIONS
             "DEBUG=$<CONFIG:Debug>")
set_property(TARGET shared static runtime
             APPEND PROPERTY INCLUDE_DIRECTORIES
             "${CMAKE_CURRENT_LIST_DIR}/include")
set_property(TARGET shared static runtime
             PROPERTY OUTPUT_NAME foo)

#
#  installation
#
include(GNUInstallDirs)

# layout:
#   <prefix>/lib*/cmake/*
#   <prefix>/lib*/
#   <prefix>/include/
set(config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}")
set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

# configuration
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(TARGETS_EXPORT_NAME "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

# configure *ConfigVersion.cmake
include(CMakePackageConfigHelpers)
write_basic_package_version_file("${version_config}"
                                 COMPATIBILITY SameMajorVersion)

# configure *Config.cmake
configure_package_config_file("cmake/Config.cmake.in"
                              "${project_config}"
                              INSTALL_DESTINATION "${config_install_dir}")

# target destinations
install(
    TARGETS shared static runtime
    EXPORT "${TARGETS_EXPORT_NAME}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

# headers:
#   include/*/add.h -> <prefix>/include/*/add.h
install(DIRECTORY "include/"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
        FILES_MATCHING PATTERN "*.h"
                       PATTERN "*.hpp")

# config:
#   <prefix>/lib/cmake/*/*Config.cmake
#   <prefix>/lib/cmake/*/*ConfigVersion.cmake
install(FILES "${project_config}" "${version_config}"
        DESTINATION "${config_install_dir}")

# config:
#   <prefix>/lib/cmake/*/*Targets.cmake
install(EXPORT "${TARGETS_EXPORT_NAME}"
        NAMESPACE "${namespace}"
        DESTINATION "${config_install_dir}")
