//
// ---------- header ----------------------------------------------------------
//
// project       comet
//
// license       infinit
//
// file          /home/mycure/infinit/comet/RAND.hh
//
// created       julien quintard   [tue mar 22 12:39:46 2011]
// updated       julien quintard   [wed mar 23 14:19:46 2011]
//

#ifndef COMET_RAND_HH
#define COMET_RAND_HH

namespace comet
{

//
// ---------- prototypes ------------------------------------------------------
//

  void RAND_cleanup(void);
  void RAND_add(const void *buf, int num, double add);
  void RAND_seed(const void *buf, int num);
  int RAND_bytes(unsigned char *buf, int num);
  int RAND_pseudorand(unsigned char *buf, int num);
  int RAND_status(void);

}

#endif
