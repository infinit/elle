//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/diary/Replay.cc
//
// created       julien quintard   [thu jun 30 09:23:09 2011]
// updated       julien quintard   [tue jul 19 20:18:54 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <pig/diary/Replay.hh>
#include <pig/diary/Live.hh>

namespace pig
{
  namespace diary
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable contains the address of the diary which is
    /// being replayed.
    ///
    Diary*				Replay::Reference = NULL;

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
    elle::Timer				Replay::Timer;

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
	      (struct ::stat*)inputs.stbuf.contents);

      if (upcall.outputs.Extract(outputs.stbuf) == elle::StatusError)
	escape("unable to extract the region");

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      struct ::stat*	stbuf = (struct ::stat*)inputs.stbuf.contents;
      struct ::stat*	_stbuf = (struct ::stat*)outputs.stbuf.contents;

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
	(struct ::fuse_file_info*)inputs.fi.contents;

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::StatusError)
	fail("unable to retrieve the file information");

      res = Replay::Reference->fuse.fgetattr(
	      inputs.path.c_str(),
	      (struct ::stat*)inputs.stbuf.contents,
	      fi);

      if (upcall.outputs.Extract(outputs.stbuf) == elle::StatusError)
	escape("unable to extract the region");

      if (upcall.outputs.Extract(outputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      struct ::stat*	stbuf = (struct ::stat*)inputs.stbuf.contents;
      struct ::stat*	_stbuf = (struct ::stat*)outputs.stbuf.contents;

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
	      (struct ::timespec*)inputs.ts.contents);

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
	(struct ::fuse_file_info*)inputs.fi.contents;

      res = Replay::Reference->fuse.opendir(
	      inputs.path.c_str(),
	      fi);

      if (upcall.outputs.Extract(outputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      struct ::fuse_file_info*	_fi =
	(struct ::fuse_file_info*)outputs.fi.contents;

      identifier = _fi->fh;

      if (Live::Add(identifier, fi->fh) == elle::StatusError)
	fail("unable to add the file information");

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
	(struct ::fuse_file_info*)inputs.fi.contents;

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::StatusError)
	fail("unable to retrieve the file information");

      /* XXX
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
	(struct ::fuse_file_info*)inputs.fi.contents;

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::StatusError)
	fail("unable to retrieve the file information");

      res = Replay::Reference->fuse.releasedir(
	      inputs.path.c_str(),
	      fi);

      if (upcall.outputs.Extract(outputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      if (Live::Remove(identifier) == elle::StatusError)
	fail("unable to remove the file information");

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
	      (mode_t)inputs.mode);

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
	      (int)inputs.mask);

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
	      (mode_t)inputs.mode);

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
	      (uid_t)inputs.uid,
	      (gid_t)inputs.gid);

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
	      (size_t)inputs.size,
	      (int)inputs.flags);

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
	      (size_t)inputs.size);

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
	      (size_t)inputs.size);

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
	      (size_t)inputs.size);

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
	(struct ::fuse_file_info*)inputs.fi.contents;

      res = Replay::Reference->fuse.create(
	      inputs.path.c_str(),
	      (mode_t)inputs.mode,
	      fi);

      if (upcall.outputs.Extract(outputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      struct ::fuse_file_info*	_fi =
	(struct ::fuse_file_info*)outputs.fi.contents;

      identifier = _fi->fh;

      if (Live::Add(identifier, fi->fh) == elle::StatusError)
	fail("unable to add the file information");

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
	(struct ::fuse_file_info*)inputs.fi.contents;

      res = Replay::Reference->fuse.open(
	      inputs.path.c_str(),
	      fi);

      if (upcall.outputs.Extract(outputs.fi) == elle::StatusError)
	escape("unable to extract the region");

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      struct ::fuse_file_info*	_fi =
	(struct ::fuse_file_info*)outputs.fi.contents;

      identifier = _fi->fh;

      if (Live::Add(identifier, fi->fh) == elle::StatusError)
	fail("unable to add the file information");

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
	(struct ::fuse_file_info*)inputs.fi.contents;

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::StatusError)
	fail("unable to retrieve the file information");

      res = Replay::Reference->fuse.write(
	      inputs.path.c_str(),
	      (char*)inputs.buf.contents,
	      (size_t)inputs.size,
	      (off_t)inputs.off,
	      fi);

      // XXX
      printf("----------------------------------------------------\n");
      if (inputs.path == "/fuse-2.8.3/conftest.err")
      {
	char		b[inputs.size];

	printf("[conftest.err] path(%s) size(%u) offset(%qu) handle(%d)\n",
	       inputs.path.c_str(),
	       (size_t)inputs.size,
	       (off_t)inputs.off,
	       (int)fi->fh);

	char buffer[4096];
	memset(buffer, 0x0, sizeof(buffer));

	/*
	int fd;
	int r;

	if ((fd = open("/tmp/XXX/fuse-2.8.3/conftest.err", O_RDONLY)) == -1)
	  escape("XXX open");

	if ((r = pread(fd, buffer, sizeof(buffer), 0)) == -1)
	  escape("XXX pread");

	close(fd);
	*/

	printf("-> %s\n", buffer);

	/*
	  Replay::Reference->fuse.read(
	  inputs.path.c_str(),
	  (char*)inputs.buf.contents,
	  (size_t)inputs.size,
	  (off_t)inputs.off,
	  fi);
	*/
      }
      printf("----------------------------------------------------\n");
      // XXX

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
	(struct ::fuse_file_info*)inputs.fi.contents;

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::StatusError)
	fail("unable to retrieve the file information");

      res = Replay::Reference->fuse.read(
	      inputs.path.c_str(),
	      (char*)inputs.buf.contents,
	      (size_t)inputs.size,
	      (off_t)inputs.off,
	      fi);

      if (upcall.outputs.Extract(outputs.buf) == elle::StatusError)
	escape("unable to extract the region");

      printf("[read] EXPECTED(%d) GOT(%d)\n", upcall.result, res);

      if (res != upcall.result)
	escape("invalid result: got(%d) expected(%d)",
	       res, upcall.result);

      if (inputs.buf != outputs.buf)
	{
	  inputs.buf.Dump();
	  outputs.buf.Dump();

	  escape("invalid buffer");
	}

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
	      (off_t)inputs.size);

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
	(struct ::fuse_file_info*)inputs.fi.contents;

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::StatusError)
	fail("unable to retrieve the file information");

      res = Replay::Reference->fuse.ftruncate(
	      inputs.path.c_str(),
	      (size_t)inputs.size,
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
	(struct ::fuse_file_info*)inputs.fi.contents;

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::StatusError)
	fail("unable to retrieve the file information");

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
	fail("unable to remove the file information");

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
    elle::Status	Replay::Initialize(Diary*		diary)
    {
      enter();

      // set the diary pointer.
      Replay::Reference = diary;

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
      elle::Callback<
	elle::Status,
	elle::Parameters<>
	>		callback(&Replay::Process);

      enter();

      // set the boundaries.
      Replay::From = from;
      Replay::To = to;

      // create the timer which will start the replaying process.
      if (Replay::Timer.Create(elle::Timer::ModeSingle,
			       callback) == elle::StatusError)
	escape("unable to create the timer");

      // start the timer.
      //
      // note that the timer is started immediately so that as soon as
      // the program enters its event loop, the timer is launched.
      if (Replay::Timer.Start(0) == elle::StatusError)
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
	    escape("unable to read the upcall");
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
	    escape("unable to read the upcall");

	  // XXX
	  printf("n(%u) :: opcode(%u)\n",
		 i, upcall.operation);

	  // forward the call to a specific method depending on
	  // the operation code.
	  switch (upcall.operation)
	    {
	    case Upcall::OperationGetattr:
	      {
		// call the method.
		if (Replay::Getattr(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationFgetattr:
	      {
		// call the method.
		if (Replay::Fgetattr(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationUtimens:
	      {
		// call the method.
		if (Replay::Utimens(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationOpendir:
	      {
		// call the method.
		if (Replay::Opendir(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationReaddir:
	      {
		// call the method.
		if (Replay::Readdir(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationReleasedir:
	      {
		// call the method.
		if (Replay::Releasedir(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationMkdir:
	      {
		// call the method.
		if (Replay::Mkdir(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationRmdir:
	      {
		// call the method.
		if (Replay::Rmdir(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationAccess:
	      {
		// call the method.
		if (Replay::Access(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationChmod:
	      {
		// call the method.
		if (Replay::Chmod(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationChown:
	      {
		// call the method.
		if (Replay::Chown(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationSetxattr:
	      {
		// call the method.
		if (Replay::Setxattr(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationGetxattr:
	      {
		// call the method.
		if (Replay::Getxattr(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationListxattr:
	      {
		// call the method.
		if (Replay::Listxattr(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationRemovexattr:
	      {
		// call the method.
		if (Replay::Removexattr(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationSymlink:
	      {
		// call the method.
		if (Replay::Symlink(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationReadlink:
	      {
		// call the method.
		if (Replay::Readlink(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationCreate:
	      {
		// call the method.
		if (Replay::Create(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationOpen:
	      {
		// call the method.
		if (Replay::Open(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationWrite:
	      {
		// call the method.
		if (Replay::Write(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationRead:
	      {
		// call the method.
		if (Replay::Read(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationTruncate:
	      {
		// call the method.
		if (Replay::Truncate(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationFtruncate:
	      {
		// call the method.
		if (Replay::Ftruncate(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationRelease:
	      {
		// call the method.
		if (Replay::Release(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationRename:
	      {
		// call the method.
		if (Replay::Rename(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    case Upcall::OperationUnlink:
	      {
		// call the method.
		if (Replay::Unlink(upcall) == elle::StatusError)
		  escape("an error occured in the replay method");

		break;
	      }
	    default:
	      {
		escape("unknown operation code");
	      }
	    }
	}

      // exit the program.
      if (elle::Program::Exit() == elle::StatusError)
	escape("unable to exit the program");

      leave();
    }

    ///
    /// this method cleans FUSE.
    ///
    elle::Status	Replay::Clean()
    {
      enter();

      // reset the diary pointer.
      Replay::Reference = NULL;

      // clean the live system.
      if (Live::Clean() == elle::StatusError)
	escape("unable to clean the live system");

      leave();
    }

  }
}
