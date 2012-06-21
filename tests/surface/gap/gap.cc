
#include <elle/print.hh>

#include <surface/gap/gap.h>

int main()
{
  elle::print("tests done.");
  gap_state_t* state = gap_new();
  gap_login(state, "toto", "titi");
}

