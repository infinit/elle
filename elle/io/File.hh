#ifndef ELLE_IO_FILE_HH
# define ELLE_IO_FILE_HH

# include <elle/types.hh>

# include <elle/io/fwd.hh>
# include <elle/standalone/fwd.hh>

namespace elle
{
  using namespace standalone;

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
                             Region&);
      static Status     Write(const Path&,
                              const Region&);
      static Status     Erase(const Path&);
      static Status     Exist(const Path&);
      static Status     Dig(const Path&);
    };

  }
}

#endif
