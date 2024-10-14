cmake_minimum_required(VERSION 3.15) # Проверка версии CMake.

# Проект
set(PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(PROJECT_NAME Windows)
#set(PROJECT_INCLUDE "${PROJECT_DIR}/Engine/Platform/Win/main.h")
#set(PROJECT_SOURCE "${PROJECT_DIR}/Engine/Platform/Win/main.cpp")

set(PROJECT_DIR_FILES "${PROJECT_DIR}/Engine/Platform/Win")
FILE(GLOB_RECURSE PROJECT_INCLUDE ${PROJECT_DIR_FILES}/*.h)
FILE(GLOB_RECURSE PROJECT_SOURCE ${PROJECT_DIR_FILES}/*.cpp)
source_group(TREE ${PROJECT_DIR_FILES} TREE PREFIX "" FILES ${PROJECT_INCLUDE} ${PROJECT_SOURCE})

# defines
add_definitions(-DWIN_PLATFORM)
#
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Библиотеки

# Engine
set(ENGINE_NAME Engine)
set(ENGINE_DIR "${PROJECT_DIR}/${ENGINE_NAME}")
set(ENGINE_SOURCE "${ENGINE_DIR}/Source")

# Game
set(GAME_NAME Game)
set(GAME_DIR "${PROJECT_DIR}/${GAME_NAME}")
set(GAME_SOURCE "${GAME_DIR}/Source")

# Resources
#set(RESOURCES_NAME Resources)
#set(RESOURCES_DIR "${PROJECT_DIR}/${RESOURCES_NAME}")
#set(RESOURCES_SOURCE_FILES "${RESOURCES_DIR}/Files")
#set(RESOURCES_SOURCE "${RESOURCES_DIR}")

# ThirdParty
#glm
set(GLM_INCLUDES "${ENGINE_DIR}/ThirdParty/glm")

# jsoncpp
set(JSONCPP_NAME jsoncpp)
set(JSONCPP_DIR "${ENGINE_DIR}/ThirdParty/jsoncpp")
set(JSONCPP_INCLUDES "${JSONCPP_DIR}/include")
set(JSONCPP_SOURCE "${JSONCPP_DIR}/src")
set(JSONCPP_LIB "${PROJECT_DIR}/../build/Win/Engine/ThirdParty/jsoncpp/src/lib_json")

FILE(GLOB_RECURSE JSONCPP_ALL_INCLUDES ${JSONCPP_INCLUDES}/*.h)
FILE(GLOB_RECURSE JSONCPP_ALL_SOURCES ${JSONCPP_SOURCE}/*.cpp)

# GLFW
set(GLFW_NAME glfw)
set(GLFW_DIR "${ENGINE_DIR}/ThirdParty/glfw")
set(GLFW_INCLUDES "${GLFW_DIR}/include")
set(GLFW_SOURCE "${GLFW_DIR}/src")
set(GLFW_LIB "${PROJECT_DIR}/../build/Win/Engine/ThirdParty/glfw/src")

FILE(GLOB_RECURSE GLFW_ALL_INCLUDES ${GLFW_INCLUDES}/*.h)
FILE(GLOB_RECURSE GLFW_ALL_SOURCES ${GLFW_SOURCE}/*.cpp)

# GLEW
set(GLEW_NAME glew)
#set(GLEW_S_NAME glew_s)
set(GLEW_DIR "${ENGINE_DIR}/ThirdParty/glew")
set(GLEW_CMAKE_DIR "${GLEW_DIR}/build/cmake")
set(GLEW_INCLUDES "${GLEW_DIR}/include")
set(GLEW_SOURCE "${GLEW_DIR}/src")
set(GLEW_LIB "${PROJECT_DIR}/../lib")

# FreeImage
set(FREEIMAGE_NAME FreeImage)
set(FREEIMAGE_DIR "${ENGINE_DIR}/ThirdParty/FreeImage")
set(FREEIMAGE_LIB "${FREEIMAGE_DIR}/build/Win32/Release")
set(FREEIMAGE_INCLUDES "${FREEIMAGE_DIR}/Source")

# PhysX 4.1
set(PHYSX_NAME PhysX)
set(PHYSX_COOKING PhysXCooking)
set(PHYSX_EXTENTIONS PhysXExtensions)
set(PHYSX_DIR "${ENGINE_DIR}/ThirdParty/PhysX")
set(PHYSX_INCLUDES "${PHYSX_DIR}/physx/include")
set(PHYSX_INCLUDES_PXSHARED "${PHYSX_DIR}/pxshared/include")
set(PHYSX_CMAKE_DIR "${PHYSX_DIR}/physx/compiler/public")
set(PHYSX_LIB "${PHYSX_DIR}/physx/bin")

# FreeType2
set(FREETYPE2_NAME freetype)
set(FREETYPE2_DIR "${ENGINE_DIR}/ThirdParty/freetype2")
set(FREETYPE2_INCLUDES "${FREETYPE2_DIR}/include")
set(GLFW_LIB "${PROJECT_DIR}/../build/Win/Engine/ThirdParty/freetype2")

# Создание

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_DIR}/../Executable)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_DIR}/../Executable)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_DIR}/../Executable)

project(${PROJECT_NAME})
add_executable(${PROJECT_NAME} ${PROJECT_SOURCE})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

# Библиотеки

# Engine
include_directories(${ENGINE_SOURCE})					# Заголовочные файлы библиотек
add_subdirectory(${ENGINE_DIR})							# Добавление подпроекта, указывается имя дирректории
target_link_libraries(${PROJECT_NAME} ${ENGINE_NAME})	# Линковка программы с библиотекой
link_directories(${ENGINE_DIR})

# Game
include_directories(${GAME_SOURCE})
add_subdirectory(${GAME_DIR})
target_link_libraries(${PROJECT_NAME} ${GAME_NAME})
link_directories(${GAME_DIR})

#Resources
#add_subdirectory(${RESOURCES_DIR})

#glm
include_directories(${GLM_INCLUDES})

# jsoncpp
include_directories(${JSONCPP_INCLUDES})
link_directories(${JSONCPP_LIB})
add_library(${JSONCPP_NAME} STATIC ${JSONCPP_ALL_SOURCES})
add_subdirectory(${JSONCPP_SOURCE})
target_link_libraries(${PROJECT_NAME} ${JSONCPP_NAME})

# glFw
include_directories(${GLFW_INCLUDES})
link_directories(${GLFW_LIB})
add_subdirectory(${GLFW_DIR})
target_link_libraries(${PROJECT_NAME} ${GLFW_NAME})

# glEw
include_directories(${GLEW_INCLUDES})
link_directories(${GLEW_LIB})
add_subdirectory(${GLEW_CMAKE_DIR})
set_target_properties( ${GLEW_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${PROJECT_DIR}/../Executable")
target_link_libraries(${PROJECT_NAME} ${GLEW_NAME})

# FreeImage
include_directories(${FREEIMAGE_INCLUDES})
link_directories(${FREEIMAGE_LIB})
target_link_libraries(${PROJECT_NAME} "${FREEIMAGE_LIB}/${FREEIMAGE_NAME}.lib")
file(COPY "${FREEIMAGE_LIB}/${FREEIMAGE_NAME}.dll" DESTINATION ${PROJECT_DIR}/../Executable/debug)

# PhysX 4.1
## I want to use the C++17 standard, forsooth!
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(TARGET_BUILD_PLATFORM "windows")
set(PX_GENERATE_STATIC_LIBRARIES true)

# PhysX-specific CMake project setup
set(NV_USE_DEBUG_WINCRT ON CACHE BOOL "Use the debug version of the CRT")
set(PHYSX_ROOT_DIR "${PHYSX_DIR}/physx")

set(PXSHARED_PATH ${PHYSX_ROOT_DIR}/../pxshared)
set(PXSHARED_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

set(CMAKEMODULES_VERSION "1.27")
set(CMAKEMODULES_PATH ${PHYSX_ROOT_DIR}/../externals/cmakemodules)
set(PX_OUTPUT_LIB_DIR ${PHYSX_ROOT_DIR}/externals/physx)
set(PX_OUTPUT_BIN_DIR ${PHYSX_ROOT_DIR}/externals/physx)

include_directories(${PHYSX_INCLUDES})
include_directories(${PHYSX_INCLUDES_PXSHARED})
link_directories(${PHYSX_LIB})
add_subdirectory(${PHYSX_ROOT_DIR}/compiler/public externals/physx)
target_link_libraries(${PROJECT_NAME} ${PHYSX_NAME})
target_link_libraries(${PROJECT_NAME} ${PHYSX_COOKING})
target_link_libraries(${PROJECT_NAME} ${PHYSX_EXTENTIONS})

# FreeType2
include_directories(${FREETYPE2_INCLUDES})
link_directories("D:/CMake/CMakeProject/build/Win/Engine/ThirdParty/freetype2/debug")
add_subdirectory(${FREETYPE2_DIR})
target_link_libraries(${PROJECT_NAME} ${FREETYPE2_NAME})
