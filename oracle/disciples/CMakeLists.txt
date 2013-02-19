add_subdirectory(metalib)

message(STATUS "copying creosus to ${LIBRARY_OUTPUT_PATH}/python/")
message(STATUS "copying meta to ${LIBRARY_OUTPUT_PATH}/python/")
message(STATUS "copying pythia to ${LIBRARY_OUTPUT_PATH}/python/")
message(STATUS "copying troll to ${LIBRARY_OUTPUT_PATH}/python/")
message(STATUS "copying static to ${LIBRARY_OUTPUT_PATH}/python/")
message(STATUS "copying trophonius to ${LIBRARY_OUTPUT_PATH}/python/")
message(STATUS "copying longinus to ${LIBRARY_OUTPUT_PATH}/python/")

file(
    COPY creosus meta pythia troll static trophonius longinus
    DESTINATION ${LIBRARY_OUTPUT_PATH}/python/
    FILES_MATCHING
        PATTERN "*.py"
        PATTERN "*.js"
        PATTERN "*.html"
        PATTERN "*.png"
        PATTERN "*.jpg"
        PATTERN "*.css"
        PATTERN "*.less"
        PATTERN "*.hh.inc"
)

message(STATUS "copying creosus-server to ${LIBRARY_OUTPUT_PATH}/python/")
message(STATUS "copying meta-server to ${LIBRARY_OUTPUT_PATH}/python/")
message(STATUS "copying meta-tests to ${LIBRARY_OUTPUT_PATH}/python/")
message(STATUS "copying admin-console to ${LIBRARY_OUTPUT_PATH}/python/")
message(STATUS "copying trophonius-server to ${LIBRARY_OUTPUT_PATH}/python/")
message(STATUS "copying longinus-server to ${LIBRARY_OUTPUT_PATH}/python/")
message(STATUS "copying erginus-server to ${LIBRARY_OUTPUT_PATH}/python/")
file(
  COPY
	creosus-server
	meta-server
	meta-tests
	admin-console
        trophonius-server
        longinus-server.py
        erginus-server
  DESTINATION ${LIBRARY_OUTPUT_PATH}/python/
)
