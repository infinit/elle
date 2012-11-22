#ifndef NUCLEUS_NEUTRON_ACCESS_HXX
# define NUCLEUS_NEUTRON_ACCESS_HXX

//
// ---------- serialize -------------------------------------------------------
//

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SPLIT(nucleus::neutron::Access);

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::neutron::Access,
                          archive,
                          value,
                          version)
{
  nucleus::neutron::Size size;
  nucleus::neutron::Size i;

  enforce(version == 0);

  archive >> base_class<nucleus::proton::ContentHashBlock>(value);

  archive >> size;

  for (i = 0; i < size; ++i)
    {
      std::shared_ptr<nucleus::neutron::Record> record{
        archive.template Construct<nucleus::neutron::Record>().release()};

      if (value._container.find(record->subject()) != value._container.end())
        throw nucleus::Exception("the record's subject '%s' seems to already "
                                 "exist", record->subject());

      auto result =
        value._container.insert(
          std::pair<nucleus::neutron::Subject const,
                    std::shared_ptr<nucleus::neutron::Record>>{
            record->subject(), record});

      if (result.second == false)
        throw Exception("unable to insert the record the container");
    }
}

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::neutron::Access,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  archive << base_class<nucleus::proton::ContentHashBlock const>(value);

  archive << static_cast<nucleus::neutron::Size>(value._container.size());

  for (auto& pair: value._container)
    {
      auto& record = pair.second;

      archive << *record;
    }
}

#endif
