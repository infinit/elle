//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// file          /home/mycure/infinit/hole/Label.hh
//
// created       julien quintard   [thu aug 11 14:01:43 2011]
// updated       julien quintard   [thu aug 11 15:02:53 2011]
//

#ifndef HOLE_LABEL_HH
#define HOLE_LABEL_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

namespace hole
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// XXX
  ///
  class Label:
    public elle::Object
  {
  public:
    //
    // constants
    //
    static const Label		Null;

    //
    // constructors & destructors
    //
    Label();
    Label(const Label&);
    ~Label();

    //
    // methods
    //
    template <typename T>
    elle::Status	Create(const T&);

    //
    // interfaces
    //

    // object
    declare(Label);
    elle::Boolean	operator==(const Label&) const;
    elle::Boolean	operator<(const Label&) const;

    // dumpable
    elle::Status	Dump(const elle::Natural32 = 0) const;

    // archivable
    elle::Status	Serialize(elle::Archive&) const;
    elle::Status	Extract(elle::Archive&);

    //
    // attributes
    //
    elle::Digest*	digest;
  };

}

//
// ---------- templates -------------------------------------------------------
//

#include <hole/Label.hxx>

#endif
