PROJECT(mkfs CXX)

ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64)

ADD_EXECUTABLE(mkfs.infinit mkfs.cc)
TARGET_LINK_LIBRARIES(mkfs.infinit pthread rt fuse elle etoile)

INSTALL(TARGETS mkfs.infinit
        DESTINATION ${INFINIT_BINDIR})
