//
// ---------- header ----------------------------------------------------------
//
// project       horizon
//
// license       infinit
//
// author        julien quintard   [sat mar 24 16:52:17 2012]
//

//
// ---------- includes --------------------------------------------------------
//

#include <horizon/macosx/Broker.hh>
#include <horizon/macosx/FUSE.hh>

using namespace horizon::macosx;

//
// ---------- slots -----------------------------------------------------------
//

///
/// this slot is called whenever an event is posted.
///
/// the method creates a fiber so that the method can block and resume later.
///
bool            Broker::event(QEvent*                           qevent)
{
  Event*        event = static_cast<Event*>(qevent);

  switch(event->operation)
    {
    case Event::OperationStatfs:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Statfs),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationGetattr:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Getattr),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationFgetattr:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Fgetattr),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationUtimens:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Utimens),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationOpendir:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Opendir),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationReaddir:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Readdir),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationReleasedir:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Releasedir),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationMkdir:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Mkdir),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationRmdir:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Rmdir),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationAccess:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Access),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationChmod:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Chmod),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationChown:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Chown),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationSetxattr:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Setxattr),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationGetxattr:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Getxattr),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationListxattr:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Listxattr),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationRemovexattr:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Removexattr),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationSymlink:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Symlink),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationReadlink:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Readlink),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationCreate:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Create),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationOpen:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Open),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationWrite:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Write),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationRead:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Read),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationTruncate:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Truncate),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationFtruncate:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Ftruncate),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationRelease:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Release),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationRename:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Rename),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    case Event::OperationUnlink:
      {
        elle::Closure<
          elle::Status,
          elle::radix::Parameters<Event*>
          >             closure(elle::concurrency::Callback<>::Infer(&Broker::Unlink),
                                event);

        if (elle::Fiber::Spawn(closure) == elle::Status::Error)
          yield(_(true), "unable to spawn a fiber");

        break;
      }
    default:
      {
        yield(_(true), "unknown event operation '%u'\n",
              event->operation);
      }
    }

  return true;
}

//
// ---------- callbacks -------------------------------------------------------
//

///
/// the callbacks below are all launched within a fiber in order for them
/// to block.
///
/// noteworthy is that, if the fiber blocks, the execution flow returns
/// to the event() method and then to the event loop.
///
/// as a result, the Event instance would be deleted. therefore, all
/// the callback below must *duplicate* the arguments required to treat
/// the event.
///
/// finally, every callback unlocks, through a semaphore, the FUSE
/// thread which had been spawn for treating the upcall. this way, the
/// upcall-specific thread is resumed so as to return its result to
/// the kernel.
///

elle::Status            Broker::Statfs(Event*                   event)
{
  const char*                                   path = event->path;
  struct ::statvfs*                             statvfs = event->statvfs;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.statfs(path, statvfs);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Getattr(Event*                  event)
{
  const char*                                   path = event->path;
  struct ::stat*                                stat = event->stat;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.getattr(path, stat);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Fgetattr(Event*                 event)
{
  const char*                                   path = event->path;
  struct ::stat*                                stat = event->stat;
  struct ::fuse_file_info*                      info = event->info;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.fgetattr(path, stat, info);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Utimens(Event*                  event)
{
  const char*                                   path = event->path;
  struct ::timespec                             ts[2] = { event->ts[0],
                                                          event->ts[1] };
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.utimens(path, ts);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Opendir(Event*                  event)
{
  const char*                                   path = event->path;
  struct ::fuse_file_info*                      info = event->info;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.opendir(path, info);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Readdir(Event*                  event)
{
  const char*                                   path = event->path;
  void*                                         buffer = event->buffer;
  ::fuse_fill_dir_t                             filler = event->filler;
  off_t                                         offset = event->offset;
  struct ::fuse_file_info*                      info = event->info;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.readdir(path, buffer, filler, offset, info);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Releasedir(Event*               event)
{
  const char*                                   path = event->path;
  struct ::fuse_file_info*                      info = event->info;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.releasedir(path, info);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Mkdir(Event*                    event)
{
  const char*                                   path = event->path;
  mode_t                                        mode = event->mode;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.mkdir(path, mode);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Rmdir(Event*                    event)
{
  const char*                                   path = event->path;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.rmdir(path);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Access(Event*                   event)
{
  const char*                                   path = event->path;
  int                                           mask = event->mask;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.access(path, mask);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Chmod(Event*                    event)
{
  const char*                                   path = event->path;
  mode_t                                        mode = event->mode;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.chmod(path, mode);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Chown(Event*                    event)
{
  const char*                                   path = event->path;
  uid_t                                         uid = event->uid;
  gid_t                                         gid = event->gid;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.chown(path, uid, gid);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Setxattr(Event*                 event)
{
  const char*                                   path = event->path;
  const char*                                   name = event->name;
  const char*                                   value = event->value;
  size_t                                        size = event->size;
  int                                           flags = event->flags;
  uint32_t                                      position = event->position;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.setxattr(path, name, value, size, flags, position);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Getxattr(Event*                 event)
{
  const char*                                   path = event->path;
  const char*                                   name = event->name;
  char*                                         value = event->value;
  size_t                                        size = event->size;
  uint32_t                                      position = event->position;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.getxattr(path, name, value, size, position);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Listxattr(Event*                event)
{
  const char*                                   path = event->path;
  char*                                         list = event->list;
  size_t                                        size = event->size;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.listxattr(path, list, size);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Removexattr(Event*              event)
{
  const char*                                   path = event->path;
  const char*                                   name = event->name;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.removexattr(path, name);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Symlink(Event*                  event)
{
  const char*                                   target = event->target;
  const char*                                   source = event->source;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.symlink(target, source);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Readlink(Event*                 event)
{
  const char*                                   path = event->path;
  char*                                         buffer =
    static_cast<char*>(event->buffer);
  size_t                                        size = event->size;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.readlink(path, buffer, size);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Create(Event*                   event)
{
  const char*                                   path = event->path;
  mode_t                                        mode = event->mode;
  struct ::fuse_file_info*                      info = event->info;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.create(path, mode, info);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Open(Event*                     event)
{
  const char*                                   path = event->path;
  struct ::fuse_file_info*                      info = event->info;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.open(path, info);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Write(Event*                    event)
{
  const char*                                   path = event->path;
  const char*                                   buffer =
    static_cast<const char*>(event->buffer);
  size_t                                        size = event->size;
  off_t                                         offset = event->offset;
  struct ::fuse_file_info*                      info = event->info;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.write(path, buffer, size, offset, info);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Read(Event*                     event)
{
  const char*                                   path = event->path;
  char*                                         buffer =
    static_cast<char*>(event->buffer);
  size_t                                        size = event->size;
  off_t                                         offset = event->offset;
  struct ::fuse_file_info*                      info = event->info;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.read(path, buffer, size, offset, info);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Truncate(Event*                 event)
{
  const char*                                   path = event->path;
  off_t                                         size = event->size;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.truncate(path, size);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Ftruncate(Event*                event)
{
  const char*                                   path = event->path;
  off_t                                         size = event->size;
  struct ::fuse_file_info*                      info = event->info;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.ftruncate(path, size, info);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Release(Event*                  event)
{
  const char*                                   path = event->path;
  struct ::fuse_file_info*                      info = event->info;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.release(path, info);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Rename(Event*                   event)
{
  const char*                                   source = event->source;
  const char*                                   target = event->target;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.rename(source, target);

  semaphore->post();

  return elle::Status::Ok;
}

elle::Status            Broker::Unlink(Event*                   event)
{
  const char*                                   path = event->path;
  boost::interprocess::interprocess_semaphore*  semaphore = event->semaphore;
  int*                                          result = event->result;

  *result = FUSE::Operations.unlink(path);

  semaphore->post();

  return elle::Status::Ok;
}
