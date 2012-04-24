#ifndef ELLE_TEST_CRYPTOGRAPHY_RANDOM_HH
# define ELLE_TEST_CRYPTOGRAPHY_RANDOM_HH

# include <sstream>

# include <elle/types.hh>
# include <elle/cryptography/KeyPair.hh>
# include <elle/standalone/Region.hh>
# include <elle/standalone/Report.hh>

# include <elle/idiom/Open.hh>

namespace elle
{
  using namespace elle::standalone;
  namespace test
  {

    ///
    /// this class provides methods for generating random numbers.
    ///
    class Random
    {
    public:
      //
      // methods
      //

      ///
      /// this method generates a random number.
      ///
      static Integer64  Generate(Integer64                      minimum,
                                 Integer64                      maximum)
      {
        Integer64       base = (Integer64)rand();

        if ((base >= minimum) && (base <= maximum))
          return (base);

        return (base % ((maximum - minimum) + 1) + minimum);
      }

      ///
      /// this method generates a random region.
      ///
      static Status     Generate(Region&                        region)
      {
        static const Natural32          MinimumRegionSize = 1;
        static const Natural32          MaximumRegionSize = 12345;

        Natural32       size = Random::Generate(MinimumRegionSize,
                                                MaximumRegionSize);
        Byte*           buffer;
        Natural32       i;

        // allocate a buffer.
        if ((buffer = (Byte*)::malloc(size)) == NULL)
          escape("unable to allocate memory");

        // randomize the buffer contents.
        for (i = 0; i < size; i++)
          *(buffer + i) =
            Random::Generate(std::numeric_limits<Character>::min(),
                             std::numeric_limits<Character>::max());

        // assign the buffer to the region.
        if (region.Acquire(buffer, size) == Status::Error)
          escape("unable to assign the buffer to the region");

        return Status::Ok;
      }
    };

  }
}

#endif
