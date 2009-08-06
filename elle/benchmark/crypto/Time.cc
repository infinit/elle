//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit (c)
//
// file          /home/mycure/infinit/elle/benchmark/crypto/Time.cc
//
// created       julien quintard   [wed aug  5 15:25:54 2009]
// updated       julien quintard   [thu aug  6 12:16:58 2009]
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
      ::gettimeofday(&this->start, NULL);

      leave();
    }

    Status		Time::Stop()
    {
      ::gettimeofday(&this->stop, NULL);

      leave();
    }

    Status		Time::Print()
    {
      Natural32		useconds;

      useconds =
	((this->stop.tv_sec * 1000000.0) + this->stop.tv_usec) -
	((this->start.tv_sec * 1000000.0) + this->start.tv_usec);

      Real		mseconds;

      mseconds = (Real)useconds / 1000.0;

      std::cout << mseconds;

      leave();
    }

  }
}
