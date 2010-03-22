//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/system/Frame.hh
//
// created       julien quintard   [sun mar 21 23:09:07 2010]
// updated       julien quintard   [mon mar 22 21:15:28 2010]
//

#ifndef ELLE_SYSTEM_FRAME_HH
#define ELLE_SYSTEM_FRAME_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>

namespace elle
{
  using namespace core;
  using namespace misc;

  namespace system
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides the functionality to create frames--i.e stack-based
    /// contexts---but also to jump from one another.
    ///
    class Frame:
      public Dumpable
    {
    public:
      //
      // constants
      //
      static const Natural32		Capacity;

      //
      // types
      //
      typedef std::list<Frame*>			Container;
      typedef Container::iterator		Iterator;
      typedef Container::const_iterator		Scoutor;

      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      static Status	Switch(Frame*); // XXXX

      static Status	Show(const Natural32 = 0);

      //static Status	Setup(); // a appeler avant le exec() pour y retourner

      // XXX Checkpoint() Snapshot() en entrant dans un slot pour pouvoir
      // reprendre a ce point la plus tard.

      // Frame::Vortex() return either Close or Distant very much like
      // setjmp().

      //
      // methods
      //
      Status		Create(const Natural32 = Capacity);

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      //
      // static attributes
      //
      static Frame			Application;
      static Frame*			Event;

      static Frame*			Current;
      static Container			Frames;

      //
      // attributes
      //
      Void*			stack;
      Void*			esp;
      Void*			ebp;
    };

//
// ---------- macro-functions -------------------------------------------------
//

// XXX temporary!!!

#define frame(_frame_)							\
  asm volatile ("movl %%ebp, %0\n\t"					\
		"movl %%esp, %1\n\t"					\
									\
		"movl %2, %%ebp\n\t"					\
		"movl %3, %%esp\n\t"					\
		: "=m" ((elle::system::Frame::Current->ebp)),		\
		  "=m" ((elle::system::Frame::Current->esp))		\
		: "g" (((_frame_)->ebp)),				\
		  "g" (((_frame_)->esp))				\
		:							\
	       );

  }
}

#endif
