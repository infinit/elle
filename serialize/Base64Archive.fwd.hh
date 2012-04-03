#ifndef ELLE_SERIALIZE_BASE64ARCHIVE_FWD_HH
# define ELLE_SERIALIZE_BASE64ARCHIVE_FWD_HH

# include "ArchiveMode.hh"

namespace elle { namespace serialize {

  template<ArchiveMode mode> class Base64Archive;

  typedef Base64Archive<ArchiveMode::Output> OutputBase64Archive;
  typedef Base64Archive<ArchiveMode::Input>  InputBase64Archive;

}} // !namespace elle::serialize

#endif /* ! BASE64ARCHIVE_FWD_HH */


