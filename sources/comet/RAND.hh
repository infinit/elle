#ifndef COMET_RAND_HH
# define COMET_RAND_HH

namespace comet
{

  void RAND_cleanup(void);
  void RAND_add(const void *buf, int num, double add);
  void RAND_seed(const void *buf, int num);
  int RAND_bytes(unsigned char *buf, int num);
  int RAND_pseudorand(unsigned char *buf, int num);
  int RAND_status(void);

}

#endif
