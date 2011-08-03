PROJECT(Etoile CXX)

ADD_LIBRARY(etoile
#  path/Cache.cc
#  path/Item.cc
  path/Path.cc
  path/Route.cc
  path/Chemin.cc
  path/Venue.cc
  path/Way.cc

#  depot/Cell.cc
  depot/Depot.cc
  depot/Hole.cc
#  depot/Record.cc
#  depot/Repository.cc
#  depot/Unit.cc

  gear/Action.cc
  gear/Actor.cc
  gear/Chronicle.cc
  gear/Context.cc
  gear/Directory.cc
  gear/File.cc
  gear/Gear.cc
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
  wall/Wall.cc

  miscellaneous/Information.cc

  journal/Journal.cc

  Etoile.cc
)

TARGET_LINK_LIBRARIES(etoile elle nucleus lune agent)
