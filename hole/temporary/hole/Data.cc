//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       kayou
//
// file          infinite/hole/sdht/Data.cc
//
// created       nicolas grandemange   [fri march  2 10:03:53 2007]
// updated       nicolas grandemange   [fri march 17 22:44:49 2007]
//


#include "Data.hh"

#include <elle/misc/Status.hh>

namespace hole
{
  Data::Data()
    : data_("Empty")
  {
  }
  
  Data::Data(std::string	str)
  {
    data_ = str;
  }
  
  std::string
  Data::GetData()
  {
    return (data_);
  }
  
  void
  Data::SetData(std::string	str)
  {
    data_ = str;
  }
  
  std::ostream& 
  operator<<		(std::ostream&	s,
			 const Data	&b)
  {
    s << b.data_;
    return s;
  }
  
}
