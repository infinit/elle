//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sun may  2 11:15:11 2010]
//

#ifndef ELLE_STANDALONE_REPORT_HXX
#define ELLE_STANDALONE_REPORT_HXX

namespace elle
{
  namespace standalone
  {

//
// ---------- object-like -----------------------------------------------------
//

    ///
    /// this method recycles a report.
    ///
    template <typename T>
    Status              Report::Recycle(const T*                object)
    {
      // release the resources.
      this->~Report();

      if (object == NULL)
        {
          // initialize the object with default values.
          new (this) T;
        }
      else
        {
          // initialize the object with defined values.
          new (this) T(*object);
        }

      // return StatusOk in order to avoid including Report, Status and Maid.
      return (StatusOk);
    }

  }
}

#endif
