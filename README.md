# <img src="docs/static_files/drake_logotype@2x.png" alt="Drake" title="Drake logotype" width="300" style="max-width:300px;">

The well-formed build system.

[![pipeline status](https://gitlab.gruntech.org/mefyl/drake/badges/master/pipeline.svg)](https://gitlab.gruntech.org/mefyl/drake/commits/master) [![coverage report](https://gitlab.gruntech.org/infinit/drake/badges/master/coverage.svg)](https://gitlab.gruntech.org/infinit/drake/commits/master)

## About drake

Drake is a build system written in Python aiming at expressivity,
consistency and extensibility.

In a nutshell:
* The declaration of the dependency graph is enforced and separated
  from the build command, creating a streamlined build process.
* Build files are plain Python, enabling you to stay declarative or
  leverage the full power of the language if needed.
* Support for new build processes can easily be added, no matter how
  complex.
* Nested projects are merged in a single graph, enabling rich
  dependency exploration.

## Project status

Drake is a ~~one man~~ few people project, initially used as an
internal tool. The core itself is still designed in a clean and
extensible well suited for collaboration. What can be lacking is
language support, which need to be implemented. So far the main
supported languages are:

* C++
* OCaml
* Python
* Go
* GNU autotools projects

Several convenience tools also have good support, such as Mako
templating, Debian packaging or Docker.

## Is drake for me ?

If you want a streamlined experience with a widely used, well
supported build system, then drake might not (yet) be for you.

If existing solution do not satisfy you, and you are ready or need to
implement the build steps you need, then drake might be the perfect
base to build upon. More so if the technology you use is one of the
already well supported one.

## Requirements

Drake requires *Python3* along with the following modules:
- [greenlet](https://pypi.python.org/pypi/greenlet) *(Micro-threads)*.
- [orderedset](https://pypi.python.org/pypi/orderedset) *(Set that remembers original insertion order)*.

## Installation

Drake is bootstrapped. From the root of the repository, simply run:

```bash
src/bin/drake --prefix=/usr/local //install
```

## Quickstart

Basic knowledge of Python 3 is assumed before using Drake. This
section stays above the hood to give a quick functional overview of
drake. If anything seems magical, it really isn't, we are just using
all the available shortcut to keep things as simple as
possible. Further section dive deeper into the actual interfaces.

### Minimal setup

The simplest setup to build with drake is a `drakefile`, which is a
python file, that contains a `configure` function. This function
defines all the buildable things, the structure of you project. Here
is the empty `drakefile`:

```python
def configure():
  pass
```

Invoking `drake` in this directory will build everything - which is,
exactly nothing.

```bash
$ drake
drake: Entering directory ...
drake: Leaving directory ...
```

### Nodes

In drake, anything that can be built is called a node. This is often a
file, the produce of a compilation for instance, but it can really be
anything: a Docker image, a database, a remote upload ...

The simplest way to create a node is through the `drake.node`
function, passing it the path to the node.

```python
import drake

def configure():
  n = drake.node('file')
  print('this is my node:', n)
```

Since this is plain Python, we can do pretty much anything, including
printing to see what it looks like. While drakefiles tend to be mostly
declarative, this can be useful. If we run `drake` now:

```bash
$ drake
this is my node: file
drake: Entering directory ...
drake: Leaving directory ...
drake: error: no builder to make file
[1]
```

Drake first ran the configure, showing our `print` statement. It then
went on to build everything, which is, our `file` node. Unfortunately
we never specified how to do so, hence the "no builder to make file"
error.

### Builders

In drake, a builder is an operations that takes zero or more nodes as
an input, and generate zero or more node as an output. Typically for
instance, a builder would compile some C++ source files into a binary
object file. This shed some light on the previous error message: we
asked to build `file`, but there is no builder for it and Drake can't
fulfill our request.

To fix this, we can use one of the simplest builder there is: the
touch builder. It has no source and simply create its target as empty
files, similarly to the `touch (1)` Unix utility. The `drake.touch`
convenience function is the fastest way to declare a touch builder:

```python
import drake

def configure():
  n = drake.node('file')
  drake.touch(n)
  print('this is my node:', n)
```

```bash
$ drake
print('this is my node:', n)
drake: Entering directory ...
Touch file
drake: Leaving directory ...
$ ls
drakefile  file
$ cat file
$
```

Running drake now creates `file` as expected. It is important to
understand that the `configure` function itself did not touch the
file. While `touch(n)` sounds imperative for concision, what it really
means is "the node `n` can be created by touching it", not "touch `n`
right now". The print statement was left to illustrate this: we first
run the `configure` entirely, as the output proves, and it's when we
actually build our project that drake will use the information
provided to actually run some commands. This is important because we
could, for instance, build some specific nodes instead of all of them,
and `file` would thus not necessarily be touched. More on that pretty
soon.

Being a build system, Drake tracks dependencies to rerun builders only
if needed. Since the creation of `file` is not dependent on anything,
and it is already created, re-running Drake will do nothing:

```bash
$ drake
drake: Entering directory ...
drake: Leaving directory ...
```

Another simple builder is the copy builder. The `drake.copy` helper
let you instantiate it to copy file(s) in a directory:

```python
import drake

def configure():
  n = drake.node('file')
  drake.touch(n)
  drake.copy(n, 'subdir')
```

```bash
$ drake
drake: Entering directory ...
Copy subdir/file
drake: Leaving directory ...
$ cat subdir/file
$
```

If we remove the created files, we can see that drake runs all the
actions in the correct order to recreate everything:

```bash
$ rm file subdir/file
$ drake
drake: Entering directory ...
Touch file
Copy subdir/file
drake: Leaving directory ...
```

### Source files and built files

So far we have only created files out of thin air, but real project
start with some files that get transformed. Drake calls files provided
to it "source nodes", and nodes produces by some builder "built
nodes". As the name indicate though, both are nodes, the only
difference being whether they have a builder or not. Let us now
provide `file` as a source node with some content instead of touching
it.

```python
import drake

def configure():
  n = drake.node('file')
  drake.copy(n, 'subdir')
```

In this setup, `file` must be present or drake will complain once more
that it has no builder. If we now run drake:

```bash
$ ls
drakefile  file
$ cat file
contents
$ drake
drake: Entering directory ...
Copy subdir/file
drake: Leaving directory ...
$ cat subdir/file
contents
$
```

As a rule of thumb, any (useful) node will always either have a
builder, either exist before running drake. Now, it might seem a bit
strange to insist on this difference, while the result is pretty much
the same. It has however its importance as the upcoming points will
show.

```bash
$ drake --clean
```

### Cleaning and other modes

We've said so far that invoking `drake` builds every node it knows
about. That's however just a default. Drake is in "build" mode by
default, but other exist. For instance, the "clean" mode is the
reciprocate, where any built file will be removed.

```bash
$ drake --build # this is the same as plain "drake"
drake: Entering directory ...
Copy subdir/file
drake: Leaving directory ...
$ drake --clean
drake: Entering directory ...
Remove subdir/file
drake: Leaving directory ...
$
```

We can see here a first difference between source nodes and built
nodes: `file` was not removed as it is a source node - we wouldn't
want drake to erase our source code.

### Build tree and source tree

Source tree and build tree are a common concept, where built files go
in a different folder -- the build tree -- that the source directories
-- the source tree. This has several advantages, notably:

* Keeping the sources clean.
* Enable to have several build tree for the same source tree -
  eg. compiled with different options.
* Avoid mistakes where built files could overwrite source files.

While Drake does support "source = build" -- what we used so far -- it
is generally considered good practice to separate them. This can be
achieved with the `--workdir` option:

```bash
$ drake --workdir _build
drake: Entering directory ...
Copy subdir/file
drake: Leaving directory ...
```

While the output of drake stays neutral no matter what the build
directory is set to, the copy really was made in "_build":

```bash
$ ls
_build  drakefile  file
$ ls _build/subdir
file
$
```

This underline another key difference between source nodes and built
nodes: the first one live in the source tree, while the latter live in
the build tree.

### Building specific nodes

So far we have always asked Drake to build "everything", it is however
possible to instead pass the list of node we want to build. If we
consider the following drakefile:

```python
import drake

def configure():
  n = drake.node('file')
  drake.touch(n)
  drake.copy(n, 'foo')
  drake.copy(n, 'bar')
```

We can build only one those files specifically:

```bash
$ drake --workdir _build foo/file
drake: Entering directory ...
Touch file
Copy foo/file
drake: Leaving directory ...
$ ls _build/
file  foo
$
```

Drake only built the node we asked for -- and its dependencies as
prerequisites of course. Note that the name of the nodes are
independent from the potential build tree, i.e. we built `foo/file`,
not `_build/foo/file`. If we ask Drake to build the rest of our
project it will now run only the missing operations to do so:

```bash
$ drake --workdir _build
drake: Entering directory ...
Copy bar/file
drake: Leaving directory ...
$
```

### Rules and non-file nodes

As previously hinted, most nodes are files, but they can really be
anything that can be built. The most basic example of this is the
`Rule` node, which represents nothing and takes no action to
build. Its only use is to fit it with dependencies, so that building
it will build all of the dependencies. This is really about grouping
nodes under a common convenience shortcut as per the `make` concept of
rules, e.g. `make all`, `make check`, ...

```python
import drake

def configure():
  # drake.nodes is like drake.node, but with multiple arguments
  a, b, c = drake.nodes('a', 'b', 'c')
  drake.touch(a)
  drake.touch(b)
  drake.touch(c)
  drake.Rule('myrule', [a, c])
```

There's a trick to invoking the rule : any node not representing a
file in Drake has its name prefixed with "//". This is to ensure there
can never be a collision between such a "virtual" node and file on
disk (avoiding `.PHONY`, if you catch my drift). That explains the
"//install" to install Drake in the first place !

```bash
$ drake --workdir _build //myrule
a b c
drake: Entering directory ...
Touch a
Touch c
drake: Leaving directory ...
$
```

Rules are often used to //install, //check the project, ...

### Configure arguments

Some builds being parametric, Drake support passing arguments to
configure. These parameters are then reflected in your drakefile help
message, and can be passed from the command line. A common usage is
for instance to define an installation prefix.


```python
import drake

def configure(prefix = '/usr/local'):
  n = drake.node('f')
  drake.Rule('install', drake.copy(n, prefix))
```

```bash
$ drake
drake: Entering directory ...
Copy /usr/local/f
copy of /usr/local/f: [Errno 13] Permission denied: '/usr/local/f'
drake: *** copy of /usr/local/f failed: [Errno 13] Permission denied: '/usr/local/f'
drake: Leaving directory ...
$ drake --prefix=/tmp
drake: Entering directory ...
Copy /tmp/f
drake: Leaving directory ...
$
```

### That's it for a quick overview of Drake !

We didn't build anything complex yet, but that's what drake is
though : a framework, a library really, providing a model to describe
builds and an engine to run them. Anything else is defined one layer
above and not technically core Drake. In a sense, even the touch and
copy builders we've seen are not core, and simply part of the
"standard Drake library" of common builders.

Drake does however come with subpackages to build specific stuff, like
`drake.cxx` to build C/C++ projects. If a library exist for you
purpose, you may proceed to its quickstart below, or you may refer to
the [drake extension guide] if you want to implement your own specific
commands.

### Further specific quickstarts

* [C++ compilation quickstart]
* [OCaml compilation quickstart]
* [Go compilation quickstart]


## Helpful Environment Variables

**DRAKE_RAW**

Setting `DRAKE_RAW=1` in your environment will cause Drake to output
the raw commands that it calls.

**DRAKE_DEBUG_BACKTRACE**

Setting `DRAKE_DEBUG_BACKTRACE=1` in your environment will cause Drake
to output a backtrace if there is an issue with your build.
