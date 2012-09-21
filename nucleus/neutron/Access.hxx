#ifndef NUCLEUS_NEUTRON_ACCESS_HXX
# define NUCLEUS_NEUTRON_ACCESS_HXX

namespace nucleus
{
  namespace neutron
  {

    ///
    /// this method updates the access block, more precisely the record
    /// associated with the _subject_.
    ///
    /// the permissions are updated along with _secret_ which can represent
    /// either a secret key which will be encrypted with the user's public
    /// key or a token already formed.
    ///
    template <typename T>
    elle::Status
    Access::Update(Subject const& subject,
                   Permissions permissions,
                   T const& secret,
                   elle::cryptography::PublicKey const& K)
    {
      // XXX[remove try/catch later]
      try
        {
          Token token(K, secret);

          if (this->Update(subject, permissions, token) == elle::Status::Error)
            escape("unable to update the record");
        }
      catch (std::exception const& e)
        {
          escape("%s", e.what());
        }

      return (elle::Status::Ok);
    }

  }
}

//
// ---------- serialize -------------------------------------------------------
//

# include <cassert>

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Access,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & base_class<nucleus::proton::ContentHashBlock>(value);
  archive & value._range;
}

#endif
