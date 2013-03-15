#ifndef ELLE_SERIALIZE_ARCHIVEMODE_HH
# define ELLE_SERIALIZE_ARCHIVEMODE_HH

# include <cassert>
# include <iosfwd>

namespace elle
{
  namespace serialize
  {

    /// ArchiveMode represent either an input or an output archive.
    enum class ArchiveMode
    {
      Input = 0x5e4d,
      Output = 0x3417e,

      input = Input,  // XXX smooth transition to norm
      output = Output,
    };

  }
} // !namespace elle::serialize

/// Makes archive mode enum is printable.
std::ostream&
operator <<(std::ostream& out,
            elle::serialize::ArchiveMode mode);

#endif /* ! ARCHIVEMODE_HH */


