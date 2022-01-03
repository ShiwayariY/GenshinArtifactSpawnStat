cmake_minimum_required(VERSION 3.17)
project(GenshinArtifactSpawnStat)

set(BUILD_SFX "${PLATFORM}/${CMAKE_BUILD_TYPE}")
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}" CACHE PATH "CMake install prefix" FORCE)
endif()

add_compile_options(-Wall -Wextra -pedantic)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(other/cpr)


set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
cmake_policy(SET CMP0100 NEW)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

include_directories(
	include
)

add_executable(GenshinArtifactSpawnStat WIN32
	src/main.cc
	include/AppWindow.hh
	src/AppWindow.cc
	include/InvestigationEntry.hh
	src/InvestigationEntry.cc
	include/DropSelectHandler.hh
	src/DropSelectHandler.cc
	resource.qrc
	other/BreezeStyleSheets/breeze.qrc
	resource.rc
)

target_link_libraries(GenshinArtifactSpawnStat
	Qt::Widgets
	cpr::cpr
)

install(TARGETS
	GenshinArtifactSpawnStat
RUNTIME DESTINATION "bin/${BUILD_SFX}")
