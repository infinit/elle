//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/radix/Trace.cc
//
// created       julien quintard   [mon apr 26 21:25:23 2010]
// updated       julien quintard   [mon jul 11 23:14:41 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/radix/Trace.hh>

#include <elle/core/Integer.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/io/Path.hh>
#include <elle/io/Directory.hh>

#include <elle/utility/Hexadecimal.hh>

#include <elle/idiom/Close.hh>
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>
# include <libgen.h>
# include <sys/types.h>
# include <dirent.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace utility;

  namespace radix
  {

//
// ---------- definitions -----------------------------------------------------
//

    ///
    /// this variable contains the path to traces directory.
    ///
    Character				Trace::Location[32];

    ///
    /// this variable contains the length of the location.
    ///
    Natural32				Trace::Length;

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// default constructor.
    ///
    Trace::Trace(Void*						address):
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
    Status		Trace::Generate()
    {
      enter();

      // retrieve the frame addresses.
      this->size = ::backtrace(this->frames, Trace::Capacity);

      leave();
    }

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps the trace.
    ///
    Status		Trace::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      char**		symbols;
      Natural32		i;

      enter();

      std::cout << alignment << "[Trace]" << std::endl;

      // dump the address.
      std::cout << alignment << Dumpable::Shift << "[Address] "
		<< this->address << std::endl;

      std::cout << alignment << Dumpable::Shift << "[Stack] " << std::endl;

      // transform the frames into human-kind-of-readable strings.
      if ((symbols = ::backtrace_symbols(this->frames, this->size)) == NULL)
	escape("unable to resolve the frame frames into symbols");

      // go through the symbols.
      for (i = 0; i < this->size; i++)
	{
	  String	line(symbols[i]);
	  Character*	symbol;
	  Integer32	first;
	  Integer32	last;
	  Integer32	index;
	  int		status;

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
	  symbol = NULL;

	  // try to demangle the name.
	  symbol = abi::__cxa_demangle(line.substr(first + 1,
						   last - 1 - first).c_str(),
				       NULL,
				       NULL,
				       &status);

	  // look at the result of the demangling process.
	  switch (status)
	    {
	    case -1:
	    case -3:
	      {
		// release the symbol, if set.
		if (symbol != NULL)
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

      leave();
    }

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// XXX
    ///
    Status		Trace::Initialize()
    {
      Path		path;

      enter();

      // create the location.
      sprintf(Trace::Location, "/tmp/XXXXXX");

      // generate a temporary location.
      ::mkdtemp(Trace::Location);

      // compute the location's length.
      Trace::Length = ::strlen(Trace::Location);

      // create the path.
      if (path.Create(Trace::Location) == StatusError)
	escape("unable to create the path");

      // if the directory exists.
      if (Directory::Exist(path) == StatusTrue)
	{
	  // clear the directory.
	  if (Directory::Clear(path) == StatusError)
	    escape("unable to clear the directory");
	}
      else
	{
	  // create the directory.
	  if (Directory::Create(path) == StatusError)
	    escape("unable to create the directory");
	}

      leave();
    }

    ///
    /// XXX
    ///
    Status		Trace::Clean()
    {
      Path		path;

      enter();

      // create the path.
      if (path.Create(Trace::Location) == StatusError)
	escape("unable to create the path");

      // if the directory exists.
      if (Directory::Exist(path) == StatusTrue)
	{
	  // clear the directory.
	  if (Directory::Clear(path) == StatusError)
	    escape("unable to clear the directory");

	  // remove the directory.
	  if (Directory::Remove(path) == StatusError)
	    escape("unable to remove the directory");
	}

      leave();
    }

    ///
    /// this method stores an additional trace.
    ///
    Status		Trace::Store(Void*			address)
    {
      Trace		trace(address);
      Natural32		size =
	sizeof (Void*) +
	Trace::Capacity * sizeof (Void*) +
	sizeof (Natural32);
      Byte		buffer[size];
      Character		path[Trace::Length +
			     1 +
			     2 + (sizeof (Void*) / sizeof (Byte)) * 2 +
			     1];
      int		fd;

      enter();

      // XXX
      //printf("Trace::Store(%p)\n", address);

      // generate the trace.
      if (trace.Generate() == StatusError)
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
	escape(::strerror(errno));

      // write the text to the file.
      if (::write(fd,
		  buffer,
		  size) != (ssize_t)size)
	{
	  ::close(fd);

	  escape(::strerror(errno));
	}

      // close the file.
      ::close(fd);

      leave();
    }

    ///
    /// this method erases a trace.
    ///
    Status		Trace::Erase(Void*			address)
    {
      Character		path[Trace::Length +
			     1 +
			     2 + (sizeof (Void*) / sizeof (Byte)) * 2 +
			     1];
      struct ::stat	stat;

      enter();

      // XXX
      //printf("Trace::Erase(%p)\n", address);

      // build the path.
      ::sprintf(path,
		"%s/%p",
		Trace::Location, address);

      // does the path points to something.
      if (::stat(path, &stat) != 0)
	{
	  Trace		trace(address);

	  // generate the trace.
	  if (trace.Generate() == StatusError)
	    escape("unable to generate the trace");

	  // dump the trace.
	  if (trace.Dump() == StatusError)
	    escape("unable to dump the trace");

	  escape("this trace does not seem to exist");
	}

      // unlink the file.
      ::unlink(path);

      leave();
    }

    ///
    /// XXX
    ///
    Status		Trace::Show(const Natural32		margin)
    {
      String		alignment(margin, ' ');
      ::DIR*		dp;
      struct ::dirent*	entry;

      enter();

      std::cout << alignment << "[Traces]" << std::endl;

      // open the directory.
      if ((dp = ::opendir(Trace::Location)) == NULL)
	escape("unable to open the directory");

      // go through the entries.
      while ((entry = ::readdir(dp)) != NULL)
	{
	  Character	path[Trace::Length +
			     1 +
			     ::strlen(entry->d_name) +
			     1];
	  struct ::stat	stat;

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

	  Character	buffer[stat.st_size];
	  int		fd;

	  // open the file.
	  if ((fd = ::open(path, O_RDONLY)) == -1)
	    escape(::strerror(errno));

	  // read the file's content.
	  if (::read(fd, buffer, stat.st_size) == -1)
	    {
	      ::close(fd);

	      escape(::strerror(errno));
	    }

	  // close the file.
	  ::close(fd);

	  Void*		address;

	  // copy the address.
	  ::memcpy(&address,
		   buffer + 0,
		   sizeof (Void*));

	  Trace		trace(address);

	  // copy the frames.
	  ::memcpy(&trace.frames,
		   buffer + sizeof (Void*),
		   Trace::Capacity * sizeof (Void*));

	  // copy the size.
	  ::memcpy(&trace.size,
		   buffer + sizeof (Void*) + Trace::Capacity * sizeof (Void*),
		   sizeof (Natural32));

	  // dump the trace.
	  if (trace.Dump(margin + 2) == StatusError)
	    escape("unable to dump the trace");
	}

      // close the directory.
      ::closedir(dp);

      leave();
    }

  }
}
