#ifndef ELLE_IO_FILE_HH
# define ELLE_IO_FILE_HH

# include <elle/types.hh>

# include <elle/io/fwd.hh>
# include <elle/fwd.hh>

namespace elle
{
  namespace io
  {

    ///
    /// this class abstracts the local file operations.
    ///
    class File
    {
    public:
      //
      // static methods
      //
      static Status     Read(const Path&,
                             Buffer&);
      static Status     Write(const Path&,
                              Buffer const&);
      static Status     Erase(const Path&);
      static Boolean    Exist(const Path&);
      static Status     Dig(const Path&);
    };

  }
}

#endif
