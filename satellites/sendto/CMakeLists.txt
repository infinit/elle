project(sendto)

message(STATUS "copying 8sendto to ${EXECUTABLE_OUTPUT_PATH}")

file(
  COPY
        8sendto.py
        DESTINATION ${EXECUTABLE_OUTPUT_PATH}/
)
