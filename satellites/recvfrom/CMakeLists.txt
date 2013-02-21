project(recvfrom)

message(STATUS "copying 8recvfrom to ${EXECUTABLE_OUTPUT_PATH}")

file(
  COPY
        8recvfrom.py
        DESTINATION ${EXECUTABLE_OUTPUT_PATH}/
)
