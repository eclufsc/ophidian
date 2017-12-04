################################################################################
# This is the CMakeLists file for the Ophidian library directory.
#
#   Each namespace should be in its own directory with its own CMakeLists.txt 
# with rules for target generation and installation. Unless you do not want to 
# make it a dinamic lib.
################################################################################

# Add all subdirectories
add_subdirectory(circuit)
add_subdirectory(design)
add_subdirectory(entity_system)
add_subdirectory(floorplan)
add_subdirectory(geometry)
add_subdirectory(interconnection)
add_subdirectory(parser)
add_subdirectory(placement)
add_subdirectory(standard_cell)
add_subdirectory(util)

# Install export
install(
    EXPORT ophidian-targets
    NAMESPACE ophidian::
    DESTINATION ${ophidian_install_share_dir}/cmake
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    ophidian-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ophidian-config.cmake
    INSTALL_DESTINATION ${ophidian_install_share_dir}/cmake
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/ophidian-config-version.cmake
  VERSION ${ophidian_VERSION}
  COMPATIBILITY SameMajorVersion 
)

install(
    FILES 
        ${CMAKE_CURRENT_BINARY_DIR}/ophidian-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/ophidian-config-version.cmake
    DESTINATION ${ophidian_install_share_dir}/cmake
)
