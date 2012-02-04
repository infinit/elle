//
// ---------- header ----------------------------------------------------------
//
// project       8diary
//
// license       infinit
//
// author        julien quintard   [sun jun 26 22:48:13 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8diary/unix/Memoirs.hh>

#include <applications/8diary/unix/Crux.hh>
#include <applications/8diary/unix/Record.hh>
#include <applications/8diary/unix/Replay.hh>

#if defined(INFINIT_LINUX)
# include <horizon/linux/Linux.hh>
#elif defined(INFINIT_MACOSX)
# include <horizon/macosx/MacOSX.hh>
#endif

namespace application
{
  namespace unix
  {

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Memoirs::Memoirs():
      mode(Memoirs::ModeUnknown)
    {
    }

    ///
    /// destructor.
    ///
    Memoirs::~Memoirs()
    {
      // nothing to do.
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method sets up the memoirs with the FUSE operations to
    /// which the upcalls must be forwarded through the memoirs.
    ///
    /// besides, this method initializes the recording session.
    ///
    elle::Status        Memoirs::Initialize(const elle::String& mountpoint,
                                            const elle::String& mirror)
    {
      enter();

      // set up the crux.
      if (Crux::Setup(mirror) == elle::StatusError)
        escape("unable to set up the crux");

      // set the mode.
      this->mode = Memoirs::ModeRecord;

      // create the archive.
      if (this->archive.Create() == elle::StatusError)
        escape("unable to create the archive");

      // initialize the record.
      if (Record::Initialize(this, mountpoint) == elle::StatusError)
        escape("unable to initialize the record");

      // set the attributes.
      this->fuse = Crux::Operations;

      leave();
    }

    ///
    /// this method sets up the memoirs with the FUSE operations to
    /// which the upcalls must be forwarded through the memoirs.
    ///
    /// besides, this method initializes the replaying session.
    ///
    elle::Status        Memoirs::Initialize(const elle::Natural32 from,
                                            const elle::Natural32 to)
    {
      enter();

      // set the mode.
      this->mode = Memoirs::ModeReplay;

      // set the offsets.
      this->offsets.from = from;
      this->offsets.to = to;

      // initialize the replay.
      if (Replay::Initialize(this) == elle::StatusError)
        escape("unable to initialize the replay");

#if defined(INFINIT_LINUX)
      // set the attributes.
      this->fuse = horizon::linux::FUSE::Operations;
#elif defined(INFINIT_MACOSX)
      // set the attributes.
      this->fuse = horizon::macosx::FUSE::Operations;
#endif

      leave();
    }

    ///
    /// this method cleans the memoirs.
    ///
    elle::Status        Memoirs::Clean()
    {
      enter();

      // depending on the mode.
      switch (this->mode)
        {
        case Memoirs::ModeRecord:
          {
            // clean the record.
            if (Record::Clean() == elle::StatusError)
              escape("unable to clean the recording session");

            break;
          }
        case Memoirs::ModeReplay:
          {
            // clean the replay.
            if (Replay::Clean() == elle::StatusError)
              escape("unable to clean the replaying session");

            break;
          }
        case Memoirs::ModeUnknown:
          {
            break;
          }
        }

      leave();
    }

    ///
    /// this method writes an upcall in the memoirs.
    ///
    elle::Status        Memoirs::Write(const Upcall&            upcall)
    {
      enter();

      // check the memoirs mode.
      if (this->mode != Memoirs::ModeRecord)
        escape("unable to write an upcall in a non-recording memoirs");

      // serialize the upcall.
      if (this->archive.Serialize(upcall) == elle::StatusError)
        escape("unable to serialize the upcall");

      leave();
    }

    ///
    /// this method reads an upcall from the memoirs.
    ///
    elle::Status        Memoirs::Read(Upcall&                   upcall)
    {
      enter();

      // check the memoirs mode.
      if (this->mode != Memoirs::ModeReplay)
        escape("unable to read an upcall from a non-replaying memoirs");

      // extract the upcall.
      if (this->archive.Extract(upcall) == elle::StatusError)
        escape("unable to extract the upcall");

      leave();
    }

    ///
    /// this method returns true if the end of the memoirs has been reached.
    ///
    elle::Status        Memoirs::End() const
    {
      enter();

      // have we reached the end of the archive.
      if (this->archive.offset == this->archive.size)
        true();

      false();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this function dumps an memoirs object.
    ///
    elle::Status        Memoirs::Dump(elle::Natural32           margin) const
    {
      elle::String      alignment(margin, ' ');

      enter();

      // display the name.
      std::cout << alignment << "[Memoirs]" << std::endl;

      // dump the parent class.
      if (application::Memoirs::Dump(margin + 2) == elle::StatusError)
        escape("unable to dump the parent class");

      // display the name.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Mode] " << this->mode << std::endl;

      // display the name.
      std::cout << alignment << elle::Dumpable::Shift
                << "[Offsets] "
                << this->offsets.from
                << ", "
                << this->offsets.to << std::endl;

      leave();
    }

  }
}
