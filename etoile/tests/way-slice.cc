//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        Alexandre Bique   [wed oct  5 13:03:06 2011]
//

#include <etoile/Etoile.hh>

int main(int argc, char ** argv)
{
#define TEST(Path)                                                      \
  do {                                                                  \
    etoile::path::Slab		name;                                   \
    etoile::path::Way		way_full(Path);                         \
    etoile::path::Way		way(etoile::path::Way(Path), name);     \
                                                                        \
    std::cout << "way full: " << way_full.path                          \
              << " - way: " << way.path << " - name: "                  \
              << name << std::endl;                                     \
  } while (0)

  TEST("/");
  TEST("tutu");
  TEST("/tutu");
  TEST("/tutu/");
  TEST("/tutu/tata");
  TEST("/tutu/tata/");

  return 0;
}
