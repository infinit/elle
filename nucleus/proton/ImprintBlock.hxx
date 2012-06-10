#ifndef  NUCLEUS_PROTON_IMPRINTBLOCKSERIALIZER_HXX
# define NUCLEUS_PROTON_IMPRINTBLOCKSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>
# include <elle/utility/Time.hh>
# include <elle/cryptography/PublicKey.hh>
# include <nucleus/proton/MutableBlock.hh>

# include <nucleus/proton/ImprintBlock.hh>

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
