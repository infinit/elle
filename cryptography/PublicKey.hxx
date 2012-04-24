#ifndef  ELLE_CRYPTOGRAPHY_PUBLICKEY_HXX
# define ELLE_CRYPTOGRAPHY_PUBLICKEY_HXX

# include <elle/standalone/Maid.hh>
# include <elle/standalone/Report.hh>

# include <elle/utility/Buffer.hh>
# include <elle/serialize/BufferArchive.hh>

namespace elle
{
  using namespace standalone;

  namespace cryptography
  {

    template<typename T>
      Status PublicKey::Encrypt(T const& in, Code& out) const
      {
        elle::utility::Buffer buf;

        try
          {
            buf.Writer() << in;
          }
        catch (std::exception const& err)
          {
            escape("Cannot save object: %s", err.what());
          }

        return this->Encrypt(
            elle::utility::WeakBuffer(buf),
            out
        );
      }

    template <typename T>
      Status PublicKey::Decrypt(Code const& in, T& out) const
      {
        elle::utility::Buffer buf;

        if (this->Decrypt(in, buf) == elle::Status::Error)
          escape("Cannot decrypt data");

        try
          {
            buf.Reader() >> out;
          }
        catch (std::exception const& err)
          {
            escape("Cannot load object: %s", err.what());
          }

        return elle::Status::Ok;
      }

    ///
    /// these methods make it easier to decrypt/sign multiple items at
    /// the same time while keeping a way to catch errors.
    ///
    /// note that the Encrypt() method cannot benefit from the variadic
    /// template arguments.
    ///

    //
    // encrypt
    //

    ///
    /// this method takes any object and encrypts it.
    ///
    /// the process obviously consists in serializing the object first.
    ///
    /// note that Encrypt() methods return an archive hidden in the
    /// code.
    ///
    //XXX
    //template <typename T1>
    //Status              PublicKey::Encrypt(const T1&            o1,
    //                                       Code&                code) const
    //{
    //  Archive           archive;

    //  // create an archive.
    //  if (archive.Create() == Status::Error)
    //    escape("unable to create the archive");

    //  // serialize the object.
    //  if (archive.Serialize(o1) == Status::Error)
    //    escape("unable to serialize the object");

    //  // encrypt the archive.
    //  if (this->Encrypt(archive, code) == Status::Error)
    //    escape("unable to encrypt the object's archive");

    //  return Status::Ok;
    //}

    //template <typename T1,
    //          typename T2>
    //Status              PublicKey::Encrypt(const T1&            o1,
    //                                       const T2&            o2,
    //                                       Code&                code) const
    //{
    //  Archive           archive;

    //  if (archive.Create() == Status::Error)
    //    escape("unable to create the archive");

    //  if (archive.Serialize(o1, o2) == Status::Error)
    //    escape("unable to serialize the object");

    //  if (this->Encrypt(archive, code) == Status::Error)
    //    escape("unable to encrypt the object's archive");

    //  return Status::Ok;
    //}

    //template <typename T1,
    //          typename T2,
    //          typename T3>
    //Status              PublicKey::Encrypt(const T1&            o1,
    //                                       const T2&            o2,
    //                                       const T3&            o3,
    //                                       Code&                code) const
    //{
    //  Archive           archive;

    //  if (archive.Create() == Status::Error)
    //    escape("unable to create the archive");

    //  if (archive.Serialize(o1, o2, o3) == Status::Error)
    //    escape("unable to serialize the object");

    //  if (this->Encrypt(archive, code) == Status::Error)
    //    escape("unable to encrypt the object's archive");

    //  return Status::Ok;
    //}

    //template <typename T1,
    //          typename T2,
    //          typename T3,
    //          typename T4>
    //Status              PublicKey::Encrypt(const T1&            o1,
    //                                       const T2&            o2,
    //                                       const T3&            o3,
    //                                       const T4&            o4,
    //                                       Code&                code) const
    //{
    //  Archive           archive;

    //  if (archive.Create() == Status::Error)
    //    escape("unable to create the archive");

    //  if (archive.Serialize(o1, o2, o3, o4) == Status::Error)
    //    escape("unable to serialize the object");

    //  if (this->Encrypt(archive, code) == Status::Error)
    //    escape("unable to encrypt the object's archive");

    //  return Status::Ok;
    //}

    //template <typename T1,
    //          typename T2,
    //          typename T3,
    //          typename T4,
    //          typename T5>
    //Status              PublicKey::Encrypt(const T1&            o1,
    //                                       const T2&            o2,
    //                                       const T3&            o3,
    //                                       const T4&            o4,
    //                                       const T5&            o5,
    //                                       Code&                code) const
    //{
    //  Archive           archive;

    //  if (archive.Create() == Status::Error)
    //    escape("unable to create the archive");

    //  if (archive.Serialize(o1, o2, o3, o4, o5) == Status::Error)
    //    escape("unable to serialize the object");

    //  if (this->Encrypt(archive, code) == Status::Error)
    //    escape("unable to encrypt the object's archive");

    //  return Status::Ok;
    //}

    //template <typename T1,
    //          typename T2,
    //          typename T3,
    //          typename T4,
    //          typename T5,
    //          typename T6>
    //Status              PublicKey::Encrypt(const T1&            o1,
    //                                       const T2&            o2,
    //                                       const T3&            o3,
    //                                       const T4&            o4,
    //                                       const T5&            o5,
    //                                       const T6&            o6,
    //                                       Code&                code) const
    //{
    //  Archive           archive;

    //  if (archive.Create() == Status::Error)
    //    escape("unable to create the archive");

    //  if (archive.Serialize(o1, o2, o3, o4, o5, o6) == Status::Error)
    //    escape("unable to serialize the object");

    //  if (this->Encrypt(archive, code) == Status::Error)
    //    escape("unable to encrypt the object's archive");

    //  return Status::Ok;
    //}

    //template <typename T1,
    //          typename T2,
    //          typename T3,
    //          typename T4,
    //          typename T5,
    //          typename T6,
    //          typename T7>
    //Status              PublicKey::Encrypt(const T1&            o1,
    //                                       const T2&            o2,
    //                                       const T3&            o3,
    //                                       const T4&            o4,
    //                                       const T5&            o5,
    //                                       const T6&            o6,
    //                                       const T7&            o7,
    //                                       Code&                code) const
    //{
    //  Archive           archive;

    //  if (archive.Create() == Status::Error)
    //    escape("unable to create the archive");

    //  if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7) == Status::Error)
    //    escape("unable to serialize the object");

    //  if (this->Encrypt(archive, code) == Status::Error)
    //    escape("unable to encrypt the object's archive");

    //  return Status::Ok;
    //}

    //template <typename T1,
    //          typename T2,
    //          typename T3,
    //          typename T4,
    //          typename T5,
    //          typename T6,
    //          typename T7,
    //          typename T8>
    //Status              PublicKey::Encrypt(const T1&            o1,
    //                                       const T2&            o2,
    //                                       const T3&            o3,
    //                                       const T4&            o4,
    //                                       const T5&            o5,
    //                                       const T6&            o6,
    //                                       const T7&            o7,
    //                                       const T8&            o8,
    //                                       Code&                code) const
    //{
    //  Archive           archive;

    //  if (archive.Create() == Status::Error)
    //    escape("unable to create the archive");

    //  if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8) == Status::Error)
    //    escape("unable to serialize the object");

    //  if (this->Encrypt(archive, code) == Status::Error)
    //    escape("unable to encrypt the object's archive");

    //  return Status::Ok;
    //}

    //template <typename T1,
    //          typename T2,
    //          typename T3,
    //          typename T4,
    //          typename T5,
    //          typename T6,
    //          typename T7,
    //          typename T8,
    //          typename T9>
    //Status              PublicKey::Encrypt(const T1&            o1,
    //                                       const T2&            o2,
    //                                       const T3&            o3,
    //                                       const T4&            o4,
    //                                       const T5&            o5,
    //                                       const T6&            o6,
    //                                       const T7&            o7,
    //                                       const T8&            o8,
    //                                       const T9&            o9,
    //                                       Code&                code) const
    //{
    //  Archive           archive;

    //  if (archive.Create() == Status::Error)
    //    escape("unable to create the archive");

    //  if (archive.Serialize(o1, o2, o3, o4, o5, o6, o7, o8, o9) == Status::Error)
    //    escape("unable to serialize the object");

    //  if (this->Encrypt(archive, code) == Status::Error)
    //    escape("unable to encrypt the object's archive");

    //  return Status::Ok;
    //}

    ////
    //// verify
    ////

    /////
    ///// this method verifies an object by serializing it before performing
    ///// the verification process.
    /////
    //template <typename T,
    //          typename... TT>
    //Status              PublicKey::Verify(const Signature&      signature,
    //                                      const T&              parameter,
    //                                      const TT&...          parameters)
    //  const
    //{
    //  Archive           archive;

    //  // create the archive.
    //  if (archive.Create() == Status::Error)
    //    escape("unable to create the archive");

    //  // serialize the object.
    //  if (archive.Serialize(parameter, parameters...) == Status::Error)
    //    escape("unable to serialize the object");

    //  // call the Verify() method.
    //  if (this->Verify(signature, archive) == Status::Error)
    //    escape("unable to verify the signature against the object's archive");

    //  return Status::Ok;
    //}

    ////
    //// decrypt
    ////

    /////
    ///// this method decrypts a code and returns a pretty newly author
    ///// object.
    /////
    //template <typename T,
    //          typename... TT>
    //Status              PublicKey::Decrypt(const Code&          code,
    //                                       T&                   parameter,
    //                                       TT&...               parameters)
    //  const
    //{
    //  Archive           archive;

    //  // extract an archive.
    //  if (this->Decrypt(code, archive) == Status::Error)
    //    escape("unable to decrypt into an archive");

    //  // extract the item.
    //  if (archive.Extract(parameter, parameters...) == Status::Error)
    //    escape("unable to extract the items");

    //  return Status::Ok;
    //}

  }
}

#endif
