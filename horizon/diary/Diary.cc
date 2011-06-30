//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/diary/Diary.cc
//
// created       julien quintard   [sun jun 26 22:48:13 2011]
// updated       julien quintard   [thu jun 30 09:27:40 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/diary/Diary.hh>
#include <pig/diary/Record.hh>
#include <pig/diary/Replay.hh>

namespace pig
{
  namespace diary
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Diary::Diary():
      mode(Diary::ModeUnknown),
      number(0)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method sets up the diary with the FUSE operations to
    /// which the upcalls must be forwarded through the diary.
    ///
    elle::Status	Diary::Setup(const ::fuse_operations&	fuse)
    {
      enter();

      // set the attributes.
      this->fuse = fuse;

      leave();
    }

    ///
    /// this method starts the recording.
    ///
    elle::Status	Diary::Record(elle::String&		mountpoint)
    {
      enter();

      // check the mode.
      if (this->mode != Diary::ModeUnknown)
	escape("unable to start recording an already running diary");

      // set the mode.
      this->mode = Diary::ModeRecord;

      // create the archive.
      if (this->archive.Create() == elle::StatusError)
	escape("unable to create the archive");

      // initialize the record.
      if (Record::Initialize(this) == elle::StatusError)
	escape("unable to initialize the record");

      // launch the record.
      if (Record::Launch(mountpoint) == elle::StatusError)
	escape("unable to launch the record");

      // clean the record.
      if (Record::Clean() == elle::StatusError)
	escape("unable to clean the record");

      leave();
    }

    ///
    /// this method starts the replaying.
    ///
    elle::Status	Diary::Replay()
    {
      enter();

      // check the mode.
      if (this->mode != Diary::ModeUnknown)
	escape("unable to start replaying an already running diary");

      // set the mode.
      this->mode = Diary::ModeReplay;

      // initialize the replay.
      if (Replay::Initialize(this) == elle::StatusError)
	escape("unable to initialize the replay");

      // launch the replay.
      if (Replay::Launch() == elle::StatusError)
	escape("unable to launch the replay");

      // clean the replay.
      if (Replay::Clean() == elle::StatusError)
	escape("unable to clean the replay");

      leave();
    }

    ///
    /// this method writes an upcall in the diary.
    ///
    elle::Status	Diary::Write(const Upcall&		upcall)
    {
      enter();

      // check the diary mode.
      if (this->mode != Diary::ModeRecord)
	escape("unable to write an upcall in a non-recording diary");

      // serialize the upcall.
      if (this->archive.Serialize(upcall) == elle::StatusError)
	escape("unable to serialize the upcall");

      // increase the number.
      this->number += 1;

      leave();
    }

    ///
    /// this method reads an upcall from the diary.
    ///
    elle::Status	Diary::Read(Upcall&			upcall)
    {
      enter();

      // check the diary mode.
      if (this->mode != Diary::ModeReplay)
	escape("unable to read an upcall from a non-replaying diary");

      // extract the upcall.
      if (this->archive.Extract(upcall) == elle::StatusError)
	escape("unable to extract the upcall");

      leave();
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Diary, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an diary object.
    ///
    elle::Status	Diary::Dump(elle::Natural32		margin) const
    {
      elle::String	alignment(margin, ' ');

      enter();

      // display the name.
      std::cout << alignment << "[Diary]" << std::endl;

      // display the name.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Mode] " << (elle::Natural32)this->mode << std::endl;

      // display the name.
      std::cout << alignment << elle::Dumpable::Shift
		<< "[Number] " << this->number << std::endl;

      // dump the archive.
      if (this->archive.Dump(margin + 2) == elle::StatusError)
	escape("unable to dump the archive");

      leave();
    }

//
// ---------- archivable ------------------------------------------------------
//

    ///
    /// this method serializes the diary object.
    ///
    elle::Status	Diary::Serialize(elle::Archive&	archive) const
    {
      enter();

      // serialize the attributes.
      if (archive.Serialize(this->number,
			    this->archive) == elle::StatusError)
	escape("unable to serialize the attributes");

      leave();
    }

    ///
    /// this method extracts the diary object.
    ///
    elle::Status	Diary::Extract(elle::Archive&		archive)
    {
      enter();

      // extract the attributes.
      if (archive.Extract(this->number,
			  this->archive) == elle::StatusError)
	escape("unable to extract the attributes");

      leave();
    }

  }
}
