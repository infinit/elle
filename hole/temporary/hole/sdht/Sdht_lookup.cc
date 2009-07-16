//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       kayou
//
// file          infinite/hole/sdht/Sdht_stabilize.cc
//
// created       nicolas grandemange   [fri march  2 10:03:53 2007]
// updated       nicolas grandemange   [fri march 17 22:44:49 2007]
//

#include "Sdht.hh"
#include "Request.hh"

#include <elle/misc/Status.hh>
#include <elle/misc/Report.hh>

using namespace elle;
using namespace elle::misc;
using namespace elle::channel;


namespace hole
{
  namespace sdht
  { 
    Status
    Sdht::lookup(const Key		&k)
    {     
      Request				req(me_, Request::LOOKUP);
      
      req.data_key_ = k;
      m_request_.add(req);
      return (lookup_request(req));
    }
    
    Status
    Sdht::lookup_response(Request	&req)
    {
      return (lookup_request(req));
    }
    
    Status
    Sdht::lookup_request(Request	&req)
    {
      std::map<Key, Node*>::iterator	it;
      Node				*n;
      bool				first = true;
      Data				data;

      if (req.data_key_ <= me_.key_)
	if (req.data_key_ >= m_data_.begin()->first)
	  return (lookup_find(req));
      
      for (it = m_node_.begin(); it != m_node_.end(); it++)
	{
	  if (it->second != NULL)
	    {
	      if (first)
		{
		  n = it->second;
		  first = false;
		}
	      if (req.data_key_ < n->key_)
		{
		  std::cout << "SEND LOOKUP: " << req.id_ << " TO :" << *n << std::endl;
		  n->chan_->send(req);
		  infinit_leave();
		}     
	      n = it->second;
	    }
	}      
      std::cout << "NOBOBY to transfer LOOKUP: " << req.id_ << std::endl;
      infinit_leave();
    }
    
    Status
    Sdht::lookup_find(Request			&req)
    {
      req = req;
      std::cout << "FIND" << std::endl;
      infinit_leave();
    }

  }
}
