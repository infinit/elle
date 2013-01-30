#include "plasma.hh"

#include <iostream>

namespace plasma
{
  std::ostream&
  operator <<(std::ostream& out,
              plasma::TransactionStatus t)
  {
    switch (t)
    {
# define TRANSACTION_STATUS(name, value)        \
      case plasma::TransactionStatus::name:     \
        out << #name;                           \
        break;
# include <oracle/disciples/meta/resources/transaction_status.hh.inc>
# undef TRANSACTION_STATUS
    }

    return out;
  }
}
