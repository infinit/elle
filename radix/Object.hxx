//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Object.hxx
//
// created       julien quintard   [sun may  2 12:21:39 2010]
// updated       julien quintard   [mon may  3 12:18:31 2010]
//

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
    template <const Format U, const Format F>
    template <typename T>
    Status		Object<U, F>::Recycle(const T*		object)
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

      // return StatusOk in order to avoid including Report, Status and Maid.
      return (StatusOk);
    }

    ///
    /// this method returns the memory footprint of the actual object.
    ///
    /// this method has been introduced because there is no way to known
    /// the size of an object when its base class is manipulated.
    ///
    /// note that this method can be automatically generating by using
    /// the Embed(Object, T) macro function.
    ///
    template <const Format U, const Format F>
    Status		Object<U, F>::Imprint(Natural32&) const
    {
      enter();

      escape("this method should never have been called");
    }

    ///
    /// this method clones the current object by allocating a new
    /// one through the copy constructor.
    ///
    template <const Format U, const Format F>
    Status		Object<U, F>::Clone(Meta*&) const
    {
      enter();

      escape("this method should never have been called");
    }

//
// ---------- operators -------------------------------------------------------
//

    ///
    /// this operator compares two objects.
    ///
    template <const Format U, const Format F>
    Boolean		Object<U, F>::operator==(const Object&) const
    {
      enter();

      flee("this method should never have been called");
    }

    ///
    /// this operator compares two objects.
    ///
    template <const Format U, const Format F>
    Boolean		Object<U, F>::operator<(const Object&) const
    {
      enter();

      flee("this method should never have been called");
    }

    ///
    /// this operator compares two objects.
    ///
    template <const Format U, const Format F>
    Boolean		Object<U, F>::operator>(const Object&) const
    {
      enter();

      flee("this method should never have been called");
    }

    ///
    /// this operator adds an object to the current one.
    ///
    template <const Format U, const Format F>
    Object<U, F>	Object<U, F>::operator+(const Object&)
    {
      Object<U, F>	result;

      enter();

      yield("this method should never have been called", result);
    }

    ///
    /// this operator copies an object.
    ///
    /// this method (i) starts by checking if the given object
    /// is not the current one before (ii) calling Recycle().
    ///
    /// note that the Embed(Object, T) macro function can be used
    /// to automatically embed the source code of this operator.
    ///
    template <const Format U, const Format F>
    Object<U,F>&	Object<U, F>::operator=(const Object&)
    {
      fail("this method should never have been called");
    }

    ///
    /// this operator compares two objects. the source code of this
    /// function can be automatically generated through the Embed(Object, T)
    /// macro function.
    ///
    template <const Format U, const Format F>
    Boolean		Object<U, F>::operator!=(const Object&) const
    {
      enter();

      flee("this method should never have been called");
    }

    ///
    /// this operator compares two objects.
    ///
    template <const Format U, const Format F>
    Boolean		Object<U, F>::operator<=(const Object&) const
    {
      enter();

      flee("this method should never have been called");
    }

    ///
    /// this operator compares two objects.
    ///
    template <const Format U, const Format F>
    Boolean		Object<U, F>::operator>=(const Object&) const
    {
      enter();

      flee("this method should never have been called");
    }

  }
}

#endif
