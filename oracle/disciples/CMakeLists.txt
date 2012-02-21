add_subdirectory(metalib)

file(
  COPY creosus meta pythia
  DESTINATION . FILES_MATCHING PATTERN "*.py"
)

file(
  COPY creosus-server meta-server meta-tests spawn-fcgi.sh
  DESTINATION .
)

