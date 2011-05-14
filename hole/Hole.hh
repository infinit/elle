//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/Hole.hh
//
// created       julien quintard   [tue apr 13 15:25:04 2010]
// updated       julien quintard   [fri may 13 14:27:30 2011]
//

#ifndef HOLE_HOLE_HH
#define HOLE_HOLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <hole/Holeable.hh>

#include <elle/idiom/Close.hh>
# include <map>
#include <elle/idiom/Open.hh>

///
/// this namespace is used by the hole storage abstraction.
///
namespace hole
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class represents the abstract Hole component.
  ///
  /// XXX protect communication between hole and etoile! use a phrase-like
  /// controller.
  ///
  class Hole
  {
  public:
    //
    // constants
    //
    static const elle::String&		Line;

    //
    // types
    //
    typedef std::map<const nucleus::Network,
		     Holeable*>			Container;
    typedef Container::iterator			Iterator;
    typedef Container::const_iterator		Scoutor;

    //
    // static methods
    //
    static elle::Status		Initialize();
    static elle::Status		Clean();

    static elle::Status		Join(const nucleus::Network&);
    static elle::Status		Leave(const nucleus::Network&);

    static elle::Status		Put(const nucleus::Network&,
				    const nucleus::Address&,
				    const elle::Derivable<nucleus::Block>&);
    static elle::Status		Get(const nucleus::Network&,
				    const nucleus::Address&);
    static elle::Status		Erase(const nucleus::Network&,
				      const nucleus::Address&);

    static elle::Status		Connection(elle::Door*&);
    static elle::Status		Error(const elle::String&);

    //
    // static attributes
    //
    static Container		Networks;
    static elle::Channel*	Channel;
  };

}

//
// ---------- includes --------------------------------------------------------
//

#include <hole/local/Local.hh>

#endif
