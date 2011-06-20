//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/utility/Settings.cc
//
// created       julien quintard   [sun apr 25 19:32:47 2010]
// updated       julien quintard   [mon jun 20 11:43:12 2011]
//

//
// ---------- includes --------------------------------------------------------
//

#include <elle/utility/Settings.hh>

#include <elle/standalone/Maid.hh>
#include <elle/standalone/Report.hh>

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
    Status		Settings::Trim(const String&		input,
				       String&			output,
				       const String&		delimiters)
    {
      Natural32		index;

      enter();

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

      leave();
    }

//
// ---------- constructors & destructors --------------------------------------
//

    ///
    /// the default assignment constructor.
    ///
    Settings::Assignment::Assignment(const String&		name,
				     const String&		value):
      name(name),
      value(value)
    {
    }

    ///
    /// the default section constructor.
    ///
    Settings::Section::Section(const String&			identifier):
      identifier(identifier)
    {
    }

    ///
    /// the section destructor.
    ///
    Settings::Section::~Section()
    {
      Settings::Section::Scoutor	scoutor;

      // for every assignment.
      for (scoutor = this->assignments.begin();
	   scoutor != this->assignments.end();
	   scoutor++)
	{
	  Settings::Assignment*		assignment = *scoutor;

	  // delete the assignment.
	  delete assignment;
	}
    }

    ///
    /// the settings destructor.
    ///
    Settings::~Settings()
    {
      Settings::Scoutor		scoutor;

      // for every section.
      for (scoutor = this->sections.begin();
	   scoutor != this->sections.end();
	   scoutor++)
	{
	  Settings::Section*	section = *scoutor;

	  // delete the section.
	  delete section;
	}
    }

//
// ---------- methods ---------------------------------------------------------
//

    ///
    /// this method adds a assignment to the section.
    ///
    Status		Settings::Section::Add(const String&	name,
					       const String&	value)
    {
      Settings::Assignment*	assignment;

      enter(instance(assignment));

      // check if another assignment with that name already exists.
      if (this->Locate(name) == StatusTrue)
	escape("an assignment with this name already exists");

      // allocate the assignment.
      assignment = new Settings::Assignment(name, value);

      // add the assignment to the container.
      this->assignments.push_back(assignment);

      // waive.
      waive(assignment);

      leave();
    }

    ///
    /// this method returns the value associated with the given name,
    /// in this section.
    ///
    Status		Settings::Section::Lookup(const String&	name,
						  String&	value)
    {
      Settings::Section::Iterator	iterator;
      Settings::Assignment*		assignment;

      enter();

      // try to locate the assignment.
      if (this->Locate(name, &iterator) != StatusTrue)
	escape("unable to locate the assignment");

      // retrieve the assignment.
      assignment = *iterator;

      // return the value.
      value = assignment->value;

      leave();
    }

    ///
    /// this method removes an assignment.
    ///
    Status		Settings::Section::Remove(const String&	name)
    {
      Settings::Section::Iterator	iterator;
      Settings::Assignment*		assignment;

      enter();

      // try to locate the assignment.
      if (this->Locate(name, &iterator) != StatusTrue)
	escape("unable to locate the assignment");

      // retrieve the assignment.
      assignment = *iterator;

      // remove the assignment.
      this->assignments.erase(iterator);

      // delete the assignment.
      delete assignment;

      leave();
    }

    ///
    /// this method locates an assignment in the section.
    ///
    Status		Settings::Section::Locate(
			  const String&				name,
			  Settings::Section::Iterator*		iterator)
    {
      Settings::Section::Iterator	i;

      enter();

      // look for the named assignment.
      for (i = this->assignments.begin();
	   i != this->assignments.end();
	   i++)
	{
	  Settings::Assignment*		assignment = *i;

	  // test if this is the looked for assignment.
	  if (assignment->name == name)
	    {
	      // return the iterator, if requested.
	      if (iterator != NULL)
		*iterator = i;

	      true();
	    }
	}

      false();
    }

    ///
    /// this method adds a section to the settings.
    ///
    Status		Settings::Add(const String&		identifier)
    {
      Settings::Section*	section;

      enter(instance(section));

      // check if another section with that identifier already exists.
      if (this->Locate(identifier) == StatusTrue)
	escape("a section with this identifier already exists");

      // allocate the section.
      section = new Settings::Section(identifier);

      // add the section to the container.
      this->sections.push_back(section);

      // waive.
      waive(section);

      leave();
    }

    ///
    /// this method returns the section associated with the given identifier.
    ///
    Status		Settings::Lookup(const String&		identifier,
					 Section*&		section)
    {
      Settings::Iterator	iterator;

      enter();

      // try to locate the section.
      if (this->Locate(identifier, &iterator) != StatusTrue)
	escape("unable to locate the section");

      // retrieve the section.
      section = *iterator;

      leave();
    }

    ///
    /// this method removes a section.
    ///
    Status		Settings::Remove(const String&		identifier)
    {
      Settings::Iterator	iterator;
      Settings::Section*	section;

      enter();

      // try to locate the section.
      if (this->Locate(identifier, &iterator) != StatusTrue)
	escape("unable to locate the section");

      // retrieve the section.
      section = *iterator;

      // remove the section.
      this->sections.erase(iterator);

      // delete the section.
      delete section;

      leave();
    }

    ///
    /// this method locates a section in the settings.
    ///
    Status		Settings::Locate(const String&		identifier,
					 Settings::Iterator*	iterator)
    {
      Settings::Iterator	i;

      enter();

      // look for the identifierd section.
      for (i = this->sections.begin();
	   i != this->sections.end();
	   i++)
	{
	  Settings::Section*		section = *i;

	  // test if this is the looked for section.
	  if (section->identifier == identifier)
	    {
	      // return the iterator, if requested.
	      if (iterator != NULL)
		*iterator = i;

	      true();
	    }
	}

      false();
    }

    ///
    /// this method returns true if the given item exists in the given
    /// section.
    ///
    Status		Settings::Find(const String&		identifier,
				       const String&		name)
    {
      Settings::Section*	section;

      enter();

      // check if the section exists.
      if (this->Locate(identifier) != StatusTrue)
	false();

      // retrieve the section.
      if (this->Lookup(identifier, section) == StatusError)
	flee("unable to retrieve the section");

      // check if the section exists.
      if (section->Locate(name) != StatusTrue)
	false();

      true();
    }

    ///
    /// this method writes an item.
    ///
    Status		Settings::Write(const String&		identifier,
					const String&		name,
					const String&		value)
    {
      Settings::Section*	section;

      enter();

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

      // add the assignment to the section.
      if (section->Add(name, value) == StatusError)
	escape("unable to add the assignment");

      leave();
    }

    ///
    /// this method reads an item which is assumed to exist.
    ///
    Status		Settings::Read(const String&		identifier,
				       const String&		name,
				       String&			value)
    {
      Settings::Section*	section;

      enter();

      // retrieve the section.
      if (this->Lookup(identifier, section) == StatusError)
	escape("unable to retrieve the section");

      // lookup the assignment in the section.
      if (section->Lookup(name, value) == StatusError)
	escape("unable to locate the assignment");

      leave();
    }

    ///
    /// this method makes it easy to sets an assignment in the given
    /// section.
    ///
    Status		Settings::Set(const String&		identifier,
				      const String&		name,
				      const String&		value)
    {
      enter();

      // write the item.
      if (this->Write(identifier, name, value) == StatusError)
	escape("unable to write the settings");

      leave();
    }

    ///
    /// this method sets a boolean.
    ///
    Status		Settings::Set(const String&		identifier,
				      const String&		name,
				      const Boolean&		value)
    {
      String		string;
      std::stringstream	stream;

      enter();

      // transform the boolean into a string.
      stream << value;

      // set the value.
      if (this->Set(identifier, name, stream.str()) == StatusError)
	escape("unable to set the value");

      leave();
    }

    ///
    /// this method sets a character.
    ///
    Status		Settings::Set(const String&		identifier,
				      const String&		name,
				      const Character&		value)
    {
      String		string;
      std::stringstream	stream;

      enter();

      // transform the boolean into a string.
      stream << value;

      // set the value.
      if (this->Set(identifier, name, stream.str()) == StatusError)
	escape("unable to set the value");

      leave();
    }

    ///
    /// this method sets a integer8.
    ///
    Status		Settings::Set(const String&		identifier,
				      const String&		name,
				      const Integer8&		value)
    {
      String		string;
      std::stringstream	stream;

      enter();

      // transform the boolean into a string.
      stream << value;

      // set the value.
      if (this->Set(identifier, name, stream.str()) == StatusError)
	escape("unable to set the value");

      leave();
    }

    ///
    /// this method sets a integer16.
    ///
    Status		Settings::Set(const String&		identifier,
				      const String&		name,
				      const Integer16&		value)
    {
      String		string;
      std::stringstream	stream;

      enter();

      // transform the boolean into a string.
      stream << value;

      // set the value.
      if (this->Set(identifier, name, stream.str()) == StatusError)
	escape("unable to set the value");

      leave();
    }

    ///
    /// this method sets a integer32.
    ///
    Status		Settings::Set(const String&		identifier,
				      const String&		name,
				      const Integer32&		value)
    {
      String		string;
      std::stringstream	stream;

      enter();

      // transform the boolean into a string.
      stream << value;

      // set the value.
      if (this->Set(identifier, name, stream.str()) == StatusError)
	escape("unable to set the value");

      leave();
    }

    ///
    /// this method sets a integer64.
    ///
    Status		Settings::Set(const String&		identifier,
				      const String&		name,
				      const Integer64&		value)
    {
      String		string;
      std::stringstream	stream;

      enter();

      // transform the boolean into a string.
      stream << value;

      // set the value.
      if (this->Set(identifier, name, stream.str()) == StatusError)
	escape("unable to set the value");

      leave();
    }

    ///
    /// this method sets a natural8.
    ///
    Status		Settings::Set(const String&		identifier,
				      const String&		name,
				      const Natural8&		value)
    {
      String		string;
      std::stringstream	stream;

      enter();

      // transform the boolean into a string.
      stream << value;

      // set the value.
      if (this->Set(identifier, name, stream.str()) == StatusError)
	escape("unable to set the value");

      leave();
    }

    ///
    /// this method sets a natural16.
    ///
    Status		Settings::Set(const String&		identifier,
				      const String&		name,
				      const Natural16&		value)
    {
      String		string;
      std::stringstream	stream;

      enter();

      // transform the boolean into a string.
      stream << value;

      // set the value.
      if (this->Set(identifier, name, stream.str()) == StatusError)
	escape("unable to set the value");

      leave();
    }

    ///
    /// this method sets a natural32.
    ///
    Status		Settings::Set(const String&		identifier,
				      const String&		name,
				      const Natural32&		value)
    {
      String		string;
      std::stringstream	stream;

      enter();

      // transform the boolean into a string.
      stream << value;

      // set the value.
      if (this->Set(identifier, name, stream.str()) == StatusError)
	escape("unable to set the value");

      leave();
    }

    ///
    /// this method sets a natural64.
    ///
    Status		Settings::Set(const String&		identifier,
				      const String&		name,
				      const Natural64&		value)
    {
      String		string;
      std::stringstream	stream;

      enter();

      // transform the boolean into a string.
      stream << value;

      // set the value.
      if (this->Set(identifier, name, stream.str()) == StatusError)
	escape("unable to set the value");

      leave();
    }

    ///
    /// this method sets a real.
    ///
    Status		Settings::Set(const String&		identifier,
				      const String&		name,
				      const Real&		value)
    {
      String		string;
      std::stringstream	stream;

      enter();

      // transform the boolean into a string.
      stream << value;

      // set the value.
      if (this->Set(identifier, name, stream.str()) == StatusError)
	escape("unable to set the value");

      leave();
    }

    ///
    /// this method makes it easy to retrieve a value from a specific section.
    ///
    Status		Settings::Get(const String&		identifier,
				      const String&		name,
				      String&			value,
				      const String		d)
    {
      enter();

      // set the value with the default.
      value = d;

      // if the item does not exist, return the default value.
      if (this->Find(identifier, name) == StatusFalse)
	leave();

      // otherwise, read the item from the settings.
      if (this->Read(identifier, name, value) == StatusError)
	escape("unable to read the item");

      leave();
    }

    ///
    /// this method gets a boolean.
    ///
    Status		Settings::Get(const String&		identifier,
				      const String&		name,
				      Boolean&			value,
				      const Boolean		d)
    {
      String			string;

      enter();

      // set the value with the default.
      value = d;

      // if the item does not exist, return the default value.
      if (this->Find(identifier, name) == StatusFalse)
	leave();

      // otherwise, read the item from the settings.
      if (this->Read(identifier, name, string) == StatusError)
	escape("unable to read the item");

      // set up the stream.
      std::istringstream	stream(string);

      // extract the value.
      stream >> value;

      // check the stream.
      if (!stream || !stream.eof())
	escape("the value's format seems to be incorrect");

      leave();
    }

    ///
    /// this method gets a character.
    ///
    Status		Settings::Get(const String&		identifier,
				      const String&		name,
				      Character&		value,
				      const Character		d)
    {
      String			string;

      enter();

      // set the value with the default.
      value = d;

      // if the item does not exist, return the default value.
      if (this->Find(identifier, name) == StatusFalse)
	leave();

      // otherwise, read the item from the settings.
      if (this->Read(identifier, name, string) == StatusError)
	escape("unable to read the item");

      // set up the stream.
      std::istringstream	stream(string);

      // extract the value.
      stream >> value;

      // check the stream.
      if (!stream || !stream.eof())
	escape("the value's format seems to be incorrect");

      leave();
    }

    ///
    /// this method gets a integer8.
    ///
    Status		Settings::Get(const String&		identifier,
				      const String&		name,
				      Integer8&			value,
				      const Integer8		d)
    {
      String			string;

      enter();

      // set the value with the default.
      value = d;

      // if the item does not exist, return the default value.
      if (this->Find(identifier, name) == StatusFalse)
	leave();

      // otherwise, read the item from the settings.
      if (this->Read(identifier, name, string) == StatusError)
	escape("unable to read the item");

      // set up the stream.
      std::istringstream	stream(string);

      // extract the value.
      stream >> value;

      // check the stream.
      if (!stream || !stream.eof())
	escape("the value's format seems to be incorrect");

      leave();
    }

    ///
    /// this method gets a integer16.
    ///
    Status		Settings::Get(const String&		identifier,
				      const String&		name,
				      Integer16&		value,
				      const Integer16		d)
    {
      String			string;

      enter();

      // set the value with the default.
      value = d;

      // if the item does not exist, return the default value.
      if (this->Find(identifier, name) == StatusFalse)
	leave();

      // otherwise, read the item from the settings.
      if (this->Read(identifier, name, string) == StatusError)
	escape("unable to read the item");

      // set up the stream.
      std::istringstream	stream(string);

      // extract the value.
      stream >> value;

      // check the stream.
      if (!stream || !stream.eof())
	escape("the value's format seems to be incorrect");

      leave();
    }

    ///
    /// this method gets a integer32.
    ///
    Status		Settings::Get(const String&		identifier,
				      const String&		name,
				      Integer32&		value,
				      const Integer32		d)
    {
      String			string;

      enter();

      // set the value with the default.
      value = d;

      // if the item does not exist, return the default value.
      if (this->Find(identifier, name) == StatusFalse)
	leave();

      // otherwise, read the item from the settings.
      if (this->Read(identifier, name, string) == StatusError)
	escape("unable to read the item");

      // set up the stream.
      std::istringstream	stream(string);

      // extract the value.
      stream >> value;

      // check the stream.
      if (!stream || !stream.eof())
	escape("the value's format seems to be incorrect");

      leave();
    }

    ///
    /// this method gets a integer64.
    ///
    Status		Settings::Get(const String&		identifier,
				      const String&		name,
				      Integer64&		value,
				      const Integer64		d)
    {
      String			string;

      enter();

      // set the value with the default.
      value = d;

      // if the item does not exist, return the default value.
      if (this->Find(identifier, name) == StatusFalse)
	leave();

      // otherwise, read the item from the settings.
      if (this->Read(identifier, name, string) == StatusError)
	escape("unable to read the item");

      // set up the stream.
      std::istringstream	stream(string);

      // extract the value.
      stream >> value;

      // check the stream.
      if (!stream || !stream.eof())
	escape("the value's format seems to be incorrect");

      leave();
    }

    ///
    /// this method gets a natural8.
    ///
    Status		Settings::Get(const String&		identifier,
				      const String&		name,
				      Natural8&			value,
				      const Natural8		d)
    {
      String			string;

      enter();

      // set the value with the default.
      value = d;

      // if the item does not exist, return the default value.
      if (this->Find(identifier, name) == StatusFalse)
	leave();

      // otherwise, read the item from the settings.
      if (this->Read(identifier, name, string) == StatusError)
	escape("unable to read the item");

      // set up the stream.
      std::istringstream	stream(string);

      // extract the value.
      stream >> value;

      // check the stream.
      if (!stream || !stream.eof())
	escape("the value's format seems to be incorrect");

      leave();
    }

    ///
    /// this method gets a natural16.
    ///
    Status		Settings::Get(const String&		identifier,
				      const String&		name,
				      Natural16&		value,
				      const Natural16		d)
    {
      String			string;

      enter();

      // set the value with the default.
      value = d;

      // if the item does not exist, return the default value.
      if (this->Find(identifier, name) == StatusFalse)
	leave();

      // otherwise, read the item from the settings.
      if (this->Read(identifier, name, string) == StatusError)
	escape("unable to read the item");

      // set up the stream.
      std::istringstream	stream(string);

      // extract the value.
      stream >> value;

      // check the stream.
      if (!stream || !stream.eof())
	escape("the value's format seems to be incorrect");

      leave();
    }

    ///
    /// this method gets a natural32.
    ///
    Status		Settings::Get(const String&		identifier,
				      const String&		name,
				      Natural32&		value,
				      const Natural32		d)
    {
      String			string;

      enter();

      // set the value with the default.
      value = d;

      // if the item does not exist, return the default value.
      if (this->Find(identifier, name) == StatusFalse)
	leave();

      // otherwise, read the item from the settings.
      if (this->Read(identifier, name, string) == StatusError)
	escape("unable to read the item");

      // set up the stream.
      std::istringstream	stream(string);

      // extract the value.
      stream >> value;

      // check the stream.
      if (!stream || !stream.eof())
	escape("the value's format seems to be incorrect");

      leave();
    }

    ///
    /// this method gets a natural64.
    ///
    Status		Settings::Get(const String&		identifier,
				      const String&		name,
				      Natural64&		value,
				      const Natural64		d)
    {
      String			string;

      enter();

      // set the value with the default.
      value = d;

      // if the item does not exist, return the default value.
      if (this->Find(identifier, name) == StatusFalse)
	leave();

      // otherwise, read the item from the settings.
      if (this->Read(identifier, name, string) == StatusError)
	escape("unable to read the item");

      // set up the stream.
      std::istringstream	stream(string);

      // extract the value.
      stream >> value;

      // check the stream.
      if (!stream || !stream.eof())
	escape("the value's format seems to be incorrect");

      leave();
    }

    ///
    /// this method gets a real.
    ///
    Status		Settings::Get(const String&		identifier,
				      const String&		name,
				      Real&			value,
				      const Real		d)
    {
      String			string;

      enter();

      // set the value with the default.
      value = d;

      // if the item does not exist, return the default value.
      if (this->Find(identifier, name) == StatusFalse)
	leave();

      // otherwise, read the item from the settings.
      if (this->Read(identifier, name, string) == StatusError)
	escape("unable to read the item");

      // set up the stream.
      std::istringstream	stream(string);

      // extract the value.
      stream >> value;

      // check the stream.
      if (!stream || !stream.eof())
	escape("the value's format seems to be incorrect");

      leave();
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
    Status		Settings::Dump(const Natural32		margin) const
    {
      String		alignment(margin, ' ');
      Settings::Scoutor	i;

      enter();

      std::cout << alignment << "[Settings]" << std::endl;

      // go through the sections.
      for (i = this->sections.begin();
	   i != this->sections.end();
	   i++)
	{
	  Settings::Section*		section = *i;
	  Settings::Section::Scoutor	j;

	  // display the section.
	  std::cout << alignment << Dumpable::Shift
		    << "[Section] " << section->identifier << std::endl;

	  // go through the assignments.
	  for (j = section->assignments.begin();
	       j != section->assignments.end();
	       j++)
	    {
	      Settings::Assignment*	assignment = *j;

	      // display the assignment.
	      std::cout << alignment << Dumpable::Shift << Dumpable::Shift
			<< "[" << assignment->name << "] "
			<< assignment->value << std::endl;
	    }
	}

      leave();
    }

//
// ---------- fileable --------------------------------------------------------
//

    ///
    /// this method loads a settings file.
    ///
    Status		Settings::Load(const Path&		path)
    {
      std::ifstream		file(path.string.c_str());
      String			line;
      Settings::Section*	section;

      enter();

      // initialize the section pointer.
      section = NULL;

      // for every line of the file.
      while (std::getline(file, line))
	{
	  String		name;
	  String		value;
	  Natural32		position;

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
	      String		identifier;

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

      leave();
    }

    ///
    /// this method stores a setting into the given file.
    ///
    Status		Settings::Store(const Path&		path) const
    {
      std::ofstream	file(path.string.c_str());
      Settings::Scoutor	i;

      enter();

      // go through the sections.
      for (i = this->sections.begin();
	   i != this->sections.end();
	   i++)
	{
	  Settings::Section*		section = *i;
	  Settings::Section::Scoutor	j;

	  // write the section identifier.
	  file << "[" << section->identifier << "]" << std::endl;

	  // go through the assignments.
	  for (j = section->assignments.begin();
	       j != section->assignments.end();
	       j++)
	    {
	      Settings::Assignment*	assignment = *j;

	      // write the assignment.
	      file << assignment->name << " = "
		   << assignment->value << std::endl;
	    }
	}

      leave();
    }

    ///
    /// this method erases the settings file.
    ///
    Status		Settings::Erase(const Path&		path) const
    {
      enter();

      // erase the file.
      if (elle::File::Erase(path) == elle::StatusError)
	escape("unable to erase the file");

      leave();
    }

    ///
    /// this method tests the settings file.
    ///
    Status		Settings::Exist(const Path&		path) const
    {
      return (elle::File::Exist(path));
    }

  }
}
