//
// ---------- header ----------------------------------------------------------
//
// project       applications
//
// license       infinit
//
// author        julien quintard   [fri dec 23 13:31:41 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8diary/Memoirs.hh>

namespace application
{

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an memoirs object.
    ///
    elle::Status        Memoirs::Dump(elle::Natural32           margin) const
    {
      elle::String      alignment(margin, ' ');

      ;

      // display the name.
      std::cout << alignment << "[Memoirs]" << std::endl;

      // dump the archive.
      if (this->archive.Dump(margin + 2) == elle::StatusError)
        escape("unable to dump the archive");

      return elle::StatusOk;
    }

//
// ---------- archivable ------------------------------------------------------
//

  ///
  /// this method serializes the memoirs object.
  ///
  elle::Status          Memoirs::Serialize(elle::Archive&       archive) const
  {
    ;

    // serialize the attributes.
    if (archive.Serialize(this->archive) == elle::StatusError)
      escape("unable to serialize the attributes");

    return elle::StatusOk;
  }

  ///
  /// this method extracts the memoirs object.
  ///
  elle::Status          Memoirs::Extract(elle::Archive&         archive)
  {
    ;

    // extract the attributes.
    if (archive.Extract(this->archive) == elle::StatusError)
      escape("unable to extract the attributes");

    return elle::StatusOk;
  }

//
// ---------- fileable --------------------------------------------------------
//

  ///
  /// this method loads the memoirs.
  ///
  /// note that since diaries can be very large---several gigabytes---the
  /// method handles the archive specifically, making sure that no
  /// copy is performed.
  ///
  elle::Status          Memoirs::Load(const elle::Path&         path)
  {
    elle::Region        region;

    ;

    // read the file's content.
    if (elle::File::Read(path, region) == elle::StatusError)
      escape("unable to read the file's content");

    // prepare the archive.
    if (this->archive.Acquire(region) == elle::StatusError)
      escape("unable to prepare the archive");

    // detach the data from the region.
    if (region.Detach() == elle::StatusError)
      escape("unable to detach the data");

    return elle::StatusOk;
  }

  ///
  /// this method stores the memoirs in its file format.
  ///
  /// note that since diaries can be very large---several gigabytes---the
  /// method handles the archive specifically, making sure that no
  /// copy is performed.
  ///
  elle::Status          Memoirs::Store(const elle::Path&        path)
    const
  {
    ;

    // write the file's content.
    if (elle::File::Write(
          path,
          elle::Region(
            this->archive.contents,
            this->archive.size)) == elle::StatusError)
      escape("unable to write the file's content");

    return elle::StatusOk;
  }

}
