//
// ---------- header ----------------------------------------------------------
//
// project       passport
//
// license       infinit
//
// author        julien quintard   [thu aug 11 15:16:11 2011]
//

#ifndef PASSPORT_PASSPORT_HH
#define PASSPORT_PASSPORT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

namespace satellite
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class implements the passport satellite.
  ///
  class Passport
  {
  public:
    //
    // enumerations
    //
    enum Operation
      {
        OperationUnknown = 0,

        OperationCreate,
        OperationDestroy,
        OperationInformation
      };

    //
    // static methods
    //
    static
    void
    Create(elle::String const& user_id,
           elle::String const& passport_name);
    static elle::Status         Destroy(elle::String const& user);
    static elle::Status         Information(elle::String const& user);
  };

}

#endif
