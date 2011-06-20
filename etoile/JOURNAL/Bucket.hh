//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/journal/Bucket.hh
//
// created       julien quintard   [sat jan 30 16:26:50 2010]
// updated       julien quintard   [fri may 20 20:16:34 2011]
//

#ifndef ETOILE_JOURNAL_BUCKET_HH
#define ETOILE_JOURNAL_BUCKET_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/journal/Item.hh>

#include <elle/idiom/Close.hh>
# include <vector>
#include <elle/idiom/Open.hh>

namespace etoile
{
  namespace journal
  {

//
// ---------- structures ------------------------------------------------------
//

    ///
    /// the bucket is used to collect the objects, hence blocks, that must
    /// be published or destroyed.
    ///
    struct Bucket:
      public elle::Entity
    {
    public:
      //
      // types
      //
      typedef std::vector<Item*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // constructors & destructors
      //
      ~Bucket();

      //
      // methods
      //
      elle::Status	Push(const nucleus::Address&,
			     nucleus::Block*);
      elle::Status	Wipe(const nucleus::Address&);

      //
      // interfaces
      //
      elle::Status	Dump(const elle::Natural32 = 0) const;

      //
      // attributes
      //
      Container		container;
    };

  }
}

#endif
