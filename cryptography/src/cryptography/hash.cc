#include <cryptography/hash.hh>

#include <openssl/err.h>
#include <openssl/evp.h>

#include <elle/Error.hh>
#include <elle/With.hh>
#include <elle/finally.hh>

#include <cryptography/cryptography.hh>

namespace infinit
{
  namespace cryptography
  {
    namespace hash
    {
      elle::Buffer
      sha256(elle::ConstWeakBuffer const& input)
      {
        cryptography::require();

        auto function = ::EVP_sha256();

        // Initialise the context.
        ::EVP_MD_CTX context;
        ::EVP_MD_CTX_init(&context);

        return elle::With<elle::Finally>(
          [&context]
          {
            if (::EVP_MD_CTX_cleanup(&context) <= 0)
              throw elle::Error(
                elle::sprintf("unable to clean the digest context: %s",
                              ::ERR_error_string(ERR_get_error(), nullptr)));
          }) << [&] ()
        {
          // Initialise the digest.
          if (::EVP_DigestInit_ex(&context, function, nullptr) <= 0)
            throw elle::Error(
              elle::sprintf("unable to initialize the digest process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Normalize behavior between no data and data of size 0.
          // by always forcing it into the second
          const elle::Byte* data = reinterpret_cast<const elle::Byte*>("");
          elle::Buffer::Size length = 0;
          if (input.contents() != nullptr)
          {
            data = input.contents();
            length = input.size();
          }

          // Update the digest with the given plain's data.
          elle::Buffer digest(EVP_MD_size(function));
          if (::EVP_DigestUpdate(&context,
                                 data,
                                 length) <= 0)
            throw elle::Error(
              elle::sprintf("unable to apply the digest process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Finalise the digest.
          unsigned int size;
          if (::EVP_DigestFinal_ex(&context,
                                   digest.mutable_contents(),
                                   &size) <=0)
            throw elle::Error(
              elle::sprintf("unable to finalize the digest process: %s",
                            ::ERR_error_string(ERR_get_error(), nullptr)));

          // Update the digest final size.
          digest.size(size);
          digest.shrink_to_fit();
          return digest;
        };
      }
    }
  }
}
