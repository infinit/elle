
message(STATUS "copying infinit-cli to ${EXECUTABLE_OUTPUT_PATH}")
file(COPY infinit-cli DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
message(STATUS "copying 8chmod to ${EXECUTABLE_OUTPUT_PATH}")
file(COPY 8chmod DESTINATION ${EXECUTABLE_OUTPUT_PATH}/)
