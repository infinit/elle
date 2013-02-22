project(recvfrom)

add_custom_target(8recvfrom
    COMMAND cp 8recvfrom.py ${EXECUTABLE_OUTPUT_PATH}/8recvfrom
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "copying 8recvfrom to ${EXECUTABLE_OUTPUT_PATH}"
    SOURCES 8recvfrom.py
    DEPENDS _gap
    )

