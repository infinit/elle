//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/neutron/Contents.hh
//
// created       julien quintard   [mon aug 10 12:07:15 2009]
// updated       julien quintard   [sat may 14 12:09:08 2011]
//

#ifndef NUCLEUS_NEUTRON_CONTENTS_HH
#define NUCLEUS_NEUTRON_CONTENTS_HH

//
// ---------- includes --------------------------------------------------------
//

#include <nucleus/proton/ContentHashBlock.hh>

#include <nucleus/neutron/Size.hh>

namespace nucleus
{
  namespace neutron
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class abstracts the data, catalog or reference by embedding
    /// it since the contents is always encrypted.
    ///
    /// therefore, once a contents is retrieved from the storage, the Extract()
    /// method is called which basically extracts an archive i.e the encrypted
    /// version of the embedded block. Then the Decrypt() method can be called
    /// in order to  (i) decrypt the embedded archive (ii) extract it (iii)
    /// build the embedded unencrypted object.
    ///
    /// the _cipher_ attribute contains the data once encrypted, often
    /// just before being stored in the storage layer since there is no
    /// benefit in encrypting the data for fun.
    ///
    template <typename T>
    class Contents:
      public proton::ContentHashBlock
    {
    public:
      //
      // constructors & destructors
      //
      Contents();
      ~Contents();

      //
      // methods
      //
      elle::Status	Create();

      elle::Status	Encrypt(const elle::SecretKey&);
      elle::Status	Decrypt(const elle::SecretKey&);

      //
      // interfaces
      //

      // object
      declare(Contents<T>);

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      T*		content;

      elle::Cipher*	cipher;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <nucleus/neutron/Contents.hxx>

#endif
