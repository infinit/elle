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
// updated       julien quintard   [wed sep  7 17:37:43 2011]
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
#include <elle/system/Platform.hh>

#include <vector>
#include <sstream>

#include <elle/idiom/Close.hh>
# include <sys/stat.h>
# include <unistd.h>
# include <fcntl.h>
# include <libgen.h>
#include <elle/idiom/Open.hh>

#if INFINIT_WIN32
# include <windows.h>
#endif

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
      enter();

      // does the link exist.
      if (Link::Exist(link) == StatusTrue)
	escape("the link seems to already exist");

      // does the target exist.
      if ((File::Exist(target) == StatusFalse) &&
	  (Directory::Exist(target) == StatusFalse))
	escape("the target does not seem to exist");

      // create the link.
#if INFINIT_UNIX
      if (::symlink(target.string.c_str(), link.string.c_str()))
        escape("symlink failed: %s -> %s: %s", link.string.c_str(),
               target.string.c_str(), ::strerror(errno));
#elif INFINIT_WIN32
      {
        struct stat st;
        if (::stat(link.string.c_str(), &st))
          escape("stat(%s) failed: %s", link.string.c_str(), ::strerror(errno));
        if (!CreateSymbolicLink(target.string.c_str(), link.string.c_str(),
                                S_ISDIR(st.st_mode) ? SYMBOLIC_LINK_FLAG_DIRECTORY : 0))
          escape("symlink failed: %s -> %s: (error: %lu)", link.string.c_str(),
                 target.string.c_str(), ::GetLastError());
      }
#else
# error "symlink not supported on your platform"
#endif

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

#if INFINIT_UNIX
      // does the path points to something.
      if (::lstat(path.string.c_str(), &stat) != 0)
	false();

      // does the path points to a regular file.
      if (!S_ISLNK(stat.st_mode))
	false();
#elif INFINIT_WIN32
      // does the path points to something.
      if (::stat(path.string.c_str(), &stat) != 0)
	false();
#else
# error "unsuported platform."
#endif

      true();
    }

    ///
    /// this method takes a path to a link and creates, if necessary,
    /// the intermediate directory leading to the file.
    ///
    Status		Link::Dig(const Path&			path)
    {
      String		target(::strdup(path.string.c_str()));
      String		directory(::dirname(
				    const_cast<char*>(target.c_str())));
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
