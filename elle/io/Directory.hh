#ifndef ELLE_IO_DIRECTORY_HH
# define ELLE_IO_DIRECTORY_HH

# include <elle/types.hh>

# include <elle/standalone/Region.hh>

# include <elle/io/Path.hh>

# include <elle/idiom/Close.hh>
#  include <list>
# include <elle/idiom/Open.hh>

namespace elle
{

  using namespace standalone;
  using namespace radix;

  namespace io
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class abstracts the local directory operations.
    ///
    class Directory
    {
    public:
      //
      // types
      //
      typedef std::list<String>                 Set;
      typedef Set::iterator                     Iterator;
      typedef Set::const_iterator               Scoutor;

      //
      // static methods
      //
      static Status     Create(const Path&);
      static Status     Remove(const Path&);
      static Status     Exist(const Path&);
      static Status     Dig(const Path&);
      static Status     Clear(const Path&);
      static Status     List(const Path&,
                             Set&);
    };

  }
}

#endif
