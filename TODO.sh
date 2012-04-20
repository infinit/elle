#!/bin/sh
# @file TODO.sh
# @author <raphael.londeix@gmail.com> Raphael Londeix
# @version @FIXME@

SCRIPT=`basename "$0"`
SCRIPTDIR=`python -c "import os;print(os.path.abspath(os.path.dirname('$0')))"`

prepare()
{
python3 << EOF

header = "$1".replace('./', '')
f_name = "${1%.*}Serializer.hxx".replace('./', '')
guard = f_name.replace('/', '_').replace('.', '_').upper()

print("#ifndef  %s" % guard)
print("# define %s" % guard)
print()
print("# include <cassert>")
print()
print("# include <elle/serialize/ArchiveSerializer.hxx>")
print()
print("# include <%s>" % header)
print()
print("ELLE_SERIALIZE_SIMPLE(%s," % header[:-3].replace('/', '::'))
print("                      archive,")
print("                      value,")
print("                      version)")
print("{")
print("  assert(version == 0);")
print()
print("  archive & value. ")
print("}")
print()
print("#endif")

EOF
}

for f in `cat TODO.serialize` ; do
	[ ! -f ${f%.*}Serializer.hxx ] && prepare $f > ${f%.*}Serializer.hxx
	echo "vim ${f%.*}Serializer.hxx ${f%.*}.cc ${f%.*}.hxx $f -p"
	vim ${f%.*}Serializer.hxx ${f%.*}.cc ${f%.*}.hxx $f -p
	prepare $f > ${f%.*}Serializer.hxx.initial
	[ "`cat ${f%.*}Serializer.hxx`" = "`cat ${f%.*}Serializer.hxx.initial`" ] && \
		rm ${f%.*}Serializer.hxx
	rm ${f%.*}Serializer.hxx.initial
	sleep 1
done

