#!/bin/sh
# @file TODO.sh
# @author <raphael.londeix@gmail.com> Raphael Londeix
# @version @FIXME@

SCRIPT=`basename "$0"`
SCRIPTDIR=`python -c "import os;print(os.path.abspath(os.path.dirname('$0')))"`

prepare()
{
python3 << EOF > $1

f_name = "$1".replace('./', '')
guard = f_name.replace('/', '_').replace('.', '_').upper()

print("#ifndef  %s" % guard)
print("# define %s" % guard)
print()
print("# include <cassert>")
print()
print("# include <elle/serialize/ArchiveSerializer.hxx>")
print()
print("# include <%s>" % "$2".replace('./', ''))
print()
print("ELLE_SERIALIZE_SIMPLE(%s," % f_name[:-4].replace('/', '::'))
print("                      archive,")
print("                      value,")
print("                      version)")
print("{")
print("  assert(version == 0);")
print("  ")
print("}")
print()
print("#endif")


EOF
}

for f in `cat TODO.serialize` ; do
	[ ! -f ${f%.*}Serializer.hxx ] && prepare ${f%.*}Serializer.hxx $f
	vim ${f%.*}Serializer.hxx ${f%.*}.cc ${f%.*}.hxx $f -p
	sleep 2
done

