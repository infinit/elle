#ifndef ELLE_RADIX_OBJECT_HXX
#define ELLE_RADIX_OBJECT_HXX

namespace elle
{
  namespace radix
  {

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method recycles an object.
    ///
    template <typename T>
    Status              Object::Recycle(const T*                object)
    {
      // release the resources.
      this->~Object();

      if (object == NULL)
        {
          // initialize the object with default values.
          new (this) T;
        }
      else
        {
          // initialize the object with defined values.
          new (this) T(*object);
        }

      return Status::Ok;
    }

  }
}

#endif
