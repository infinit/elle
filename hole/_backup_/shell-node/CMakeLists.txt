QT4_WRAP_CPP(ShellNode_MOCS
  ReadLineThread.hh
  ShellApp.hh)

ADD_EXECUTABLE(shell-node
  main.cc
  ReadLineThread.cc ReadLineThread.hh
  ShellApp.cc ShellApp.hh

  ${ShellNode_MOCS})

TARGET_LINK_LIBRARIES(shell-node
  hole
  ${QT_QTNETWORK_LIBRARIES}
  ${QT_QTCORE_LIBRARIES}
  readline ncursesw)