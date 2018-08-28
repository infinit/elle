#include <utility>

#include <elle/Printable.hh>
#include <elle/attribute.hh>
#include <elle/err.hh>
#include <elle/print.hh>
#include <elle/range.hh>

namespace elle
{
  template <typename Product, typename LRange, typename RRange>
  class cartesian_product_iterator
    : public elle::Printable::as<
    cartesian_product_iterator<Product, LRange, RRange>>
  {
  public:
    using Self = cartesian_product_iterator;
    cartesian_product_iterator()
      : _lhs(nullptr)
      , _rhs(nullptr)
      , _l()
      , _r()
    {}

    cartesian_product_iterator(LRange const& lhs, RRange const& rhs)
      : _lhs(&lhs)
      , _rhs(&rhs)
      , _l(lhs.begin())
      , _r(rhs.begin())
    {
      if (this->_l == this->_lhs->end() || this->_r == this->_rhs->end())
        this->_lhs = this->_rhs = nullptr;
    }

    bool
    operator == (Self const& other) const
    {
      return this->_l == other._l && this->_r == other._r ||
        this->_lhs == nullptr && other._lhs == nullptr;
    }

    bool
    operator != (Self const& other) const
    {
      return !(*this == other);
    }

    auto
    operator * () const
    {
      return this->_product(*this->_l, *this->_r);
    }

    auto
    operator ++ ()
    {
      if (this->_lhs == nullptr || this->_rhs == nullptr)
        elle::err("past the bound cartesian_product iterator");
      ++this->_r;
      if (this->_r == this->_rhs->end())
      {
        ++this->_l;
        this->_r = this->_rhs->begin();
      }
      if (this->_l == this->_lhs->end())
        this->_lhs = this->_rhs = nullptr;
      return *this;
    }

    auto
    operator ++ (int)
    {
      auto res = *this;
      ++*this;
      return res;
    }

    struct MakePair
    {
      template <typename L, typename R>
      auto
      operator () (L&& l, R&& r) const
      {
        return std::make_pair(std::forward<L>(l), std::forward<R>(r));
      }
    };

    ELLE_ATTRIBUTE(LRange const*, lhs);
    ELLE_ATTRIBUTE(RRange const*, rhs);
    ELLE_ATTRIBUTE(typename LRange::const_iterator, l);
    ELLE_ATTRIBUTE(typename RRange::const_iterator, r);
    ELLE_ATTRIBUTE((typename std::conditional<
                    std::is_same<Product, void>::value,
                    MakePair,
                    Product>::type),
                   product);
  public:
    using iterator_category = std::input_iterator_tag;
    using value_type = decltype(*std::declval<Self>());
    using difference_type = int;
    using reference = value_type;
    using pointer = value_type*;
    void
    print(std::ostream& s) const
    {
      if (this->_lhs != nullptr && this->_rhs != nullptr)
        elle::print(
          s, "cartesian_product_iterator({}, {})",
          this->_l - this->_lhs->begin(), this->_r - this->_rhs->begin());
      else
        elle::print(s, "cartesian_product_iterator()");
    }
  };

  template <typename Product, typename LRange, typename RRange>
  struct CartesianProduct
  {
    using Iterator = cartesian_product_iterator<Product, std::remove_cv_reference_t<LRange>, std::remove_cv_reference_t<RRange>>;

    template <typename L, typename R>
    CartesianProduct(L&& l, R&& r)
      : _l(std::forward<L>(l))
      , _r(std::forward<R>(r))
    {}

    auto
    begin() const
    {
      return Iterator(this->_l, this->_r);
    }

    auto
    end() const
    {
      return Iterator();
    }

    using iterator = Iterator;
    using const_iterator = Iterator;
    using value_type = typename std::iterator_traits<Iterator>::value_type;

  private:
    LRange _l;
    RRange _r;
  };

  template <typename Product = void, typename LRange, typename RRange>
  auto
  cartesian_product(LRange&& lhs, RRange&& rhs)
  {
    return CartesianProduct<Product, LRange, RRange>(
      std::forward<LRange>(lhs), std::forward<RRange>(rhs));
  };
}
