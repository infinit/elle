//
// ---------- header ----------------------------------------------------------
//
// project       infinite
//
// license       GPL
//
// file          infinite/hole/hole.cc
//
// created       nicolas grandemange   [fri april 29 10:03:53 2007]
// updated       nicolas grandemange   ??????

//
// ---------- namespace -------------------------------------------------------
//
#include <iostream>

#include "Hole.hh"

#include <elle/misc/Network.hh>
#include <viewer/ubi/Viewer.hh>

using namespace viewer;

namespace hole
{
  
  Hole::Hole()
  {
      Viewer::init();
  }

  Hole::~Hole()
  {  
  }
  

}
