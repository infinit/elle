project(Etoile CXX)

add_library(etoile ${LINK_METHOD}
  path/Path.cc
  path/Route.cc
  path/Chemin.cc
  path/Venue.cc
  path/Way.cc

  depot/Depot.cc

  gear/Actor.cc
  gear/Context.cc
  gear/Directory.cc
  gear/File.cc
  gear/Gear.cc
  gear/Group.cc
  gear/Guard.cc
  gear/Identifier.cc
  gear/Link.cc
  gear/Object.cc
  gear/Scope.cc

  automaton/Access.cc
  automaton/Attributes.cc
  automaton/Author.cc
  automaton/Directory.cc
  automaton/Ensemble.cc
  automaton/File.cc
  automaton/Group.cc
  automaton/Link.cc
  automaton/Object.cc
  automaton/Rights.cc

  wall/Attributes.cc
  wall/Access.cc
  wall/Directory.cc
  wall/File.cc
  wall/Group.cc
  wall/Link.cc
  wall/Object.cc
  wall/Path.cc

  portal/Application.cc
  portal/Manifest.cc
  portal/Portal.cc

  abstract/Object.cc
  abstract/Group.cc

  journal/Journal.cc

  shrub/Shrub.cc
  shrub/Riffle.cc

#  nest/Nest.cc
#  nest/Pod.cc

  Etoile.cc
)
