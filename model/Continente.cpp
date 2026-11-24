#include "Continente.h"
#include "Territorio.h"

Continente::Continente(const std::string& nome, int bonus) : nome(nome), bonus(bonus) {}

std::string Continente::getNome() const { return nome; }

int Continente::getBonus() const { return bonus; }

void Continente::adicionarTerritorio(Territorio* territorio) { territorios.push_back(territorio); }

bool Continente::controladoPor(const Jogador* jogador) const {
    for (Territorio* territorio : territorios) {
        if (territorio->getDono() != jogador) return false;
    }
    return true;
}
