#pragma once
#include "Jogador.h"
#include "Mapa.h"
#include <cstddef>
#include <cstdint>
#include <deque>
#include <memory>
#include <random>
#include <string>
#include <vector>

class Jogo {
    Mapa mapa_;
    std::vector<std::unique_ptr<Jogador>> jogadores_;
    std::deque<std::unique_ptr<Carta>> baralho_;
    std::vector<std::unique_ptr<Carta>> descarte_;
    std::mt19937 rng_;
    FaseTurno fase_ = FaseTurno::Reforco;
    std::size_t jogadorAtual_ = 0;
    int numeroTrocas_ = 0;
    bool conquistouNesteTurno_ = false;
    bool turnoAtivo_ = false;
    bool encerrado_ = false;
    Jogador* vencedor_ = nullptr;
    void configurar(std::vector<std::string> nomes);
    void criarBaralho();
    void atribuirObjetivos();
    void comprarCarta(Jogador& jogador);
    void recomporBaralho();
    int exercitosPorTroca();
    int reforcosPorTurno(const Jogador& jogador) const;
    bool verificarVitoria();
    void proximoJogador();
    int iniciarTurno();
    void exigirFase(FaseTurno fase) const;
    void trocarCartasSePossivel(Jogador& jogador, bool automatico);
    void executarTurnoHumano();
    void executarTurnoBot();
    void mostrarMapa() const;
    void mostrarEstado(const Jogador& jogador) const;
public:
    explicit Jogo(std::vector<std::string> nomes, std::uint32_t semente = std::random_device{}());
    void executar();
    void executarDemo(int maxTurnos = 400);
    // API do motor, independente de terminal: pronta para a futura interface 2D.
    int iniciarTurnoAtual();
    void reforcar(int idTerritorio, int quantidade);
    RelatorioCombate atacarTerritorio(int idOrigem, int idAlvo, int dados);
    void moverTropas(int idOrigem, int idDestino, int quantidade);
    int trocarCartas();
    void avancarFase();
    Jogador& jogadorAtual() noexcept { return *jogadores_[jogadorAtual_]; }
    const Jogador& jogadorAtual() const noexcept { return *jogadores_[jogadorAtual_]; }
    bool encerrado() const noexcept { return encerrado_; }
    bool turnoAtivo() const noexcept { return turnoAtivo_; }
    bool houveConquistaNesteTurno() const noexcept { return conquistouNesteTurno_; }
    const Mapa& mapa() const noexcept { return mapa_; }
    const std::vector<std::unique_ptr<Jogador>>& jogadores() const noexcept { return jogadores_; }
    FaseTurno fase() const noexcept { return fase_; }
    const Jogador* vencedor() const noexcept { return vencedor_; }
};
