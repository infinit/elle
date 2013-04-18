#ifndef ELLE_ORDERABLE_HH
# define ELLE_ORDERABLE_HH

# include <elle/Comparable.hh>

namespace elle
{
  template <typename Self, typename T = Self>
  class Orderable:
    public Comparable<Self, T>
  {
  public:
    enum class Order
    {
      less = -1,
      equal = 0,
      more = 1,
    };
    bool
    operator < (T const& other) const;
    bool
    operator <= (T const& other) const;
    bool
    operator > (T const& other) const;
    bool
    operator >= (T const& other) const;
  private:
    friend class Comparable<T>;
    bool
    _compare(T const& other) const;
  };
}

# include <elle/Orderable.hxx>

#endif
