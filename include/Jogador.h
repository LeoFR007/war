#pragma once
#include "Carta.h"
#include "Enums.h"
#include "Territorio.h"
#include <memory>
#include <random>
#include <string>
#include <vector>

class Objetivo;

class Jogador {
    std::string nome_;
    CorJogador cor_;
    std::vector<Territorio*> territorios_;
    std::unique_ptr<Objetivo> objetivo_;
    int exercitosDisponiveis_ = 0;
    std::vector<std::unique_ptr<Carta>> cartas_;
    Jogador* eliminadoPor_ = nullptr;
public:
    Jogador(std::string nome, CorJogador cor);
    ~Jogador(); // Classe Objetivo completa no .cpp
    Jogador(const Jogador&) = delete;
    Jogador& operator=(const Jogador&) = delete;
    const std::string& nome() const noexcept { return nome_; }
    CorJogador cor() const noexcept { return cor_; }
    const std::vector<Territorio*>& territorios() const noexcept { return territorios_; }
    int exercitosDisponiveis() const noexcept { return exercitosDisponiveis_; }
    Jogador* eliminadoPor() const noexcept { return eliminadoPor_; }
    const Objetivo* objetivo() const noexcept { return objetivo_.get(); }
    std::size_t totalCartas() const noexcept { return cartas_.size(); }
    const std::vector<std::unique_ptr<Carta>>& cartas() const noexcept { return cartas_; }
    bool ativo() const noexcept { return !territorios_.empty(); }
    bool possui(const Territorio& territorio) const noexcept;
    void definirObjetivo(std::unique_ptr<Objetivo> objetivo);
    void receberExercitos(int quantidade);
    void distribuirExercitos(Territorio& destino, int quantidade);
    RelatorioCombate atacar(Territorio& origem, Territorio& alvo, int dados, std::mt19937& rng);
    void moverTropas(Territorio& origem, Territorio& destino, int quantidade);
    void conquistarTerritorio(Territorio& territorio);
    void receberCarta(std::unique_ptr<Carta> carta);
    bool podeTrocarCartas() const;
    std::vector<std::unique_ptr<Carta>> trocarCartas();
};
