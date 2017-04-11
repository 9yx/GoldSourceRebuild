###################################################
#                                                 #
#                                                 #
#   GoldSource Rebuild CMake build file           #
#                                                 #
#                                                 #
###################################################
cmake_minimum_required( VERSION 3.6 )

include( cmake/InputFilesList.cmake )
include( cmake/MSVCRuntime.cmake )

project( GoldSourceRebuild )

include( cmake/WinXPSupport.cmake )

#Needs static CRT to avoid alloc issues.
set( MSVC_RUNTIME "static" )

configure_msvc_runtime()

set(variables
  CMAKE_C_FLAGS_DEBUG
  CMAKE_C_FLAGS_MINSIZEREL
  CMAKE_C_FLAGS_RELEASE
  CMAKE_C_FLAGS_RELWITHDEBINFO
  CMAKE_CXX_FLAGS_DEBUG
  CMAKE_CXX_FLAGS_MINSIZEREL
  CMAKE_CXX_FLAGS_RELEASE
  CMAKE_CXX_FLAGS_RELWITHDEBINFO
)
message(STATUS "Initial build flags:")
foreach(variable ${variables})
  message(STATUS "  '${variable}': ${${variable}}")
endforeach()
message(STATUS "")

#C++14 support
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y" )

if( UNIX )
	#Additional debug info for GDB.
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g" )
endif()

#Base dir for engine outputs
set( ENGINE_BASE_PATH "${CMAKE_SOURCE_DIR}/test_game" CACHE PATH "Path to deploy binaries to. Equivalent to common/Half-Life, but defaults to test_game for testing only" )
set( GAME_BASE_PATH "${ENGINE_BASE_PATH}/valve" )

MESSAGE( STATUS "Base Engine Path: ${ENGINE_BASE_PATH}" )
MESSAGE( STATUS "Base Game Path: ${GAME_BASE_PATH}" )

set( SRC_DIR ${CMAKE_SOURCE_DIR}/src )
set( EXTERNAL_DIR ${SRC_DIR}/external )

if( WIN32 )
	set( LIB_DIR ${SRC_DIR}/lib/public )
	set( BIN_DIR ${SRC_DIR}/bin/public )
elseif( APPLE )
	set( LIB_DIR ${SRC_DIR}/lib/osx32 )
	set( BIN_DIR ${LIB_DIR} )
elseif( UNIX )
	set( LIB_DIR ${SRC_DIR}/lib/linux32 )
	set( BIN_DIR ${LIB_DIR} )
else()
	set( LIB_DIR )
	set( BIN_DIR ${LIB_DIR} )
	MESSAGE( WARNING "Unknown target platform; cannot determine library directory" )
endif()

#Shared preprocessor definitions
set( SHARED_DEFS 
	NO_MALLOC_OVERRIDE
)

if( MSVC )
	set( SHARED_DEFS
		${SHARED_DEFS}
		_CRT_SECURE_NO_WARNINGS
		_SCL_SECURE_NO_WARNINGS
	)
	
	set( LINUX_32BIT_FLAG "" )
else()
	#All non-MSVC targets need this. That includes Cygwin and Mingw.
	#Match Source 2013 in defining GNUC for UNIX platforms.
	#Match Source 2013 in defining POSIX for UNIX platforms.
	#Enable 64 bit fopen functions for use in the filesystem.
	set( SHARED_DEFS
		${SHARED_DEFS}
		GNUC
		POSIX
		_LARGEFILE_SOURCE
		_LARGEFILE64_SOURCE
	)

	if( APPLE )
		#Define OSX so code can check for it.
		set( SHARED_DEFS
			${SHARED_DEFS}
			OSX
		)
	endif()

	if( UNIX )
		#Define LINUX so code can check for it.
		#Define _LINUX for Source SDK dependencies.
		set( SHARED_DEFS
			${SHARED_DEFS}
			LINUX
			_LINUX
		)
	endif()

	set( LINUX_32BIT_FLAG "-m32" )
endif()

if( MSVC )
	#Set Windows subsystem
	set( SHARED_ENGINE_LINKER_FLAGS
		${SHARED_ENGINE_LINKER_FLAGS} "/SUBSYSTEM:WINDOWS "
)
elseif( UNIX )
	#From the Github 2013 Makefile, match the settings for Unix environments.
	# force 387 for FP math so the precision between win32 and linux and osx match
	# Trigger an error if any code tries to use an implicit return type
	# Default visibility is hidden unless explicitly altered with __attribute__( visibility() )
	set( SHARED_ENGINE_LINKER_FLAGS
		${SHARED_ENGINE_LINKER_FLAGS} "-mfpmath=387 -Werror=return-type -fvisibility=hidden "
	)

	if( APPLE )
		set( SHARED_ENGINE_LINKER_FLAGS
			${SHARED_ENGINE_LINKER_FLAGS} "-momit-leaf-frame-pointer -mtune=core2 "
		)
	else()
		#Linux, Cygwin, etc.
	endif()
endif()

#Shared directories
#Include paths for external dependencies. Should always come after all project paths.
set( SHARED_EXTERNAL_INCLUDE_PATHS
)

set( SHARED_INCLUDE_PATHS
)

#Find shared dependencies
set( THREADS_PREFER_PTHREAD_FLAG ON )
find_package( Threads REQUIRED )

if( UNIX )
	set( UNIX_FS_LIB stdc++fs )
else()
	set( UNIX_FS_LIB )
endif()

set( STEAM_API_SO ${BIN_DIR}/steam_api${CMAKE_SHARED_LIBRARY_SUFFIX} )

if( WIN32 )
	set( SDL2_NAME SDL2${CMAKE_STATIC_LIBRARY_SUFFIX} )
	set( SDL2_SO_NAME SDL2${CMAKE_SHARED_LIBRARY_SUFFIX} )
elseif( APPLE )
	set( SDL2_NAME libSDL2-2.0.0.dylib )
	set( SDL2_SO_NAME ${SDL2_NAME} )
elseif( UNIX )
	set( SDL2_NAME libSDL2-2.0.so.0 )
	set( SDL2_SO_NAME ${SDL2_NAME} )
else()
	set( SDL2_NAME )
	set( SDL2_SO_NAME ${SDL2_NAME} )
endif()

set( SDL2_SO ${EXTERNAL_DIR}/SDL2/lib/${SDL2_SO_NAME} )

#Copy libraries so they can be linked properly.
set( LIBS_TO_COPY ${STEAM_API_SO} ${SDL2_SO} )

foreach( lib IN LISTS LIBS_TO_COPY )
	MESSAGE( STATUS "Copying ${lib}" )
	file( COPY ${lib} DESTINATION ${ENGINE_BASE_PATH} )
endforeach()

#Link with the library in the deployment directory if present, this is needed to ensure that the correct library file is loaded.
find_library( STEAM_API steam_api PATHS ${ENGINE_BASE_PATH} ${LIB_DIR} NO_DEFAULT_PATH )
find_library( SDL2 ${SDL2_NAME} PATHS ${ENGINE_BASE_PATH} ${EXTERNAL_DIR}/SDL2/lib NO_DEFAULT_PATH )

add_subdirectory( src )
