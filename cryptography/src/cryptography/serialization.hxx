#ifndef INFINIT_CRYPTOGRAPHY_SERIALIZATION_HXX
# define INFINIT_CRYPTOGRAPHY_SERIALIZATION_HXX

# include <cryptography/Plain.hh>
# include <cryptography/evp.hh>

# include <elle/serialize/BaseArchive.hxx>

# include <elle/Buffer.hh>
# include <elle/log.hh>

namespace infinit
{
  namespace cryptography
  {
    /*----------.
    | Functions |
    `----------*/

    template <typename T>
    elle::Buffer
    serialize(T const& value)
    {
      ELLE_LOG_COMPONENT("infinit.cryptography");
      ELLE_DEBUG_FUNCTION("T");
      ELLE_DUMP("value: %x", value);

      static_assert(std::is_same<T, Plain>::value == false,
                    "this call should never have occured");
      static_assert(std::is_same<T, elle::Buffer>::value == false,
                    "this call should never have occured");
      static_assert(std::is_same<T, elle::WeakBuffer>::value == false,
                    "this call should never have occured");
      static_assert(std::is_same<T, elle::ConstWeakBuffer>::value == false,
                    "this call should never have occured");

      elle::Buffer archive;
      archive.writer() << value;

      return (archive);
    }
  }
}

#endif
