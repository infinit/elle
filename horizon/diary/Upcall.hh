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
// updated       julien quintard   [wed jun 29 17:54:24 2011]
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
      elle::Status	Result(const elle::Natural32);

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
      elle::Archive	archive;
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <pig/diary/Upcall.hxx>

#endif
