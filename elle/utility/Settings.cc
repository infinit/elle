//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// author        julien quintard   [sun apr 25 19:32:47 2010]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/utility/Settings.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>
#include <elle/standalone/Region.hh>

#include <elle/io/File.hh>

namespace elle
{
  namespace utility
  {

//
// ---------- static methods --------------------------------------------------
//

    ///
    /// this method trims a given string according to the given delimiters.
    ///
    Status              Settings::Trim(const String&            input,
                                       String&                  output,
                                       const String&            delimiters)
    {
      elle::String::size_type   index;

      ;

      // first, copy the input in the output.
      output.assign(input);

      // remove the last characters.
      if ((index = output.find_last_not_of(delimiters)) != String::npos)
        output.erase(index + 1);

      // remove the first characters.
      if ((index = output.find_first_not_of(delimiters)) != String::npos)
        output.erase(0, index);
      else
        output.erase();

      return elle::StatusOk;
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default assignment constructor.
    ///
    Settings::Assignment::Assignment(const String&              name,
                                     const String&              value):
      name(name),
      value(value)
    {
    }

    ///
    /// the default section constructor.
    ///
    Settings::Section::Section(const String&                    identifier):
      identifier(identifier)
    {
    }

    ///
    /// the section destructor.
    ///
    Settings::Section::~Section()
    {
      Settings::Section::Scoutor        scoutor;

      // for every assignment.
      for (scoutor = this->assignments.begin();
           scoutor != this->assignments.end();
           scoutor++)
        {
          Settings::Assignment*         assignment = *scoutor;

          // delete the assignment.
          delete assignment;
        }
    }

    ///
    /// the settings destructor.
    ///
    Settings::~Settings()
    {
      Settings::Scoutor         scoutor;

      // for every section.
      for (scoutor = this->sections.begin();
           scoutor != this->sections.end();
           scoutor++)
        {
          Settings::Section*    section = *scoutor;

          // delete the section.
          delete section;
        }

      // clear the container.
      this->sections.clear();
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method returns true if the given assignment exists within
    /// the section.
    ///
    Status              Settings::Section::Exist(const String&  name)
    {
      ;

      // locate the assignment.
      if (this->Locate(name) != StatusTrue)
        return elle::StatusFalse;

      return elle::StatusTrue;
    }

    ///
    /// this method adds a assignment to the section.
    ///
    Status              Settings::Section::Add(const String&    name,
                                               const String&    value)
    {
      Settings::Assignment*     assignment;

      // check if another assignment with that name already exists.
      if (this->Locate(name) == StatusTrue)
        escape("an assignment with this name already exists");

      // allocate the assignment.
      assignment = new Settings::Assignment(name, value);

      // add the assignment to the container.
      this->assignments.push_back(assignment);

      return elle::StatusOk;
    }

    ///
    /// this method returns the value associated with the given name,
    /// in this section.
    ///
    Status              Settings::Section::Lookup(const String& name,
                                                  String&       value)
    {
      Settings::Section::Iterator       iterator;
      Settings::Assignment*             assignment;

      ;

      // try to locate the assignment.
      if (this->Locate(name, &iterator) != StatusTrue)
        escape("unable to locate the assignment");

      // retrieve the assignment.
      assignment = *iterator;

      // return the value.
      value = assignment->value;

      return elle::StatusOk;
    }

    ///
    /// this method updates an assignment in the section.
    ///
    Status              Settings::Section::Update(const String& name,
                                                  const String& value)
    {
      Settings::Section::Iterator       iterator;
      Settings::Assignment*             assignment;

      ;

      // retrieve the assignment.
      if (this->Locate(name, &iterator) != StatusTrue)
        escape("this assignment does not seem to exist");

      // retrieve the assignment.
      assignment = *iterator;

      // update the assignment.
      assignment->value = value;

      return elle::StatusOk;
    }

    ///
    /// this method removes an assignment.
    ///
    Status              Settings::Section::Remove(const String& name)
    {
      Settings::Section::Iterator       iterator;
      Settings::Assignment*             assignment;

      ;

      // try to locate the assignment.
      if (this->Locate(name, &iterator) != StatusTrue)
        escape("unable to locate the assignment");

      // retrieve the assignment.
      assignment = *iterator;

      // remove the assignment.
      this->assignments.erase(iterator);

      // delete the assignment.
      delete assignment;

      return elle::StatusOk;
    }

    ///
    /// this method locates an assignment in the section.
    ///
    Status              Settings::Section::Locate(
                          const String&                         name,
                          Settings::Section::Iterator*          iterator)
    {
      Settings::Section::Iterator       i;

      ;

      // look for the named assignment.
      for (i = this->assignments.begin();
           i != this->assignments.end();
           i++)
        {
          Settings::Assignment*         assignment = *i;

          // test if this is the looked for assignment.
          if (assignment->name == name)
            {
              // return the iterator, if requested.
              if (iterator != NULL)
                *iterator = i;

              return elle::StatusTrue;
            }
        }

      return elle::StatusFalse;
    }

    ///
    /// this method adds a section to the settings.
    ///
    Status              Settings::Add(const String&             identifier)
    {
      Settings::Section*        section;

      // check if another section with that identifier already exists.
      if (this->Locate(identifier) == StatusTrue)
        escape("a section with this identifier already exists");

      // allocate the section.
      section = new Settings::Section(identifier);

      // add the section to the container.
      this->sections.push_back(section);

      return elle::StatusOk;
    }

    ///
    /// this method returns the section associated with the given identifier.
    ///
    Status              Settings::Lookup(const String&          identifier,
                                         Section*&              section)
    {
      Settings::Iterator        iterator;

      // try to locate the section.
      if (this->Locate(identifier, &iterator) != StatusTrue)
        escape("unable to locate the section");

      // retrieve the section.
      section = *iterator;

      return elle::StatusOk;
    }

    ///
    /// this method removes a section.
    ///
    Status              Settings::Remove(const String&          identifier)
    {
      Settings::Iterator        iterator;
      Settings::Section*        section;

      ;

      // try to locate the section.
      if (this->Locate(identifier, &iterator) != StatusTrue)
        escape("unable to locate the section");

      // retrieve the section.
      section = *iterator;

      // remove the section.
      this->sections.erase(iterator);

      // delete the section.
      delete section;

      return elle::StatusOk;
    }

    ///
    /// this method locates a section in the settings.
    ///
    Status              Settings::Locate(const String&          identifier,
                                         Settings::Iterator*    iterator)
    {
      Settings::Iterator        i;

      ;

      // look for the identifierd section.
      for (i = this->sections.begin();
           i != this->sections.end();
           i++)
        {
          Settings::Section*            section = *i;

          // test if this is the looked for section.
          if (section->identifier == identifier)
            {
              // return the iterator, if requested.
              if (iterator != NULL)
                *iterator = i;

              return elle::StatusTrue;
            }
        }

      return elle::StatusFalse;
    }

    ///
    /// this method returns true if the given item exists in the given
    /// section.
    ///
    Status              Settings::Find(const String&            identifier,
                                       const String&            name)
    {
      Settings::Section*        section;

      ;

      // check if the section exists.
      if (this->Locate(identifier) != StatusTrue)
        return elle::StatusFalse;

      // retrieve the section.
      if (this->Lookup(identifier, section) == StatusError)
        flee("unable to retrieve the section");

      // check if the section exists.
      if (section->Locate(name) != StatusTrue)
        return elle::StatusFalse;

      return elle::StatusTrue;
    }

    ///
    /// this method writes an item.
    ///
    Status              Settings::Write(const String&           identifier,
                                        const String&           name,
                                        const String&           value)
    {
      Settings::Section*        section;

      ;

      // check if the section exists.
      if (this->Locate(identifier) != StatusTrue)
        {
          // create the section.
          if (this->Add(identifier) == StatusError)
            escape("unable to create the section");
        }

      // retrieve the section.
      if (this->Lookup(identifier, section) == StatusError)
        escape("unable to retrieve the section");

      // check if the assignment exists.
      if (section->Exist(name) != StatusTrue)
        {
          // add the assignment to the section.
          if (section->Add(name, value) == StatusError)
            escape("unable to add the assignment");
        }
      else
        {
          // otherwise, update the assignment.
          if (section->Update(name, value) == StatusError)
            escape("unable to update the assignment");
        }

      return elle::StatusOk;
    }

    ///
    /// this method reads an item which is assumed to exist.
    ///
    Status              Settings::Read(const String&            identifier,
                                       const String&            name,
                                       String&                  value)
    {
      Settings::Section*        section;

      ;

      // retrieve the section.
      if (this->Lookup(identifier, section) == StatusError)
        escape("unable to retrieve the section");

      // lookup the assignment in the section.
      if (section->Lookup(name, value) == StatusError)
        escape("unable to locate the assignment");

      return elle::StatusOk;
    }

//
// ---------- object ----------------------------------------------------------
//

    ///
    /// this macro-function call generates the object.
    ///
    embed(Settings, _());

//
// ---------- dumpable --------------------------------------------------------
//

    ///
    /// this method dumps a settings object.
    ///
    Status              Settings::Dump(const Natural32          margin) const
    {
      String            alignment(margin, ' ');
      Settings::Scoutor i;

      ;

      std::cout << alignment << "[Settings]" << std::endl;

      // go through the sections.
      for (i = this->sections.begin();
           i != this->sections.end();
           i++)
        {
          Settings::Section*            section = *i;
          Settings::Section::Scoutor    j;

          // display the section.
          std::cout << alignment << Dumpable::Shift
                    << "[Section] " << section->identifier << std::endl;

          // go through the assignments.
          for (j = section->assignments.begin();
               j != section->assignments.end();
               j++)
            {
              Settings::Assignment*     assignment = *j;

              // display the assignment.
              std::cout << alignment << Dumpable::Shift << Dumpable::Shift
                        << "[" << assignment->name << "] "
                        << assignment->value << std::endl;
            }
        }

      return elle::StatusOk;
    }

//
// ---------- fileable --------------------------------------------------------
//

    ///
    /// this method loads a settings file.
    ///
    Status              Settings::Load(const Path&              path)
    {
      std::istringstream        stream;
      Region                    region;
      String                    line;
      Settings::Section*        section;

      ;

      // read the content.
      if (File::Read(path, region) == StatusError)
        escape("unable to read the file");

      // set up the stream.
      stream.str(String(reinterpret_cast<const char*>(region.contents),
                        region.size));

      // initialize the section pointer.
      section = NULL;

      // for every line of the file.
      while (std::getline(stream, line))
        {
          String                name;
          String                value;
          size_t                position;

          // first, trim the line.
          if (Settings::Trim(line, line) == StatusError)
            escape("unable to trim the line");

          // if the line is empty, ignore it.
          if (line.length() == 0)
            continue;

          // if the line is a comment line, ignore it too.
          if (line[0] == '#')
            continue;

          // create a new section if the [ character is found.
          if (line[0] == '[')
            {
              String            identifier;

              // extract the section name.
              if (Settings::Trim(line.substr(1, line.find(']') - 1),
                                 identifier) == StatusError)
                escape("unable to trim the section");

              // add the section.
              if (this->Add(identifier) == StatusError)
                escape("unable to add a new section");

              // set the current section.
              if (this->Lookup(identifier, section) == StatusError)
                escape("unable to retrieve the section");

              continue;
            }

          // locate the equal sign and ignore the line if not valid.
          if ((position = line.find('=')) == String::npos)
            continue;

          // if there is no section, stop.
          if (section == NULL)
            escape("unable to add a setting without a section");

          // extract the name.
          if (Settings::Trim(line.substr(0, position), name) == StatusError)
            escape("unable to trim the name");

          // extract the value.
          if (Settings::Trim(line.substr(position + 1), value) == StatusError)
            escape("unable to trim the value");

          // add the assignment to the section.
          if (section->Add(name, value) == StatusError)
            escape("unable to add the setting to the section");
        }

      return elle::StatusOk;
    }

    ///
    /// this method stores a setting into the given file.
    ///
    Status              Settings::Store(const Path&             path) const
    {
      std::ostringstream        stream;
      Region                    region;
      String                    string;
      Settings::Scoutor         i;

      ;

      // go through the sections.
      for (i = this->sections.begin();
           i != this->sections.end();
           i++)
        {
          Settings::Section*            section = *i;
          Settings::Section::Scoutor    j;

          // write the section identifier.
          stream << "[" << section->identifier << "]" << std::endl;

          // go through the assignments.
          for (j = section->assignments.begin();
               j != section->assignments.end();
               j++)
            {
              Settings::Assignment*     assignment = *j;

              // write the assignment.
              stream << assignment->name << " = "
                     << assignment->value << std::endl;
            }
        }

      // retrieve the string.
      string = stream.str();

      // wrap the stream in a region.
      if (region.Wrap(reinterpret_cast<const Byte*>(string.c_str()),
                      string.length()) == StatusError)
        escape("unable to wrap the stream");

      // write the content.
      if (File::Write(path, region) == StatusError)
        escape("unable to write the file");

      return elle::StatusOk;
    }

    ///
    /// this method erases the settings file.
    ///
    Status              Settings::Erase(const Path&             path) const
    {
      ;

      // erase the file.
      if (File::Erase(path) == StatusError)
        escape("unable to erase the file");

      return elle::StatusOk;
    }

    ///
    /// this method tests the settings file.
    ///
    Status              Settings::Exist(const Path&             path) const
    {
      return (File::Exist(path));
    }

  }
}
