#ifndef ELLE_BUFFER_HH
# define ELLE_BUFFER_HH

namespace elle
{
  /* An array of bytes and its size.
   *
   * This class is NOTHING but a glorified std::pair<char*, int> used
   * to represent C-style buffers as one entity, with some useful
   * shortcuts and facilities.  It has no intelligence or memory
   * managment whatsoever, and shouldn't have any.
   * XXX Why not using elle::utility::WeakBuffer ??
   */
  class Buffer
  {
  public:
    typedef unsigned int Size;
    typedef char* Data;

    Buffer(Data data, Size size);

    Size size;
    Data data;
  };
}

#endif
