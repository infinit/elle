//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/hole/Block.cc
//
// created       julien quintard   [fri sep 11 22:44:58 2009]
// updated       julien quintard   [fri may 28 18:01:15 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/hole/Block.hh>

namespace etoile
{
  namespace hole
  {

//
// ---------- definitions -----------------------------------------------------
//

  ///
  /// this string defines the block files extension.
  ///
  const elle::String		Block::Extension = ".blk";

//
// ---------- constructs & destructors ----------------------------------------
//

    Block::Block():
      family(FamilyUnknown),
      component(ComponentUnknown)
    {
    }

    Block::Block(const Family&					family,
		 const Component&				component):
      family(family),
      component(component)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method computes the address of the block.
    ///
    elle::Status	Block::Bind()
    {
      enter();

      fail("this method should never have been called");
    }

    ///
    /// this method returns StatusTrue if the block is valid, StatusFalse
    /// otherwise.
    ///
    elle::Status	Block::Validate(const Address&) const
    {
      enter();

      fail("this method should never have been called");
    }

    ///
    /// this method loads the block.
    ///
    elle::Status	Block::Load()
    {
      elle::String	path;
      elle::Unique	unique;
      elle::Region	region;

      enter();

      // first, turns the block's address into a string.
      if (address.Save(unique) == elle::StatusError)
	escape("unable to save the address' unique");

      // compute the path.
      path =
	lune::User::Hole + elle::System::Path::Separator +
	unique + Block::Extension;

      // read the file's content.
      if (File::Read(path, region) == StatusError)
	escape("unable to read the file's content");

      // decode and extract the object.
      if (Hexadecimal::Decode(String((char*)region.contents, region.size),
			      *this) == StatusError)
	escape("unable to decode the object");

      leave();
    }

    ///
    /// this method stores the block in its file format in the given universe.
    ///
    elle::Status	Block::Store() const
    {
      elle::String	path;
      elle::Unique	unique;
      elle::Region	region;
      elle::String	string;

      enter();

      // first, turns the block's address into a string.
      if (address.Save(unique) == elle::StatusError)
	escape("unable to save the address' unique");

      // compute the path.
      path =
	lune::User::Hole + elle::System::Path::Separator +
	unique + Block::Extension;

      // encode in hexadecimal.
      if (Hexadecimal::Encode(*this, string) == StatusError)
	escape("unable to encode the object in hexadecimal");

      // wrap the string.
      if (region.Wrap((Byte*)string.c_str(),
		      string.length()) == StatusError)
	escape("unable to wrap the string in a region");

      // write the file's content.
      if (File::Write(path, region) == StatusError)
	escape("unable to write the file's content");

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Block, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the block's internals.
    ///
    elle::Status	Block::Dump(const elle::Natural32	margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      std::cout << alignment << "[Block]" << std::endl;

      // dump the block address.
      if (this->address.Dump(margin + 2 ) == elle::StatusError)
	escape("unable to dump the address");

      // dump the family.
      std::cout << alignment << elle::Dumpable::Shift << "[Family] "
		<< (elle::Natural32)this->family << std::endl;

      // dump the component.
      std::cout << alignment << elle::Dumpable::Shift << "[Component] "
		<< (elle::Natural32)this->component << std::endl;

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method archives the block attributes.
    ///
    elle::Status	Block::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize((elle::Natural8&)this->family,
			    (elle::Natural8&)this->component) ==
	  elle::StatusError)
	escape("unable to serialize the block's attributes");

      leave();
    }

    ///
    /// this method extracts the attributes.
    ///
    elle::Status	Block::Extract(elle::Archive&		archive)
    {
      enter();

      // extracts the attributes.
      if (archive.Extract((elle::Natural8&)this->family,
			  (elle::Natural8&)this->component) ==
	  elle::StatusError)
	escape("unable to extract the block's attributes");

      leave();
    }

  }
}
