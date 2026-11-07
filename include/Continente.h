#pragma once
#include <string>
#include <vector>

class Jogador;
class Territorio;

class Continente {
    std::string nome_;
    int bonusExercitos_;
    std::vector<Territorio*> territorios_;
public:
    Continente(std::string nome, int bonus);
    const std::string& nome() const noexcept { return nome_; }
    int bonusExercitos() const noexcept { return bonusExercitos_; }
    const std::vector<Territorio*>& territorios() const noexcept { return territorios_; }
    void adicionarTerritorio(Territorio& territorio);
    bool verificarControle(const Jogador& jogador) const;
    int calcularBonus(const Jogador& jogador) const;
};
