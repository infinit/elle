//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/context/Directory.cc
//
// created       julien quintard   [sat aug 22 02:14:09 2009]
// updated       julien quintard   [wed mar  3 16:08:33 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Directory.hh>

namespace etoile
{
  namespace context
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor
    ///
    Directory::Directory():
      catalog(NULL)
    {
    }

    ///
    /// the copy constructor.
    ///
    Directory::Directory(const Directory&			context)
    {
      // XXX \todo XXX
      printf("NOT IMPLEMENTED YET\n");
    }

    ///
    /// the destructor
    ///
    Directory::~Directory()
    {
      // release the catalog.
      if (this->catalog != NULL)
	delete this->catalog;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Directory::Register(journal::Set::Container&	set)
    {
      enter();

      // call the object register method.
      if (Object::Register(set) == StatusError)
	escape("unable to register the object context");

      // if there is catalog.
      if (this->catalog != NULL)
	set.push_front(this->catalog);

      leave();
    }

  }
}
