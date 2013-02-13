#ifndef  ELLE_SERIALIZE_INSERT_HH
# define ELLE_SERIALIZE_INSERT_HH

# include "BinaryArchive.hh"

namespace elle
{
	namespace serialize
	{

		template <typename Archive>
		class Insertor;

		template <typename Archive>
		class StringInsertor;

		/// Create an output archive that serialize to a file.
		template <typename Archive = OutputBinaryArchive>
		Insertor<Archive>
		to_file(std::string const& path);

		/// Create an output archive that deserialize to a string.
		template <typename Archive = OutputBinaryArchive>
		StringInsertor<Archive>
		to_string(std::string& str);

	}
}

# include "insert.hxx"

#endif
