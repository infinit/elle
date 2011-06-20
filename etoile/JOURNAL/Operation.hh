//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/journal/Operation.hh
//
// created       julien quintard   [mon apr  5 21:21:30 2010]
// updated       julien quintard   [fri may 20 20:16:04 2011]
//

#ifndef ETOILE_JOURNAL_OPERATION_HH
#define ETOILE_JOURNAL_OPERATION_HH

namespace etoile
{
  namespace journal
  {

//
// ---------- enumerations ----------------------------------------------------
//

    ///
    /// this enumeration defines the type of operation to carry on, either
    /// push the block onto the hole or pop i.e remove a block.
    ///
    enum Operation
      {
	OperationPush,
	OperationWipe
      };

  }
}

#endif
