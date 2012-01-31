
add_custom_command(
  OUTPUT  8infinit
  COMMAND cp ../../8infinit 8infinit && strip -s 8infinit
  DEPENDS 8infinit
)

add_custom_command(
  OUTPUT  8dictionary
  COMMAND cp ../../applications/8dictionary/8dictionary 8dictionary && strip -s 8dictionary
  DEPENDS 8dictionary
)

add_custom_command(
  OUTPUT  8access
  COMMAND cp ../../applications/8access/8access 8access && strip -s 8access
  DEPENDS 8access
)

add_custom_command(
  OUTPUT  8updater
  COMMAND cp ../../plasma/updater/updater 8updater && strip -s 8updater
  DEPENDS updater
)

add_custom_command(
  OUTPUT  8watchdog
  COMMAND cp ../../plasma/watchdog/watchdog 8watchdog && strip -s 8watchdog
  DEPENDS watchdog
)

# Yes CMAKE sucks while disambiguate targets (and don't warn either)
set(BINARIES
  ${CMAKE_CURRENT_BINARY_DIR}/8infinit
  ${CMAKE_CURRENT_BINARY_DIR}/8dictionary
  ${CMAKE_CURRENT_BINARY_DIR}/8watchdog
  ${CMAKE_CURRENT_BINARY_DIR}/8updater
  ${CMAKE_CURRENT_BINARY_DIR}/8access
)

add_custom_target(released_binaries ALL DEPENDS ${BINARIES})
