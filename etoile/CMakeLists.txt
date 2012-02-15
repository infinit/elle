PROJECT(Etoile CXX)

ADD_LIBRARY(etoile
  path/Path.cc
  path/Route.cc
  path/Chemin.cc
  path/Venue.cc
  path/Way.cc

  depot/Depot.cc

  gear/Action.cc
  gear/Actor.cc
  gear/Context.cc
  gear/Directory.cc
  gear/File.cc
  gear/Gear.cc
  gear/Guard.cc
  gear/Identifier.cc
  gear/Link.cc
  gear/Object.cc
  gear/Scope.cc
  gear/Transcript.cc

  automaton/Access.cc
  automaton/Attributes.cc
  automaton/Author.cc
  automaton/Directory.cc
  automaton/File.cc
  automaton/Link.cc
  automaton/Object.cc
  automaton/Rights.cc

  wall/Attributes.cc
  wall/Access.cc
  wall/Directory.cc
  wall/File.cc
  wall/Link.cc
  wall/Object.cc
  wall/Path.cc

  portal/Application.cc
  portal/Portal.cc
  portal/Wrapper.cc

  miscellaneous/Abstract.cc

  journal/Journal.cc

  shrub/Shrub.cc
  shrub/Riffle.cc

  Etoile.cc
)

TARGET_LINK_LIBRARIES(etoile nucleus lune agent)
