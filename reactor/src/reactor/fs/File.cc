#include <reactor/fs/File.hh>
#include <reactor/operation.hh>
#include <reactor/exception.hh>

#include <elle/Buffer.hh>

namespace reactor
{
  namespace fs
  {
    File::~File()
    {
    }

    namespace
    {
      // Base file operation
      class FileOperation
        : public Operation
      {
      protected:
        boost::asio::fs::File& _file;

      public:
        FileOperation(Scheduler& scheduler,
                      boost::asio::fs::File& file)
          : Operation{scheduler}
          , _file(file)
        {}

      private:
        void
        _abort() override
        {
          // XXX implement boost::asio::fs
        }

      protected:
        void
        _wakeup(boost::system::error_code const& err)
        {
          if (err)
          {
            if (err == boost::system::errc::operation_canceled)
              return;
            this->_raise<Exception>(err.message());
          }
          this->_signal();
        }
      };

      // Read operation
      class ReadOperation
        : public FileOperation
      {
      private:
        elle::Buffer& _out;

      public:
        ReadOperation(Scheduler& scheduler,
                      boost::asio::fs::File& file,
                      elle::Buffer& out)
          : FileOperation{scheduler, file}
          , _out(out)
        {}

      private:
        void
        _start() override
        {
          this->_file.async_read_some(
            boost::asio::buffer(this->_out.mutable_contents(),
                                this->_out.size()),
            [this] (boost::system::error_code const& err,
                    size_t const bytes_read) {
              this->_out.size(bytes_read);
              this->_wakeup(err);
            });
        }
      };

      // Read operation
      class WriteOperation
        : public FileOperation
      {
      private:
        elle::ConstWeakBuffer const& _in;

      public:
        WriteOperation(Scheduler& scheduler,
                       boost::asio::fs::File& file,
                       elle::ConstWeakBuffer const& in)
          : FileOperation{scheduler, file}
          , _in(in)
        {}

      private:
        void
        _start() override
        {
          this->_file.async_write_some(
            boost::asio::buffer(this->_in.contents(),
                                this->_in.size()),
            [this] (boost::system::error_code const& err,
                    size_t const bytes_read) {
              // XXX Check that everythin has been written
              this->_wakeup(err);
            });
        }
      };
    }

    void
    File::read_some(elle::Buffer& out,
                    DurationOpt timeout)
    {
      ReadOperation(this->_scheduler, this->_file, out).run(timeout);
    }

    void
    File::write_some(elle::ConstWeakBuffer const& in,
                     DurationOpt timeout)
    {
      WriteOperation(this->_scheduler, this->_file, in).run(timeout);
    }
  }
}
