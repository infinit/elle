//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// author        julien quintard   [wed nov  2 17:14:08 2011]
//

#ifndef NUCLEUS_NEUTRON_ACCESS_HXX
#define NUCLEUS_NEUTRON_ACCESS_HXX

namespace nucleus
{
  namespace neutron
  {

//
// ---------- methods ---------------------------------------------------------
//

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

      ;

      // retrieve the record.
      if (this->Lookup(subject, record) == elle::StatusError)
        escape("unable to retrieve the subject's record");

      // update the record with the new permissions and stuff.
      if (record->Update(subject,
                         permissions,
                         stuff) == elle::StatusError)
        escape("unable to update the record");

      // set the block as dirty.
      this->_state = proton::StateDirty;

      return elle::StatusOk;
    }

  }
}

#endif
