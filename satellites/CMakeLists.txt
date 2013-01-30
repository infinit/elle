set(LIST_OF_SATELLITES
        "access;authority;dictionary;group;network;passport;progress;setup;transfer;user"
        CACHE STRING "list of satellites")

foreach(SATELLITE IN LISTS LIST_OF_SATELLITES)
    message(STATUS "enable 8${SATELLITE} compilation")
    add_subdirectory(${SATELLITE})
endforeach()
