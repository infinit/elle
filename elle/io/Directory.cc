//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/io/Directory.cc
//
// created       julien quintard   [thu may 27 16:18:11 2010]
// updated       julien quintard   [sun jun 19 22:15:11 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/io/Directory.hh>
#include <elle/io/File.hh>
#include <elle/io/Link.hh>

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
# include <sys/types.h>
# include <dirent.h>
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
    /// this method creates a directory at the given path.
    ///
    Status		Directory::Create(const Path&		path)
    {
      enter();

      // does the directory already exist.
      if (Directory::Exist(path) == StatusTrue)
	escape("the directory seems to already exist");

      // dig the directory which will hold the target directory.
      if (Directory::Dig(path) == StatusError)
	escape("unable to dig the chain of directories");

      // create the directory.
      if (::mkdir(path.string.c_str(), 0700) != 0)
	escape(::strerror(errno));

      leave();
    }

    ///
    /// this method removes a directory.
    ///
    Status		Directory::Remove(const Path&		path)
    {
      enter();

      // does the directory exist.
      if (Directory::Exist(path) == StatusFalse)
	escape("the directory does not seem to exist");

      // remove the directory.
      ::rmdir(path.string.c_str());

      leave();
    }

    ///
    /// this method returns true if the pointed to directory exists.
    ///
    Status		Directory::Exist(const Path&			path)
    {
      struct ::stat		stat;

      enter();

      // does the path points to something.
      if (::stat(path.string.c_str(), &stat) != 0)
	false();

      // does the path points to a directory.
      if (!S_ISDIR(stat.st_mode))
	false();

      true();
    }

    ///
    /// this method takes a path to a directory and creates, if necessary,
    /// the intermediate directories leading to the target.
    ///
    Status		Directory::Dig(const Path&			path)
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

    ///
    /// this method recursively removes everything in the given directory.
    ///
    Status		Directory::Clear(const Path&		path)
    {
      ::DIR*		directory;
      struct ::dirent*	entry;

      enter();

      // is the path pointing to a valid directory.
      if (Directory::Exist(path) == StatusFalse)
	escape("the path does not reference a directory");

      // open the directory.
      if ((directory = ::opendir(path.string.c_str())) == NULL)
	escape("unable to open the directory");

      // go through the entries.
      while ((entry = ::readdir(directory)) != NULL)
	{
	  Path		target;

	  // ignore the . and ..
	  if ((::strcmp(entry->d_name, ".") == 0) ||
	      (::strcmp(entry->d_name, "..") == 0))
	    continue;

	  // create the target path.
	  if (target.Create(path.string +
			    elle::System::Path::Separator +
			    elle::String(entry->d_name)) == StatusError)
	    escape("unable to create the target path");

	  // perform an action depending on the nature of the target.
	  switch (entry->d_type)
	    {
	    case DT_DIR:
	      {
		// empty it as well.
		if (Directory::Clear(target) == elle::StatusError)
		  escape("unable to empty a subdirectory");

		// remove the directory.
		if (Directory::Remove(target) == StatusError)
		  escape("unable to remove the subdirectory");

		break;
	      }
	    case DT_REG:
	      {
		// remove the file.
		if (File::Erase(target) == StatusError)
		  escape("unable to remove the file");

		break;
	      }
	    case DT_LNK:
	      {
		// remove the link.
		if (Link::Erase(target) == StatusError)
		  escape("unable to remove the link");

		break;
	      }
	    default:
	      {
		escape("unhandled file system object type");
	      }
	    }
	}

      // close the directory.
      ::closedir(directory);

      leave();
    }

  }
}
