#ifndef PLASMA_PLASMA_HXX
# define PLASMA_PLASMA_HXX

# include <elle/serialize/Serializer.hh>
# include <elle/serialize/NamedValue.hh>

ELLE_SERIALIZE_NO_FORMAT(plasma::Transaction);

ELLE_SERIALIZE_SIMPLE(plasma::Transaction, ar, res, version)
{
  enforce(version == 0);

  ar & elle::serialize::named("transaction_id", res.transaction_id);
  ar & elle::serialize::named("sender_id", res.sender_id);
  ar & elle::serialize::named("sender_fullname", res.sender_fullname);
  ar & elle::serialize::named("sender_device_id", res.sender_device_id);
  ar & elle::serialize::named("recipient_id", res.recipient_id);
  ar & elle::serialize::named("recipient_fullname", res.recipient_fullname);
  ar & elle::serialize::named("recipient_device_id", res.recipient_device_id);
  ar & elle::serialize::named("recipient_device_name", res.recipient_device_id);
  ar & elle::serialize::named("network_id", res.network_id);
  ar & elle::serialize::named("message", res.message);
  ar & elle::serialize::named("first_filename", res.first_filename);
  ar & elle::serialize::named("files_count", res.files_count);
  ar & elle::serialize::named("total_size", res.total_size);
  ar & elle::serialize::named("is_directory", res.is_directory);
  ar & elle::serialize::named("status", res.status);
}

#endif
