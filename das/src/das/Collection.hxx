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
    auto inserted = this->_contents.emplace(t.*key, std::move(t));
    if (!inserted.second)
      throw elle::Error(elle::sprintf("duplicate key: %s", t.*key));
    this->_added(inserted.first->second);
    this->_changed();
  }

  template <typename T, typename K, K (T::*key)>
  void
  IndexList<T, K, key>::remove(K const& k)
  {
    if (this->_contents.erase(k) > 0)
    {
      this->_removed(k);
      this->_changed();
    }
    else
      throw elle::Error(elle::sprintf("mising key: %s", k));
  }

  template <typename T, typename K, K (T::*key)>
  typename IndexList<T, K, key>::iterator
  IndexList<T, K, key>::erase(iterator position)
  {
    return IndexList<T, K, key>::iterator(this->_contents.erase(position));
  }

  template <typename T, typename K, K (T::*key)>
  template <class... Args>
  std::pair<typename IndexList<T, K, key>::iterator, bool>
  IndexList<T, K, key>::emplace( Args&&... args )
  {
    T element(std::forward<Args>(args)...);
    auto res = this->_contents.emplace(element.*key, std::move(element));
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
        this->add(e);
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
