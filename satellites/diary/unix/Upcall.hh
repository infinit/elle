#ifndef DIARY_UNIX_UPCALL_HH
# define DIARY_UNIX_UPCALL_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>
# include <elle/utility/Buffer.hh>
# include <elle/serialize/fwd.hh>
# include <elle/idiom/Open.hh>

namespace satellite
{
  #undef unix
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
      public elle::radix::Object
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
      template<typename... T>
        elle::Status    ExtractInputs(T&... value);
      template<typename... T>
        elle::Status    ExtractOutputs(T&... value);


      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // archivable
      //elle::Status      Serialize(elle::Archive&) const;
      //elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      Operation         operation;
      elle::Integer32   result;
    private:
      elle::utility::Buffer _inputs;
      elle::utility::Buffer _outputs;
      ELLE_SERIALIZE_FRIEND_FOR(Upcall);
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <satellites/diary/unix/Upcall.hxx>

#endif
