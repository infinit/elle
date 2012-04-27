//
// ---------- header ----------------------------------------------------------
//
// project       diary
//
// license       infinit
//
// author        julien quintard   [thu jun 30 09:23:09 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <satellites/diary/unix/Replay.hh>
#include <satellites/diary/unix/Live.hh>

#if defined(INFINIT_LINUX)
# include <horizon/linux/Linux.hh>
#elif defined(INFINIT_MACOSX)
# include <horizon/macosx/MacOSX.hh>
#endif

namespace satellite
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
    Memoirs*                            Replay::Reference = NULL;

    ///
    /// this variable contains the entrance which triggers the replay.
    ///
    elle::concurrency::Entrance<
      elle::Status,
      elle::radix::Parameters<>
      >*                                Replay::Entrance = NULL;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// the method below replay an upcall by forwarding the call to
    /// the Infinit Crux while making sure that the ouptuts and returned
    /// value match the recorded one.
    ///

    elle::Status        Replay::Getattr(Upcall&                 upcall)
    {
      struct
      {
        elle::String    path;
        elle::standalone::Region    stbuf;
      }                 inputs;
      struct
      {
        elle::standalone::Region    stbuf;
      }                 outputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.stbuf) == elle::Status::Error)
        escape("unable to extract the region");

      res = Replay::Reference->fuse.getattr(
              inputs.path.c_str(),
              reinterpret_cast<struct ::stat*>(inputs.stbuf.contents));

      if (upcall.outputs.Extract(outputs.stbuf) == elle::Status::Error)
        escape("unable to extract the region");

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      struct ::stat*    stbuf =
        reinterpret_cast<struct ::stat*>(inputs.stbuf.contents);
      struct ::stat*    _stbuf =
        reinterpret_cast<struct ::stat*>(outputs.stbuf.contents);

      /* XXX to check
      if (stbuf->st_uid != _stbuf->st_uid)
        escape("invalid UID: got(%u) expected(%u)",
               stbuf->st_uid, _stbuf->st_uid);
      if (stbuf->st_gid != _stbuf->st_gid)
        escape("invalid GID");

      if (stbuf->st_mode != _stbuf->st_mode)
        escape("invalid mode");

      if (stbuf->st_size != _stbuf->st_size)
        escape("invalid size");
      */

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Fgetattr(Upcall&                upcall)
    {
      struct
      {
        elle::String    path;
        elle::standalone::Region    stbuf;
        elle::standalone::Region    fi;
      }                 inputs;
      struct
      {
        elle::standalone::Region    stbuf;
        elle::standalone::Region    fi;
      }                 outputs;
      elle::Natural64   identifier;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.stbuf) == elle::Status::Error)
        escape("unable to extract the region");

      if (upcall.inputs.Extract(inputs.fi) == elle::Status::Error)
        escape("unable to extract the region");

      struct ::fuse_file_info*  fi =
        reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::Status::Error)
        escape("unable to retrieve the file information");

      res = Replay::Reference->fuse.fgetattr(
              inputs.path.c_str(),
              reinterpret_cast<struct ::stat*>(inputs.stbuf.contents),
              fi);

      if (upcall.outputs.Extract(outputs.stbuf) == elle::Status::Error)
        escape("unable to extract the region");

      if (upcall.outputs.Extract(outputs.fi) == elle::Status::Error)
        escape("unable to extract the region");

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      struct ::stat*    stbuf =
        reinterpret_cast<struct ::stat*>(inputs.stbuf.contents);
      struct ::stat*    _stbuf =
        reinterpret_cast<struct ::stat*>(outputs.stbuf.contents);

      /* XXX to check
      if (stbuf->st_uid != _stbuf->st_uid)
        escape("invalid UID: got(%u) expected(%u)",
               stbuf->st_uid, _stbuf->st_uid);

      if (stbuf->st_gid != _stbuf->st_gid)
        escape("invalid GID");

      if (stbuf->st_mode != _stbuf->st_mode)
        escape("invalid mode");

      if (stbuf->st_size != _stbuf->st_size)
        escape("invalid size");
      */

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Utimens(Upcall&                 upcall)
    {
      struct
      {
        elle::String    path;
        elle::standalone::Region    ts;
      }                 inputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.ts) == elle::Status::Error)
        escape("unable to extract the region");

      res = Replay::Reference->fuse.utimens(
              inputs.path.c_str(),
              reinterpret_cast<struct ::timespec*>(inputs.ts.contents));

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Opendir(Upcall&                 upcall)
    {
      struct
      {
        elle::String    path;
        elle::standalone::Region    fi;
      }                 inputs;
      struct
      {
        elle::standalone::Region    fi;
      }                 outputs;
      elle::Natural64   identifier;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.fi) == elle::Status::Error)
        escape("unable to extract the region");

      struct ::fuse_file_info*  fi =
        reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      res = Replay::Reference->fuse.opendir(
              inputs.path.c_str(),
              fi);

      if (upcall.outputs.Extract(outputs.fi) == elle::Status::Error)
        escape("unable to extract the region");

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      struct ::fuse_file_info*  _fi =
        reinterpret_cast<struct ::fuse_file_info*>(outputs.fi.contents);

      identifier = _fi->fh;

      if (Live::Add(identifier, fi->fh) == elle::Status::Error)
        escape("unable to add the file information");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    // XXX
    int filler(void* buf, const char* name, const struct stat* stbuf, off_t off)
    {
      /* XXX[taken from CBFS]
      DIR_ENUM_INFO* pinfo = (DIR_ENUM_INFO*)buf;
      struct stat attr;

      if(NULL == buf) return 1;

      if (pinfo->CheckDirEmpty) {
        if ((strcmp(name, ".") && strcmp(name, ".."))) {
          return 1;
        }
        else
          return 0;
      }

      ASSERT(CBFS_NTC_DIRECTORY_ENUM == NodeType(pinfo));

      if (stbuf != NULL)
        attr = *stbuf;

      pinfo->dirlist.insert(DIRSTAT::value_type(name, attr));
      */

      return 0;
    }

    elle::Status        Replay::Readdir(Upcall&                 upcall)
    {
      struct
      {
        elle::String    path;
        elle::Natural64 offset;
        elle::standalone::Region    fi;
      }                 inputs;
      struct
      {
        elle::standalone::Region    fi;
      }                 outputs;
      elle::Natural64   identifier;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      // XXX to improve with buf

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.offset) == elle::Status::Error)
        escape("unable to extract the offset");

      if (upcall.inputs.Extract(inputs.fi) == elle::Status::Error)
        escape("unable to extract the region");

      struct ::fuse_file_info*  fi =
        reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::Status::Error)
        escape("unable to retrieve the file information");

      /* XXX implement a filler-like function which must be passed
      res = Replay::Reference->fuse.readdir(
              inputs.path.c_str(),
              fi);

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);
      */

      if (upcall.outputs.Extract(outputs.fi) == elle::Status::Error)
        escape("unable to extract the region");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Releasedir(Upcall&              upcall)
    {
      struct
      {
        elle::String    path;
        elle::standalone::Region    fi;
      }                 inputs;
      struct
      {
        elle::standalone::Region    fi;
      }                 outputs;
      elle::Natural64   identifier;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.fi) == elle::Status::Error)
        escape("unable to extract the region");

      struct ::fuse_file_info*  fi =
        reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::Status::Error)
        escape("unable to retrieve the file information");

      res = Replay::Reference->fuse.releasedir(
              inputs.path.c_str(),
              fi);

      if (upcall.outputs.Extract(outputs.fi) == elle::Status::Error)
        escape("unable to extract the region");

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      if (Live::Remove(identifier) == elle::Status::Error)
        escape("unable to remove the file information");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Mkdir(Upcall&                   upcall)
    {
      struct
      {
        elle::String    path;
        elle::Natural32 mode;
      }                 inputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.mode) == elle::Status::Error)
        escape("unable to extract the number");

      res = Replay::Reference->fuse.mkdir(
              inputs.path.c_str(),
              static_cast<mode_t>(inputs.mode));

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Rmdir(Upcall&                   upcall)
    {
      struct
      {
        elle::String    path;
      }                 inputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      res = Replay::Reference->fuse.rmdir(
              inputs.path.c_str());

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Access(Upcall&                  upcall)
    {
      struct
      {
        elle::String    path;
        elle::Integer32 mask;
      }                 inputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.mask) == elle::Status::Error)
        escape("unable to extract the number");

      res = Replay::Reference->fuse.access(
              inputs.path.c_str(),
              static_cast<int>(inputs.mask));

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Chmod(Upcall&                   upcall)
    {
      struct
      {
        elle::String    path;
        elle::Natural32 mode;
      }                 inputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.mode) == elle::Status::Error)
        escape("unable to extract the number");

      res = Replay::Reference->fuse.chmod(
              inputs.path.c_str(),
              static_cast<mode_t>(inputs.mode));

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Chown(Upcall&                   upcall)
    {
      struct
      {
        elle::String    path;
        elle::Natural32 uid;
        elle::Natural32 gid;
      }                 inputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.uid) == elle::Status::Error)
        escape("unable to extract the number");

      if (upcall.inputs.Extract(inputs.gid) == elle::Status::Error)
        escape("unable to extract the number");

      res = Replay::Reference->fuse.chown(
              inputs.path.c_str(),
              static_cast<uid_t>(inputs.uid),
              static_cast<gid_t>(inputs.gid));

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

#if defined(HAVE_SETXATTR)
# if defined(INFINIT_LINUX)
    elle::Status        Replay::Setxattr(Upcall&                upcall)
    {
      struct
      {
        elle::String    path;
        elle::String    name;
        elle::String    value;
        elle::Natural64 size;
        elle::Integer32 flags;
      }                 inputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.name) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.value) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.size) == elle::Status::Error)
        escape("unable to extract the number");

      if (upcall.inputs.Extract(inputs.flags) == elle::Status::Error)
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

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Getxattr(Upcall&                upcall)
    {
      struct
      {
        elle::String    path;
        elle::String    name;
        elle::String    value;
        elle::Natural64 size;
      }                 inputs;
      struct
      {
        elle::String    value;
      }                 outputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.name) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.value) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.size) == elle::Status::Error)
        escape("unable to extract the number");

      char              value[inputs.size];

      res = Replay::Reference->fuse.getxattr(
        inputs.path.c_str(),
        inputs.name.c_str(),
        value,
        static_cast<size_t>(inputs.size));

      if (upcall.outputs.Extract(outputs.value) == elle::Status::Error)
        escape("unable to extract the string");

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      if (elle::String(value, res) != outputs.value)
        escape("invalid value: got(%s) expected(%s)",
               elle::String(value, res).c_str(), outputs.value.c_str());

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Listxattr(Upcall&               upcall)
    {
      struct
      {
        elle::String    path;
        elle::String    list;
        elle::Natural64 size;
      }                 inputs;
      struct
      {
        elle::String    list;
      }                 outputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.list) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.size) == elle::Status::Error)
        escape("unable to extract the number");

      char              list[inputs.size];

      res = Replay::Reference->fuse.listxattr(
        inputs.path.c_str(),
        list,
        static_cast<size_t>(inputs.size));

      if (upcall.outputs.Extract(outputs.list) == elle::Status::Error)
        escape("unable to extract the string");

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      if (elle::String(list, res) != outputs.list)
        escape("invalid value: got(%s) expected(%s)",
               elle::String(list, res).c_str(), outputs.list.c_str());

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Removexattr(Upcall&             upcall)
    {
      struct
      {
        elle::String    path;
        elle::String    name;
      }                 inputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.name) == elle::Status::Error)
        escape("unable to extract the string");

      res = Replay::Reference->fuse.removexattr(
        inputs.path.c_str(),
        inputs.name.c_str());

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }
# elif defined(INFINIT_MACOSX)
    elle::Status        Replay::Setxattr(Upcall&                upcall)
    {
      struct
      {
        elle::String    path;
        elle::String    name;
        elle::String    value;
        elle::Natural64 size;
        elle::Integer32 flags;
        elle::Natural32 position;
      }                 inputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.name) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.value) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.size) == elle::Status::Error)
        escape("unable to extract the number");

      if (upcall.inputs.Extract(inputs.flags) == elle::Status::Error)
        escape("unable to extract the number");

      if (upcall.inputs.Extract(inputs.position) == elle::Status::Error)
        escape("unable to extract the number");

      res = Replay::Reference->fuse.setxattr(
        inputs.path.c_str(),
        inputs.name.c_str(),
        inputs.value.c_str(),
        static_cast<size_t>(inputs.size),
        static_cast<int>(inputs.flags),
        static_cast<uint32_t>(inputs.position));

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Getxattr(Upcall&                upcall)
    {
      struct
      {
        elle::String    path;
        elle::String    name;
        elle::String    value;
        elle::Natural64 size;
        elle::Natural32 position;
      }                 inputs;
      struct
      {
        elle::String    value;
      }                 outputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.name) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.value) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.size) == elle::Status::Error)
        escape("unable to extract the number");

      if (upcall.inputs.Extract(inputs.position) == elle::Status::Error)
        escape("unable to extract the number");

      char              value[inputs.size];

      res = Replay::Reference->fuse.getxattr(
        inputs.path.c_str(),
        inputs.name.c_str(),
        value,
        static_cast<size_t>(inputs.size),
        static_cast<uint32_t>(inputs.position));

      if (upcall.outputs.Extract(outputs.value) == elle::Status::Error)
        escape("unable to extract the string");

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      if (elle::String(value, res) != outputs.value)
        escape("invalid value: got(%s) expected(%s)",
               elle::String(value, res).c_str(), outputs.value.c_str());

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Listxattr(Upcall&               upcall)
    {
      struct
      {
        elle::String    path;
        elle::String    list;
        elle::Natural64 size;
      }                 inputs;
      struct
      {
        elle::String    list;
      }                 outputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.list) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.size) == elle::Status::Error)
        escape("unable to extract the number");

      char              list[inputs.size];

      res = Replay::Reference->fuse.listxattr(
        inputs.path.c_str(),
        list,
        static_cast<size_t>(inputs.size));

      if (upcall.outputs.Extract(outputs.list) == elle::Status::Error)
        escape("unable to extract the string");

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      if (elle::String(list, res) != outputs.list)
        escape("invalid value: got(%s) expected(%s)",
               elle::String(list, res).c_str(), outputs.list.c_str());

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Removexattr(Upcall&             upcall)
    {
      struct
      {
        elle::String    path;
        elle::String    name;
      }                 inputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.name) == elle::Status::Error)
        escape("unable to extract the string");

      res = Replay::Reference->fuse.removexattr(
        inputs.path.c_str(),
        inputs.name.c_str());

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }
# endif
#endif

    elle::Status        Replay::Symlink(Upcall&                 upcall)
    {
      struct
      {
        elle::String    to;
        elle::String    from;
      }                 inputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.to) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.from) == elle::Status::Error)
        escape("unable to extract the string");

      res = Replay::Reference->fuse.symlink(
              inputs.to.c_str(),
              inputs.from.c_str());

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Readlink(Upcall&                upcall)
    {
      struct
      {
        elle::String    path;
        elle::String    buf;
        elle::Natural64 size;
      }                 inputs;
      struct
      {
        elle::String    buf;
      }                 outputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.buf) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.size) == elle::Status::Error)
        escape("unable to extract the number");

      char              buf[inputs.size];

      res = Replay::Reference->fuse.readlink(
              inputs.path.c_str(),
              buf,
              static_cast<size_t>(inputs.size));

      if (upcall.outputs.Extract(outputs.buf) == elle::Status::Error)
        escape("unable to extract the string");

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      if (elle::String(buf) != outputs.buf)
        escape("invalid value: got(%s) expected(%s)",
               elle::String(buf).c_str(), outputs.buf.c_str());

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Create(Upcall&                  upcall)
    {
      struct
      {
        elle::String    path;
        elle::Natural32 mode;
        elle::standalone::Region    fi;
      }                 inputs;
      struct
      {
        elle::standalone::Region    fi;
      }                 outputs;
      elle::Natural64   identifier;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.mode) == elle::Status::Error)
        escape("unable to extract the number");

      if (upcall.inputs.Extract(inputs.fi) == elle::Status::Error)
        escape("unable to extract the region");

      struct ::fuse_file_info*  fi =
        reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      res = Replay::Reference->fuse.create(
              inputs.path.c_str(),
              static_cast<mode_t>(inputs.mode),
              fi);

      if (upcall.outputs.Extract(outputs.fi) == elle::Status::Error)
        escape("unable to extract the region");

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      struct ::fuse_file_info*  _fi =
        reinterpret_cast<struct ::fuse_file_info*>(outputs.fi.contents);

      identifier = _fi->fh;

      if (Live::Add(identifier, fi->fh) == elle::Status::Error)
        escape("unable to add the file information");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Open(Upcall&                    upcall)
    {
      struct
      {
        elle::String    path;
        elle::standalone::Region    fi;
      }                 inputs;
      struct
      {
        elle::standalone::Region    fi;
      }                 outputs;
      elle::Natural64   identifier;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.fi) == elle::Status::Error)
        escape("unable to extract the region");

      struct ::fuse_file_info*  fi =
        reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      res = Replay::Reference->fuse.open(
              inputs.path.c_str(),
              fi);

      if (upcall.outputs.Extract(outputs.fi) == elle::Status::Error)
        escape("unable to extract the region");

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      struct ::fuse_file_info*  _fi =
        reinterpret_cast<struct ::fuse_file_info*>(outputs.fi.contents);

      identifier = _fi->fh;

      if (Live::Add(identifier, fi->fh) == elle::Status::Error)
        escape("unable to add the file information");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Write(Upcall&                   upcall)
    {
      struct
      {
        elle::String    path;
        elle::standalone::Region    buf;
        elle::Natural64 size;
        elle::Natural64 off;
        elle::standalone::Region    fi;
      }                 inputs;
      struct
      {
        elle::standalone::Region    fi;
      }                 outputs;
      elle::Natural64   identifier;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.buf) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.size) == elle::Status::Error)
        escape("unable to extract the number");

      if (upcall.inputs.Extract(inputs.off) == elle::Status::Error)
        escape("unable to extract the number");

      if (upcall.inputs.Extract(inputs.fi) == elle::Status::Error)
        escape("unable to extract the region");

      struct ::fuse_file_info*  fi =
        reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::Status::Error)
        escape("unable to retrieve the file information");

      res = Replay::Reference->fuse.write(
              inputs.path.c_str(),
              reinterpret_cast<char*>(inputs.buf.contents),
              static_cast<size_t>(inputs.size),
              static_cast<off_t>(inputs.off),
              fi);

      if (upcall.outputs.Extract(outputs.fi) == elle::Status::Error)
        escape("unable to extract the region");

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Read(Upcall&                    upcall)
    {
      struct
      {
        elle::String    path;
        elle::standalone::Region    buf;
        elle::Natural64 size;
        elle::Natural64 off;
        elle::standalone::Region    fi;
      }                 inputs;
      struct
      {
        elle::standalone::Region    buf;
      }                 outputs;
      elle::Natural64   identifier;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.buf) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.size) == elle::Status::Error)
        escape("unable to extract the number");

      if (upcall.inputs.Extract(inputs.off) == elle::Status::Error)
        escape("unable to extract the number");

      if (upcall.inputs.Extract(inputs.fi) == elle::Status::Error)
        escape("unable to extract the region");

      struct ::fuse_file_info*  fi =
        reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::Status::Error)
        escape("unable to retrieve the file information");

      res = Replay::Reference->fuse.read(
              inputs.path.c_str(),
              reinterpret_cast<char*>(inputs.buf.contents),
              static_cast<size_t>(inputs.size),
              static_cast<off_t>(inputs.off),
              fi);

      if (upcall.outputs.Extract(outputs.buf) == elle::Status::Error)
        escape("unable to extract the region");

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      if (inputs.buf != outputs.buf)
        escape("invalid buffer");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Truncate(Upcall&                upcall)
    {
      struct
      {
        elle::String    path;
        elle::Natural64 size;
      }                 inputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.size) == elle::Status::Error)
        escape("unable to extract the number");

      res = Replay::Reference->fuse.truncate(
              inputs.path.c_str(),
              static_cast<off_t>(inputs.size));

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Ftruncate(Upcall&               upcall)
    {
      struct
      {
        elle::String    path;
        elle::Natural64 size;
        elle::standalone::Region    fi;
      }                 inputs;
      struct
      {
        elle::standalone::Region    fi;
      }                 outputs;
      elle::Natural64   identifier;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.size) == elle::Status::Error)
        escape("unable to extract the number");

      if (upcall.inputs.Extract(inputs.fi) == elle::Status::Error)
        escape("unable to extract the region");

      struct ::fuse_file_info*  fi =
        reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::Status::Error)
        escape("unable to retrieve the file information");

      res = Replay::Reference->fuse.ftruncate(
              inputs.path.c_str(),
              static_cast<size_t>(inputs.size),
              fi);

      if (upcall.outputs.Extract(outputs.fi) == elle::Status::Error)
        escape("unable to extract the region");

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Release(Upcall&                 upcall)
    {
      struct
      {
        elle::String    path;
        elle::standalone::Region    fi;
      }                 inputs;
      struct
      {
        elle::standalone::Region    fi;
      }                 outputs;
      elle::Natural64   identifier;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.fi) == elle::Status::Error)
        escape("unable to extract the region");

      struct ::fuse_file_info*  fi =
        reinterpret_cast<struct ::fuse_file_info*>(inputs.fi.contents);

      identifier = fi->fh;

      if (Live::Retrieve(identifier, fi->fh) == elle::Status::Error)
        escape("unable to retrieve the file information");

#include <elle/idiom/Close.hh>
      res = Replay::Reference->fuse.release(
              inputs.path.c_str(),
              fi);
#include <elle/idiom/Open.hh>

      if (upcall.outputs.Extract(outputs.fi) == elle::Status::Error)
        escape("unable to extract the region");

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      if (Live::Remove(identifier) == elle::Status::Error)
        escape("unable to remove the file information");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Rename(Upcall&                  upcall)
    {
      struct
      {
        elle::String    from;
        elle::String    to;
      }                 inputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.from) == elle::Status::Error)
        escape("unable to extract the string");

      if (upcall.inputs.Extract(inputs.to) == elle::Status::Error)
        escape("unable to extract the string");

      res = Replay::Reference->fuse.rename(
              inputs.from.c_str(),
              inputs.to.c_str());

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    elle::Status        Replay::Unlink(Upcall&                  upcall)
    {
      struct
      {
        elle::String    path;
      }                 inputs;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Replay::%s()\n",
               __FUNCTION__);

      if (upcall.inputs.Extract(inputs.path) == elle::Status::Error)
        escape("unable to extract the string");

      res = Replay::Reference->fuse.unlink(
              inputs.path.c_str());

      if (res != upcall.result)
        escape("invalid result: got(%d) expected(%d)",
               res, upcall.result);

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Replay::%s()\n",
               __FUNCTION__);

      return elle::Status::Ok;
    }

    ///
    /// this method initializes the replaying process.
    ///
    elle::Status        Replay::Initialize(Memoirs*             memoirs)
    {
      // set the memoirs pointer.
      Replay::Reference = memoirs;

      // initialize the live system.
      if (Live::Initialize() == elle::Status::Error)
        escape("unable to initialize the live system");

      // allocate the entrance.
      Replay::Entrance =
        new elle::concurrency::Entrance<
          elle::Status,
          elle::radix::Parameters<>
          >(elle::concurrency::Closure<
              elle::Status,
              elle::radix::Parameters<>
              >(elle::concurrency::Callback<>::Infer(&Replay::Process)));

#if defined(INFINIT_LINUX)
      {
        // initialize the horizon's Linux complete implementation.
        //
        // note that the Linux is not set up i.e not mounted as this is
        // not required to replay a diary.
        if (horizon::linux::Linux::Initialize() == elle::Status::Error)
          escape("unable to initialize the horizon");
      }
#elif defined(INFINIT_MACOSX)
      {
        // initialize the horizon's MacOS X complete implementation.
        //
        // note that the MacOS X is not set up i.e not mounted as this is
        // not required to replay a diary.
        if (horizon::macosx::MacOSX::Initialize() == elle::Status::Error)
          escape("unable to initialize the horizon");
      }
#endif

      return elle::Status::Ok;
    }

    ///
    /// this method processes an upcall.
    ///
    elle::Status        Replay::Process()
    {
      elle::Natural32   i;

      // first, go through the upcalls that must be ignored.
      for (i = 0;
           (i < Replay::Reference->offsets.from) &&
             (Replay::Reference->End() == elle::Status::False);
           i++)
        {
          Upcall                upcall;

          // retrieve the upcall.
          if (Replay::Reference->Read(upcall) == elle::Status::Error)
            goto _error;
        }

      // then go through the remaining upcalls up to 'to'.
      for (;
           (i < Replay::Reference->offsets.to) &&
             (Replay::Reference->End() == elle::Status::False);
            i++)
        {
          Upcall        upcall;

          // retrieve the upcall.
          if (Replay::Reference->Read(upcall) == elle::Status::Error)
            goto _error;

          // display the index and opcode to keep track of
          // the execution.
          printf("#%u upcall(%u)\n", i, upcall.operation);

          // forward the call to a specific method depending on
          // the operation code.
          switch (upcall.operation)
            {
            case Upcall::OperationGetattr:
              {
                // call the method.
                if (Replay::Getattr(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationFgetattr:
              {
                // call the method.
                if (Replay::Fgetattr(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationUtimens:
              {
                // call the method.
                if (Replay::Utimens(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationOpendir:
              {
                // call the method.
                if (Replay::Opendir(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationReaddir:
              {
                // call the method.
                if (Replay::Readdir(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationReleasedir:
              {
                // call the method.
                if (Replay::Releasedir(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationMkdir:
              {
                // call the method.
                if (Replay::Mkdir(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationRmdir:
              {
                // call the method.
                if (Replay::Rmdir(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationAccess:
              {
                // call the method.
                if (Replay::Access(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationChmod:
              {
                // call the method.
                if (Replay::Chmod(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationChown:
              {
                // call the method.
                if (Replay::Chown(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
#if defined(HAVE_SETXATTR)
            case Upcall::OperationSetxattr:
              {
                // call the method.
                if (Replay::Setxattr(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationGetxattr:
              {
                // call the method.
                if (Replay::Getxattr(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationListxattr:
              {
                // call the method.
                if (Replay::Listxattr(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationRemovexattr:
              {
                // call the method.
                if (Replay::Removexattr(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
#endif
            case Upcall::OperationSymlink:
              {
                // call the method.
                if (Replay::Symlink(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationReadlink:
              {
                // call the method.
                if (Replay::Readlink(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationCreate:
              {
                // call the method.
                if (Replay::Create(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationOpen:
              {
                // call the method.
                if (Replay::Open(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationWrite:
              {
                // call the method.
                if (Replay::Write(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationRead:
              {
                // call the method.
                if (Replay::Read(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationTruncate:
              {
                // call the method.
                if (Replay::Truncate(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationFtruncate:
              {
                // call the method.
                if (Replay::Ftruncate(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationRelease:
              {
                // call the method.
                if (Replay::Release(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationRename:
              {
                // call the method.
                if (Replay::Rename(upcall) == elle::Status::Error)
                  goto _error;

                break;
              }
            case Upcall::OperationUnlink:
              {
                // call the method.
                if (Replay::Unlink(upcall) == elle::Status::Error)
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

      // display a message.
      std::cout << "The sequence of file system operations have been "
                << "successfully replayed"
                << std::endl;

      // exit the program.
      elle::concurrency::Program::Exit();

      return elle::Status::Ok;

    _error:
      // dump the report.
      show();

      // display a message.
      std::cout << "An error occured while replaying the diary"
                << std::endl;
      std::cout << "Feel free to take a look at the logs for more information"
                << std::endl;

      // exit the program.
      elle::concurrency::Program::Exit();

      return elle::Status::Ok;
    }

    ///
    /// this method cleans the replaying process.
    ///
    elle::Status        Replay::Clean()
    {
#if defined(INFINIT_LINUX)
      // clean the horizon.
      if (horizon::linux::Linux::Clean() == elle::Status::Error)
        escape("unable to clean the horizon");
#elif defined(INFINIT_MACOSX)
      // clean the horizon.
      if (horizon::macosx::MacOSX::Clean() == elle::Status::Error)
        escape("unable to clean the horizon");
#endif

      // delete the entrance.
      if (Replay::Entrance != NULL)
        delete Replay::Entrance;

      // reset the memoirs pointer.
      Replay::Reference = NULL;

      // clean the live system.
      if (Live::Clean() == elle::Status::Error)
        escape("unable to clean the live system");

      return elle::Status::Ok;
    }

  }
}
