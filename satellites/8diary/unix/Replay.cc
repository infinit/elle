//
// ---------- header ----------------------------------------------------------
//
// project       8diary
//
// license       infinit
//
// author        julien quintard   [thu jun 30 09:23:09 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8diary/unix/Replay.hh>
#include <applications/8diary/unix/Live.hh>

namespace application
{
#undef unix
  namespace unix
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable contains the address of the memoirs which is
    /// being replayed.
    ///
    Memoirs*				Replay::Reference = NULL;

    ///
    /// this variable contains the starting index.
    ///
    elle::Natural32			Replay::From = 0;

    ///
    /// this variable contains the ending index.
    ///
    elle::Natural32			Replay::To =
      elle::Type<elle::Natural32>::Maximum;

    ///
    /// this variable contains the timer which triggers the replay.
    ///
    /// this timer is required in order to wait for program's loop to
    /// start.
    ///
    elle::Timer*			Replay::Timer = NULL;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    elle::Status	Replay::Getattr(Upcall&			upcall)
    {
      struct
      {
	elle::String	path;
	elle::Region	stbuf;
      }			inputs;
      struct
      {
	elle::Region	stbuf;
      }			outputs;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.stbuf) == elle::StatusError)
	escape("unable to extract the region");

      res = Replay::Reference->fuse.getattr(
	      inputs.path.c_str(),
	      reinterpret_cast<struct ::stat*>(inputs.stbuf.contents));

      if (upcall.outputs.Extract(outputs.stbuf) == elle::StatusError)
	escape("unable to extract the region");

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      struct ::stat*	stbuf =
	reinterpret_cast<struct ::stat*>(inputs.stbuf.contents);
      struct ::stat*	_stbuf =
	reinterpret_cast<struct ::stat*>(outputs.stbuf.contents);

      if (stbuf->st_uid != _stbuf->st_uid)
	escape("invalid UID: got(%u) expected(%u)",
	       stbuf->st_uid, _stbuf->st_uid);
      /*
      if (stbuf->st_gid != _stbuf->st_gid)
	escape("invalid GID");

      if (stbuf->st_mode != _stbuf->st_mode)
	escape("invalid mode");

      if (stbuf->st_size != _stbuf->st_size)
	escape("invalid size");
      */
      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Fgetattr(Upcall&		upcall)
    {
      struct
      {
	elle::String	path;
	elle::Region	stbuf;
	elle::Region	fi;
      }			inputs;
      struct
      {
	elle::Region	stbuf;
	elle::Region	fi;
      }			outputs;
      elle::Natural64	identifier;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.stbuf) == elle::StatusError)
	escape("unable to extract the region");

      if (upcall.inputs.Extract(inputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      struct ::fuse_file_info*	fi =
	reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::StatusError)
	escape("unable to retrieve the file information");

      res = Replay::Reference->fuse.fgetattr(
	      inputs.path.c_str(),
	      reinterpret_cast<struct ::stat*>(inputs.stbuf.contents),
	      fi);

      if (upcall.outputs.Extract(outputs.stbuf) == elle::StatusError)
	escape("unable to extract the region");

      if (upcall.outputs.Extract(outputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      struct ::stat*	stbuf =
	reinterpret_cast<struct ::stat*>(inputs.stbuf.contents);
      struct ::stat*	_stbuf =
	reinterpret_cast<struct ::stat*>(outputs.stbuf.contents);

      if (stbuf->st_uid != _stbuf->st_uid)
	escape("invalid UID: got(%u) expected(%u)",
	       stbuf->st_uid, _stbuf->st_uid);
      /* XXX
      if (stbuf->st_gid != _stbuf->st_gid)
	escape("invalid GID");

      if (stbuf->st_mode != _stbuf->st_mode)
	escape("invalid mode");

      if (stbuf->st_size != _stbuf->st_size)
	escape("invalid size");
      */

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Utimens(Upcall&			upcall)
    {
      struct
      {
	elle::String	path;
	elle::Region	ts;
      }			inputs;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.ts) == elle::StatusError)
	escape("unable to extract the region");

      res = Replay::Reference->fuse.utimens(
	      inputs.path.c_str(),
	      reinterpret_cast<struct ::timespec*>(inputs.ts.contents));

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Opendir(Upcall&			upcall)
    {
      struct
      {
	elle::String	path;
	elle::Region	fi;
      }			inputs;
      struct
      {
	elle::Region	fi;
      }			outputs;
      elle::Natural64	identifier;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      struct ::fuse_file_info*	fi =
	reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      res = Replay::Reference->fuse.opendir(
	      inputs.path.c_str(),
	      fi);

      if (upcall.outputs.Extract(outputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      struct ::fuse_file_info*	_fi =
	reinterpret_cast<struct ::fuse_file_info*>(outputs.fi.contents);

      identifier = _fi->fh;

      if (Live::Add(identifier, fi->fh) == elle::StatusError)
	escape("unable to add the file information");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Readdir(Upcall&			upcall)
    {
      struct
      {
	elle::String	path;
	elle::Natural64	offset;
	elle::Region	fi;
      }			inputs;
      struct
      {
	elle::Region	fi;
      }			outputs;
      elle::Natural64	identifier;
      int		res;

      enter();

      // XXX to improve with buf

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.offset) == elle::StatusError)
	escape("unable to extract the offset");

      if (upcall.inputs.Extract(inputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      struct ::fuse_file_info*	fi =
	reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::StatusError)
	escape("unable to retrieve the file information");

      /* XXX implement a filler-like function which must be passed
      res = Replay::Reference->fuse.readdir(
	      inputs.path.c_str(),
	      fi);

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);
      */

      if (upcall.outputs.Extract(outputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      // XXX

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Releasedir(Upcall&		upcall)
    {
      struct
      {
	elle::String	path;
	elle::Region	fi;
      }			inputs;
      struct
      {
	elle::Region	fi;
      }			outputs;
      elle::Natural64	identifier;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      struct ::fuse_file_info*	fi =
	reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::StatusError)
	escape("unable to retrieve the file information");

      res = Replay::Reference->fuse.releasedir(
	      inputs.path.c_str(),
	      fi);

      if (upcall.outputs.Extract(outputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      if (Live::Remove(identifier) == elle::StatusError)
	escape("unable to remove the file information");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Mkdir(Upcall&			upcall)
    {
      struct
      {
	elle::String	path;
	elle::Natural32	mode;
      }			inputs;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.mode) == elle::StatusError)
	escape("unable to extract the number");

      res = Replay::Reference->fuse.mkdir(
	      inputs.path.c_str(),
	      static_cast<mode_t>(inputs.mode));

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Rmdir(Upcall&			upcall)
    {
      struct
      {
	elle::String	path;
      }			inputs;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      res = Replay::Reference->fuse.rmdir(
	      inputs.path.c_str());

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Access(Upcall&			upcall)
    {
      struct
      {
	elle::String	path;
	elle::Integer32	mask;
      }			inputs;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.mask) == elle::StatusError)
	escape("unable to extract the number");

      res = Replay::Reference->fuse.access(
	      inputs.path.c_str(),
	      static_cast<int>(inputs.mask));

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Chmod(Upcall&			upcall)
    {
      struct
      {
	elle::String	path;
	elle::Natural32	mode;
      }			inputs;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.mode) == elle::StatusError)
	escape("unable to extract the number");

      res = Replay::Reference->fuse.chmod(
	      inputs.path.c_str(),
	      static_cast<mode_t>(inputs.mode));

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Chown(Upcall&			upcall)
    {
      struct
      {
	elle::String	path;
	elle::Natural32	uid;
	elle::Natural32	gid;
      }			inputs;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.uid) == elle::StatusError)
	escape("unable to extract the number");

      if (upcall.inputs.Extract(inputs.gid) == elle::StatusError)
	escape("unable to extract the number");

      res = Replay::Reference->fuse.chown(
	      inputs.path.c_str(),
	      static_cast<uid_t>(inputs.uid),
	      static_cast<gid_t>(inputs.gid));

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Setxattr(Upcall&		upcall)
    {
      struct
      {
	elle::String	path;
	elle::String	name;
	elle::String	value;
	elle::Natural64	size;
	elle::Integer32	flags;
      }			inputs;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.name) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.value) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.size) == elle::StatusError)
	escape("unable to extract the number");

      if (upcall.inputs.Extract(inputs.flags) == elle::StatusError)
	escape("unable to extract the number");

      res = Replay::Reference->fuse.setxattr(
	      inputs.path.c_str(),
	      inputs.name.c_str(),
	      inputs.value.c_str(),
	      static_cast<size_t>(inputs.size),
	      static_cast<int>(inputs.flags));

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Getxattr(Upcall&		upcall)
    {
      struct
      {
	elle::String	path;
	elle::String	name;
	elle::String	value;
	elle::Natural64	size;
      }			inputs;
      struct
      {
	elle::String	value;
      }			outputs;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.name) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.value) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.size) == elle::StatusError)
	escape("unable to extract the number");

      char		value[inputs.size];

      res = Replay::Reference->fuse.getxattr(
	      inputs.path.c_str(),
	      inputs.name.c_str(),
	      value,
	      static_cast<size_t>(inputs.size));

      if (upcall.outputs.Extract(outputs.value) == elle::StatusError)
	escape("unable to extract the string");

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      if (elle::String(value, res) != outputs.value)
	escape("invalid value: got(%s) expected(%s)",
	       elle::String(value, res).c_str(), outputs.value.c_str());

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Listxattr(Upcall&		upcall)
    {
      struct
      {
	elle::String	path;
	elle::String	list;
	elle::Natural64	size;
      }			inputs;
      struct
      {
	elle::String	list;
      }			outputs;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.list) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.size) == elle::StatusError)
	escape("unable to extract the number");

      char		list[inputs.size];

      res = Replay::Reference->fuse.listxattr(
	      inputs.path.c_str(),
	      list,
	      static_cast<size_t>(inputs.size));

      if (upcall.outputs.Extract(outputs.list) == elle::StatusError)
	escape("unable to extract the string");

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      if (elle::String(list, res) != outputs.list)
	escape("invalid value: got(%s) expected(%s)",
	       elle::String(list, res).c_str(), outputs.list.c_str());

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Removexattr(Upcall&		upcall)
    {
      struct
      {
	elle::String	path;
	elle::String	name;
      }			inputs;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.name) == elle::StatusError)
	escape("unable to extract the string");

      res = Replay::Reference->fuse.removexattr(
	      inputs.path.c_str(),
	      inputs.name.c_str());

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Symlink(Upcall&			upcall)
    {
      struct
      {
	elle::String	to;
	elle::String	from;
      }			inputs;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.to) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.from) == elle::StatusError)
	escape("unable to extract the string");

      res = Replay::Reference->fuse.symlink(
	      inputs.to.c_str(),
	      inputs.from.c_str());

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Readlink(Upcall&		upcall)
    {
      struct
      {
	elle::String	path;
	elle::String	buf;
	elle::Natural64	size;
      }			inputs;
      struct
      {
	elle::String	buf;
      }			outputs;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.buf) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.size) == elle::StatusError)
	escape("unable to extract the number");

      char		buf[inputs.size];

      res = Replay::Reference->fuse.readlink(
	      inputs.path.c_str(),
	      buf,
	      static_cast<size_t>(inputs.size));

      if (upcall.outputs.Extract(outputs.buf) == elle::StatusError)
	escape("unable to extract the string");

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      if (elle::String(buf) != outputs.buf)
	escape("invalid value: got(%s) expected(%s)",
	       elle::String(buf).c_str(), outputs.buf.c_str());

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Create(Upcall&			upcall)
    {
      struct
      {
	elle::String	path;
	elle::Natural32	mode;
	elle::Region	fi;
      }			inputs;
      struct
      {
	elle::Region	fi;
      }			outputs;
      elle::Natural64	identifier;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.mode) == elle::StatusError)
	escape("unable to extract the number");

      if (upcall.inputs.Extract(inputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      struct ::fuse_file_info*	fi =
	reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      res = Replay::Reference->fuse.create(
	      inputs.path.c_str(),
	      static_cast<mode_t>(inputs.mode),
	      fi);

      if (upcall.outputs.Extract(outputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      struct ::fuse_file_info*	_fi =
	reinterpret_cast<struct ::fuse_file_info*>(outputs.fi.contents);

      identifier = _fi->fh;

      if (Live::Add(identifier, fi->fh) == elle::StatusError)
	escape("unable to add the file information");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Open(Upcall&			upcall)
    {
      struct
      {
	elle::String	path;
	elle::Region	fi;
      }			inputs;
      struct
      {
	elle::Region	fi;
      }			outputs;
      elle::Natural64	identifier;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      struct ::fuse_file_info*	fi =
	reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      res = Replay::Reference->fuse.open(
	      inputs.path.c_str(),
	      fi);

      if (upcall.outputs.Extract(outputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      struct ::fuse_file_info*	_fi =
	reinterpret_cast<struct ::fuse_file_info*>(outputs.fi.contents);

      identifier = _fi->fh;

      if (Live::Add(identifier, fi->fh) == elle::StatusError)
	escape("unable to add the file information");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Write(Upcall&			upcall)
    {
      struct
      {
	elle::String	path;
	elle::Region	buf;
	elle::Natural64	size;
	elle::Natural64	off;
	elle::Region	fi;
      }			inputs;
      struct
      {
	elle::Region	fi;
      }			outputs;
      elle::Natural64	identifier;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.buf) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.size) == elle::StatusError)
	escape("unable to extract the number");

      if (upcall.inputs.Extract(inputs.off) == elle::StatusError)
	escape("unable to extract the number");

      if (upcall.inputs.Extract(inputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      struct ::fuse_file_info*	fi =
	reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::StatusError)
	escape("unable to retrieve the file information");

      res = Replay::Reference->fuse.write(
	      inputs.path.c_str(),
	      reinterpret_cast<char*>(inputs.buf.contents),
	      static_cast<size_t>(inputs.size),
	      static_cast<off_t>(inputs.off),
	      fi);

      if (upcall.outputs.Extract(outputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Read(Upcall&			upcall)
    {
      struct
      {
	elle::String	path;
	elle::Region	buf;
	elle::Natural64	size;
	elle::Natural64	off;
	elle::Region	fi;
      }			inputs;
      struct
      {
	elle::Region	buf;
      }			outputs;
      elle::Natural64	identifier;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.buf) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.size) == elle::StatusError)
	escape("unable to extract the number");

      if (upcall.inputs.Extract(inputs.off) == elle::StatusError)
	escape("unable to extract the number");

      if (upcall.inputs.Extract(inputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      struct ::fuse_file_info*	fi =
	reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::StatusError)
	escape("unable to retrieve the file information");

      res = Replay::Reference->fuse.read(
	      inputs.path.c_str(),
	      reinterpret_cast<char*>(inputs.buf.contents),
	      static_cast<size_t>(inputs.size),
	      static_cast<off_t>(inputs.off),
	      fi);

      if (upcall.outputs.Extract(outputs.buf) == elle::StatusError)
	escape("unable to extract the region");

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      if (inputs.buf != outputs.buf)
	escape("invalid buffer");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Truncate(Upcall&		upcall)
    {
      struct
      {
	elle::String	path;
	elle::Natural64	size;
      }			inputs;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.size) == elle::StatusError)
	escape("unable to extract the number");

      res = Replay::Reference->fuse.truncate(
	      inputs.path.c_str(),
	      static_cast<off_t>(inputs.size));

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Ftruncate(Upcall&		upcall)
    {
      struct
      {
	elle::String	path;
	elle::Natural64	size;
	elle::Region	fi;
      }			inputs;
      struct
      {
	elle::Region	fi;
      }			outputs;
      elle::Natural64	identifier;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.size) == elle::StatusError)
	escape("unable to extract the number");

      if (upcall.inputs.Extract(inputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      struct ::fuse_file_info*	fi =
	reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::StatusError)
	escape("unable to retrieve the file information");

      res = Replay::Reference->fuse.ftruncate(
	      inputs.path.c_str(),
	      static_cast<size_t>(inputs.size),
	      fi);

      if (upcall.outputs.Extract(outputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Release(Upcall&			upcall)
    {
      struct
      {
	elle::String	path;
	elle::Region	fi;
      }			inputs;
      struct
      {
	elle::Region	fi;
      }			outputs;
      elle::Natural64	identifier;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      struct ::fuse_file_info*	fi =
	reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::StatusError)
	escape("unable to retrieve the file information");

#include <elle/idiom/Close.hh>
      res = Replay::Reference->fuse.release(
	      inputs.path.c_str(),
	      fi);
#include <elle/idiom/Open.hh>

      if (upcall.outputs.Extract(outputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      if (Live::Remove(identifier) == elle::StatusError)
	escape("unable to remove the file information");

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Rename(Upcall&			upcall)
    {
      struct
      {
	elle::String	from;
	elle::String	to;
      }			inputs;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.from) == elle::StatusError)
	escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.to) == elle::StatusError)
	escape("unable to extract the string");

      res = Replay::Reference->fuse.rename(
	      inputs.from.c_str(),
	      inputs.to.c_str());

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      leave();
    }

    ///
    /// XXX
    ///
    elle::Status	Replay::Unlink(Upcall&			upcall)
    {
      struct
      {
	elle::String	path;
      }			inputs;
      int		res;

      enter();

      if (upcall.inputs.Extract(inputs.path) == elle::StatusError)
	escape("unable to extract the string");

      res = Replay::Reference->fuse.unlink(
	      inputs.path.c_str());

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      leave();
    }

    ///
    /// this method initializes FUSE.
    ///
    elle::Status	Replay::Initialize(Memoirs*		memoirs)
    {
      enter();

      // set the memoirs pointer.
      Replay::Reference = memoirs;

      // initialize the live system.
      if (Live::Initialize() == elle::StatusError)
	escape("unable to initialize the live system");

      leave();
    }

    ///
    /// this method launches the replaying.
    ///
    elle::Status	Replay::Launch(const elle::Natural32	from,
				       const elle::Natural32	to)
    {
      enter();

      // set the boundaries.
      Replay::From = from;
      Replay::To = to;

      // allocate the timer.
      Replay::Timer = new elle::Timer;

      // create the timer which will start the replaying process.
      if (Replay::Timer->Create(elle::Timer::ModeSingle) == elle::StatusError)
	escape("unable to create the timer");

      // subscribe to the timer's signal.
      if (Replay::Timer->signal.timeout.Subscribe(
	    elle::Callback<>::Infer(&Replay::Process)) == elle::StatusError)
	escape("unable to subscribe to the signal");

      // start the timer.
      //
      // note that the timer is started immediately so that as soon as
      // the program enters its event loop, the timer is launched.
      if (Replay::Timer->Start(0) == elle::StatusError)
	escape("unable to start the timer");

      leave();
    }

    ///
    /// this method processes an upcall.
    ///
    elle::Status	Replay::Process()
    {
      elle::Natural32	i;

      enter();

      // first, go through the upcalls that must be ignored.
      for (i = 0;
	   (i < Replay::From) &&
	     (Replay::Reference->End() == elle::StatusFalse);
	   i++)
	{
	  Upcall		upcall;

	  // retrieve the upcall.
	  if (Replay::Reference->Read(upcall) == elle::StatusError)
	    goto _error;
	}

      // then go through the remaining upcalls up to 'to'.
      for (;
	   (i < Replay::To) &&
	     (Replay::Reference->End() == elle::StatusFalse);
	    i++)
	{
	  Upcall	upcall;

	  // retrieve the upcall.
	  if (Replay::Reference->Read(upcall) == elle::StatusError)
	    goto _error;

	  // log the upcall.
	  log("[Replay] index(%u) opcode(%u)",
	      i,
	      upcall.operation);

	  // forward the call to a specific method depending on
	  // the operation code.
	  switch (upcall.operation)
	    {
	    case Upcall::OperationGetattr:
	      {
		// call the method.
		if (Replay::Getattr(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationFgetattr:
	      {
		// call the method.
		if (Replay::Fgetattr(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationUtimens:
	      {
		// call the method.
		if (Replay::Utimens(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationOpendir:
	      {
		// call the method.
		if (Replay::Opendir(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationReaddir:
	      {
		// call the method.
		if (Replay::Readdir(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationReleasedir:
	      {
		// call the method.
		if (Replay::Releasedir(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationMkdir:
	      {
		// call the method.
		if (Replay::Mkdir(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationRmdir:
	      {
		// call the method.
		if (Replay::Rmdir(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationAccess:
	      {
		// call the method.
		if (Replay::Access(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationChmod:
	      {
		// call the method.
		if (Replay::Chmod(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationChown:
	      {
		// call the method.
		if (Replay::Chown(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationSetxattr:
	      {
		// call the method.
		if (Replay::Setxattr(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationGetxattr:
	      {
		// call the method.
		if (Replay::Getxattr(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationListxattr:
	      {
		// call the method.
		if (Replay::Listxattr(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationRemovexattr:
	      {
		// call the method.
		if (Replay::Removexattr(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationSymlink:
	      {
		// call the method.
		if (Replay::Symlink(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationReadlink:
	      {
		// call the method.
		if (Replay::Readlink(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationCreate:
	      {
		// call the method.
		if (Replay::Create(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationOpen:
	      {
		// call the method.
		if (Replay::Open(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationWrite:
	      {
		// call the method.
		if (Replay::Write(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationRead:
	      {
		// call the method.
		if (Replay::Read(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationTruncate:
	      {
		// call the method.
		if (Replay::Truncate(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationFtruncate:
	      {
		// call the method.
		if (Replay::Ftruncate(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationRelease:
	      {
		// call the method.
		if (Replay::Release(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationRename:
	      {
		// call the method.
		if (Replay::Rename(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    case Upcall::OperationUnlink:
	      {
		// call the method.
		if (Replay::Unlink(upcall) == elle::StatusError)
		  goto _error;

		break;
	      }
	    default:
	      {
		report("unknown operation code '%u'",
		       upcall.operation);

		goto _error;
	      }
	    }
	}

    _error:
      // exit the program.
      elle::Program::Exit();

      leave();
    }

    ///
    /// this method cleans FUSE.
    ///
    elle::Status	Replay::Clean()
    {
      enter();

      // delete the timer.
      if (Replay::Timer != NULL)
	delete Replay::Timer;

      // reset the memoirs pointer.
      Replay::Reference = NULL;

      // clean the live system.
      if (Live::Clean() == elle::StatusError)
	escape("unable to clean the live system");

      leave();
    }

  }
}
