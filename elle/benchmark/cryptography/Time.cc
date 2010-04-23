//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/benchmark/cryptography/Time.cc
//
// created       julien quintard   [wed aug  5 15:25:54 2009]
// updated       julien quintard   [thu mar 11 16:17:33 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include "Time.hh"

namespace elle
{
  namespace benchmark
  {

//
// ---------- methods ---------------------------------------------------------
//

    Status		Time::Start()
    {
      enter();

      ::gettimeofday(&this->start, NULL);

      leave();
    }

    Status		Time::Stop()
    {
      enter();

      ::gettimeofday(&this->stop, NULL);

      leave();
    }

    Status		Time::Print()
    {
      Natural32		useconds;

      enter();

      useconds =
	((this->stop.tv_sec * 1000000.0) + this->stop.tv_usec) -
	((this->start.tv_sec * 1000000.0) + this->start.tv_usec);

      Real		mseconds;

      mseconds = (Real)useconds / 1000.0;

      std::cout << mseconds << " ms";

      leave();
    }

  }
}
