//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/io/Link.cc
//
// created       julien quintard   [sun may 22 13:08:22 2011]
// updated       julien quintard   [sun may 22 13:38:11 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/Link.hh>
#include <elle/io/File.hh>
#include <elle/io/Directory.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

#include <elle/system/System.hh>

#include <vector>
#include <sstream>

#include <elle/idiom/Close.hh>
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>
# include <libgen.h>
#include <elle/idiom/Open.hh>

namespace elle
{
  using namespace system;

  namespace io
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method creates a link.
    ///
    Status		Link::Create(const Path&		link,
				     const Path&		target)
    {
      struct ::stat	status;
      int		fd;

      enter();

      // does the link exist.
      if (Link::Exist(link) == StatusTrue)
	escape("the link seems to already exist");

      // does the target exist.
      if ((File::Exist(target) == StatusFalse) &&
	  (Directory::Exist(target) == StatusFalse))
	escape("the target does not seem to exist");

      // create the link.
      ::symlink(target.string.c_str(), link.string.c_str());

      leave();
    }

    ///
    /// this method erases the given link path.
    ///
    Status		Link::Erase(const Path&			path)
    {
      enter();

      // does the link exist.
      if (Link::Exist(path) == StatusFalse)
	escape("the link does not seem to exist");

      // unlink the link.
      ::unlink(path.string.c_str());

      leave();
    }

    ///
    /// this method returns true if the pointed to link exists.
    ///
    Status		Link::Exist(const Path&			path)
    {
      struct ::stat		stat;

      enter();

      // does the path points to something.
      if (::lstat(path.string.c_str(), &stat) != 0)
	false();

      // does the path points to a regular file.
      if (!S_ISLNK(stat.st_mode))
	false();

      true();
    }

    ///
    /// this method takes a path to a link and creates, if necessary,
    /// the intermediate directory leading to the file.
    ///
    Status		Link::Dig(const Path&			path)
    {
      String		target(::strdup(path.string.c_str()));
      String		directory(::dirname((char*)target.c_str()));
      std::stringstream	stream(directory);
      String		item;
      Path		chemin;

      enter();

      // go through the components of the path.
      while (std::getline(stream, item, System::Path::Separator))
	{
	  // update the intermediate chemin.
	  if (chemin.string.empty() && item.empty())
	    chemin.string = System::Path::Separator;
	  else
	    {
	      chemin.string.append(item);
	      chemin.string.append(1, System::Path::Separator);
	    }

	  // retrieve information on the path. should this operation fail
	  // would mean that the target directory does not exist.
	  if (Directory::Exist(chemin) == StatusFalse)
	    {
	      // create the intermediate directory.
	      if (Directory::Create(chemin) == StatusError)
		escape("unable to create the intermediate directory");
	    }
	}

      leave();
    }

  }
}
