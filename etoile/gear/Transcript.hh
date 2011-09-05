//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Transcript.hh
//
// created       julien quintard   [fri jun 17 13:10:45 2011]
// updated       julien quintard   [sun sep  4 16:26:30 2011]
//

#ifndef ETOILE_GEAR_TRANSCRIPT_HH
#define ETOILE_GEAR_TRANSCRIPT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/gear/Action.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a set of actions to be performed on the
    /// storage layer such as pushing this block, removing this one and so
    /// on.
    ///
    class Transcript:
      public elle::Object
    {
    public:
      //
      // types
      //
      typedef std::list<Action*>		Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // constructors & destructors
      //
      ~Transcript();

      //
      // methods
      //
      elle::Status	Push(const nucleus::Address&,
			     const nucleus::Block*);
      elle::Status	Wipe(const nucleus::Address&);

      elle::Status	Flush();

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
}

#endif
