
add_subdirectory(binaries)

add_custom_command(
  OUTPUT release.xml
  COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/make_release.py ${CMAKE_CURRENT_BINARY_DIR} ${INFINIT_VERSION_MAJOR} ${INFINIT_VERSION_MINOR}
  DEPENDS make_release.py released_binaries
)

add_custom_target(release ALL DEPENDS release.xml)
