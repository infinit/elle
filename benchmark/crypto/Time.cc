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
// updated       julien quintard   [wed aug  5 16:22:04 2009]
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
      Real		useconds;

      useconds =
	(((Real)this->stop.tv_sec * 1000000.0) + (Real)this->stop.tv_usec) -
	(((Real)this->start.tv_sec * 1000000.0) + (Real)this->start.tv_usec);

      Natural32		ms;
      Natural32		us;

      ms = useconds / 1000.0;
      us = useconds - ((Real)ms * 1000.0);

      std::cout << ms << "." << us;

      leave();
    }

  }
}
