cmake_minimum_required(VERSION 3.16)
project(ProjetoWAR VERSION 2.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(war_core
    src/Territorio.cpp src/Continente.cpp src/Mapa.cpp
    src/Objetivo.cpp src/Carta.cpp src/Jogador.cpp src/Jogo.cpp
)
target_include_directories(war_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
if(MSVC)
    target_compile_options(war_core PRIVATE /W4)
else()
    target_compile_options(war_core PRIVATE -Wall -Wextra -Wpedantic)
endif()
add_executable(war main.cpp)
target_link_libraries(war PRIVATE war_core)

enable_testing()
add_executable(war_tests tests/testes.cpp)
target_link_libraries(war_tests PRIVATE war_core)
add_test(NAME teste_unidades COMMAND war_tests)
add_test(NAME teste_demo COMMAND war --demo --jogadores 4 --seed 42 --max-turnos 25)

# Testes de fumaca da simulacao em quantidades extremas de jogadores.
add_test(NAME demo_2_jogadores COMMAND war --demo --jogadores 2 --seed 2026 --max-turnos 30)
add_test(NAME demo_6_jogadores COMMAND war --demo --jogadores 6 --seed 1234 --max-turnos 30)

# GUI opcional. Build padrao continua independente de bibliotecas externas.
option(WAR_BUILD_GUI "Compilar interface grafica 2D com raylib" OFF)
if(WAR_BUILD_GUI)
    find_package(raylib 5 QUIET)
    if(NOT TARGET raylib AND NOT TARGET raylib::raylib)
        include(FetchContent)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
        FetchContent_Declare(raylib
            GIT_REPOSITORY https://github.com/raysan5/raylib.git
            GIT_TAG 6.2
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(raylib)
    endif()
    add_executable(war_gui gui/main_gui.cpp)
    target_link_libraries(war_gui PRIVATE war_core)
    if(TARGET raylib::raylib)
        target_link_libraries(war_gui PRIVATE raylib::raylib)
    else()
        target_link_libraries(war_gui PRIVATE raylib)
    endif()
    if(NOT MSVC)
        target_compile_options(war_gui PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()
