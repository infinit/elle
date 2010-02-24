#ifndef HOLE_CMP_HH
# define HOLE_CMP_HH

namespace hole
{
  /** This template brings comparaison operators if you already have
   * a Cmp() method. */
  template<class C, typename T>
  class Cmp
  {
  public:
    inline bool operator<(const T & other) const;
    inline bool operator<=(const T & other) const;
    inline bool operator==(const T & other) const;
    inline bool operator>=(const T & other) const;
    inline bool operator>(const T & other) const;
  };
}

# include "hole/Cmp.hxx"

#endif /* !HOLE_CMP_HH */
