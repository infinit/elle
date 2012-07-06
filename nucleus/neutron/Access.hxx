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
    /// the permissions are updated along with _stuff_ which can represent
    /// either a secret key which will be encrypted with the user's public
    /// key or a token already formed.
    ///
    template <typename T>
    elle::Status        Access::Update(const Subject&           subject,
                                       const Permissions&       permissions,
                                       const T&                 stuff)
    {
      Record*           record;

      // retrieve the record.
      if (this->Lookup(subject, record) == elle::Status::Error)
        escape("unable to retrieve the subject's record");

      // update the record with the new permissions and stuff.
      if (record->Update(subject,
                         permissions,
                         stuff) == elle::Status::Error)
        escape("unable to update the record");

      // set the block as dirty.
      this->state = proton::StateDirty;

      return elle::Status::Ok;
    }

  }
}

//
// ---------- serialize -------------------------------------------------------
//

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SIMPLE(nucleus::neutron::Access,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & static_cast<nucleus::proton::ContentHashBlock&>(value);
  archive & value.range;
}

#endif
