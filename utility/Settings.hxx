//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sun jun 26 20:57:08 2011]
//

#ifndef ELLE_UTILITY_SETTINGS_HXX
#define ELLE_UTILITY_SETTINGS_HXX

namespace elle
{
  namespace utility
  {

//
// ---------- types -----------------------------------------------------------
//

    ///
    /// this base template allows for specialized-template.
    ///
    template <typename T>
    struct SettingsType
    {
      static const Byte         Value = Settings::TypeUnknown;
    };

    ///
    /// this macro-function link the type to the enum value in a simple
    /// call.
    ///
#define SettingsDeclare(_type_)                                         \
  template <>                                                           \
  struct SettingsType<_type_>                                           \
  {                                                                     \
    static const Byte           Value = Settings::Type ## _type_;       \
  };

    ///
    /// these macro-function calls actually generate the specialized-templates
    /// for every basic type of the elle library.
    ///
    SettingsDeclare(Null);
    SettingsDeclare(Boolean);
    SettingsDeclare(Character);
    SettingsDeclare(Real);
    SettingsDeclare(Integer8);
    SettingsDeclare(Integer16);
    SettingsDeclare(Integer32);
    SettingsDeclare(Integer64);
    SettingsDeclare(Natural8);
    SettingsDeclare(Natural16);
    SettingsDeclare(Natural32);
    SettingsDeclare(Natural64);
    SettingsDeclare(String);

//
// ---------- behaviours ------------------------------------------------------
//

    ///
    /// this method sets a basic type.
    ///
    template <typename T, Boolean C>
    Status      Settings::Behaviour<T, C>::Set(Settings&        settings,
                                               const String&    identifier,
                                               const String&    name,
                                               const T&         value)
    {
      String    string;

      ;

      // transform the value into a string-based format.
      if (Variable::Convert(value, string) == StatusFalse)
        escape("unable to convert the value for '%s' to the section '%s'",
               name.c_str(), identifier.c_str());

      // write the value.
      if (settings.Write(identifier, name, string) == StatusError)
        escape("unable to write the value '%s' for '%s' to the section '%s'",
               string.c_str(), name.c_str(), identifier.c_str());

      return elle::StatusOk;
    }

    ///
    /// this method gets a basic type.
    ///
    template <typename T, Boolean C>
    Status      Settings::Behaviour<T, C>::Get(Settings&        settings,
                                               const String&    identifier,
                                               const String&    name,
                                               T&               value)
    {
      String    string;

      ;

      // if the item does not exist, return the default value.
      if (settings.Find(identifier, name) == StatusFalse)
        escape("unable to locate the value for '%s' in section '%s'",
               name.c_str(), identifier.c_str());

      // otherwise, read the item from the settings.
      if (settings.Read(identifier, name, string) == StatusError)
        escape("unable to read the value for '%s' to the section '%s'",
               name.c_str(), identifier.c_str());

      // convert the string into the type.
      if (Variable::Convert(string, value) == StatusFalse)
        escape("unable to convert the value '%s' for '%s' to the section '%s'",
               string.c_str(), name.c_str(), identifier.c_str());

      return elle::StatusOk;
    }

    ///
    /// this method gets a basic type, with a potential default value.
    ///
    template <typename T, Boolean C>
    Status      Settings::Behaviour<T, C>::Get(Settings&        settings,
                                               const String&    identifier,
                                               const String&    name,
                                               T&               value,
                                               const T          D)
    {
      String    string;

      ;

      // if the item does not exist, return the default value.
      if (settings.Find(identifier, name) == StatusFalse)
        {
          // set the value with the default.
          value = D;

          return elle::StatusOk;
        }

      // otherwise, read the item from the settings.
      if (settings.Read(identifier, name, string) == StatusError)
        escape("unable to read the value for '%s' to the section '%s'",
               name.c_str(), identifier.c_str());

      // convert the string into the type.
      if (Variable::Convert(string, value) == StatusFalse)
        escape("unable to convert the value '%s' for '%s' to the section '%s'",
               string.c_str(), name.c_str(), identifier.c_str());

      return elle::StatusOk;
    }

    ///
    /// this method sets a compound type.
    ///
    /// note that such objects must inherits the Uniquable<F> interface,
    /// hence provide the Save() and Restore() methods.
    ///
    template <typename T>
    template <const Format F>
    Status      Settings::Behaviour<T, true>::Set(
                  Settings&                                     settings,
                  const String&                                 identifier,
                  const String&                                 name,
                  const Uniquable<F>&                           object)
    {
      Unique    unique;

      ;

      // save the object in a unique representation.
      if (object.Save(unique) == StatusError)
        escape("unable to save the object for '%s' to the section '%s'",
               name.c_str(), identifier.c_str());

      // write the value.
      if (settings.Write(identifier, name, unique) == StatusError)
        escape("unable to write the value '%s' for '%s' to the section '%s'",
               unique.c_str(), name.c_str(), identifier.c_str());

      return elle::StatusOk;
    }

    ///
    /// this method gets a compound type.
    ///
    template <typename T>
    template <const Format F>
    Status      Settings::Behaviour<T, true>::Get(
                  Settings&                                     settings,
                  const String&                                 identifier,
                  const String&                                 name,
                  Uniquable<F>&                                 object)
    {
      Unique    unique;

      ;

      // if the item does not exist, return the default value.
      if (settings.Find(identifier, name) == StatusFalse)
        escape("unable to locate the item for '%s' to the section '%s'",
               name.c_str(), identifier.c_str());

      // otherwise, read the item from the settings.
      if (settings.Read(identifier, name, unique) == StatusError)
        escape("unable to read the value for '%s' to the section '%s'",
               name.c_str(), identifier.c_str());

      // restore the object.
      if (object.Restore(unique) == StatusError)
        escape("unable to restore the object '%s' for '%s' to the "
               "section '%s'",
               unique.c_str(), name.c_str(), identifier.c_str());

      return elle::StatusOk;
    }

    ///
    /// this method gets a compound type, with a potential default value.
    ///
    template <typename T>
    template <const Format F>
    Status      Settings::Behaviour<T, true>::Get(
                  Settings&                                     settings,
                  const String&                                 identifier,
                  const String&                                 name,
                  Uniquable<F>&                                 object,
                  const Uniquable<F>                            D)
    {
      Unique    unique;

      ;

      // if the item does not exist, return the default value.
      if (settings.Find(identifier, name) == StatusFalse)
        {
          // save the default value.
          if (D.Save(unique) == StatusError)
            escape("unable to save the default value for '%s' to the "
                   "section '%s'",
                   name.c_str(), identifier.c_str());

          // restore the object with the default unique.
          if (object.Restore(unique) == StatusError)
            escape("unable to restore the object for '%s' to the "
                   "section '%s'",
                   name.c_str(), identifier.c_str());

          return elle::StatusOk;
        }

      // otherwise, read the item from the settings.
      if (settings.Read(identifier, name, unique) == StatusError)
        escape("unable to read the value for '%s' to the section '%s'",
               name.c_str(), identifier.c_str());

      // restore the object.
      if (object.Restore(unique) == StatusError)
        escape("unable to restore the object '%s' for '%s' to the "
               "section '%s'",
               unique.c_str(), name.c_str(), identifier.c_str());

      return elle::StatusOk;
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method wraps the Set() method by forwarding the call
    /// according to the given type.
    ///
    template <typename T>
    Status              Settings::Set(const String&             identifier,
                                      const String&             name,
                                      const T&                  value)
    {
      return
        (Settings::Behaviour<T,
                             SettingsType<T>::Value
                               ==
                             Settings::TypeUnknown>::Set(*this,
                                                         identifier,
                                                         name,
                                                         value));
    }

    ///
    /// this method wraps the Get() method by forwarding the call
    /// according to the given type.
    ///
    template <typename T>
    Status              Settings::Get(const String&             identifier,
                                      const String&             name,
                                      T&                        value)
    {
      return
        (Settings::Behaviour<T,
                             SettingsType<T>::Value
                               ==
                             Settings::TypeUnknown>::Get(*this,
                                                         identifier,
                                                         name,
                                                         value));
    }

    ///
    /// this method wraps the default-based Get() method by forwarding the call
    /// according to the given type.
    ///
    template <typename T>
    Status              Settings::Get(const String&             identifier,
                                      const String&             name,
                                      T&                        value,
                                      const T&                  D)
    {
      return
        (Settings::Behaviour<T,
                             SettingsType<T>::Value
                               ==
                             Settings::TypeUnknown>::Get(*this,
                                                         identifier,
                                                         name,
                                                         value,
                                                         D));
    }

  }
}

#endif
