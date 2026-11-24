#pragma once
#include <string>
#include <vector>

class Jogador;
class Territorio;

// Grupo de territorios. Quem controla todos ganha exercitos extras (bonus).
class Continente {
private:
    std::string nome;
    int bonus;
    std::vector<Territorio*> territorios;

public:
    Continente(const std::string& nome, int bonus);

    std::string getNome() const;
    int getBonus() const;

    void adicionarTerritorio(Territorio* territorio);
    bool controladoPor(const Jogador* jogador) const;
};
