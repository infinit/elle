#ifndef ELLE_SERIALIZE_ARCHIVEMODE_HH
# define ELLE_SERIALIZE_ARCHIVEMODE_HH

# include <cassert>
# include <iosfwd>

namespace elle { namespace serialize {

    /// ArchiveMode represent either an input or an output archive.
    enum class ArchiveMode
    {
      Input = 0x5e4d,
      Output = 0x3417e,
    };

    inline std::ostream& operator <<(std::ostream& out, ArchiveMode mode)
      {
        assert(mode == ArchiveMode::Input || mode == ArchiveMode::Output);
        return out << (mode == ArchiveMode::Input ? "Input" : "Output");
      }
}} // !namespace elle::serialize

#endif /* ! ARCHIVEMODE_HH */


