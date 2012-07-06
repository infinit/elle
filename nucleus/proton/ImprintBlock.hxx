#ifndef  NUCLEUS_PROTON_IMPRINTBLOCK_HXX
# define NUCLEUS_PROTON_IMPRINTBLOCK_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

namespace nucleus
{
  namespace proton
  {
    namespace detail
    {
      template<elle::serialize::ArchiveMode>
        struct ImprintBlockUpdate
        {
          // Do nothing default implem
          static void update(ImprintBlock&) {}
        };

      template<>
        struct ImprintBlockUpdate<elle::serialize::ArchiveMode::Input>
        {
          static void update(ImprintBlock& blk)
          {
            if (blk.owner.subject.Create(blk.owner.K) == elle::Status::Error)
              throw std::runtime_error("unable to create the owner subject");
          }
        };
    }
  }
}

ELLE_SERIALIZE_SIMPLE(nucleus::proton::ImprintBlock,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & static_cast<nucleus::proton::MutableBlock&>(value);
  archive & value.stamp;
  archive & value.salt;
  archive & value.owner.K;
  nucleus::proton::detail::ImprintBlockUpdate<Archive::mode>::update(value);
}

#endif
