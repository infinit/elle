//
// ---------- header ----------------------------------------------------------
//
// project       diary
//
// license       infinit
//
// author        julien quintard   [fri dec 23 13:31:41 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <satellites/diary/Memoirs.hh>

#include <elle/idiom/Open.hh>

namespace satellite
{

  Memoirs::Memoirs()
      : archive()
      , offset(0)
    {}
//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an memoirs object.
    ///
    elle::Status        Memoirs::Dump(elle::Natural32           margin) const
    {
      elle::String      alignment(margin, ' ');

      // display the name.
      std::cout << alignment << "[Memoirs]" << std::endl;

      // dump the archive.
      if (this->archive.Dump(margin + 2) == elle::Status::Error)
        escape("unable to dump the archive");

      return elle::Status::Ok;
    }

}
