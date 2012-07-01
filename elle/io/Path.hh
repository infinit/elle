#ifndef  ELLE_IO_PATH_HH
# define ELLE_IO_PATH_HH

#include <elle/types.hh>

#include <elle/radix/Object.hh>

#include <elle/io/fwd.hh>

#include <elle/idiom/Open.hh>

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
      public radix::Object
    {
    public:
      //
      // methods
      //
      Status            Create(const String&);
      Status            Create(const Pattern&);

      template <typename T>
      Status            Complete(T);
      template <typename T,
                typename... TT>
      Status            Complete(T,
                                 TT...);
      Status            Complete(const String&,
                                 const String&);

      //
      // interfaces
      //

      // object
      declare(Path);
      Boolean           operator==(const Path&) const;

      // dumpable
      Status            Dump(const Natural32 = 0) const;

      // archivable: nothing

    private:
      String            _string;

    public:
      // XXX
      std::string const& str() const
      { return this->_string; }
      std::string& str()
      { return this->_string; }
    };

  }
}

#include <elle/io/Path.hxx>

#endif
