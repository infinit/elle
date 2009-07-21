#include <stdlib.h>

namespace hole
{
  template <class T>
  T * FreeList<T>::freelist = 0;

  template <class T>
  FreeList<T>::FreeList()
    : freelistNext(0)
  {
  }

  template <class T>
  void * FreeList<T>::operator new(size_t size)
  {
    if (!freelist)
      return ::malloc (size);
    T * item = freelist;
    freelist = item->freelistNext;
    return item;
  }

  template <class T>
  void FreeList<T>::operator delete(void * p)
  {
    T * item = reinterpret_cast<T *> (p);
    item->freelistNext = freelist;
    freelist = item;
  }
}
