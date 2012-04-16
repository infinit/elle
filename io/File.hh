#ifndef ELLE_IO_FILE_HH
# define ELLE_IO_FILE_HH

# include <elle/standalone/Region.hh>

# include <elle/io/Path.hh>

namespace elle
{
  using namespace standalone;
  using namespace radix;

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
