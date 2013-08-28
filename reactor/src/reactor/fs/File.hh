#ifndef REACTOR_FS_FILE_HH
# define REACTOR_FS_FILE_HH

# include <asio-fs/File.hh>

# include <reactor/scheduler.hh>

# include <elle/IOStream.hh>

# include <boost/filesystem/path.hpp>

namespace reactor
{
  namespace fs
  {
    /// Export Flag and Permission enumerations.
    using boost::asio::fs::Flag;
    using boost::asio::fs::Permission;

    class File
    {
    private:
      Scheduler& _scheduler;
      boost::asio::fs::File _file;
    public:
      /// Construct a file instance by forwarding args to the AsioFile
      /// constructor (prepended with the io_service).
      template<typename... Args>
      File(Scheduler& sched,
           Args&&... args);

      virtual
      ~File();

    public:
      void
      read_some(elle::Buffer& out,
                DurationOpt timeout = DurationOpt{});
      void
      write_some(elle::ConstWeakBuffer const& in,
                 DurationOpt timeout = DurationOpt{});
    };
  }
}

# include <reactor/fs/File.hxx>

#endif
