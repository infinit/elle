add_subdirectory(metalib)

file(
  COPY creosus meta pythia troll
  DESTINATION . FILES_MATCHING PATTERN "*.py"
)

file(
  COPY
	creosus-server
	meta-server
	meta-tests
	production-meta-server.sh
	production-creosus-server.sh
  DESTINATION .
)

