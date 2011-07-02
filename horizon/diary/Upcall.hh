//
// ---------- header ----------------------------------------------------------
//
// project       pig
//
// license       infinit
//
// file          /home/mycure/infinit/pig/diary/Upcall.hh
//
// created       julien quintard   [tue jun 28 14:58:51 2011]
// updated       julien quintard   [fri jul  1 21:34:21 2011]
//

#ifndef PIG_DIARY_UPCALL_HH
#define PIG_DIARY_UPCALL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace pig
{
  namespace diary
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
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
      elle::Status	Create(const Operation);

      template <typename... T>
      elle::Status	Inputs(const T&...);
      template <typename... T>
      elle::Status	Outputs(const T&...);
      elle::Status	Result(const elle::Integer32);

      //
      // interfaces
      //

      // object
      declare(Upcall);

      // dumpable
      elle::Status	Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status	Serialize(elle::Archive&) const;
      elle::Status	Extract(elle::Archive&);

      //
      // attributes
      //
      Operation		operation;
      elle::Archive	inputs;
      elle::Archive	outputs;
      elle::Integer32	result;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <pig/diary/Upcall.hxx>

#endif
