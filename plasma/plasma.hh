#ifndef PLASMA_PLASMA_HH
# define PLASMA_PLASMA_HH

# include <iosfwd>
# include <string>

namespace plasma
{

  struct Transaction
  {
    std::string           transaction_id;
    std::string           sender_id;
    std::string           sender_fullname;
    std::string           sender_device_id;
    std::string           recipient_id;
    std::string           recipient_fullname;
    std::string           recipient_device_id;
    std::string           recipient_device_name;
    std::string           network_id;
    std::string           message;
    std::string           first_filename;
    int                   files_count;
    int                   total_size;
    int                   is_directory;
    int                   status;
  };

  enum class TransactionStatus: int
  {
# define TRANSACTION_STATUS(name, value)                                       \
    name = value,
# include <oracle/disciples/meta/resources/transaction_status.hh.inc>
# undef TRANSACTION_STATUS
  };

  std::ostream&
  operator <<(std::ostream& out,
              TransactionStatus t);
}

# include "plasma.hxx"

#endif
