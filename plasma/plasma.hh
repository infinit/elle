#ifndef PLASMA_PLASMA_HH
# define PLASMA_PLASMA_HH

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
    std::string           first_filename;
    int                   files_count;
    int                   total_size;
    int                   is_directory;
    int                   status;
  };
}

# include "plasma.hxx"

#endif
