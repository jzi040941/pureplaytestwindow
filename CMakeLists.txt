
cmake_minimum_required (VERSION 3.8)

project ("pureplaytestwindow")

set(HEAD)
set(SRC)
set(INCL)
set(COMPILE_OPTION)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Sources.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/AudioDrive.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Libsamplerate.cmake)
list(APPEND INCL
	#${CMAKE_CURRENT_SOURCE_DIR}/algorithms
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/include/audio
	${CMAKE_CURRENT_SOURCE_DIR}/include/util
	${CMAKE_CURRENT_SOURCE_DIR}/lib
)

add_executable(${PROJECT_NAME} ${SRC} ${HEAD})
#add_executable (${PROJECT_NAME} "pureplaytestwindow.cpp")
target_include_directories(${PROJECT_NAME}
	PRIVATE
	${INCL}
  )
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_OPTION} )
target_link_libraries(${PROJECT_NAME} ${LINKLIBS})
set_target_properties(${PROJECT_NAME} PROPERTIES  CXX_STANDARD 14 )
# Add source to this project's executable.


# TODO: Add tests and install targets if needed.
