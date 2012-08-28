#ifndef  ELLE_IO_PATH_HH
# define ELLE_IO_PATH_HH

#include <elle/Printable.hh>
#include <elle/idiom/Open.hh>
#include <elle/io/fwd.hh>
#include <elle/radix/Object.hh>
#include <elle/types.hh>

namespace elle
{
  namespace io
  {

    ///
    /// this class abstracts a path representation.
    ///
    /// note that a path may be completed since its syntax considers
    /// the pattern %name% as representing a component to be provided later.
    ///
    class Path:
      public radix::Object,
      public elle::Printable
    {
    public:
      //
      // constructors & destructors
      //
      Path();
      Path(Pattern const& pattern);
      template <typename T,
                typename... TT>
      Path(Pattern const& pattern,
           T const& piece,
           TT const&... pieces);

      //
      // methods
      //
      Status            Create(const String&);
      Status            Create(const Pattern&);

      Status
      Complete();
      template <typename T>
      Status            Complete(T const&);
      template <typename T,
                typename... TT>
      Status            Complete(T const&,
                                 TT const&...);
      Status            Complete(String const&,
                                 String const&);

      //
      // interfaces
      //

      // object
      declare(Path);
      Boolean           operator==(const Path&) const;

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      //
      // attributes
      //
    private:
      String            _string;

    public:
      // XXX
      std::string const& string() const
      { return this->_string; }
      std::string& string()
      { return this->_string; }

    /*----------.
    | Printable |
    `----------*/

    public:
      virtual void print(std::ostream& s) const;
    };
  }
}

#include <elle/io/Path.hxx>

#endif
