//
// ---------- header ----------------------------------------------------------
//
// project       diary
//
// license       infinit
//
// author        julien quintard   [tue jun 28 14:58:51 2011]
//
 
#ifndef DIARY_UNIX_UPCALL_HH
#define DIARY_UNIX_UPCALL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace satellite
{
  namespace unix
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a FUSE upcall which contains the operation
    /// code, the inputs and outputs arguments along with the returned value.
    ///
    class Upcall:
      public elle::Object
    {
    public:
      //
      // enumerations
      //
      enum Operation
        {
          OperationGetattr,
          OperationFgetattr,
          OperationUtimens,
          OperationOpendir,
          OperationReaddir,
          OperationReleasedir,
          OperationMkdir,
          OperationRmdir,
          OperationAccess,
          OperationChmod,
          OperationChown,
          OperationSetxattr,
          OperationGetxattr,
          OperationListxattr,
          OperationRemovexattr,
          OperationSymlink,
          OperationReadlink,
          OperationCreate,
          OperationOpen,
          OperationWrite,
          OperationRead,
          OperationTruncate,
          OperationFtruncate,
          OperationRelease,
          OperationRename,
          OperationUnlink
        };

      //
      // methods
      //
      elle::Status      Create(const Operation);

      template <typename... T>
      elle::Status      Inputs(const T&...);
      template <typename... T>
      elle::Status      Outputs(const T&...);
      elle::Status      Result(const elle::Integer32);

      //
      // interfaces
      //

      // object
      declare(Upcall);

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status      Serialize(elle::Archive&) const;
      elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      Operation         operation;
      elle::Archive     inputs;
      elle::Archive     outputs;
      elle::Integer32   result;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <satellites/diary/unix/Upcall.hxx>

#endif
