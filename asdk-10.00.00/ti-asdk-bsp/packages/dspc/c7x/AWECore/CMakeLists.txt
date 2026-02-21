cmake_minimum_required(VERSION 3.10)
project(AWECoreExamples)

# Output executable goes in AWECore/Bin folder
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
include_directories(
	${CMAKE_SOURCE_DIR}/Include
	${CMAKE_SOURCE_DIR}/Designs
)

# Add import libraries
add_library(AWECoreStatic STATIC IMPORTED)
if (UNIX)
	set_target_properties(AWECoreStatic PROPERTIES
		IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/Lib/libAWECore.a)
else ()
	# MSVC
	set_target_properties(AWECoreStatic PROPERTIES
		IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/Lib/AWECore.lib)	
endif (UNIX)


# Build ControlApp 
add_executable(ControlApp 
	${CMAKE_SOURCE_DIR}/Source/ControlApp.c
	${CMAKE_SOURCE_DIR}/Designs/passthrough_InitAWB.c
)
target_link_libraries(ControlApp AWECoreStatic)


if (UNIX)
	if(QNX)
		set(SKIP_EXT_AUDIO_LIBS TRUE)
		set(NO_PTHREAD True)
	endif()	

	# Create LinuxApp if appropriate
	set(linuxAppSource
		${CMAKE_SOURCE_DIR}/Source/LinuxApp.c
		${CMAKE_SOURCE_DIR}/Source/AWECoreUtils.c
	)
	add_executable(LinuxApp ${linuxAppSource})
	target_link_libraries(LinuxApp AWECoreStatic m)
	if (NOT NO_PTHREAD)
		target_link_libraries(LinuxApp pthread)
	elseif (ALT_PTHREAD)
		target_link_libraries(LinuxApp ${ALT_PTHREAD})
	endif ()
	target_link_libraries(ControlApp m)	

	# Create LinuxAppMulti if appropriate
	set(linuxAppMultiSource
		${CMAKE_SOURCE_DIR}/Source/LinuxAppMulti.c
		${CMAKE_SOURCE_DIR}/Source/AWECoreUtils.c
	)
	add_executable(LinuxAppMulti ${linuxAppMultiSource})
	target_link_libraries(LinuxAppMulti AWECoreStatic m)
	if (NOT NO_PTHREAD)
		target_link_libraries(LinuxAppMulti pthread)
	elseif (ALT_PTHREAD)
		target_link_libraries(LinuxAppMulti ${ALT_PTHREAD})
	endif ()
	if (QNX)
		target_link_libraries(ControlApp socket)
		target_link_libraries(LinuxApp socket)
		target_link_libraries(LinuxAppMulti socket)	
	endif (QNX)

endif (UNIX)
