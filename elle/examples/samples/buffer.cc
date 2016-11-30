#include <elle/Buffer.hh>
#include <elle/attribute.hh>

int
main()
{
  // Construct a Buffer.
  elle::Buffer buffer("something interesting");
  ELLE_ASSERT_EQ(buffer[3], 'e');

  // Construct a ConstWeakBuffer on a slice of the original buffer memory.
  elle::ConstWeakBuffer slice(buffer.contents() + 4, 2);
  ELLE_ASSERT_EQ(slice.string(), "th");

  // Construct a WeakBuffer on another slice of the original buffer memory.
  elle::WeakBuffer mslice(buffer.contents() + 6, 2);
  ELLE_ASSERT_EQ(mslice.string(), "in");
  // Edit the memory.
  mslice[0] = 'o';
  ELLE_ASSERT_EQ(mslice.string(), "on");

  // The original buffer benefits from the change done previously.
  ELLE_ASSERT_EQ(buffer[6], 'o');
  ELLE_ASSERT_EQ(buffer.string(), "somethong interesting");
  return 0;
}
