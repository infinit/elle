#include <cassert>

#include <elle/print.hh>

#include <surface/gap/gap.h>

int main()
{
  gap_state_t* state = gap_new("pif");
  if (gap_login(state, "monemail@caramail.com", "BIET") != 0)
    assert(false && "Should be able to log in");
  if (gap_register(state, "my name", "monemail@caramail.com", "BIET") != 0)
    {
      if (gap_login(state, "monemail@caramail.com", "BIET") != 0)
        assert(false && "Should be able to log in");
    }
  elle::print("tests done.");
  return 0;
}

