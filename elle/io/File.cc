//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/io/File.cc
//
// created       julien quintard   [thu may 27 16:18:11 2010]
// updated       julien quintard   [thu may 27 17:31:00 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/File.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/idiom/Close.hh>
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace io
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method reads the given file's content.
    ///
    Status		File::Read(const String&		path,
				   Region&			data)
    {
      struct ::stat	status;
      int		fd;

      enter();

      // retrieve information on the associat.
      if (::stat(path.c_str(), &status) == -1)
	escape(::strerror(errno));

      // prepare the data.
      if (data.Prepare(status.st_size) == StatusError)
	escape("unable to prepare the region");

      // set the correct size.
      data.size = status.st_size;

      // open the file.
      if ((fd = ::open(path.c_str(), O_RDONLY)) == -1)
	escape(::strerror(errno));

      // read the file's content.
      if (::read(fd, data.contents, data.size) == -1)
	escape(::strerror(errno));

      // close the file.
      ::close(fd);

      leave();
    }

    ///
    /// this method writes the given data into the given file.
    ///
    Status		File::Write(const String&		path,
				    const Region&		data)
    {
      int		fd;

      enter();

      // open the file.
      if ((fd = ::open(path.c_str(),
		       O_CREAT | O_TRUNC | O_WRONLY,
		       0600)) == -1)
	escape(::strerror(errno));

      // write the text to the file.
      if (::write(fd,
		  data.contents,
		  data.size) != data.size)
	{
	  ::close(fd);

	  escape(::strerror(errno));
	}

      // close the file.
      ::close(fd);

      leave();
    }

    ///
    /// this method erases the given file path.
    ///
    Status		File::Erase(const String&		path)
    {
      // unlink the file.
      ::unlink(path.c_str());
    }

  }
}
