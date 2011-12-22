//
// ---------- header ----------------------------------------------------------
//
// project       8diary
//
// license       infinit
//
// author        julien quintard   [tue jun 28 22:17:27 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <applications/8diary/unix/Record.hh>
#include <applications/8diary/unix/Upcall.hh>

#include <facade/unix/FUSE.hh>

namespace application
{
#undef unix
  namespace unix
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this structure contains the function pointers to the FUSE
    /// implementation routines.
    ///
    struct ::fuse_operations		Record::Operations;

    ///
    /// this variable contains the address of the memoirs which is
    /// being recorded.
    ///
    Memoirs*				Record::Reference = NULL;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// XXX general description
    ///

    int			Record::Getattr(const char*		path,
					struct ::stat*		stbuf)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationGetattr) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
	    elle::String(path),
	    elle::Region(reinterpret_cast<elle::Byte*>(stbuf),
			 sizeof (struct ::stat))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.getattr(path, stbuf);

      if (upcall.Outputs(
	    elle::Region(reinterpret_cast<elle::Byte*>(stbuf),
			 sizeof (struct ::stat))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
	fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
	fail("unable to write the diay");

      return res;
    }

    int			Record::Fgetattr(const char*		path,
					 struct ::stat*		stbuf,
					 struct ::fuse_file_info* fi)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationFgetattr) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(path),
	    elle::Region(reinterpret_cast<elle::Byte*>(stbuf),
			 sizeof (struct ::stat)),
	    elle::Region(reinterpret_cast<elle::Byte*>(fi),
			 sizeof (struct ::fuse_file_info))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.fgetattr(path, stbuf, fi);

      if (upcall.Outputs(
	    elle::Region(reinterpret_cast<elle::Byte*>(stbuf),
			 sizeof (struct ::stat)),
	    elle::Region(reinterpret_cast<elle::Byte*>(fi),
			 sizeof (struct ::fuse_file_info))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Utimens(const char*		path,
					const struct ::timespec	ts[2])
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationUtimens) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
	    elle::String(path),
	    elle::Region(reinterpret_cast<const elle::Byte*>(ts),
			 sizeof (struct ::timespec[2]))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.utimens(path, ts);

      if (upcall.Outputs() == elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Opendir(const char*		path,
					struct ::fuse_file_info* fi)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationOpendir) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
	    elle::String(path),
	    elle::Region(reinterpret_cast<elle::Byte*>(fi),
			 sizeof (struct ::fuse_file_info))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.opendir(path, fi);

      if (upcall.Outputs(
	    elle::Region(reinterpret_cast<elle::Byte*>(fi),
			 sizeof (struct ::fuse_file_info))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Readdir(const char*		path,
					void*			buf,
					::fuse_fill_dir_t	filler,
					off_t			offset,
					struct ::fuse_file_info* fi)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationReaddir) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(path),
	    static_cast<elle::Natural64>(offset),
	    elle::Region(reinterpret_cast<elle::Byte*>(fi),
			 sizeof (struct ::fuse_file_info))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.readdir(path, buf, filler, offset, fi);

      if (upcall.Outputs(
	    elle::Region(reinterpret_cast<elle::Byte*>(fi),
			 sizeof (struct ::fuse_file_info))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Releasedir(const char*		path,
					   struct ::fuse_file_info* fi)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationReleasedir) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(path),
	    elle::Region(reinterpret_cast<elle::Byte*>(fi),
			 sizeof (struct ::fuse_file_info))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.releasedir(path, fi);

      if (upcall.Outputs(
	    elle::Region(reinterpret_cast<elle::Byte*>(fi),
			 sizeof (struct ::fuse_file_info))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Mkdir(const char*		path,
				      mode_t			mode)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationMkdir) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(path),
	    static_cast<elle::Natural32>(mode)) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.mkdir(path, mode);

      if (upcall.Outputs() == elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Rmdir(const char*		path)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationRmdir) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(path)) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.rmdir(path);

      if (upcall.Outputs() == elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Access(const char*		path,
				       int			mask)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationAccess) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(path),
	    static_cast<elle::Integer32>(mask)) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.access(path, mask);

      if (upcall.Outputs() == elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Chmod(const char*		path,
				      mode_t			mode)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationChmod) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(path),
	    static_cast<elle::Natural32>(mode)) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.chmod(path, mode);

      if (upcall.Outputs() == elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Chown(const char*		path,
				      uid_t			uid,
				      gid_t			gid)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationChown) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(path),
	    static_cast<elle::Natural32>(uid),
	    static_cast<elle::Natural32>(gid)) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.chown(path, uid, gid);

      if (upcall.Outputs() == elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

#ifdef HAVE_SETXATTR

    int			Record::Setxattr(const char*		path,
					 const char*		name,
					 const char*		value,
					 size_t			size,
					 int			flags)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationSetxattr) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(path),
  	    elle::String(name),
  	    elle::String(value),
	    static_cast<elle::Natural64>(size),
	    static_cast<elle::Integer32>(flags)) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.setxattr(path, name, value, size, flags);

      if (upcall.Outputs() == elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Getxattr(const char*		path,
					 const char*		name,
					 char*			value,
					 size_t			size)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationGetxattr) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(path),
  	    elle::String(name),
  	    elle::String(value, size),
	    static_cast<elle::Natural64>(size)) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.getxattr(path, name, value, size);

      if (upcall.Outputs(
	    elle::String(value)) ==
	  elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Listxattr(const char*		path,
					  char*			list,
					  size_t		size)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationListxattr) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
	    elle::String(path),
	    elle::String(list, size)
	    static_cast<elle::Natural64>(size)) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.listxattr(path, list, size);

      if (upcall.Outputs(
	    elle::String(list)) ==
	  elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Removexattr(const char*		path,
					    const char*		name)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationRemovexattr) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(path),
  	    elle::String(name)) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.removexattr(path, name);

      if (upcall.Outputs() == elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

#endif

    int			Record::Symlink(const char*		to,
					const char*		from)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationSymlink) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(to),
  	    elle::String(from)) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.symlink(to, from);

      if (upcall.Outputs() == elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Readlink(const char*		path,
					 char*			buf,
					 size_t			size)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationReadlink) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(path),
  	    elle::String(buf, size),
	    static_cast<elle::Natural64>(size)) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.readlink(path, buf, size);

      if (upcall.Outputs(
	    elle::String(buf)) ==
	  elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Create(const char*		path,
				       mode_t			mode,
				       struct ::fuse_file_info*	fi)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationCreate) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(path),
	    static_cast<elle::Natural32>(mode),
	    elle::Region(reinterpret_cast<elle::Byte*>(fi),
			 sizeof (struct ::fuse_file_info))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.create(path, mode, fi);

      if (upcall.Outputs(
	    elle::Region(reinterpret_cast<elle::Byte*>(fi),
			 sizeof (struct ::fuse_file_info))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Open(const char*		path,
				     struct ::fuse_file_info*	fi)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationOpen) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
	    elle::String(path),
	    elle::Region(reinterpret_cast<elle::Byte*>(fi),
			 sizeof (struct ::fuse_file_info))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.open(path, fi);

      if (upcall.Outputs(
	    elle::Region(reinterpret_cast<elle::Byte*>(fi),
			 sizeof (struct ::fuse_file_info))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Write(const char*		path,
				      const char*		buf,
				      size_t			size,
				      off_t			offset,
				      struct ::fuse_file_info*	fi)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationWrite) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(path),
	    elle::Region(reinterpret_cast<const elle::Byte*>(buf),
			 size),
	    static_cast<elle::Natural64>(size),
	    static_cast<elle::Natural64>(offset),
	    elle::Region(reinterpret_cast<const elle::Byte*>(fi),
			 sizeof (struct ::fuse_file_info))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.write(path, buf, size, offset, fi);

      if (upcall.Outputs(
	    elle::Region(reinterpret_cast<elle::Byte*>(fi),
			 sizeof (struct ::fuse_file_info))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Read(const char*		path,
				     char*			buf,
				     size_t			size,
				     off_t			offset,
				     struct ::fuse_file_info*	fi)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationRead) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(path),
	    elle::Region(reinterpret_cast<elle::Byte*>(buf),
			 size),
	    static_cast<elle::Natural64>(size),
	    static_cast<elle::Natural64>(offset),
	    elle::Region(reinterpret_cast<elle::Byte*>(fi),
			 sizeof (struct ::fuse_file_info))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.read(path, buf, size, offset, fi);

      if (upcall.Outputs(
	    elle::Region(reinterpret_cast<elle::Byte*>(buf),
			 size)) ==
	  elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Truncate(const char*		path,
					 off_t			size)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationTruncate) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(path),
	    static_cast<elle::Natural64>(size)) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.truncate(path, size);

      if (upcall.Outputs() == elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Ftruncate(const char*		path,
					  off_t			size,
					  struct ::fuse_file_info* fi)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationFtruncate) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(path),
	    static_cast<elle::Natural64>(size),
	    elle::Region(reinterpret_cast<elle::Byte*>(fi),
			 sizeof (struct ::fuse_file_info))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.ftruncate(path, size, fi);

      if (upcall.Outputs() == elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Release(const char*		path,
					struct ::fuse_file_info* fi)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationRelease) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(path),
	    elle::Region(reinterpret_cast<elle::Byte*>(fi),
			 sizeof (struct ::fuse_file_info))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

#include <elle/idiom/Close.hh>
      res = Record::Reference->fuse.release(path, fi);
#include <elle/idiom/Open.hh>

      if (upcall.Outputs(
	    elle::Region(reinterpret_cast<elle::Byte*>(fi),
			 sizeof (struct ::fuse_file_info))) ==
	  elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Rename(const char*		from,
				       const char*		to)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationRename) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
  	    elle::String(from),
  	    elle::String(to)) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.rename(from, to);

      if (upcall.Outputs() == elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    int			Record::Unlink(const char*		path)
    {
      Upcall		upcall;
      int		res;

      if (upcall.Create(Upcall::OperationUnlink) == elle::StatusError)
	fail("unable to create the upcall");

      if (upcall.Inputs(
	    elle::String(path)) ==
	  elle::StatusError)
	fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.unlink(path);

      if (upcall.Outputs() == elle::StatusError)
	fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::StatusError)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::StatusError)
        fail("unable to write the diay");

      return res;
    }

    ///
    /// this method initializes FUSE.
    ///
    elle::Status	Record::Initialize(Memoirs*		memoirs,
					   const elle::String&	mountpoint)
    {
      enter();

      // set the memoirs pointer.
      Record::Reference = memoirs;

      //
      // initialize the FUSE operations.
      //
      {
	// set all the pointers to zero.
	::memset(&Record::Operations, 0x0, sizeof (::fuse_operations));

	// Record::Operations.statfs: not supported
	Record::Operations.getattr = Record::Getattr;
	Record::Operations.fgetattr = Record::Fgetattr;
	Record::Operations.utimens = Record::Utimens;

	Record::Operations.opendir = Record::Opendir;
	Record::Operations.readdir = Record::Readdir;
	Record::Operations.releasedir = Record::Releasedir;
	Record::Operations.mkdir = Record::Mkdir;
	Record::Operations.rmdir = Record::Rmdir;

	Record::Operations.access = Record::Access;
	Record::Operations.chmod = Record::Chmod;
	Record::Operations.chown = Record::Chown;
#ifdef HAVE_SETXATTR
	Record::Operations.setxattr = Record::Setxattr;
	Record::Operations.getxattr = Record::Getxattr;
	Record::Operations.listxattr = Record::Listxattr;
	Record::Operations.removexattr = Record::Removexattr;
#endif

	// Record::Operations.link: not supported
	Record::Operations.readlink = Record::Readlink;
	Record::Operations.symlink = Record::Symlink;

	Record::Operations.create = Record::Create;
	// Record::Operations.mknod: not supported
	Record::Operations.open = Record::Open;
	Record::Operations.write = Record::Write;
	Record::Operations.read = Record::Read;
	Record::Operations.truncate = Record::Truncate;
	Record::Operations.ftruncate = Record::Ftruncate;
	Record::Operations.release = Record::Release;

	Record::Operations.rename = Record::Rename;
	Record::Operations.unlink = Record::Unlink;

	// the flag_nullpath_ok is not activated in order to get
	// as much debug information as possible.
	Record::Operations.flag_nullpath_ok = 0;
      }

      // initialize FUSE.
      if (facade::unix::FUSE::Initialize(Record::Operations) ==
	  elle::StatusError)
	escape("unable to initialize FUSE");

      // set up FUSE.
      if (facade::unix::FUSE::Setup(mountpoint) == elle::StatusError)
	escape("unable to set up FUSE");

      leave();
    }

    ///
    /// this method cleans FUSE.
    ///
    elle::Status	Record::Clean()
    {
      enter();

      // reset the memoirs pointer.
      Record::Reference = NULL;

      leave();
    }

  }
}
