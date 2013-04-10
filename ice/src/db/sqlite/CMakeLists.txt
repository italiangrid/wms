add_definitions(-Wall -O4 -DSQLITE_THREADSAFE=1
              -D_FILE_OFFSET_BITS=64
              -DSQLITE_ENABLE_MEMORY_MANAGEMENT=1
              -DSQLITE_ENABLE_UPDATE_DELETE_LIMIT=1
              -DSTDC_HEADERS=1
              -DHAVE_SYS_TYPES_H=1
              -DHAVE_SYS_STAT_H=1
              -DHAVE_STDLIB_H=1
              -DHAVE_STRING_H=1
              -DHAVE_MEMORY_H=1
              -DHAVE_STRINGS_H=1
              -DHAVE_INTTYPES_H=1
              -DHAVE_STDINT_H=1
              -DHAVE_UNISTD_H=1
              -DHAVE_DLFCN_H=1
              -DHAVE_FDATASYNC=1
              -DHAVE_USLEEP=1
              -DHAVE_LOCALTIME_R=1
              -DHAVE_GMTIME_R=1
              -DHAVE_READLINE=1
)

include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/src
		    ${Boost_INCLUDE_DIRS}
		    /usr/include)

add_custom_target(ICEDBSQLITE
  DEPENDS	
  sqlite3.c
)

add_library(sqlite3
  STATIC
  sqlite3.c
  )
