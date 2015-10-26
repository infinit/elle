#ifndef CRYPTOGRAPHY_HH
# define CRYPTOGRAPHY_HH

# include <elle/types.hh>
# include <elle/test.hh>
# include <elle/serialization/json.hh>

# include <functional>

namespace infinit
{
  namespace cryptography
  {
    namespace test
    {
      /// Check that every one of the given archives can be upgraded while
      /// remaining consistent and valid.
      ///
      /// Note that the given archives are assumed to be base64-string
      /// representations.
      template <typename T>
      void
      formats(std::vector<std::string> const& archives,
              std::function<void (T const&)> operate = nullptr)
      {
        std::vector<std::unique_ptr<T>> objects(archives.size());

        // First, deserialize all the objects from the archives,
        // call the operate function and finally check thatt
        // reserializing the object generates the same archive.
        for (uint32_t i = 0; i < archives.size(); ++i)
        {
          std::stringstream stream1(archives[i]);
          typename elle::serialization::json::SerializerIn input1(stream1);
          objects[i].reset(new T(input1));

          if (operate != nullptr)
            operate(*objects[i]);

          std::stringstream stream2;
          {
            typename elle::serialization::json::SerializerOut output2(stream2);
            objects[i]->serialize(output2);
          }

          typename elle::serialization::json::SerializerIn input3(stream2);
          T _o(input3);

          BOOST_CHECK_EQUAL(*objects[i], _o);
        }
      }
    }
  }
}

#endif
