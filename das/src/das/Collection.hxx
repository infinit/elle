#ifndef DAS_COLLECTION_HXX
# define DAS_COLLECTION_HXX

namespace das
{
  template <typename T, typename K, K (T::*key)>
  IndexList<T, K, key>::IndexList()
    : _contents()
  {}

  template <typename T, typename K, K (T::*key)>
  void
  IndexList<T, K, key>::add(T t)
  {
    this->_contents.emplace(t.*key, std::move(t));
  }

  template <typename T, typename K, K (T::*key)>
  bool
  IndexList<T, K, key>::remove(K const& k)
  {
    return this->_contents.erase(k) != this->_contents.end();
  }

  template <typename T, typename K, K (T::*key)>
  template <typename C>
  void
  IndexList<T, K, key>::reset(C const& c)
  {
    this->_contents.clear();
    for (auto const& e: c)
      this->add(e);
  }

  template <typename T, typename K, K (T::*key)>
  T&
  IndexList<T, K, key>::get(K const& k)
  {
    return this->_contents.at(k);
  }

  template <typename T, typename K, K (T::*key)>
  T const&
  IndexList<T, K, key>::get(K const& k) const
  {
    return const_cast<Self*>(this)->get(k);
  }

  /*----------.
  | Container |
  `----------*/

  template <typename T, typename K, K (T::*key)>
  typename IndexList<T, K, key>::iterator
  IndexList<T, K, key>::begin()
  {
    return iter_values(this->_contents);
  }

  template <typename T, typename K, K (T::*key)>
  typename IndexList<T, K, key>::iterator
  IndexList<T, K, key>::end()
  {
    return this->_contents.end();
  }

  template <typename T, typename K, K (T::*key)>
  typename IndexList<T, K, key>::const_iterator
  IndexList<T, K, key>::begin() const
  {
    return iter_values(this->_contents);
  }

  template <typename T, typename K, K (T::*key)>
  typename IndexList<T, K, key>::const_iterator
  IndexList<T, K, key>::end() const
  {
    return this->_contents.end();
  }

  template <typename T, typename K, K (T::*key)>
  typename IndexList<T, K, key>::size_type
  IndexList<T, K, key>::size() const
  {
    return this->_contents.size();
  }
}

#endif
