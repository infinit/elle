#ifndef  COMMON_COMMON_HH
# define COMMON_COMMON_HH

# include <string>

# define COMMON_DEFAULT_INFINIT_HOME ".infinit"
# define COMMON_DEFAULT_META_PROTOCOL "http"
# define COMMON_DEFAULT_META_HOST "meta.api.infinit.io"
# define COMMON_DEFAULT_META_PORT 12345
# define COMMON_DEFAULT_RESOURCES_ROOT_URL "http://download.infinit.io"

namespace common
{

  /// All infinit related generic variables
  namespace infinit
  {

    /// Returns infinit home directory.
    /// Defaults to ~/.infinit on unices but could be changed by exporting
    /// INFINIT_HOME variable.
    std::string const& home();

    /// Returns binary path from its name.
    /// Defaults to ${INFINIT_HOME}/bin/binary_name but if INFINIT_BUILD_DIR is
    /// exported, it will return the accurate binary path inside the build
    /// directory.
    /// Raises an exception when ensure is true and the path does not refer to a
    /// valid binary.
    std::string const& binary_path(std::string const& name, bool ensure = true);

  }

  /// System and Operating System related stuffs
  namespace system
  {

    /// Returns the user home directory.
    std::string const& home_directory();

    /// Returns the platform name (linux, macosx or windows)
    std::string const& platform();

    /// Returns the architecture in bits.
    unsigned int architecture();

  }

  /// URLs to access meta server
  namespace meta
  {

    /// Returns the protocol used by meta (http or https)
    /// Can be overriden by INFINIT_META_PROTOCOL
    std::string const& protocol();

    /// Returns the host of the meta server.
    /// Can be overriden by INFINIT_META_HOST.
    std::string const& host();

    /// Returns the port of the meta server
    /// Can be overriden by INFINIT_META_PORT.
    short port();

    /// Returns the url to the meta server. If INFINIT_META_URL is defined, its
    /// value will be returned.
    std::string const& url();

  }

  /// All resources URIs
  namespace resources
  {

    /// Returns the root url for download infinit resources.  It will use
    /// current platform and architecture when not provided. If
    /// INFINIT_RESOURCES_ROOT_URL is defined, it will be used to build base
    /// url.
    std::string base_url(char const* platform = nullptr,
                         unsigned int architecture = 0);

    /// Returns the manifest url (Behave as resources_url()).
    std::string manifest_url(char const* platform = nullptr,
                             unsigned int architecture = 0);

  }

  // XXX The watchdog use a local socket for now, but it will be redesigned
  namespace watchdog
  {
    /// The name of the local socket.
    std::string const& server_name();

    // XXX scheduled for deletion
    /// Returns the path to the watchdog identity file.
    std::string const& identity_path();
  }

  // XXX scheduled for deletion
  /// Returns the path to the passport file.
  std::string const& passport_path();


} // !common

#endif
