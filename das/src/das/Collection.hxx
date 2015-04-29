#ifndef DAS_COLLECTION_HXX
# define DAS_COLLECTION_HXX

namespace das
{
  template <typename T, typename K, K (T::*key)>
  IndexList<T, K, key>::IndexList()
    : _contents()
  {}

  /*----------.
  | Modifiers |
  `----------*/

  template <typename T, typename K, K (T::*key)>
  void
  IndexList<T, K, key>::add(T t)
  {
    this->emplace(std::move(t));
  }

  template <typename T, typename K, K (T::*key)>
  void
  IndexList<T, K, key>::remove(K const& k)
  {
    auto it = this->find(k);
    if (it == this->end())
      throw elle::Error(elle::sprintf("mising key: %s", k));
    this->erase(it);
  }

  template <typename T, typename K, K (T::*key)>
  typename IndexList<T, K, key>::iterator
  IndexList<T, K, key>::erase(iterator position)
  {
    K k = position->first;
    auto it = IndexList<T, K, key>::iterator(this->_contents.erase(position));
    this->_removed(k);
    this->_changed();
    return it;
  }

  template <typename T, typename K, K (T::*key)>
  template <class... Args>
  std::pair<typename IndexList<T, K, key>::iterator, bool>
  IndexList<T, K, key>::emplace(Args&&... args)
  {
    auto res = this->_emplace(std::forward<Args&&>(args)...);
    this->_added(res.first->second);
    this->_changed();
    return std::move(res);
  }

  template <typename T, typename K, K (T::*key)>
  template <class... Args>
  std::pair<typename IndexList<T, K, key>::iterator, bool>
  IndexList<T, K, key>::_emplace(Args&&... args)
  {
    T element(std::forward<Args>(args)...);
    auto res = this->_contents.emplace(element.*key, std::move(element));
    if (!res.second)
      throw elle::Error(elle::sprintf("duplicate key: %s", element.*key));
    return std::make_pair(iterator(res.first), res.second);
  }

  template <typename T, typename K, K (T::*key)>
  template <typename C>
  void
  IndexList<T, K, key>::reset(C const& c)
  {
    if (!this->empty() || !c.empty())
    {
      this->_contents.clear();
      for (auto const& e: c)
        this->_emplace(e);
      this->_reset();
      this->_changed();
    }
  }

  template <typename T, typename K, K (T::*key)>
  T&
  IndexList<T, K, key>::get(K const& k)
  {
    try
    {
      return this->_contents.at(k);
    }
    catch (std::out_of_range const&)
    {
      throw elle::Error(elle::sprintf("missing key: %s", k));
    }

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
  typename IndexList<T, K, key>::iterator
  IndexList<T, K, key>::find(K const& k)
  {
    return this->_contents.find(k);
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
  typename IndexList<T, K, key>::const_iterator
  IndexList<T, K, key>::find(K const& k) const
  {
    return this->_contents.find(k);
  }

  template <typename T, typename K, K (T::*key)>
  typename IndexList<T, K, key>::size_type
  IndexList<T, K, key>::size() const
  {
    return this->_contents.size();
  }


  template <typename T, typename K, K (T::*key)>
  bool
  IndexList<T, K, key>::empty() const
  {
    return this->_contents.empty();
  }
}

#endif
