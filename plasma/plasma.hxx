#ifndef PLASMA_PLASMA_HXX
# define PLASMA_PLASMA_HXX

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(plasma::Transaction, ar, res, version)
{
  enforce(version == 0);

  ar & named("transaction_id", res.transaction_id);
  ar & named("sender_id", res.sender_id);
  ar & named("sender_fullname", res.sender_fullname);
  ar & named("sender_device_id", res.sender_device_id);
  ar & named("recipient_id", res.recipient_id);
  ar & named("recipient_fullname", res.recipient_fullname);
  ar & named("recipient_device_id", res.recipient_device_id);
  ar & named("recipient_device_name", res.recipient_device_id);
  ar & named("network_id", res.network_id);
  ar & named("first_filename", res.first_filename);
  ar & named("files_count", res.files_count);
  ar & named("total_size", res.total_size);
  ar & named("is_directory", res.is_directory);
  ar & named("status", res.status);
}

#endif
