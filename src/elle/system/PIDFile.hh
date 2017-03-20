#ifndef ELLE_PIDFILE_HH
# define ELLE_PIDFILE_HH

# include <boost/filesystem.hpp>

# include <elle/attribute.hh>

namespace elle
{
  /// File containing a process ID
  ///
  /// The file only contains a numeric process ID in decimal, encoded latin-1. No
  /// terminating newline.
  ///
  class PIDFile
  {
  /*-------------.
  | Construction |
  `-------------*/
  public:
    /// Create the PID file containing the current process id.
    ///
    /// @param path The path where to create the file.
    ///
    PIDFile(boost::filesystem::path path);
    /// Delete the file.
    ~PIDFile();
    /// PID file path.
    ELLE_ATTRIBUTE_R(boost::filesystem::path, path);

  public:
    /// Parse a PID file.
    ///
    /// @param path The path to the PID file to parse.
    /// @returns The PID contained in the file
    /// @throw elle::Error if the file is not readable or its content cannot be
    ///                    interpreted.
    static
    int
    read(boost::filesystem::path const& path);
  };
}

#endif
