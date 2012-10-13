#include <elle/system/platform.hh>

# if defined(INFINIT_LINUX)

#  include <elle/radix/Trace.hh>

#  include <elle/types.hh>

#  include <elle/standalone/Report.hh>

#  include <elle/io/Path.hh>
#  include <elle/io/Directory.hh>

#  include <elle/idiom/Close.hh>
#   include <sys/stat.h>
#   include <unistd.h>
#   include <fcntl.h>
#   include <libgen.h>
#   include <sys/types.h>
#   include <dirent.h>
#  include <elle/idiom/Open.hh>

namespace elle
{

  namespace radix
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable contains the path to traces directory.
    ///
    Character                           Trace::Location[32];

    ///
    /// this variable contains the length of the location.
    ///
    Natural32                           Trace::Length;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Trace::Trace(Void*                                          address):
      address(address),
      size(0)
    {
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method generates a trace for the current stack.
    ///
    Status              Trace::Generate()
    {
      // retrieve the frame addresses.
      this->size = ::backtrace(this->frames, Trace::Capacity);

      return Status::Ok;
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the trace.
    ///
    Status              Trace::Dump(const Natural32             margin) const
    {
      String            alignment(margin, ' ');
      char**            symbols;
      Natural32         i;

      std::cout << alignment << "[Trace]" << std::endl;

      // dump the address.
      std::cout << alignment << Dumpable::Shift << "[Address] "
                << this->address << std::endl;

      std::cout << alignment << Dumpable::Shift << "[Stack] " << std::endl;

      // transform the frames into human-kind-of-readable strings.
      if ((symbols = ::backtrace_symbols(this->frames, this->size)) == nullptr)
        escape("unable to resolve the frame frames into symbols");

      // go through the symbols.
      for (i = 0; i < this->size; i++)
        {
          String        line(symbols[i]);
          Character*    symbol;
          // XXX check the code below (previous type was int32_t)
          size_t        first;
          size_t        last;
          size_t        index;
          int           status;

          // find a parenthese, indicating the beginning of the mangled name.
          if ((index = line.find_first_of("(")) == String::npos)
            {
              // display the identifier, as such.
              std::cout << alignment << Dumpable::Shift << Dumpable::Shift
                        << line << std::endl;

              continue;
            }

          // set the first index as following the parenthese.
          first = index;

          // find the + character, indicating the end of the mangled name.
          if ((index = line.find_first_of("+", first)) == String::npos)
            {
              // display the identifier, as such.
              std::cout << alignment << Dumpable::Shift << Dumpable::Shift
                        << line << std::endl;

              continue;
            }

          // set the last index as precedin the + character.
          last = index;

          // ignore if the name is empty.
          if ((last - 1) == first)
            {
              // display the identifier, as such.
              std::cout << alignment << Dumpable::Shift << Dumpable::Shift
                        << line << std::endl;

              continue;
            }

          // initialize the symbol.
          symbol = nullptr;

          // try to demangle the name.
          symbol = abi::__cxa_demangle(line.substr(first + 1,
                                                   last - 1 - first).c_str(),
                                       nullptr,
                                       nullptr,
                                       &status);

          // look at the result of the demangling process.
          switch (status)
            {
            case -1:
            case -3:
              {
                // release the symbol, if set.
                if (symbol != nullptr)
                  ::free(symbol);

                // release the symbols array.
                ::free(symbols);

                // an error occured.
                escape("an error occured while demangling a symbol");
              }
            case -2:
              {
                // display the identifier, as such.
                std::cout << alignment << Dumpable::Shift << Dumpable::Shift
                          << line << std::endl;

                continue;
              }
            }

          // display the demangled symbol.
          std::cout << alignment << Dumpable::Shift << Dumpable::Shift
                    << symbol << std::endl;

          // release the symbol's memory.
          ::free(symbol);
        }

      // release the symbols array.
      ::free(symbols);

      return Status::Ok;
    }

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method initializes the trace system.
    ///
    Status              Trace::Initialize()
    {
      io::Path              path;

      // create the location.
      ::sprintf(Trace::Location, "/tmp/XXXXXX");

      // generate a temporary location.
      if (!::mkdtemp(Trace::Location))
        escape("unable to create temporary directory");

      // compute the location's length.
      Trace::Length = ::strlen(Trace::Location);

      // create the path.
      if (path.Create(Trace::Location) == Status::Error)
        escape("unable to create the path");

      // if the directory exists.
      if (io::Directory::Exist(path) == Status::True)
        {
          // clear the directory.
          if (io::Directory::Clear(path) == Status::Error)
            escape("unable to clear the directory");
        }
      else
        {
          // create the directory.
          if (io::Directory::Create(path) == Status::Error)
            escape("unable to create the directory");
        }

      return Status::Ok;
    }

    ///
    /// this method cleans the system.
    ///
    Status              Trace::Clean()
    {
      io::Path              path;

      // create the path.
      if (path.Create(Trace::Location) == Status::Error)
        escape("unable to create the path");

      // if the directory exists.
      if (io::Directory::Exist(path) == Status::True)
        {
          // clear the directory.
          if (io::Directory::Clear(path) == Status::Error)
            escape("unable to clear the directory");

          // remove the directory.
          if (io::Directory::Remove(path) == Status::Error)
            escape("unable to remove the directory");
        }

      return Status::Ok;
    }

    ///
    /// this method stores an additional trace.
    ///
    Status              Trace::Store(Void*                      address)
    {
      Trace             trace(address);
      Natural32         size =
        sizeof (Void*) +
        Trace::Capacity * sizeof (Void*) +
        sizeof (Natural32);
      Byte              buffer[size];
      Character         path[Trace::Length +
                             1 +
                             2 + (sizeof (Void*) / sizeof (Byte)) * 2 +
                             1];
      int               fd;

      // generate the trace.
      if (trace.Generate() == Status::Error)
        escape("unable to generate the trace");

      // copy the data into the buffer.
      ::memcpy(buffer + 0,
               &trace.address,
               sizeof (Void*));

      ::memcpy(buffer + sizeof (Void*),
               &trace.frames,
               Trace::Capacity * sizeof (Void*));

      ::memcpy(buffer + sizeof (Void*) + Trace::Capacity * sizeof (Void*),
               &trace.size,
               sizeof (Natural32));

      // build the path.
      ::sprintf(path,
                "%s/%p",
                Trace::Location, address);

      // open the file.
      if ((fd = ::open(path,
                       O_CREAT | O_TRUNC | O_WRONLY,
                       0600)) == -1)
        escape("%s", ::strerror(errno));

      // write the text to the file.
      if (::write(fd,
                  buffer,
                  size) != static_cast<ssize_t>(size))
        {
          ::close(fd);

          escape("%s", ::strerror(errno));
        }

      // close the file.
      ::close(fd);

      return Status::Ok;
    }

    ///
    /// this method erases a trace.
    ///
    Status              Trace::Erase(Void*                      address)
    {
      Character         path[Trace::Length +
                             1 +
                             2 + (sizeof (Void*) / sizeof (Byte)) * 2 +
                             1];
      struct ::stat     stat;

      // build the path.
      ::sprintf(path,
                "%s/%p",
                Trace::Location, address);

      // does the path points to something.
      if (::stat(path, &stat) != 0)
        {
          Trace         trace(address);

          // generate the trace.
          if (trace.Generate() == Status::Error)
            escape("unable to generate the trace");

          // dump the trace.
          if (trace.Dump() == Status::Error)
            escape("unable to dump the trace");

          escape("this trace does not seem to exist");
        }

      // unlink the file.
      ::unlink(path);

      return Status::Ok;
    }

    ///
    /// this method displays the remaining addresses i.e the traces
    /// of the addresses which have not been released.
    ///
    Status              Trace::Show(const Natural32             margin)
    {
      String            alignment(margin, ' ');
      ::DIR*            dp;
      struct ::dirent*  entry;

      std::cout << alignment << "[Traces]" << std::endl;

      // open the directory.
      if ((dp = ::opendir(Trace::Location)) == nullptr)
        escape("unable to open the directory");

      // go through the entries.
      while ((entry = ::readdir(dp)) != nullptr)
        {
          Character     path[Trace::Length +
                             1 +
                             ::strlen(entry->d_name) +
                             1];
          struct ::stat stat;

          // ignore the '.' and '..' entries.
          if ((::strcmp(entry->d_name, ".") == 0) ||
              (::strcmp(entry->d_name, "..") == 0))
            continue;

          // build the path.
          ::sprintf(path,
                    "%s/%s",
                    Trace::Location, entry->d_name);

          // retrieve information on the file.
          if (::stat(path, &stat) != 0)
            escape("unable to retrieve information on the file");

          Character     buffer[stat.st_size];
          int           fd;

          // open the file.
          if ((fd = ::open(path, O_RDONLY)) == -1)
            escape("%s", ::strerror(errno));

          // read the file's content.
          if (::read(fd, buffer, static_cast<size_t>(stat.st_size)) == -1)
            {
              ::close(fd);

              escape("%s", ::strerror(errno));
            }

          // close the file.
          ::close(fd);

          Void*         address;

          // copy the address.
          ::memcpy(&address,
                   buffer + 0,
                   sizeof (Void*));

          Trace         trace(address);

          // copy the frames.
          ::memcpy(&trace.frames,
                   buffer + sizeof (Void*),
                   Trace::Capacity * sizeof (Void*));

          // copy the size.
          ::memcpy(&trace.size,
                   buffer + sizeof (Void*) + Trace::Capacity * sizeof (Void*),
                   sizeof (Natural32));

          // dump the trace.
          if (trace.Dump(margin + 2) == Status::Error)
            escape("unable to dump the trace");
        }

      // close the directory.
      ::closedir(dp);

      return Status::Ok;
    }

  }
}

#endif
