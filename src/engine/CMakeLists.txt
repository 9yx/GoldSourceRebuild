#
#Hardware engine (OpenGL)
#

set( PROJECT_NAME hw )

#Add in the directories
add_sources(
	${SRC_DIR}/cl_dll/wrect.h
	${SRC_DIR}/common/GameUI/CareerDefs.h
	${SRC_DIR}/common/beamdef.h
	${SRC_DIR}/common/cl_entity.h
	${SRC_DIR}/common/com_model.h
	${SRC_DIR}/common/const.h
	${SRC_DIR}/common/crc.h
	${SRC_DIR}/common/CRegistry.cpp
	${SRC_DIR}/common/CRegistry.h
	${SRC_DIR}/common/cvardef.h
	${SRC_DIR}/common/dlight.h
	${SRC_DIR}/common/entity_state.h
	${SRC_DIR}/common/event_args.h
	${SRC_DIR}/common/event_flags.h
	${SRC_DIR}/common/FilePaths.cpp
	${SRC_DIR}/common/FilePaths.h
	${SRC_DIR}/common/in_buttons.h
	${SRC_DIR}/common/IRegistry.h
	${SRC_DIR}/common/netadr.h
	${SRC_DIR}/common/particledef.h
	${SRC_DIR}/common/pmtrace.h
	${SRC_DIR}/common/r_efx.h
	${SRC_DIR}/common/ref_params.h
	${SRC_DIR}/common/Sequence.h
	${SRC_DIR}/common/studio_event.h
	${SRC_DIR}/common/usercmd.h
	${SRC_DIR}/common/weaponinfo.h
	${SRC_DIR}/common/winheaders.h
	${SRC_DIR}/common/winsani_in.h
	${SRC_DIR}/common/winsani_out.h
	${SRC_DIR}/pm_shared/pm_debug.h
	${SRC_DIR}/pm_shared/pm_defs.h
	${SRC_DIR}/pm_shared/pm_info.h
	${SRC_DIR}/pm_shared/pm_materials.h
	${SRC_DIR}/pm_shared/pm_movevars.h
	${SRC_DIR}/public/engine_launcher_api.h
	${SRC_DIR}/public/interface.cpp
	${SRC_DIR}/public/interface.h
	${SRC_DIR}/public/keydefs.h
	${CMAKE_CURRENT_SOURCE_DIR}/vgui2/BaseUISurface.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/vgui2/BaseUISurface.h
	anorms.h
	APIProxy.h
	buildnum.cpp
	buildnum.h
	cdll_int.cpp
	cdll_int.h
	cl_main.cpp
	client.h
	cmd.cpp
	cmd.h
	common.cpp
	common.h
	console.cpp
	console.h
	custom.h
	customentity.h
	cvar.cpp
	cvar.h
	edict.h
	eiface.h
	filesystem.cpp
	filesystem.h
	filesystem_internal.cpp
	gl_screen.cpp
	gl_vidnt.cpp
	gl_warp.cpp
	host.cpp
	IEngine.h
	IGame.h
	info.cpp
	info.h
	keys.cpp
	keys.h
	mathlib.cpp
	mathlib.h
	mem.cpp
	mem.h
	modinfo.h
	net.h
	net_api.cpp
	pr_cmds.cpp
	pr_cmds.h
	progdefs.h
	progs.h
	protocol.h
	qgl.cpp
	qgl.h
	quakedef.h
	render.h
	shake.h
	snd.h
	snd_mix.cpp
	snd_sdl.cpp
	studio.h
	sv_log.cpp
	sv_log.h
	sv_main.cpp
	sv_main.h
	sv_steam3.cpp
	sv_steam3.h
	sys.h
	sys_dll.cpp
	sys_dll2.cpp
	sys_engine.cpp
	sys_getmodes.cpp
	sys_getmodes.h
	sys_sdlwind.cpp
	traceinit.cpp
	traceinit.h
	vgui_int.cpp
	vgui_int.h
	vgui_intwrap.cpp
	vgui_intwrap2.cpp
	vid.h
	zone.cpp
	zone.h
)

preprocess_sources()

add_library( ${PROJECT_NAME} SHARED ${PREP_SRCS} )

target_include_directories( ${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}
	${SRC_DIR}
	${SRC_DIR}/cl_dll
	${SRC_DIR}/common
	${SRC_DIR}/pm_shared/
	${SRC_DIR}/public
	${SRC_DIR}/public/tier1
	${SHARED_INCLUDE_PATHS}
	${EXTERNAL_DIR}/SDL2/include
	${EXTERNAL_DIR}/GLEW/include
	${OPENGL_INCLUDE_DIR}
	${SHARED_EXTERNAL_INCLUDE_PATHS}
)

target_compile_definitions( ${PROJECT_NAME} PRIVATE
	${SHARED_DEFS}
	USE_BREAKPAD_HANDLER
	GLEW_STATIC
)

#Find dependencies
set( PROJECT_LIBS )
if( WIN32 )
	set( PROJECT_LIBS ${PROJECT_LIBS} wsock32 ws2_32 )
endif()

#Link with dependencies
target_link_libraries( ${PROJECT_NAME} 
	${SDL2}
	${GLEW2}
	${OPENGL_LIBRARIES}
	tier1
	tier0
	${STEAM_API}
	${PROJECT_LIBS}
	${SHARED_DEPENDENCIES}
)

#CMake places libraries in /Debug or /Release on Windows, so explicitly set the paths for both.
#On Linux, it uses LIBRARY_OUTPUT_DIRECTORY
set_target_properties( ${PROJECT_NAME} PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY "${ENGINE_BASE_PATH}"
	RUNTIME_OUTPUT_DIRECTORY_DEBUG "${ENGINE_BASE_PATH}"
	RUNTIME_OUTPUT_DIRECTORY_RELEASE "${ENGINE_BASE_PATH}"
)

set( PROJECT_LINK_FLAGS )

if( NOT MSVC AND NOT APPLE )
	#Generate a map file that provides information about the linking stage.
	set( PROJECT_LINK_FLAGS
		${PROJECT_LINK_FLAGS} "-Wl,-Map,${PROJECT_NAME}_map.txt "
	)
endif()

set_target_properties( ${PROJECT_NAME} PROPERTIES
	COMPILE_FLAGS "${LINUX_32BIT_FLAG}"
	LINK_FLAGS "${SHARED_ENGINE_LINKER_FLAGS} ${PROJECT_LINK_FLAGS} ${LINUX_32BIT_FLAG}"
)

#No lib prefix
set_target_properties( ${PROJECT_NAME} PROPERTIES PREFIX "" )

#Create filters
create_source_groups( "${CMAKE_SOURCE_DIR}" )

clear_sources()

#
#End library
#