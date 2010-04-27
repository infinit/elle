//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/PIG.hh
//
// created       julien quintard   [fri jul 31 22:11:24 2009]
// updated       julien quintard   [tue apr 27 18:10:20 2010]
//

#ifndef PIG_PIG_HH
#define PIG_PIG_HH

//
// ---------- macros ----------------------------------------------------------
//

#define FUSE_USE_VERSION		26

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <etoile/Etoile.hh>
#include <map/Map.hh>

#include <elle/idiom/Close.hh>
# include <fuse.h>
# include <sys/types.h>
# include <pwd.h>
# include <attr/xattr.h>
#include <elle/idiom/Open.hh>

namespace pig
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// XXX
  ///
  class PIG
  {
  public:
    //
    // constants
    //
    static const String			Path;
    static const String			Line;

    static const etoile::kernel::Size	Frame;

    //
    // callbacks
    //

    // general purpose
    static int		Getattr(const char*,
				struct stat*);
    static int		Fgetattr(const char*,
				 struct stat*,
				 struct fuse_file_info*);
    static int		Utimens(const char*,
				const struct timespec[2]);

    // directory
    static int		Opendir(const char*,
				struct fuse_file_info*);
    static int		Readdir(const char*,
				void*,
				fuse_fill_dir_t,
				off_t,
				struct fuse_file_info*);
    static int		Releasedir(const char*,
				   struct fuse_file_info*);
    static int		Mkdir(const char*,
			      mode_t);
    static int		Rmdir(const char*);

    // access
    static int		Access(const char*,
			       int);
    static int		Chmod(const char*,
			      mode_t);
    static int		Chown(const char*,
			      uid_t,
			      gid_t);

    // attribute
    static int		Setxattr(const char*,
				 const char*,
				 const char*,
				 size_t,
				 int);
    static int		Getxattr(const char*,
				 const char*,
				 char*,
				 size_t);
    static int		Listxattr(const char*,
				  char*,
				  size_t);
    static int		Removexattr(const char*,
				    const char*);

    // lock
    static int		Lock(const char*,
			     struct fuse_file_info*,
			     int,
			     struct flock*);

    // link
    static int		Symlink(const char*,
				const char*);
    static int		Readlink(const char*,
				 char*,
				 size_t);

    // file
    static int		Create(const char*,
			       mode_t,
			       struct fuse_file_info*);
    static int		Open(const char*,
			     struct fuse_file_info*);
    static int		Write(const char*,
			      const char*,
			      size_t,
			      off_t,
			      struct fuse_file_info*);
    static int		Read(const char*,
			     char*,
			     size_t,
			     off_t,
			     struct fuse_file_info*);
    static int		Truncate(const char*,
				 off_t);
    static int		Ftruncate(const char*,
				  off_t,
				  struct fuse_file_info*);
    static int		Release(const char*,
				struct fuse_file_info*);

    // objects
    static int		Rename(const char*,
			       const char*);
    static int		Unlink(const char*);

    // sync
    static int		Fsync(const char*,
			      int,
			      struct fuse_file_info*);
    static int		Fsyncdir(const char*,
				 int,
				 struct fuse_file_info*);

    //
    // static methods
    //
    static Status	Initialize();
    static Status	Clean();

    //
    // template methods
    //

    ///
    /// this method releases no identifier.
    ///
    static Void		Release()
    {
      // nothing to do
    }

    ///
    /// XXX
    ///
    template <typename T>
    static Void		Release(T&		identifier)
    {
      PIG::Channel.Call(Inputs<TagObjectDiscard>(identifier),
			Outputs<TagOk>());
    }

    ///
    /// XXX
    ///
    template <typename T, typename... TT>
    static Void		Release(T&		identifier,
				TT&...		identifiers)
    {
      Release(identifier);
      Release(identifiers...);
    }

    //
    // static attributes
    //
    static Door					Channel;

    static PublicKey				K;
    static String				Phrase;

    static etoile::kernel::Subject*		Subject;

    struct					Somebody
    {
      static uid_t				UID;
      static gid_t				GID;
    };

    struct					Maps
    {
      static map::Map<PublicKey>		Users;
      static map::Map<etoile::hole::Address>	Groups;
    };
  };

//
// ---------- macro-functions -------------------------------------------------
//

#define error(_errno_, _identifiers_...)				\
  do									\
    {									\
      show();								\
									\
      PIG::Release(_identifiers_);					\
									\
      purge();								\
									\
      return (-(_errno_));						\
    } while (false)

#define skip(_errno_, _identifiers_...)					\
  do									\
    {									\
      PIG::Release(_identifiers_);					\
									\
      purge();								\
									\
      return (-(_errno_));						\
    } while (false)

}

#endif
