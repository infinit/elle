
#include <sstream>
#include <iostream>

#include "elle/serialize/Base64Archive.hh"

namespace se = elle::serialize;

int main()
{
  std::stringstream ss;

    {
      se::OutputBase64Archive ar(ss);

      ar & 1;
      ar & 2;
      ar & 3;

      // stream fully flushed when the archive is destroyed
    }
  assert(ss.str().size() == 16);
  assert(ss.str()[15] != '=');

  std::cout << "tests done.\n";
  return 0;
}



