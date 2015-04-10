#ifndef DAS_COLLECTION_HH
# define DAS_COLLECTION_HH

# include <elle/attribute.hh>

# include <das/Model.hh>
# include <das/Object.hh>

namespace das
{
  template <typename T, typename K, K (T::*key), typename Model>
  class Collection
  {
  public:
    class Update
      : public std::vector<typename Model::Update>
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
          for (auto& elt: collection)
            if (elt.*key == k.get())
            {
              update.apply(elt);
              return;
            }
          // FIXME: do better
          collection.emplace_back();
          update.apply(collection.back());
        }
      }
    };
  };
}

#endif
