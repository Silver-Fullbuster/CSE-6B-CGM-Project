cmake_minimum_required(VERSION 3.0)
project(pong)

set(SDL2_Flags "-mwindows -Wl,--no-undefined -static-libgcc")

set(SDL2_Includes "${PROJECT_SOURCE_DIR}/include")
set(SDL2_LibDir   "${PROJECT_SOURCE_DIR}/lib")

add_library(SDL2     STATIC IMPORTED)
add_library(SDL2main STATIC IMPORTED)
set_property(TARGET SDL2     PROPERTY IMPORTED_LOCATION "${SDL2_LibDir}/libSDL2.a")
set_property(TARGET SDL2main PROPERTY IMPORTED_LOCATION "${SDL2_LibDir}/libSDL2main.a")

set(SDL2_Libs mingw32 SDL2 SDL2main m dinput8 dxguid dxerr8 user32 gdi32 winmm imm32 ole32 oleaut32 shell32 version uuid)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ${SDL2_Flags}")

set(SOURCE_FILES main.cpp)

add_executable(pong ${SOURCE_FILES})

target_include_directories(pong SYSTEM PRIVATE ${SDL2_Includes})
target_link_libraries(pong ${SDL2_Libs})