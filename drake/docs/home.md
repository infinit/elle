# <img src="static_files/drake_logotype@2x.png" alt="Drake" title="Drake" width="300" style="max-width:300px;">

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

## Getting started

The [quickstart](docs/quickstarts/quickstart.md) will give you
the minimal operational understanding of Drake.
