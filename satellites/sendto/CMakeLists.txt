project(sendto)

add_custom_target(8sendto
    COMMAND cp 8sendto.py ${EXECUTABLE_OUTPUT_PATH}/8sendto
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "copying 8sendto to ${EXECUTABLE_OUTPUT_PATH}"
    SOURCES 8sendto.py
    DEPENDS _gap
    )
