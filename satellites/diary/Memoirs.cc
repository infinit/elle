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

//
// ---------- archivable ------------------------------------------------------
//

  ///
  /// this method serializes the memoirs object.
  ///
  //elle::Status          Memoirs::Serialize(elle::Archive&       archive) const
  //{
  //  // serialize the attributes.
  //  if (archive.Serialize(this->archive) == elle::Status::Error)
  //    escape("unable to serialize the attributes");

  //  return elle::Status::Ok;
  //}

  /////
  ///// this method extracts the memoirs object.
  /////
  //elle::Status          Memoirs::Extract(elle::Archive&         archive)
  //{
  //  // extract the attributes.
  //  if (archive.Extract(this->archive) == elle::Status::Error)
  //    escape("unable to extract the attributes");

  //  return elle::Status::Ok;
  //}

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
  //elle::Status          Memoirs::Load(const elle::Path&         path)
  //{
  //  elle::standalone::Region        region;

  //  // read the file's content.
  //  if (elle::io::File::Read(path, region) == elle::Status::Error)
  //    escape("unable to read the file's content");

  //  // prepare the archive.
  //  if (this->archive.Acquire(region) == elle::Status::Error)
  //    escape("unable to prepare the archive");

  //  // detach the data from the region.
  //  if (region.Detach() == elle::Status::Error)
  //    escape("unable to detach the data");

  //  return elle::Status::Ok;
  //}

  ///
  /// this method stores the memoirs in its file format.
  ///
  /// note that since diaries can be very large---several gigabytes---the
  /// method handles the archive specifically, making sure that no
  /// copy is performed.
  ///
  //elle::Status          Memoirs::Store(const elle::Path&        path)
  //  const
  //{
  //  // write the file's content.
  //  if (elle::io::File::Write(
  //        path,
  //        elle::standalone::Region(
  //          this->archive.contents,
  //          this->archive.size)) == elle::Status::Error)
  //    escape("unable to write the file's content");

  //  return elle::Status::Ok;
  //}

}
