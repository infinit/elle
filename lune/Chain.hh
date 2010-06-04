//
// ---------- header ----------------------------------------------------------
//
// project       lune
//
// license       infinit
//
// file          /home/mycure/infinit/lune/Chain.hh
//
// created       julien quintard   [mon may 24 13:26:17 2010]
// updated       julien quintard   [fri may 28 18:06:08 2010]
//

#ifndef LUNE_CHAIN_HH
#define LUNE_CHAIN_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <lune/Passport.hh>

#include <elle/idiom/Close.hh>
# include <sys/types.h>
# include <dirent.h>
#include <elle/idiom/Open.hh>

namespace lune
{

//
// ---------- classes ---------------------------------------------------------
//

// XXX charger au fur et a mesure!
// A REVOIR

  ///
  /// this chain entity represents the collection of passports associated
  /// with a user.
  ///
  class Chain:
    public elle::Entity
  {
  public:
    //
    // types
    //
    typedef std::pair<elle::String, lune::Passport*>	Value;
    typedef std::map<elle::String, lune::Passport*>	Container;
    typedef Container::iterator				Iterator;
    typedef Container::const_iterator			Scoutor;

    //
    // constructors & destructors
    //
    ~Chain();

    //
    // methods
    //
    elle::Status	Load(const lune::Identity&);
    elle::Status	Lookup(const elle::String&,
			       lune::Passport*&);

    //
    // interfaces
    //

    // dumpable
    elle::Status	Dump(const elle::Natural32 = 0) const;

    //
    // attributes
    //
    Container		container;
  };

}

#endif
