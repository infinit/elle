#ifndef TEST_UNIT_UNIT_HH
# define TEST_UNIT_UNIT_HH

# define TEST_SERIALIZE_FINALIZE(_namespace_, _type_)                    \
  namespace test                                                        \
  {                                                                     \
    class _type_:                                                       \
      public _namespace_::_type_,                                       \
      public elle::serialize::SerializableMixin<_type_>                 \
    {                                                                   \
    public:                                                             \
      template <typename... T>                                          \
      _type_(T&&... arguments):                                         \
        _namespace_::_type_(std::forward<T>(arguments)...)              \
      {                                                                 \
      }                                                                 \
    };                                                                  \
  }                                                                     \
                                                                        \
  ELLE_SERIALIZE_SIMPLE(test::_type_,                                   \
                        archive,                                        \
                        value,                                          \
                        version)                                        \
  {                                                                     \
    enforce(version == 0);                                              \
                                                                        \
    archive & base_class<_namespace_::_type_>(value);                   \
  }

#endif
