# Network block device

This is an implementation of the
[NBD protocol](https://nbd.sourceforge.io) on top of
[elle::reactor](../src/elle/reactor). One may implement any block
device backend by subclassing `elle::nbd::Server::Device` and
implementing the relevant methods, and adding them to a
`elle::nbd::Server`. An example implementation over a file is provided
in `bin/nbd-file.cc`.
