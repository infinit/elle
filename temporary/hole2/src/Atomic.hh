//
// Made by fabien le mentec <texane@gmail.com>
// 
// Started on  Sun Jan 25 10:30:17 2009 texane
// Last update Sun Jan 25 11:04:54 2009 texane
//



#ifndef ATOMIC_HH_INCLUDED
# define ATOMIC_HH_INCLUDED



#ifdef _WIN32_WINNT
# include <windows.h>
#endif



class AtomicIntegral
{
private:
  long value_;

public:

  AtomicIntegral(int value)
  {
    value_ = value;
  }

  long operator--()
  {
#ifdef _WIN32_WINNT
    return InterlockedDecrement(&value_);
#else
    // gcc builting for atomic types
    return __sync_add_and_fetch(&value_, -1);
#endif
  }

};



#endif // ! ATOMIC_HH_INCLUDED
