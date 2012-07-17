add_subdirectory(metalib)

file(
    COPY creosus meta pythia troll static
    DESTINATION .
    FILES_MATCHING
        PATTERN "*.py"
        PATTERN "*.js"
        PATTERN "*.html"
        PATTERN "*.png"
        PATTERN "*.jpg"
        PATTERN "*.css"
        PATTERN "*.less"
)

file(
  COPY
	creosus-server
	meta-server
	meta-tests
	admin-console
	production-meta-server.sh
	production-creosus-server.sh
  DESTINATION .
)
