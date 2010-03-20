//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Result.hh
//
// created       julien quintard   [fri mar 19 23:49:12 2010]
// updated       julien quintard   [sat mar 20 14:32:15 2010]
//

#ifndef ETOILE_WALL_RESULT_HH
#define ETOILE_WALL_RESULT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a code returns to the caller indicating
    /// whether the operation was successful.
    ///
    /// if not, a report composed of a sequence of messages is attached.
    ///
    class Result:
      public Entity,
      public Dumpable, public Archivable
    {
    public:
      //
      // constants
      //
      static const Result		Ok;
      static const Result		Error;

      //
      // constructors & destructors
      //
      Result();
      Result(const Status);

      //
      // interfaces
      //

      // entity
      declare(Entity, Result);
      Boolean		operator==(const Result&) const;

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      Status		status;
      Report		report;
    };

  }
}

#endif
