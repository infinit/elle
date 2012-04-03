#ifndef ELLE_SERIALIZE_ARCHIVEMODE_HH
# define ELLE_SERIALIZE_ARCHIVEMODE_HH

namespace elle { namespace serialize {

    /// ArchiveMode represent either an input or an output archive.
    enum class ArchiveMode
    {
      Input = 0x5e4d,
      Output = 0x3417e,
    };

}} // !namespace elle::serialize

#endif /* ! ARCHIVEMODE_HH */


