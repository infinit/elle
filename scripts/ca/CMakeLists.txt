PROJECT(ca CXX)

ADD_EXECUTABLE(mkuser mkuser.cc)
TARGET_LINK_LIBRARIES(mkuser elle etoile)
