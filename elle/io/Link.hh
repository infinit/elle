#ifndef ELLE_IO_LINK_HH
# define ELLE_IO_LINK_HH

# include <elle/standalone/Region.hh>

# include <elle/io/Path.hh>

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
    /// this class abstracts the local link operations.
    ///
    class Link
    {
    public:
      //
      // static methods
      //
      static Status     Create(const Path&,
                               const Path&);
      static Status     Erase(const Path&);
      static Status     Exist(const Path&);
      static Status     Dig(const Path&);
    };

  }
}

#endif
