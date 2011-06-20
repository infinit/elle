//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/gear/Operation.hh
//
// created       julien quintard   [fri jun 10 14:28:17 2011]
// updated       julien quintard   [fri jun 17 16:51:05 2011]
//

#ifndef ETOILE_GEAR_OPERATION_HH
#define ETOILE_GEAR_OPERATION_HH

namespace etoile
{
  namespace gear
  {

    enum Operation
      {
	OperationObjectLoad,
	OperationObjectCreate,
      };

    struct Operation
    {
      struct Object
      {
	const Natural32		Load = 1;
      };
    };

    Operation::Object::Load
  }
}

#endif
