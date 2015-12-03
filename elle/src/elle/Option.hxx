#ifndef ELLE_OPTION_HXX
# define ELLE_OPTION_HXX

# include <algorithm>
# include <type_traits>
# include <utility>

# include <elle/Error.hh>
# include <elle/TypeInfo.hh>
# include <elle/meta.hh>
# include <elle/printf.hh>

namespace elle
{
  namespace _details
  {
    template <std::size_t Size, int Index, typename ... Args>
    class OptionHelper
    {
    public:
      OptionHelper()
        : _index(-1)
      {}

      OptionHelper(OptionHelper<Size, Index, Args...>&&)
        : _index(-1)
      {}

      OptionHelper(OptionHelper<Size, Index, Args...> const&)
        : _index(-1)
      {}

      void
      reset()
      {}

      void
      _reset()
      {}

      void
      _emplace()
      {}

      template <typename T>
      void
      _assign(T&& source)
      {}

      template <typename T>
      void
      _assign(T const& source)
      {}

      static constexpr std::size_t size = Size;
    protected:
      char _buffer[size];
      int _index;
    };

    template <std::size_t Size, int Index, typename Head, typename ... Tail>
    class OptionHelper<Size, Index, Head, Tail ...>
      : public OptionHelper<(sizeof(Head) > Size ? sizeof(Head) : Size),
                             Index + 1, Tail ...>
    {
    public:
      typedef
        OptionHelper<(sizeof(Head) > Size ? sizeof(Head) : Size),
                     Index + 1, Tail ...>
        Super;

      OptionHelper() = default;

      // template <typename ... Args>
      // typename std::enable_if_exists<
      //   decltype(Head(std::declval<Args>()...)), void>::type
      // reset(Args&& ... args)
      // {
      //   new (this->_buffer) Head(std::forward<Args>(args)...);
      //   this->_index = Index;
      // }

      template <typename T, typename A>
      typename std::enable_if<std::is_same<T, Head>::value, void>::type
      _emplace(A&& value)
      {
        new (this->_buffer) Head(std::forward<A>(value));
        this->_index = Index;
      }
      using Super::_emplace;

      void
      _reset()
      {
        if (this->_index == Index)
        {
          char* buffer = this->_buffer;
          reinterpret_cast<Head*>(buffer)->~Head();
        }
        else
          this->Super::_reset();
      }

      template <typename T>
      void
      _assign(T&& source)
      {
        if (source._index == Index)
        {
          char* buffer = source._buffer;
          new (this->_buffer) Head(reinterpret_cast<Head&&>(*buffer));
        }
        else
          this->Super::_assign(std::move(source));
      }

      template <typename T>
      void
      _assign(T const& source)
      {
        if (source._index == Index)
        {
          char const* buffer = source._buffer;
          new (this->_buffer) Head(reinterpret_cast<Head const&>(*buffer));
        }
        else
          this->Super::_assign(source);
      }

      ~OptionHelper()
      {
        if (this->_index == Index)
        {
          char* buffer = this->_buffer;
          reinterpret_cast<Head&>(*buffer).~Head();
        }
      }

      OptionHelper(OptionHelper<Size, Index, Head, Tail...>&& source)
        : Super(static_cast<Super&&>(source))
      {
        if (source._index == Index)
        {
          char* buffer = source._buffer;
          new (this->_buffer) Head(reinterpret_cast<Head&&>(*buffer));
          this->_index = Index;
        }
      }

      OptionHelper(OptionHelper<Size, Index, Head, Tail...> const& source)
        : Super(static_cast<Super const&>(source))
      {
        if (source._index == Index)
        {
          char const* buffer = source._buffer;
          new (this->_buffer) Head(reinterpret_cast<Head const&>(*buffer));
          this->_index = Index;
        }
      }
    };
  }

  template <typename ... Types>
  Option<Types ...>::Option(Option<Types...>&& source)
    : _details::OptionHelper<0u, 0, Types ...>(
      static_cast<_details::OptionHelper<0u, 0, Types ...>&&>(source))
  {
    ELLE_ASSERT_NEQ(this->_index, -1);
  };

  template <typename ... Types>
  Option<Types ...>::Option(Option<Types ...> const& source)
    : _details::OptionHelper<0u, 0, Types ...>(
      static_cast<_details::OptionHelper<0u, 0, Types ...> const&>(source))
  {
    ELLE_ASSERT_NEQ(this->_index, -1);
  };

  template <typename ... Types>
  Option<Types ...>::Option(Option<Types ...>& source)
    : _details::OptionHelper<0u, 0, Types ...>(
      static_cast<_details::OptionHelper<0u, 0, Types ...>&>(source))
  {
    ELLE_ASSERT_NEQ(this->_index, -1);
  };

  template <typename ... Types>
  template <typename T>
  Option<Types ...>::Option(T&& value)
  {
    this->template _emplace<typename std::remove_cv_reference<T>::type>(
      std::forward<T>(value));
  }

  template <typename ... Types>
  Option<Types ...>&
  Option<Types ...>::operator =(Option<Types ...>&& option)
  {
    this->_reset();
    this->_assign(std::move(option));
    return *this;
  }

  template <typename ... Types>
  Option<Types ...>&
  Option<Types ...>::operator =(Option<Types ...> const& option)
  {
    this->_reset();
    this->_assign(option);
    return *this;
  }

  template <typename ... Types>
  template <typename T>
  void
  Option<Types ...>::emplace(T&& value)
  {
    typedef typename std::remove_cv_reference<T>::type Plain;
    this->_reset();
    this->template _emplace<Plain>(std::forward<T>(value));
  };

  template <typename ... Types>
  template <typename T>
  T&
  Option<Types ...>::get()
  {
    if (this->_index != meta::List<Types ...>::template index_of<T>::value)
      throw elle::Error(elle::sprintf("option is not a %s",
                                      elle::type_info<T>()));
    char* buffer = this->_buffer;
    return reinterpret_cast<T&>(*buffer);
  };

  template <typename ... Types>
  template <typename T>
  bool
  Option<Types ...>::is() const
  {
    return this->_index == meta::List<Types ...>::template index_of<T>::value;
  };
}

#endif
