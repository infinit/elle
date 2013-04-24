#ifndef DOPENSSL_H
# define DOPENSSL_H

# include <assert.h>
# define DOPENSSL_CHECK(_condition_)                                    \
  assert(_condition_);

#endif
