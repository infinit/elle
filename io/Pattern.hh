#ifndef ELLE_IO_PATTERN_HH
# define ELLE_IO_PATTERN_HH

#include <elle/types.hh>

#include <elle/radix/Object.hh>

#include <elle/idiom/Open.hh>

namespace elle
{
  namespace io
  {

    ///
    /// this class represents a path pattern.
    ///
    /// although the common syntax considers the string %name% as
    /// representing a variable component, programmers are welcome to use
    /// the syntax they prefer.
    ///
    class Pattern:
      public radix::Object
    {
    public:
      //
      // constructors & destructors
      //
      Pattern();
      Pattern(elle::String const& string);

      //
      // methods
      //
      Status            Create(const String&);

      //
      // interfaces
      //

      // object
      declare(Pattern);
      Boolean           operator==(const Pattern&) const;

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
      String            string;
    };

  }
}

#endif
