//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/core/Type.hh
//
// created       julien quintard   [mon apr 26 15:49:09 2010]
// updated       julien quintard   [sun jun 26 21:07:01 2011]
//

#ifndef ELLE_CORE_TYPE_HH
#define ELLE_CORE_TYPE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Null.hh>
#include <elle/core/Boolean.hh>
#include <elle/core/Character.hh>
#include <elle/core/Integer.hh>
#include <elle/core/Large.hh>
#include <elle/core/Natural.hh>
#include <elle/core/Real.hh>
#include <elle/core/String.hh>
#include <elle/core/Void.hh>

#include <elle/radix/Status.hh>

namespace elle
{
  using namespace radix;

  namespace core
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides two methods for enabling the programmer to
    /// retrieve capacity properties according to a variable instead
    /// of a type.
    ///
    class Variable
    {
    public:
      //
      // methods
      //
      template <typename T>
      static T		Minimum(const T&);
      template <typename T>
      static T		Maximum(const T&);

      static Status	Convert(const Boolean&,
				String&);
      static Status	Convert(const Character&,
				String&);
      static Status	Convert(const Integer8&,
				String&);
      static Status	Convert(const Integer16&,
				String&);
      static Status	Convert(const Integer32&,
				String&);
      static Status	Convert(const Integer64&,
				String&);
      static Status	Convert(const Natural8&,
				String&);
      static Status	Convert(const Natural16&,
				String&);
      static Status	Convert(const Natural32&,
				String&);
      static Status	Convert(const Natural64&,
				String&);
      static Status	Convert(const Real&,
				String&);

      static Status	Convert(const String&,
				Boolean&);
      static Status	Convert(const String&,
				Character&);
      static Status	Convert(const String&,
				Integer8&);
      static Status	Convert(const String&,
				Integer16&);
      static Status	Convert(const String&,
				Integer32&);
      static Status	Convert(const String&,
				Integer64&);
      static Status	Convert(const String&,
				Natural8&);
      static Status	Convert(const String&,
				Natural16&);
      static Status	Convert(const String&,
				Natural32&);
      static Status	Convert(const String&,
				Natural64&);
      static Status	Convert(const String&,
				Real&);

      template <typename T>
      static Status	Convert(const T&,
				T&);

      template <typename T1,
		typename T2>
      static Status	Convert(const T1&,
				T2&);
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/core/Type.hxx>

#endif
