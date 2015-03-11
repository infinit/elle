#ifndef DAS_COLLECTION_HH
# define DAS_COLLECTION_HH

# include <elle/attribute.hh>

# include <das/Model.hh>

namespace das
{
  template <typename T, typename K, K (T::*key)>
  class Collection
    : public das::Model
  {
  public:
    ELLE_ATTRIBUTE_R(std::vector<std::shared_ptr<T>>, elements);
  };
}

#endif
