//
// ---------- header ----------------------------------------------------------
//
// project       diary
//
// license       infinit
//
// author        julien quintard   [tue jun 28 22:17:27 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <satellites/diary/unix/Record.hh>
#include <satellites/diary/unix/Upcall.hh>

#include <Infinit.hh>

#if defined(INFINIT_LINUX)
# include <horizon/linux/FUSE.hh>
#elif defined(INFINIT_MACOSX)
# include <horizon/macosx/FUSE.hh>
#endif

///
/// the HAVE_SETXATTR macro is undefined because the calls crash on MacOS X.
///
#if defined(INFINIT_MACOSX)
# undef HAVE_SETXATTR
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
    /// this structure contains the function pointers to the FUSE
    /// implementation routines.
    ///
    struct ::fuse_operations            Record::Operations;

    ///
    /// this variable contains the address of the memoirs which is
    /// being recorded.
    ///
    Memoirs*                            Record::Reference = NULL;

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// the methods below wrap the FUSE mirror implementation by recorded
    /// every upcall including the operation code, inputs, outputs and
    /// returned value.
    ///

    int                 Record::Getattr(const char*             path,
                                        struct ::stat*          stbuf)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %p)\n",
               __FUNCTION__,
               path, stat);

      if (upcall.Create(Upcall::OperationGetattr) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(stbuf),
                         sizeof (struct ::stat))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.getattr(path, stbuf);

      if (upcall.Outputs(
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(stbuf),
                         sizeof (struct ::stat))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %p)\n",
               __FUNCTION__,
               path, stat);

      return res;
    }

    int                 Record::Fgetattr(const char*            path,
                                         struct ::stat*         stbuf,
                                         struct ::fuse_file_info* fi)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %p)\n",
               __FUNCTION__,
               path, stat);

      if (upcall.Create(Upcall::OperationFgetattr) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(stbuf),
                         sizeof (struct ::stat)),
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(fi),
                         sizeof (struct ::fuse_file_info))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.fgetattr(path, stbuf, fi);

      if (upcall.Outputs(
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(stbuf),
                         sizeof (struct ::stat)),
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(fi),
                         sizeof (struct ::fuse_file_info))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %p)\n",
               __FUNCTION__,
               path, stat);

      return res;
    }

    int                 Record::Utimens(const char*             path,
                                        const struct ::timespec ts[2])
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, ...)\n",
               __FUNCTION__,
               path);

      if (upcall.Create(Upcall::OperationUtimens) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            elle::standalone::Region(reinterpret_cast<const elle::Byte*>(ts),
                         sizeof (struct ::timespec[2]))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.utimens(path, ts);

      if (upcall.Outputs() == elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, ...)\n",
               __FUNCTION__,
               path);

      return res;
    }

    int                 Record::Opendir(const char*             path,
                                        struct ::fuse_file_info* fi)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %p)\n",
               __FUNCTION__,
               path, fi);

      if (upcall.Create(Upcall::OperationOpendir) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(fi),
                         sizeof (struct ::fuse_file_info))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.opendir(path, fi);

      if (upcall.Outputs(
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(fi),
                         sizeof (struct ::fuse_file_info))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %p)\n",
               __FUNCTION__,
               path, fi);

      return res;
    }

    int                 Record::Readdir(const char*             path,
                                        void*                   buf,
                                        ::fuse_fill_dir_t       filler,
                                        off_t                   offset,
                                        struct ::fuse_file_info* fi)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %p, %p, %qu, %p)\n",
               __FUNCTION__,
               path, buf, filler,
               static_cast<elle::Natural64>(offset), fi);

      if (upcall.Create(Upcall::OperationReaddir) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            static_cast<elle::Natural64>(offset),
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(fi),
                         sizeof (struct ::fuse_file_info))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.readdir(path, buf, filler, offset, fi);

      if (upcall.Outputs(
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(fi),
                         sizeof (struct ::fuse_file_info))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %p, %p, %qu, %p)\n",
               __FUNCTION__,
               path, buf, filler,
               static_cast<elle::Natural64>(offset), fi);

      return res;
    }

    int                 Record::Releasedir(const char*          path,
                                           struct ::fuse_file_info* fi)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %p)\n",
               __FUNCTION__,
               path, fi);

      if (upcall.Create(Upcall::OperationReleasedir) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(fi),
                         sizeof (struct ::fuse_file_info))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.releasedir(path, fi);

      if (upcall.Outputs(
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(fi),
                         sizeof (struct ::fuse_file_info))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %p)\n",
               __FUNCTION__,
               path, fi);

      return res;
    }

    int                 Record::Mkdir(const char*               path,
                                      mode_t                    mode)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, 0%o)\n",
               __FUNCTION__,
               path, mode);

      if (upcall.Create(Upcall::OperationMkdir) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            static_cast<elle::Natural32>(mode)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.mkdir(path, mode);

      if (upcall.Outputs() == elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, 0%o)\n",
               __FUNCTION__,
               path, mode);

      return res;
    }

    int                 Record::Rmdir(const char*               path)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s)\n",
               __FUNCTION__,
               path);

      if (upcall.Create(Upcall::OperationRmdir) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.rmdir(path);

      if (upcall.Outputs() == elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s)\n",
               __FUNCTION__,
               path);

      return res;
    }

    int                 Record::Access(const char*              path,
                                       int                      mask)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, 0%o)\n",
               __FUNCTION__,
               path, mask);

      if (upcall.Create(Upcall::OperationAccess) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            static_cast<elle::Integer32>(mask)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.access(path, mask);

      if (upcall.Outputs() == elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, 0%o)\n",
               __FUNCTION__,
               path, mask);

      return res;
    }

    int                 Record::Chmod(const char*               path,
                                      mode_t                    mode)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, 0%o)\n",
               __FUNCTION__,
               path, mode);

      if (upcall.Create(Upcall::OperationChmod) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            static_cast<elle::Natural32>(mode)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.chmod(path, mode);

      if (upcall.Outputs() == elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, 0%o)\n",
               __FUNCTION__,
               path, mode);

      return res;
    }

    int                 Record::Chown(const char*               path,
                                      uid_t                     uid,
                                      gid_t                     gid)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %u, %u)\n",
               __FUNCTION__,
               path, uid, gid);

      if (upcall.Create(Upcall::OperationChown) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            static_cast<elle::Natural32>(uid),
            static_cast<elle::Natural32>(gid)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.chown(path, uid, gid);

      if (upcall.Outputs() == elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %u, %u)\n",
               __FUNCTION__,
               path, uid, gid);

      return res;
    }

#if defined(HAVE_SETXATTR)
# if defined(INFINIT_LINUX)
    int                 Record::Setxattr(const char*            path,
                                         const char*            name,
                                         const char*            value,
                                         size_t                 size,
                                         int                    flags)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %s, %p, %u, 0x%x)\n",
               __FUNCTION__,
               path, name, value, size, flags);

      if (upcall.Create(Upcall::OperationSetxattr) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            elle::String(name),
            elle::String(value),
            static_cast<elle::Natural64>(size),
            static_cast<elle::Integer32>(flags)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.setxattr(path, name, value, size, flags);

      if (upcall.Outputs() == elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %s, %p, %u, 0x%x)\n",
               __FUNCTION__,
               path, name, value, size, flags);

      return res;
    }

    int                 Record::Getxattr(const char*            path,
                                         const char*            name,
                                         char*                  value,
                                         size_t                 size)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %s, %p, %u)\n",
               __FUNCTION__,
               path, name, value, size);

      if (upcall.Create(Upcall::OperationGetxattr) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            elle::String(name),
            size != 0 ? elle::String(value, size) : "",
            static_cast<elle::Natural64>(size)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.getxattr(path, name, value, size);

      if (upcall.Outputs(
            size != 0 ? elle::String(value) : "") ==
          elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %s, %p, %u)\n",
               __FUNCTION__,
               path, name, value, size);

      return res;
    }

    int                 Record::Listxattr(const char*           path,
                                          char*                 list,
                                          size_t                size)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %p, %u)\n",
               __FUNCTION__,
               path, list, size);

      if (upcall.Create(Upcall::OperationListxattr) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            elle::String(list, size),
            static_cast<elle::Natural64>(size)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.listxattr(path, list, size);

      if (upcall.Outputs(
            elle::String(list)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %p, %u)\n",
               __FUNCTION__,
               path, list, size);

      return res;
    }

    int                 Record::Removexattr(const char*         path,
                                            const char*         name)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %s)\n",
               __FUNCTION__,
               path, name);

      if (upcall.Create(Upcall::OperationRemovexattr) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            elle::String(name)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.removexattr(path, name);

      if (upcall.Outputs() == elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %s)\n",
               __FUNCTION__,
               path, name);

      return res;
    }
# elif defined(INFINIT_MACOSX)
    int                 Record::Setxattr(const char*            path,
                                         const char*            name,
                                         const char*            value,
                                         size_t                 size,
                                         int                    options,
                                         uint32_t               position)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %s, %p, %lu, 0x%x, %u)\n",
               __FUNCTION__,
               path, name, value, size, options, position);

      if (upcall.Create(Upcall::OperationSetxattr) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            elle::String(name),
            elle::String(value),
            static_cast<elle::Natural64>(size),
            static_cast<elle::Integer32>(options),
            static_cast<elle::Natural32>(position)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.setxattr(path, name, value, size, options, position);

      if (upcall.Outputs() == elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %s, %p, %lu, 0x%x, %u)\n",
               __FUNCTION__,
               path, name, value, size, options, position);

      return res;
    }

    int                 Record::Getxattr(const char*            path,
                                         const char*            name,
                                         char*                  value,
                                         size_t                 size,
                                         uint32_t               position)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %s, %p, %lu, %u)\n",
               __FUNCTION__,
               path, name, value, size, position);

      if (upcall.Create(Upcall::OperationGetxattr) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            elle::String(name),
            elle::String(value, size),
            static_cast<elle::Natural64>(size),
            static_cast<elle::Natural32>(position)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.getxattr(path, name, value, size, position);

      if (upcall.Outputs(
            elle::String(value)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %s, %p, %lu, %u)\n",
               __FUNCTION__,
               path, name, value, size, position);

      return res;
    }

    int                 Record::Listxattr(const char*           path,
                                          char*                 list,
                                          size_t                size)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %p, %lu)\n",
               __FUNCTION__,
               path, list, size);

      if (upcall.Create(Upcall::OperationListxattr) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            elle::String(list, size),
            static_cast<elle::Natural64>(size)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.listxattr(path, list, size);

      if (upcall.Outputs(
            elle::String(list)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %p, %lu)\n",
               __FUNCTION__,
               path, list, size);

      return res;
    }

    int                 Record::Removexattr(const char*         path,
                                            const char*         name)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %s)\n",
               __FUNCTION__,
               path, name);

      if (upcall.Create(Upcall::OperationRemovexattr) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            elle::String(name)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.removexattr(path, name);

      if (upcall.Outputs() == elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %s)\n",
               __FUNCTION__,
               path, name);

      return res;
    }
# endif
#endif

    int                 Record::Symlink(const char*             to,
                                        const char*             from)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %s)\n",
               __FUNCTION__,
               to, from);

      if (upcall.Create(Upcall::OperationSymlink) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(to),
            elle::String(from)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.symlink(to, from);

      if (upcall.Outputs() == elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %s)\n",
               __FUNCTION__,
               to, from);

      return res;
    }

    int                 Record::Readlink(const char*            path,
                                         char*                  buf,
                                         size_t                 size)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %p, %qu)\n",
               __FUNCTION__,
               path, buf, static_cast<elle::Natural64>(size));

      if (upcall.Create(Upcall::OperationReadlink) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            elle::String(buf, size),
            static_cast<elle::Natural64>(size)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.readlink(path, buf, size);

      if (upcall.Outputs(
            elle::String(buf)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %p, %qu)\n",
               __FUNCTION__,
               path, buf, static_cast<elle::Natural64>(size));

      return res;
    }

    int                 Record::Create(const char*              path,
                                       mode_t                   mode,
                                       struct ::fuse_file_info* fi)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, 0%o, %p)\n",
               __FUNCTION__,
               path, mode, fi);

      if (upcall.Create(Upcall::OperationCreate) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            static_cast<elle::Natural32>(mode),
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(fi),
                         sizeof (struct ::fuse_file_info))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.create(path, mode, fi);

      if (upcall.Outputs(
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(fi),
                         sizeof (struct ::fuse_file_info))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, 0%o, %p)\n",
               __FUNCTION__,
               path, mode, fi);

      return res;
    }

    int                 Record::Open(const char*                path,
                                     struct ::fuse_file_info*   fi)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %p)\n",
               __FUNCTION__,
               path, fi);

      if (upcall.Create(Upcall::OperationOpen) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(fi),
                         sizeof (struct ::fuse_file_info))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.open(path, fi);

      if (upcall.Outputs(
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(fi),
                         sizeof (struct ::fuse_file_info))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %p)\n",
               __FUNCTION__,
               path, fi);

      return res;
    }

    int                 Record::Write(const char*               path,
                                      const char*               buf,
                                      size_t                    size,
                                      off_t                     offset,
                                      struct ::fuse_file_info*  fi)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %p, %qu, %qu, %p)\n",
               __FUNCTION__,
               path, buf, static_cast<elle::Natural64>(size),
               static_cast<elle::Natural64>(offset), fi);

      if (upcall.Create(Upcall::OperationWrite) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            elle::standalone::Region(reinterpret_cast<const elle::Byte*>(buf),
                         size),
            static_cast<elle::Natural64>(size),
            static_cast<elle::Natural64>(offset),
            elle::standalone::Region(reinterpret_cast<const elle::Byte*>(fi),
                         sizeof (struct ::fuse_file_info))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.write(path, buf, size, offset, fi);

      if (upcall.Outputs(
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(fi),
                         sizeof (struct ::fuse_file_info))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %p, %qu, %qu, %p)\n",
               __FUNCTION__,
               path, buf, static_cast<elle::Natural64>(size),
               static_cast<elle::Natural64>(offset), fi);

      return res;
    }

    int                 Record::Read(const char*                path,
                                     char*                      buf,
                                     size_t                     size,
                                     off_t                      offset,
                                     struct ::fuse_file_info*   fi)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %p, %qu, %qu, %p)\n",
               __FUNCTION__,
               path, buf, static_cast<elle::Natural64>(size),
               static_cast<elle::Natural64>(offset), fi);

      if (upcall.Create(Upcall::OperationRead) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(buf),
                         size),
            static_cast<elle::Natural64>(size),
            static_cast<elle::Natural64>(offset),
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(fi),
                         sizeof (struct ::fuse_file_info))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.read(path, buf, size, offset, fi);

      if (upcall.Outputs(
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(buf),
                         size)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %p, %qu, %qu, %p)\n",
               __FUNCTION__,
               path, buf, static_cast<elle::Natural64>(size),
               static_cast<elle::Natural64>(offset), fi);

      return res;
    }

    int                 Record::Truncate(const char*            path,
                                         off_t                  size)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %qu)\n",
               __FUNCTION__,
               path, static_cast<elle::Natural64>(size));

      if (upcall.Create(Upcall::OperationTruncate) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            static_cast<elle::Natural64>(size)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.truncate(path, size);

      if (upcall.Outputs() == elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %qu)\n",
               __FUNCTION__,
               path, static_cast<elle::Natural64>(size));

      return res;
    }

    int                 Record::Ftruncate(const char*           path,
                                          off_t                 size,
                                          struct ::fuse_file_info* fi)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %qu, %p)\n",
               __FUNCTION__,
               path, static_cast<elle::Natural64>(size), fi);

      if (upcall.Create(Upcall::OperationFtruncate) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            static_cast<elle::Natural64>(size),
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(fi),
                         sizeof (struct ::fuse_file_info))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.ftruncate(path, size, fi);

      if (upcall.Outputs(
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(fi),
                         sizeof (struct ::fuse_file_info))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %qu, %p)\n",
               __FUNCTION__,
               path, static_cast<elle::Natural64>(size), fi);

      return res;
    }

    int                 Record::Release(const char*             path,
                                        struct ::fuse_file_info* fi)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %p)\n",
               __FUNCTION__,
               path, fi);

      if (upcall.Create(Upcall::OperationRelease) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path),
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(fi),
                         sizeof (struct ::fuse_file_info))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

#include <elle/idiom/Close.hh>
      res = Record::Reference->fuse.release(path, fi);
#include <elle/idiom/Open.hh>

      if (upcall.Outputs(
            elle::standalone::Region(reinterpret_cast<elle::Byte*>(fi),
                         sizeof (struct ::fuse_file_info))) ==
          elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %p)\n",
               __FUNCTION__,
               path, fi);

      return res;
    }

    int                 Record::Rename(const char*              from,
                                       const char*              to)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s, %s)\n",
               __FUNCTION__,
               from, to);

      if (upcall.Create(Upcall::OperationRename) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(from),
            elle::String(to)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.rename(from, to);

      if (upcall.Outputs() == elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s, %s)\n",
               __FUNCTION__,
               from, to);

      return res;
    }

    int                 Record::Unlink(const char*              path)
    {
      Upcall            upcall;
      int               res;

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] Record::%s(%s)\n",
               __FUNCTION__,
               path);

      if (upcall.Create(Upcall::OperationUnlink) == elle::Status::Error)
        fail("unable to create the upcall");

      if (upcall.Inputs(
            elle::String(path)) ==
          elle::Status::Error)
        fail("unable to specify the upcall's inputs");

      res = Record::Reference->fuse.unlink(path);

      if (upcall.Outputs() == elle::Status::Error)
        fail("unable to specify the upcall's outputs");

      if (upcall.Result(res) == elle::Status::Error)
        fail("unable to specify the upcall's result");

      if (Record::Reference->Write(upcall) == elle::Status::Error)
        fail("unable to write the diay");

      // debug.
      if (Infinit::Configuration.satellites.debug == true)
        printf("[satellites] /Record::%s(%s)\n",
               __FUNCTION__,
               path);

      return res;
    }

    ///
    /// this method initializes FUSE.
    ///
    elle::Status        Record::Initialize(Memoirs*             memoirs,
                                           const elle::String&  mountpoint)
    {
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
#if defined(HAVE_SETXATTR)
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

      // set the mountpoint.
      Infinit::Mountpoint = mountpoint;

#if defined(INFINIT_LINUX)
      {
        // initialize FUSE.
        if (horizon::linux::FUSE::Initialize(
              Record::Operations) == elle::Status::Error)
          escape("unable to initialize FUSE");
      }
#elif defined(INFINIT_MACOSX)
      {
        // initialize FUSE.
        if (horizon::macosx::FUSE::Initialize(
              Record::Operations) == elle::Status::Error)
          escape("unable to initialize FUSE");
      }
#endif

      return elle::Status::Ok;
    }

    ///
    /// this method cleans FUSE.
    ///
    elle::Status        Record::Clean()
    {
#if defined(INFINIT_LINUX)
      // clean FUSE.
      if (horizon::linux::FUSE::Clean() == elle::Status::Error)
        escape("unable to clean FUSE");
#elif defined(INFINIT_MACOSX)
      // clean FUSE.
      if (horizon::macosx::FUSE::Clean() == elle::Status::Error)
        escape("unable to clean FUSE");
#endif

      // reset the memoirs pointer.
      Record::Reference = NULL;

      return elle::Status::Ok;
    }

  }
}
