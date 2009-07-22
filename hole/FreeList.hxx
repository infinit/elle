#include <stdlib.h>

namespace hole
{
  template <class T>
  T * FreeList<T>::freelist_ = 0;

  template <class T>
  FreeList<T>::FreeList()
    : freelistNext_(0)
  {
  }

  template <class T>
  void * FreeList<T>::operator new(size_t size)
  {
    if (!freelist_)
      return ::malloc (size);
    T * item = freelist_;
    freelist_ = item->freelistNext_;
    return item;
  }

  // Be carefull of this one
  template <class T>
  void * FreeList<T>::operator new(size_t size, const T *)
  {
    if (!freelist_)
      return ::malloc (size);
    T * item = freelist_;
    freelist_ = item->freelistNext_;
    return item;
  }

  template <class T>
  void FreeList<T>::operator delete(void * p)
  {
    T * item = reinterpret_cast<T *> (p);
    item->freelistNext_ = freelist_;
    freelist_ = item;
  }
}
