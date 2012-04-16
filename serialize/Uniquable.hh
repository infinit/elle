#ifndef  ELLE_SERIALIZE_UNIQUABLE_HH
# define ELLE_SERIALIZE_UNIQUABLE_HH

# include <elle/types.hh>

# include <elle/io/Unique.hh>

namespace elle
{
  namespace serialize
  {

    ///
    /// This class enables the inherited class to be represented in a unique
    /// compact string-based form.
    ///
    /// Besides, this unique string representation can be used to save and
    /// restore the original object from/to main memory. indeed a unique
    /// instance is nothing more that a base64/hexadecimal/etc.  representation
    /// of its archive.
    ///
    /// Uses the Strangly Recurring Template Pattern.
    ///
    template<typename T>
    class Uniquable
    {
    public:
      ///
      /// this method returns in the Unique, the string-based representation of
      /// the object's archive.
      ///
      Status            Save(Unique& out) const;

      ///
      /// this method reconstructs the living object based on the unique string
      /// representation.
      ///
      Status            Restore(Unique const& in);
    };

  }
}

#include <elle/serialize/Uniquable.hxx>

#endif
