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
// updated       julien quintard   [fri may  6 17:58:20 2011]
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

#include <Infinit.hh>
#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>
#include <lune/Lune.hh>
#include <etoile/Etoile.hh>

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
    static const elle::String		Line;

    static const nucleus::Size		Range;

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
    static elle::Status	Initialize(const elle::String&,
				   const elle::String&);
    static elle::Status	Clean();

    //
    // template methods
    //

    ///
    /// this method releases no identifier.
    ///
    static elle::Void	Release()
    {
      // nothing to do
    }

    ///
    /// this method discard modifications applied onto the given identified
    /// context.
    ///
    template <typename T>
    static elle::Void	Release(T&		identifier)
    {
      PIG::Channel.Call(elle::Inputs<etoile::TagObjectDiscard>(identifier),
			elle::Outputs<etoile::TagOk>());
    }

    ///
    /// this method releases a set of contexts.
    ///
    template <typename T, typename... TT>
    static elle::Void	Release(T&		identifier,
				TT&...		identifiers)
    {
      Release(identifier);
      Release(identifiers...);
    }

    //
    // static attributes
    //
    static elle::Door				Channel;

    static nucleus::Subject*			Subject;

    struct					Somebody
    {
      static uid_t				UID;
      static gid_t				GID;
    };

    static lune::Dictionary			Dictionary;
  };

//
// ---------- macro-functions -------------------------------------------------
//

#define error(_text_, _errno_, _identifiers_...)			\
  do									\
    {									\
      report(elle::standalone::Report::TypeError, _text_);		\
									\
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
