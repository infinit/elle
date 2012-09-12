project(diary CXX)

if(INFINIT_LINUX OR INFINIT_MACOSX)
  add_executable(8diary
    Diary.cc
    Memoirs.cc

    unix/Crux.cc
    unix/Live.cc
    unix/Memoirs.cc
    unix/Record.cc
    unix/Replay.cc
    unix/Upcall.cc
  )
endif(INFINIT_LINUX OR INFINIT_MACOSX)
