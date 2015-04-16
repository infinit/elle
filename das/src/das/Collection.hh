#ifndef DAS_COLLECTION_HH
# define DAS_COLLECTION_HH

# include <elle/attribute.hh>

# include <das/Model.hh>
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

  /*-------------.
  | Construction |
  `-------------*/
  public:
    IndexList();

  /*----------.
  | Modifiers |
  `----------*/
  public:
    void
    add(T t);
    bool
    remove(K const& k);
    template <typename C>
    void
    reset(C const& collection);
    T&
    get(K const& k);
    T const&
    get(K const& k) const;

  /*--------.
  | Details |
  `--------*/
  private:
    typedef std::unordered_map<K, T> Contents;
    Contents _contents;

  /*----------.
  | Container |
  `----------*/
  public:
    typedef T value_type;
    typedef T& reference;
    typedef T const& const_reference;
    typedef std::values_iterator<K, T> iterator;
    typedef std::const_values_iterator<K, T> const_iterator;
    typedef typename Contents::difference_type difference_type;
    typedef typename Contents::size_type size_type;
    iterator
    begin();
    iterator
    end();
    const_iterator
    begin() const;
    const_iterator
    end() const;
    size_type
    size() const;
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
        this->serialize(this->remove, "$remove");
      }

      boost::optional<bool> remove;
    };

    class Update
      : public std::vector<ElementUpdate>
    {
    public:
      template <typename C>
      void
      apply(C& collection)
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
          {
            // FIXME: do better
            collection.emplace_back();
            update.apply(collection.back());
          }
        }
      }
    };
  };
}

# include <das/Collection.hxx>

#endif
