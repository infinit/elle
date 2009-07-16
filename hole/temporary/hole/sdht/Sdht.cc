//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       kayou
//
// file          infinite/hole/sdht/Sdht.cc
//
// created       nicolas grandemange   [fri march  2 10:03:53 2007]
// updated       nicolas grandemange   [fri march 17 22:44:49 2007]
//

#include <boost/thread/thread.hpp>

#include "Sdht.hh"

#include <elle/misc/Status.hh>
#include <viewer/ubi/Viewer.hh>

using namespace elle;
using namespace viewer;
using namespace elle::misc;
using namespace elle::channel;

namespace hole
{
  namespace sdht
  { 
    Sdht::Sdht(const Ip		&ip,
	       const Port	&port)
      : me_(ip, port),
	m_node_(me_)
    {
    }
    
    Sdht::~Sdht()
    {
      std::cout << "Whaiting the looker..." << std::endl;
      looker_->join();
      std::cout << "Whaiting the listener..." << std::endl;
      listener_->join();
      Viewer::remove_node(me_.key_);
      delete listener_;
    }

    Status
    Sdht::print()
    {
      std::cout << "--------------------------------------------------------------------------" << std::endl;
      std::cout << "Key: " << me_.key_ << std::endl;
      m_node_.print();
      m_data_.print();
      //m_request_.print(); //uncomment this, and auto clean the request map
      std::cout << "--------------------------------------------------------------------------" << std::endl;
      return StatusOk;
    }
    
  }
}
