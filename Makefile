# Projeto WAR - compilacao com g++ (Linux/WSL)
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -Imodel -Iview -Icontroller
MODEL = $(wildcard model/*.cpp)
VIEW = $(wildcard view/*.cpp)
CONTROLLER = $(wildcard controller/*.cpp)
SFML = -lsfml-graphics -lsfml-window -lsfml-system

# Jogo no terminal (nao precisa de SFML)
war: $(MODEL) main/main.cpp
	$(CXX) $(CXXFLAGS) $^ -o war

# Interface grafica (precisa de: sudo apt install libsfml-dev)
war_gui: $(MODEL) $(VIEW) $(CONTROLLER) main/main_gui.cpp
	$(CXX) $(CXXFLAGS) $^ -o war_gui $(SFML)

jogar: war_gui
	./war_gui

run: war
	./war

demo: war
	./war --demo --jogadores 4 --seed 42

uml:
	dot -Tpng docs/UML.dot -o docs/UML.png

clean:
	rm -f war war_gui

.PHONY: jogar run demo uml clean
