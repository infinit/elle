
#include <elle/radix/Object.hh>
#include <elle/standalone/Log.hh>
#include <elle/standalone/Report.hh>

namespace elle
{
  namespace radix
  {

    ///
    /// this method returns the memory footprint of the actual object.
    ///
    /// this method has been introduced because there is no way to known
    /// the size of an object when its base class is manipulated.
    ///
    Status              Object::Imprint(Natural32&) const
    {
      escape("this method should never have been called");
    }

    ///
    /// this method clones the current object by allocating a new
    /// one through the copy constructor.
    ///
    Status              Object::Clone(Object*&) const
    {
      escape("this method should never have been called");
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean             Object::operator==(const Object&) const
    {
      throw("this method should never have been called");
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean             Object::operator<(const Object&) const
    {
      throw("this method should never have been called");
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean             Object::operator>(const Object&) const
    {
      throw("this method should never have been called");
    }

    ///
    /// this operator adds an object to the current one.
    ///
    Object              Object::operator+(const Object&)
    {
      throw("this method should never have been called");
    }

    ///
    /// this operator copies an object.
    ///
    /// this method (i) starts by checking if the given object
    /// is not the current one before (ii) calling Recycle().
    ///
    Object&             Object::operator=(const Object&)
    {
      throw("this method should never have been called");
    }

    ///
    /// this operator compares two objects. the source code of this
    /// function can be automatically generated through the Embed(Object, T)
    /// macro function.
    ///
    Boolean             Object::operator!=(const Object&) const
    {
      throw("this method should never have been called");
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean             Object::operator<=(const Object&) const
    {
      throw("this method should never have been called");
    }

    ///
    /// this operator compares two objects.
    ///
    Boolean             Object::operator>=(const Object&) const
    {
      throw("this method should never have been called");
    }

  }
}
