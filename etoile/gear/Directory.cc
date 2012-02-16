//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [sat aug 22 02:14:09 2009]
//

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/gear/Directory.hh>
#include <etoile/gear/Nature.hh>

#include <etoile/depot/Depot.hh>

#include <agent/Agent.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the constructor
    ///
    Directory::Directory():
      Object(NatureDirectory),

      contents(NULL)
    {
    }

    ///
    /// the destructor
    ///
    Directory::~Directory()
    {
      // release the contents.
      if (this->contents != NULL)
        delete this->contents;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the contents along the the inherited object context.
    ///
    elle::Status        Directory::Dump(const elle::Natural32   margin) const
    {
      elle::String      alignment(margin, ' ');

      std::cout << alignment << "[Directory]" << std::endl;

      // dump the inherited object.
      if (Object::Dump(margin + 2) == elle::StatusError)
        escape("unable to dump the inherited object");

      // dump the contents.
      if (this->contents != NULL)
        {
          if (this->contents->Dump(margin + 4) == elle::StatusError)
            escape("unable to dump the contents");
        }
      else
        {
          std::cout << alignment << elle::Dumpable::Shift
                    << "[Contents] " << elle::none << std::endl;
        }

      return elle::StatusOk;
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the directory.
    ///
    elle::Status        Directory::Serialize(elle::Archive&     archive) const
    {
      // serialize the contents.
      if (this->contents == NULL)
        {
          // serialize the contents.
          if (archive.Serialize(*this->contents) == elle::StatusError)
            escape("unable to serialize the contents");
        }
      else
        {
          // serialize 'none'.
          if (archive.Serialize(elle::none) == elle::StatusError)
            escape("unable to serialize 'none'");
        }

      return elle::StatusOk;
    }

    ///
    /// this method extracts the directory.
    ///
    elle::Status        Directory::Extract(elle::Archive&       archive)
    {
      elle::Archive::Type       type;

      // fetch the next element's type.
      if (archive.Fetch(type) == elle::StatusError)
        escape("unable to fetch the next element's type");

      // extract the access.
      if (type == elle::Archive::TypeNull)
        {
          // extract 'none'.
          if (archive.Extract(elle::none) == elle::StatusError)
            escape("unable to extract 'none'");
        }
      else
        {
          // allocate an contents.
          this->contents = new nucleus::Contents<typename Directory::C>;

          // extract the contents.
          if (archive.Extract(*this->contents) == elle::StatusError)
            escape("unable to extract the contents");
        }

      return elle::StatusOk;
    }

  }
}
