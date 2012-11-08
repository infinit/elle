#ifndef ELLE_FINALLY_HH
# define ELLE_FINALLY_HH

# include <boost/preprocessor/cat.hpp>

# define ELLE_FINALLY(Lambda)                                   \
  auto BOOST_PP_CAT(_Finally_action_, __LINE__) = Lambda;       \
  class BOOST_PP_CAT(_Finally_, __LINE__)                       \
  {                                                             \
    public:                                                     \
    inline                                                      \
      BOOST_PP_CAT(_Finally_, __LINE__)                         \
      (decltype(BOOST_PP_CAT(_Finally_action_, __LINE__))       \
       const& action)                                           \
      : _action(action)                                         \
    {}                                                          \
    inline                                                      \
      ~BOOST_PP_CAT(_Finally_, __LINE__)()                      \
    {                                                           \
      this->_action();                                          \
    }                                                           \
    private:                                                    \
    decltype(BOOST_PP_CAT(_Finally_action_, __LINE__))          \
      const& _action;                                           \
  } BOOST_PP_CAT(_finally_, __LINE__)                           \
    (BOOST_PP_CAT(_Finally_action_, __LINE__));                 \

#endif
