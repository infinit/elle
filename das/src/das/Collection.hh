#ifndef DAS_COLLECTION_HH
# define DAS_COLLECTION_HH

# include <boost/signals2.hpp>

# include <elle/attribute.hh>

# include <das/model.hh>
# include <das/Object.hh>

namespace das
{
  template <typename T, typename K, K (T::*key)>
  class IndexList
  {
  /*------.
  | Types |
  `------*/
  public:
    typedef IndexList<T, K, key> Self;
    typedef T value_type;
    typedef T& reference;
    typedef T const& const_reference;
    typedef std::values_iterator<K, T> iterator;
    typedef std::const_values_iterator<K, T> const_iterator;
    typedef std::unordered_map<K, T> Contents;
    typedef typename Contents::difference_type difference_type;
    typedef typename Contents::size_type size_type;

  /*-------------.
  | Construction |
  `-------------*/
  public:
    IndexList();

  /*----------.
  | Observers |
  `----------*/
  public:
    iterator
    begin();
    iterator
    end();
    iterator
    find(K const& k);
    const_iterator
    begin() const;
    const_iterator
    end() const;
    const_iterator
    find(K const& k) const;
    size_type
    size() const;
    bool
    empty() const;

  /*----------.
  | Modifiers |
  `----------*/
  public:
    void
    add(T t);
    void
    remove(K const& k);
    template <typename C>
    void
    reset(C const& collection);
    T&
    get(K const& k);
    T const&
    get(K const& k) const;
    iterator
    erase(iterator position);
    template <class... Args>
    std::pair<iterator, bool>
    emplace(Args&&... args);
    template <class... Args>
    std::pair<iterator, bool>
    _emplace(Args&&... args);

  /*------.
  | Hooks |
  `------*/
  public:
    /// Trigger whenever an individual element is added.
    ELLE_ATTRIBUTE_RX(boost::signals2::signal<void (T&)>, added);
    /// Trigger whenever an individual element is removed.
    ELLE_ATTRIBUTE_RX(boost::signals2::signal<void (K const&)>, removed);
    /// Trigger whenever the whole lists is reset.
    ELLE_ATTRIBUTE_RX(boost::signals2::signal<void ()>, reset);
    /// Trigger whenever the list is changed in any way
    ELLE_ATTRIBUTE_RX(boost::signals2::signal<void ()>, changed);

  /*--------.
  | Details |
  `--------*/
  private:
    Contents _contents;
  };


  template <typename C, typename ElementUpdate, typename T>
  struct Inserter
  {
    static
    void
    insert(C& c, ElementUpdate const& u)
    {
      // FIXME: do better
      T element;
      u.apply(element);
      c.emplace(std::move(element));
    }
  };

  template <typename ElementUpdate, typename T>
  struct Inserter<std::vector<T>, ElementUpdate, T>
  {
    static
      void
      insert(std::vector<T>& c, ElementUpdate const& u)
    {
      // FIXME: do better
      T element;
      u.apply(element);
      c.emplace_back(std::move(element));
    }
  };

  template <typename T, typename K, K (T::*key), typename Model>
  class Collection
  {
  public:
    class ElementUpdate
      : public Model::Update
    {
    public:
      ElementUpdate() = default;

      ElementUpdate(typename Model::Update&& update)
        : Model::Update(std::move(update))
      {}

      void
      serialize(elle::serialization::Serializer& s)
      {
        this->Model::Update::serialize(s);
        s.serialize("$remove", this->remove);
      }

      boost::optional<bool> remove;
    };

    class Update
      : public std::vector<ElementUpdate>
    {
    public:
      typedef std::vector<ElementUpdate> Super;

      template <typename C>
      void
      apply(C& collection) const
      {
        for (auto& update: *this)
        {
          typedef das::Field<T, K, key, void> KeyField;
          auto& k = update.*KeyField::template Member<UpdateMember>::member;
          if (!k)
            throw elle::Error(
              elle::sprintf("key \"%s\" missing from a collection of %s update",
                            /* KeyField::name */ "FIXME",
                            elle::demangle(typeid(T).name())));
          auto it = std::find_if(
            collection.begin(),
            collection.end(),
            [&](T const& elt) { return elt.*key == k.get(); });
          if (it != collection.end())
            if (update.remove)
              collection.erase(it);
            else
              update.apply(*it);
          else
            Inserter<C, ElementUpdate, T>::insert(collection, update);
        }
      }

      void
      serialize(elle::serialization::Serializer& s)
      {
        s.serialize_forward(static_cast<Super&>(*this));
      }
    };
  };
}

# include <das/Collection.hxx>

#endif
