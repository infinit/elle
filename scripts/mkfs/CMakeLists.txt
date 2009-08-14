PROJECT(mkfs CXX)

ADD_EXECUTABLE(mkfs.infinit mkfs.cc)
TARGET_LINK_LIBRARIES(mkfs.infinit elle etoile)
