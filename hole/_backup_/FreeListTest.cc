#include <stdio.h>

#include "hole/FreeList.hh"

class Chiche : public hole::FreeList<Chiche>
{
public:
  Chiche()
    : hole::FreeList<Chiche>()
  {
    puts(__PRETTY_FUNCTION__);
  }

  Chiche(const Chiche &)
    : hole::FreeList<Chiche>()
  {
    puts(__PRETTY_FUNCTION__);
  }

  ~Chiche()
  {
    puts(__PRETTY_FUNCTION__);
  }
};

int main()
{
  Chiche * c1 = new Chiche;
  delete c1;
  c1 = new Chiche;
  Chiche * c2 = new Chiche(*c1);
  delete c1;
  delete c2;
  return 0;
}
