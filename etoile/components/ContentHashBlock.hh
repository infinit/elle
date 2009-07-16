//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/infinit/components/ContentHashBlock.hh
//
// created       julien quintard   [tue feb 17 12:33:12 2009]
// updated       julien quintard   [wed mar 11 16:10:50 2009]
//

#ifndef ETOILE_COMPONENTS_CONTENTHASHBLOCK_HH
#define ETOILE_COMPONENTS_CONTENTHASHBLOCK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/components/Block.hh>

namespace etoile
{
  namespace components
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class ContentHashBlock:
      public Block
    {
    public:
      //
      // constants
      //
      static const String		Class;

      //
      // constructors & destructors
      //
      ContentHashBlock()
      {
	ContentHashBlock::New(*this);
      }

      ~ContentHashBlock()
      {
	ContentHashBlock::Delete(*this);
      }

      //
      // methods
      //
      Status		Seal();

      //
      // interfaces
      //

      // object
      static Status	New(ContentHashBlock&);
      static Status	Delete(ContentHashBlock&);

      ContentHashBlock&	operator=(const ContentHashBlock&);
      Boolean		operator==(const ContentHashBlock&);
      Boolean		operator!=(const ContentHashBlock&);

      // dumpable
      Status		Dump(const Natural32 = 0);

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);
    };

  }
}

#endif
