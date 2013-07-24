#ifndef CRYPTOGRAPHY_HH
# define CRYPTOGRAPHY_HH

# define BOOST_TEST_DYN_LINK
# include <boost/test/unit_test.hpp>

# include <elle/types.hh>
# include <elle/serialize/insert.hh>
# include <elle/serialize/extract.hh>
# include <elle/serialize/Base64Archive.hh>
# include <elle/serialize/JSONArchive.hh>

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

        // Then, for each object, try to upgrade it sequentially until
        // reaching the last possible format while testing every version.
        for (elle::Natural32 i = 0; i < objects.size(); ++i)
        {
          // XXX vector (initially empty) for holding the upgraded version of
          //     object[i].
          // XXX upgraded[0] = objects[i]

          for (elle::Natural32 j = i + 1; j < objects.size(); ++j)
          {
            // XXX upgrade upgraded[j] to upgraded[j + 1]
            // XXX check que chaque version upgraded equals the others of
            //     the same object.
          }
        }
      }
    }
  }
}

#endif
