cmake_minimum_required(VERSION 3.17)
project(GenshinArtifactSpawnStat)

add_compile_options(-Wall -Wextra -pedantic)
set(CMAKE_CXX_STANDARD 17)
set(BUILD_SFX "${PLATFORM}/${CMAKE_BUILD_TYPE}")

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
	../RecentFileExplorer/BreezeStyleSheets/breeze.qrc
)

target_link_libraries(GenshinArtifactSpawnStat
	Qt::Widgets
)

install(TARGETS
	GenshinArtifactSpawnStat
DESTINATION "${PROJECT_SOURCE_DIR}/bin/${BUILD_SFX}")
