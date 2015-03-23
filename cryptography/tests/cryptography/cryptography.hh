#ifndef CRYPTOGRAPHY_HH
# define CRYPTOGRAPHY_HH

# include <elle/types.hh>
# include <elle/serialize/insert.hh>
# include <elle/serialize/extract.hh>
# include <elle/serialize/Base64Archive.hh>
# include <elle/serialize/JSONArchive.hh>
# include <elle/test.hh>

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
      formats(std::vector<elle::String> const& archives,
              std::function<void (T const&)> operate = nullptr)
      {
        std::vector<std::unique_ptr<T>> objects(archives.size());

        // First, deserialize all the objects from the archives,
        // call the operate function and finally check thatt
        // reserializing the object generates the same archive.
        for (elle::Natural32 i = 0; i < archives.size(); ++i)
        {
          auto extractor =
            elle::serialize::from_string<
              elle::serialize::InputBase64Archive>(archives[i]);
          objects[i].reset(new T(extractor));

          if (operate != nullptr)
            operate(*objects[i]);

          elle::String archive;
            elle::serialize::to_string<
              elle::serialize::OutputBase64Archive>(archive) << *objects[i];

          BOOST_CHECK_EQUAL(archives[i], archive);
        }
      }
    }
  }
}

#endif
