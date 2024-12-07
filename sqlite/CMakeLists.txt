cmake_minimum_required(VERSION 3.0)

project(sqlite3-cmake)

option(SQLITE_THREADSAFE "Build thread safe version" ON)

add_executable(sqlite sqlite3.c shell.c)
add_library(sqlite3 STATIC sqlite3.c)

add_definitions(-DSQLITE_ENABLE_RTREE)
add_definitions(-DSQLITE_ENABLE_FTS4)
add_definitions(-DSQLITE_ENABLE_FTS5)
add_definitions(-DSQLITE_ENABLE_JSON1)
add_definitions(-DSQLITE_ENABLE_RBU)
add_definitions(-DSQLITE_ENABLE_STAT4)

if(${SQLITE_THREADSAFE})
    # thread safe version
else()
    add_definitions(-DSQLITE_THREADSAFE=0)
endif()

target_include_directories(sqlite PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(sqlite3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
  add_custom_command(TARGET sqlite POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sqlite> ${CMAKE_BINARY_DIR}/sqlite3.exe
    DEPENDS sqlite
  )
  install(FILES ${CMAKE_BINARY_DIR}/sqlite3.exe DESTINATION bin)
else()
  include(FindThreads)
  target_link_libraries(sqlite m ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS})
  install(TARGETS sqlite RUNTIME DESTINATION bin)
endif()

install(FILES src/sqlite3.h src/sqlite3ext.h DESTINATION include)
install(TARGETS sqlite3 LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)