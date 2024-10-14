cmake_minimum_required(VERSION 3.8)

# Переменные
set(CMAKE_CXX_STANDARD 20)

# Расположение файлов библиотеки
FILE(GLOB_RECURSE INCLUDES ${GAME_SOURCE}/*.h)
FILE(GLOB_RECURSE SOURCES ${GAME_SOURCE}/*.cpp)
source_group(TREE ${GAME_SOURCE} TREE PREFIX "" FILES ${INCLUDES} ${SOURCES})

# Расположение заголовочных файлов
include_directories(${ENGINE_SOURCE})

# ThirdPathy
include_directories(${GLM_INCLUDES})
include_directories(${JSONCPP_INCLUDES})
include_directories(${GLFW_INCLUDES})
include_directories(${GLAD_INCLUDES})
include_directories(${FREEIMAGE_INCLUDES})
include_directories(${IMGUI_INCLUDES})
include_directories(${FREETYPE2_INCLUDES})
# CUDA include_directories(${CUDA_SOURCE})

if(ENABLE_PHYSIC)
	include_directories(${PHYSX_INCLUDES})
	include_directories(${PHYSX_INCLUDES_PXSHARED})
endif()

# Создание статической библиотеки
add_library(${GAME_NAME} STATIC ${INCLUDES} ${SOURCES})
