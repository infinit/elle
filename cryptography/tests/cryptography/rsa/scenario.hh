#if defined(INFINIT_CRYPTOGRAPHY_ROTATION)

# ifndef SCENARIO_HH
#  define SCENARIO_HH

#  include "../cryptography.hh"

#  include <cryptography/SecretKey.hh>
#  include <cryptography/Exception.hh>
#  include <cryptography/random.hh>
#  include <cryptography/rsa/Seed.hh>
#  include <cryptography/rsa/KeyPair.hh>

//
// ---------- Entry -----------------------------------------------------------
//

// Represent an entry in an access control list that references a group.
//
// The group is referenced in a specific version (both _version_ and _pass_K_)
// that will be required to derive past passes (_pass_k_).
class Entry
{
public:
  Entry(elle::Natural32 version,
        infinit::cryptography::rsa::PublicKey const& pass_K,
        infinit::cryptography::Code const& key):
    _version(version),
    _pass_K(pass_K),
    _key(key)
  {}

  Entry(Entry const& other):
    _version(other._version),
    _pass_K(other._pass_K),
    _key(other._key)
  {}

private:
  ELLE_ATTRIBUTE_R(elle::Natural32 const, version);
  ELLE_ATTRIBUTE_R(infinit::cryptography::rsa::PublicKey const, pass_K);
  ELLE_ATTRIBUTE_R(infinit::cryptography::Code const, key);
};

//
// ---------- ACL -------------------------------------------------------------
//

// Represent an access control list i.e a list of entries referencing groups.
class ACL
{
public:
  ACL()
  {}

  ACL(ACL const& other)
  {
    for (auto const& iterator: other._entries)
      this->_entries[iterator.first] = new Entry(*iterator.second);
  }

public:
  void
  grant(infinit::cryptography::rsa::PublicKey const& address,
        elle::Natural32 version,
        infinit::cryptography::rsa::PublicKey const& pass_K,
        infinit::cryptography::SecretKey const& key)
  {
    this->_entries[address] = new Entry(version, pass_K, pass_K.encrypt(key));
  }

  void
  revoke(infinit::cryptography::rsa::PublicKey const& address)
  {
    if (this->_entries.erase(address) != 1)
      throw infinit::cryptography::Exception(
        elle::sprintf("unable to remove %s from the entries", address));
  }

  Entry*
  retrieve(infinit::cryptography::rsa::PublicKey const& address)
  {
    auto iterator = this->_entries.find(address);
    if (iterator == this->_entries.end())
      throw infinit::cryptography::Exception(
        elle::sprintf("unable to find the entry for %s", address));

    Entry* entry = iterator->second;

    return (entry);
  }

  infinit::cryptography::SecretKey
  key(infinit::cryptography::rsa::PublicKey const& address,
      infinit::cryptography::rsa::PrivateKey const& pass_k)
  {
    Entry* entry = this->retrieve(address);

    pass_k.decrypt<infinit::cryptography::SecretKey>(entry->key());

    return (pass_k.decrypt<infinit::cryptography::SecretKey>(entry->key()));
  }

  void
  update(infinit::cryptography::SecretKey const& key,
         infinit::cryptography::rsa::PublicKey const& address,
         elle::Natural32 const version,
         infinit::cryptography::rsa::PublicKey const& pass_K)
  {
    Entry* _entry = this->retrieve(address);

    this->_entries[address] = new Entry(version,
                                        pass_K,
                                        pass_K.encrypt(key));

    delete _entry;
  }

  elle::Natural32
  size() const
  {
    return (this->_entries.size());
  }

  void
  upgrade(infinit::cryptography::SecretKey const& key)
  {
    for (auto iterator = this->_entries.begin();
         iterator != this->_entries.end();
         ++iterator)
    {
      Entry*& entry = iterator->second;
      Entry* _entry = entry;

      entry =
        new Entry(_entry->version(),
                  _entry->pass_K(),
                  _entry->pass_K().encrypt(key));

      delete _entry;
    }
  }

private:
  std::unordered_map<infinit::cryptography::rsa::PublicKey, Entry*> _entries;
};

//
// ---------- Object ----------------------------------------------------------
//

// Represent something we would like to control the access on in a decentralized
// way.
class Object
{
public:
  Object(infinit::cryptography::rsa::KeyPair const& owner_keypair,
         elle::String const& content):
    Object(owner_keypair.K(),
           content)
  {}

private:
  Object(infinit::cryptography::rsa::PublicKey const& owner_K,
         elle::String const& content):
    Object(owner_K,
           infinit::cryptography::secretkey::generate(
             infinit::cryptography::Cipher::aes256,
             256),
           content)
  {}

  Object(infinit::cryptography::rsa::PublicKey const& owner_K,
         infinit::cryptography::SecretKey const& key,
         elle::String const& content):
    _owner_K(owner_K),
    _key(owner_K.encrypt(key)),
    _content(key.encrypt(content))
  {}

  Object(Object const& object,
         infinit::cryptography::SecretKey const& key,
         elle::String const& content):
    _owner_K(object._owner_K),
    _key(object._owner_K.encrypt(key)),
    _content(key.encrypt(content)),
    _acl(object._acl)
  {}

public:
  Object
  write(infinit::cryptography::rsa::PrivateKey const& owner_k,
        elle::String const& content)
  {
    infinit::cryptography::SecretKey _key = this->key(owner_k);
    infinit::cryptography::SecretKey key =
      infinit::cryptography::secretkey::generate(_key.cipher(),
                                                 _key.length());

    Object object(*this, key, content);

    object.acl().upgrade(key);

    return (object);
  }

  elle::String
  read(infinit::cryptography::SecretKey const& key) const
  {
    return (key.decrypt<elle::String>(this->_content));
  }

  infinit::cryptography::SecretKey
  key(infinit::cryptography::rsa::PrivateKey const& owner_k) const
  {
    return (owner_k.decrypt<infinit::cryptography::SecretKey>(this->_key));
  }

private:
  ELLE_ATTRIBUTE_R(infinit::cryptography::rsa::PublicKey, owner_K);
  ELLE_ATTRIBUTE(infinit::cryptography::Code, key);
  ELLE_ATTRIBUTE(infinit::cryptography::Code, content);
  ELLE_ATTRIBUTE_RX(ACL, acl);
};

//
// ---------- Member ----------------------------------------------------------
//

// Represent a group member. Every member is given the _seed_ and _pass_k_
// but those are obviously encrypted with the member's public key to keep
// them private to the member alone.
class Member
{
public:
  Member(infinit::cryptography::Code const& pass_k,
         infinit::cryptography::Code const& seed):
    _pass_k(pass_k),
    _seed(seed)
  {}

  Member(Member const& other):
    _pass_k(other._pass_k),
    _seed(other._seed)
  {}

private:
  ELLE_ATTRIBUTE_R(infinit::cryptography::Code const, pass_k);
  ELLE_ATTRIBUTE_R(infinit::cryptography::Code const, seed);
};

//
// ---------- Members ---------------------------------------------------------
//

// Represent a list of members in a group.
class Members
{
public:
  void
  add(infinit::cryptography::rsa::PublicKey const& user_K,
      infinit::cryptography::rsa::PrivateKey const& pass_k,
      infinit::cryptography::rsa::Seed const& seed)
  {
    this->_members[user_K] = new Member(user_K.encrypt(pass_k),
                                        user_K.encrypt(seed));
  }

  void
  remove(infinit::cryptography::rsa::PublicKey const& user_K)
  {
    if (this->_members.erase(user_K) != 1)
      throw infinit::cryptography::Exception(
        elle::sprintf("unable to remove %s from the members", user_K));
  }

  Member*
  retrieve(infinit::cryptography::rsa::PublicKey const& user_K)
  {
    auto iterator = this->_members.find(user_K);
    if (iterator == this->_members.end())
      throw infinit::cryptography::Exception(
        elle::sprintf("unable to find the member for %s", user_K));

    Member* member = iterator->second;

    return (member);
  }

  infinit::cryptography::rsa::PrivateKey
  pass_k(infinit::cryptography::rsa::KeyPair const& user_keypair)
  {
    Member* member = this->retrieve(user_keypair.K());

    return (
      user_keypair.k().decrypt<infinit::cryptography::rsa::PrivateKey>(
        member->pass_k()));
  }

  infinit::cryptography::rsa::Seed
  seed(infinit::cryptography::rsa::KeyPair const& user_keypair)
  {
    Member* member = this->retrieve(user_keypair.K());

    return (
      user_keypair.k().decrypt<infinit::cryptography::rsa::Seed>(
        member->seed()));
  }

  void
  upgrade(infinit::cryptography::rsa::PrivateKey const& pass_k,
          infinit::cryptography::rsa::Seed const& seed)
  {
    for (auto iterator = this->_members.begin();
         iterator != this->_members.end();
         ++iterator)
    {
      infinit::cryptography::rsa::PublicKey user_K = iterator->first;
      Member*& member = iterator->second;
      Member* _member = member;

      member =
        new Member(user_K.encrypt(pass_k),
                   user_K.encrypt(seed));

      delete _member;
    }
  }

  elle::Natural32
  size() const
  {
    return (this->_members.size());
  }

private:
  std::unordered_map<infinit::cryptography::rsa::PublicKey, Member*> _members;
};

//
// ---------- Group -----------------------------------------------------------
//

// Represent a group which one would like to manipulate as a single entity
// to grant/revoke access to an object.
class Group
{
public:
  Group(infinit::cryptography::rsa::PublicKey const& manager_K):
    Group(infinit::cryptography::rsa::keypair::generate(2048),
          manager_K)
  {}

private:
  Group(infinit::cryptography::rsa::KeyPair const& keypair,
        infinit::cryptography::rsa::PublicKey const& manager_K):
    Group(keypair.K(),
          0,
          manager_K,
          infinit::cryptography::rsa::seed::generate(keypair),
          Members{})
  {}

  Group(infinit::cryptography::rsa::PublicKey const& address,
        elle::Natural32 const version,
        infinit::cryptography::rsa::PublicKey const& manager_K,
        infinit::cryptography::rsa::Seed const& seed,
        Members const& members):
    Group(address,
          version,
          manager_K,
          seed,
          infinit::cryptography::rsa::KeyPair(seed),
          members)
  {}

  Group(infinit::cryptography::rsa::PublicKey const& address,
        elle::Natural32 const version,
        infinit::cryptography::rsa::PublicKey const& manager_K,
        infinit::cryptography::rsa::Seed const& seed,
        infinit::cryptography::rsa::KeyPair const& pass,
        Members const& members):
    _address(address),
    _version(version),
    _manager_K(manager_K),
    _seed(manager_K.encrypt(seed)),
    _pass_K(pass.K()),
    _pass_k(manager_K.encrypt(pass.k())),
    _members(members)
  {}

private:
  Group(Group const& group,
        infinit::cryptography::rsa::Seed const& seed):
    Group(group._address,
          group._version + 1,
          group._manager_K,
          seed,
          group._members)
  {}

public:
  infinit::cryptography::rsa::PrivateKey
  pass_k(infinit::cryptography::rsa::PrivateKey const& manager_k) const
  {
    return (
      manager_k.decrypt<infinit::cryptography::rsa::PrivateKey>(this->_pass_k));
  }

  infinit::cryptography::rsa::Seed
  seed(infinit::cryptography::rsa::PrivateKey const& manager_k) const
  {
    return (
      manager_k.decrypt<infinit::cryptography::rsa::Seed>(this->_seed));
  }

  Group
  rotate(infinit::cryptography::rsa::PrivateKey const& manager_k)
  {
    infinit::cryptography::rsa::Seed seed =
      manager_k.decrypt<infinit::cryptography::rsa::Seed>(this->_seed);
    infinit::cryptography::rsa::Seed _seed = manager_k.rotate(seed);

    Group group(*this, _seed);

    infinit::cryptography::rsa::PrivateKey _pass_k = group.pass_k(manager_k);

    group.members().upgrade(_pass_k, _seed);

    return (group);
  }

private:
  ELLE_ATTRIBUTE_R(infinit::cryptography::rsa::PublicKey const, address);
  ELLE_ATTRIBUTE_R(elle::Natural32, version);
  ELLE_ATTRIBUTE_R(infinit::cryptography::rsa::PublicKey const, manager_K);
  ELLE_ATTRIBUTE(infinit::cryptography::Code, seed);
  ELLE_ATTRIBUTE_R(infinit::cryptography::rsa::PublicKey, pass_K);
  ELLE_ATTRIBUTE(infinit::cryptography::Code, pass_k);
  ELLE_ATTRIBUTE_RX(Members, members);
};

# endif

#endif
