//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       kayou
//
// file          infinite/hole/sdht/Map_Node.cc
//
// created       nicolas grandemange   [fri march  2 10:03:53 2007]
// updated       nicolas grandemange   [fri march 17 22:44:49 2007]
//


#include "Map_Node.hh"

#include <map>
#include <elle/misc/Status.hh>
#include <viewer/ubi/Viewer.hh>

using namespace viewer;

namespace hole
{
  namespace sdht
  { 
    Map_Node::Map_Node(const Node		&me)
      : me_(me)
    {
      Key		puissance(1);
      Key		new_k;
      int		cpt = 0;
      
      for (cpt = 0; cpt < (KEYLEN * 8); cpt++)
	{  
	  new_k = me_.key_ + puissance;
	  c_[new_k] = NULL;
	  puissance = puissance * 2;
	}
      std::cout << cpt << " perfect key have been computed" << std::endl;
    }
 
    Status
    Map_Node::sendto(const Key			&k,
		     const Request		&req)
    {
      std::map<Key, Node*, cmp_key>::iterator	it;
      
      it = c_.find(k);
      if (it == c_.end())
	{
	  std::cout << "Cannot send to an unknow key" << std::endl;
	  infinit_fail();
	}
      if (it->second->chan_->send(req) != StatusOk)
	{
	  report.Display("Send to");
	  infinit_fail();  
	}
      infinit_leave();
    }
    
    ///
    /// WARNING  
    /// -gerer le cas d un look sur un cle plus petite que me_.key_
    /// -gerer le cas circulaire
    ///
    Status
    Map_Node::look(Node				&n)
    {
      std::map<Key, Node*>::iterator		it;
      std::map<Key, Node*>::iterator		it_back;
      
      for (it_back = it = c_.begin(); it != c_.end(); it++)
	{
	  if (n.key_ < it->first)
	    {
	      if (it_back->second)
		{
		  if (n.key_ < it_back->second->key_)
		    {
		      Viewer::remove_edge(me_.key_, it_back->second->key_);
		      delete it_back->second;
		      it_back->second = new Node(n.ip_, n.port_);
		      Viewer::add_edge(me_.key_, it_back->second->key_);
		      std::cout << "Adding new node to the contact list over an old value" << std::endl;
		      print();
		    }
		}
	      else
		{
		  it_back->second = new Node(n.ip_, n.port_);
		  Viewer::add_edge(me_.key_, it_back->second->key_);    
		  std::cout << "Adding new node to the contact list" << std::endl;
		  print();
		}
	      infinit_leave();
	    } 
	  it_back = it;
	}      
      std::cout << "Look not match" << std::endl;
      infinit_leave();
    }
    
    Status
    Map_Node::touch(const Key			&k)
    {
      std::map<Key, Node*, cmp_key>::iterator	it;
      
      it = c_.begin();
      while ((it != c_.end()) && (!it->second || (it->second->key_ != k)))
	it++;
      if (it == c_.end())
	{
	  std::cout << "Cannot TOUCH an unknow key" << std::endl;
	  infinit_fail();
	}
      it->second->life_ = NODE_LIFE;
      return StatusOk;
    }
    
    std::map<Key, Node*>::iterator	
    Map_Node::begin()
    {
      return c_.begin();
    }
      
    std::map<Key, Node*>::iterator	
    Map_Node::end()
    {
      return c_.end();
    }

    Status
    Map_Node::print()
    {
      bool					empty = true;
      std::map<Key, Node*>::iterator		it;
      
      std::cout << "Map_Node list:" << std::endl;
      
      for (it = c_.begin(); it != c_.end(); it++)
	if (it->second)
	  {
	    empty = false;
	    std::cout << "   Perfect Key: " << it->first << " node: " << *(it->second) << std::endl;
	  }
	else
	    std::cout << "   Perfect Key: " << it->first << " node: NULL" << std::endl;
	  
      if (empty)
	std::cout << "   Empty" << std::endl;
	
	  infinit_leave();
    }
  }
}
