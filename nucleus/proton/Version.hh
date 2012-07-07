#ifndef NUCLEUS_PROTON_VERSION_HH
# define NUCLEUS_PROTON_VERSION_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>

# include <elle/idiom/Open.hh>

namespace nucleus
{
  namespace proton
  {

    ///
    /// this class represents a version number which are used to distinguish
    /// the versions related to a mutable block.
    ///
    class Version:
      public elle::radix::Object
    {
    public:
      //
      // types
      //
      typedef elle::Natural64           Type;

      //
      // constants
      //
      static const Version              First;
      static const Version              Last;

      static const Version              Any;
      static const Version&             Some;

      //
      // constructors & destructors
      //
      Version();
      Version(const Type);

      //
      // methods
      //
      elle::Status              Create(const Type);

      //
      // interfaces
      //

      // object
      declare(Version);
      elle::Boolean             operator==(const Version&) const;
      elle::Boolean             operator<(const Version&) const;
      elle::Boolean             operator>(const Version&) const;
      Version&                  operator+=(const elle::Natural32);
      Version                   operator+(const Version&) const;

      // dumpable
      elle::Status              Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Type                      number;
    };


  }
}

std::ostream& operator <<(std::ostream& out, nucleus::proton::Version const& v);

# include <nucleus/proton/Version.hxx>

#endif
