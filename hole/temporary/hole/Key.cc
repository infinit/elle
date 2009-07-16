//
// ---------- header ----------------------------------------------------------
//
// project       il
//
// license       kayou
//
// file          infinit/hole/sdht/Contact.cc
//
// created       nicolas grandemange   [fri march  2 10:03:53 2007]
// updated       nicolas grandemange   [fri march 17 22:44:49 2007]
//

#include "Key.hh"

#include <iostream>
#include <iomanip>

#include <openssl/sha.h>

#include <elle/misc/Status.hh>
#include <elle/misc/Report.hh>

using namespace elle;
using namespace elle::misc;

namespace hole
{
  Key::Key(const Ip			&ip,
	   const Port			&port)
  {
    SHA_CTX				c;
    std::ostringstream			code;
    

    code << ip << ':' << port;
    
    memset(k_, 0, KEYLEN);
    if (SHA1_Init(&c) != 1)
      {
	infinit_append("SHA1 init fail");
	return;
      }
    if (SHA1_Update(&c, code.str().c_str(), 14) != 1)
      {
	infinit_append("SHA1 update fail");
	return;
      }
    //WRITE EXACTLY 20Byte
    if (SHA1_Final(k_, &c) != 1)
      {
	infinit_append("SHA1 final fail");
	return;
      }
  }
  
  Key::Key()
  {
    memset(k_, 0, KEYLEN);
  }

  Key::Key(unsigned char	v)
  {
    memset(k_, 0, KEYLEN - 1);
    k_[KEYLEN - 1] = v;
  }
  
  Natural(16)	
  Key::to_natural16() const
  {
    Natural(16)	res = 0;
    Integer(8) i = 0;
    
    for (i = 0; i < (16 / 8); i++)
      res = res * 256 + k_[i];
    
      return (res);
  }
  
  Integer(8)
  Key::compar(const Key		&b) const
  {
    Integer(8) i = 0;
    
    for (i = 0; i < KEYLEN; i++)
      {
	if (this->k_[i] < b.k_[i])
	  return -1;
	if (this->k_[i] > b.k_[i])
	  return 1;
      }
    return 0;
  }
  
  bool
  Key::operator == (const Key		&b) const
  {
    return (compar(b) == 0);
  }

  bool
  Key::operator != (const Key		&b) const
  {
    return (compar(b) != 0);
  }
  
  bool
  Key::operator < (const Key		&b) const
  {
    return (compar(b) < 0);
  }
  
  bool
  Key::operator <= (const Key		&b) const
  {
    return (compar(b) <= 0);
  }
  
  bool
  Key::operator > (const Key		&b) const
  {
    return (compar(b) > 0);
  }
  
  bool
  Key::operator >= (const Key		&b) const
  {
    return (compar(b) >= 0);
  }
  
  Key
  Key::operator + (const Key		&b) const
  {
    Key		res = *this;

    if (b == Key())
      return res;

    Natural(16) iSum = 0;

    for (int iIndex = KEYLEN - 1; iIndex >= 0; iIndex--)
      {
	iSum += res.k_[iIndex];
	iSum += b.k_[iIndex];
	res.k_[iIndex] = (unsigned char)iSum;
	iSum = iSum >> 8;
      }
    return res;
  }

  Key
  Key::operator - (const Key		&b) const
  {
    Key		res = *this;

    if (b == Key())
      return *this;
    
    Integer(16) iSum = 0;    

    for (int iIndex = KEYLEN - 1; iIndex >= 0; iIndex--)
      {
	iSum += k_[iIndex];
	iSum -= b.k_[iIndex];
	res.k_[iIndex] = (unsigned char)iSum;
	iSum = iSum >> 8;
      }
    return res;
  }
  
  Key
  Key::operator * (const unsigned char		b) const
  {
    Key		res;
    
    if (b == 0)
      return res;

    Natural(16) iMul = 0;
    Natural(16) iRet = 0;

    for (int iIndex = KEYLEN - 1; iIndex >= 0; iIndex--)
      {
	iMul = this->k_[iIndex];
	iMul *= b;
	iMul += iRet;
	res.k_[iIndex] = (unsigned char) iMul;
	iRet = iMul >> 8;
      }
    return res;
  }
  
  Status
  Key::print() const
  {
    std::cout << *this << std::endl;
    return (StatusOk);
  }
  
  std::string	
  Key::to_string() const
  {
    std::stringstream		str;
    
    str << *this;
    return (str.str());
  }
  
  Integer(8)
  compar(const Key		&a,
	 const Key		&b)
  {
    return (a.compar(b));
  }
  
  
  std::ostream&     
  operator<<		(std::ostream&	s,
			 const Key	&b)
  {
    int i = 0;
    
    for (i = 0; i < (KEYLEN - 1); i++)
      {
	s << std::hex << std::setw(2) << std::setfill('0');
	s << (Natural(16))b.k_[i] << ":";
      }
    s << std::hex << std::setw(2) << std::setfill('0');
    s << (Natural(16))b.k_[i];
    s << std::dec << std::setfill(' ');
    return s;
  }
  
 
}

