#ifndef HOLE_FREELIST_HH
# define HOLE_FREELIST_HH

# include <sys/types.h>

namespace hole
{
  template <class T>
  class FreeList
  {
  public:
    FreeList();

    void * operator new(size_t size);
    void operator delete(void * p);

  private:
    static T * freelist;
    T * freelistNext;
  };
}

# include "FreeList.hxx"

#endif /* !HOLE_FREELIST_HH */
