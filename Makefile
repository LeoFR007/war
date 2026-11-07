# Projeto WAR - compilacao direta com GNU Make (Linux/WSL)
# Requisitos: g++ com suporte a C++17 e make.

CXX := g++
CPPFLAGS := -Iinclude
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Wpedantic -MMD -MP

BUILD_DIR := build-make
CORE_SOURCES := $(wildcard src/*.cpp)
CORE_OBJECTS := $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(CORE_SOURCES))
MAIN_OBJECT := $(BUILD_DIR)/main.o
TEST_OBJECT := $(BUILD_DIR)/testes.o
EXECUTABLE := $(BUILD_DIR)/war
TEST_EXECUTABLE := $(BUILD_DIR)/war_tests

JOGADORES ?= 3
SEED ?= 42
MAX_TURNOS ?= 100

.DEFAULT_GOAL := all
.PHONY: all war run demo test clean rebuild help uml gui gui-build gui-test

all: $(EXECUTABLE)
war: $(EXECUTABLE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(MAIN_OBJECT): main.cpp | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(TEST_OBJECT): tests/testes.cpp | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(EXECUTABLE): $(CORE_OBJECTS) $(MAIN_OBJECT)
	$(CXX) $^ -o $@

$(TEST_EXECUTABLE): $(CORE_OBJECTS) $(TEST_OBJECT)
	$(CXX) $^ -o $@

run: $(EXECUTABLE)
	./$(EXECUTABLE) --jogadores $(JOGADORES)

demo: $(EXECUTABLE)
	./$(EXECUTABLE) --demo --jogadores $(JOGADORES) --seed $(SEED) --max-turnos $(MAX_TURNOS)

test: $(TEST_EXECUTABLE) $(EXECUTABLE)
	./$(TEST_EXECUTABLE)
	./$(EXECUTABLE) --demo --jogadores 4 --seed 42 --max-turnos 25 > /dev/null
	@echo "Todos os testes passaram."

uml:
	@command -v dot >/dev/null || (echo "Graphviz nao instalado. Rode: sudo apt install graphviz" && exit 1)
	dot -Tpng docs/UML.dot -o docs/UML.png
	dot -Tsvg docs/UML.dot -o docs/UML.svg
	dot -Tpng docs/UML_resumo.dot -o docs/UML_resumo.png

# raylib sera encontrada pelo CMake ou baixada automaticamente do GitHub.
gui-build:
	cmake -S . -B build-gui -DWAR_BUILD_GUI=ON -DCMAKE_BUILD_TYPE=Release
	cmake --build build-gui --target war_gui -j 2

gui: gui-build
	./build-gui/war_gui

# Compila os testes do motor sem exigir biblioteca grafica.
gui-test: test
	@echo "Motor do WAR validado. Para abrir a janela: make gui"

clean:
	rm -rf $(BUILD_DIR)

rebuild: clean all

help:
	@echo "Projeto WAR - comandos GNU Make (Linux/WSL)"
	@echo "  make          - compila o jogo por terminal"
	@echo "  make gui      - compila e abre a interface 2D raylib (WSLg)"
	@echo "  make gui-build - compila a interface 2D sem executar"
	@echo "  make run      - inicia a partida interativa (3 jogadores)"
	@echo "  make demo     - executa uma simulacao automatica"
	@echo "  make test     - executa os testes"
	@echo "  make clean    - apaga a pasta build-make"
	@echo "  make rebuild  - compila novamente do zero"
	@echo "  make uml      - regenera diagramas (requer Graphviz)"
	@echo "  make run JOGADORES=4"
	@echo "  make demo JOGADORES=4 SEED=42 MAX_TURNOS=100"

-include $(CORE_OBJECTS:.o=.d) $(MAIN_OBJECT:.o=.d) $(TEST_OBJECT:.o=.d)
