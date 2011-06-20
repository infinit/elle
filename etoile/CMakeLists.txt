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

  gear/Identifier.cc

#  automaton/Automaton.cc
#  automaton/Directory.cc
#  automaton/File.cc
#  automaton/Link.cc
#  automaton/Object.cc

  wall/Attributes.cc
  wall/Access.cc
  wall/Directory.cc
  wall/File.cc
  wall/Link.cc
  wall/Object.cc
  wall/Path.cc
#  wall/User.cc
  wall/Wall.cc

  miscellaneous/Information.cc

  Etoile.cc
)

TARGET_LINK_LIBRARIES(etoile elle nucleus lune)
