#pragma once
#include "Carta.h"
#include "Jogador.h"
#include "Mapa.h"
#include <memory>
#include <random>
#include <string>
#include <vector>

// Resultado de um ataque, usado para mostrar os dados na tela.
struct ResultadoAtaque {
    std::vector<int> dadosAtaque;
    std::vector<int> dadosDefesa;
    int perdasAtaque = 0;
    int perdasDefesa = 0;
    bool conquistou = false;
};

// Controla a partida: mapa, jogadores, baralho, turnos e regras de combate.
class Jogo {
private:
    Mapa mapa;
    std::vector<std::unique_ptr<Jogador>> jogadores;
    std::vector<Carta> baralho;
    std::mt19937 gerador;           // sorteios (dados, territorios, objetivos)
    int jogadorAtual;
    int turno;                          // quantos turnos ja comecaram
    int trocasRealizadas;
    bool conquistouNoTurno;
    bool fim;
    Jogador* vencedor;
    std::vector<std::string> historico; // eventos da partida (mostrados na interface grafica)

    void distribuirTerritorios();
    void criarBaralho();
    void sortearObjetivos();
    int calcularReforcos(const Jogador* jogador) const;
    void verificarVitoria(Jogador* jogador);
    std::vector<int> rolarDados(int quantidade);

public:
    // Os primeiros "quantidadeHumanos" jogadores sao humanos; os demais sao bots.
    Jogo(const std::vector<std::string>& nomes, int quantidadeHumanos, unsigned int semente);

    // Partida completa no terminal.
    void executar(int maxTurnos);

    // Turno dividido em duas etapas (usado pelo terminal e pela interface grafica).
    int iniciarTurno();     // reforcos + troca obrigatoria; retorna os reforcos recebidos
    void finalizarTurno();  // carta por conquista, verifica vitoria e passa a vez

    ResultadoAtaque atacar(Jogador* atacante, Territorio* origem, Territorio* alvo, int dados);
    int trocarCartas(Jogador* jogador);
    void encerrar();
    void registrarEvento(const std::string& texto);

    Mapa& getMapa();
    const Mapa& getMapa() const;    // versao somente leitura (usada pela interface grafica)
    bool terminou() const;
    const Jogador* getVencedor() const;
    int getQuantidadeJogadores() const;
    Jogador* getJogador(int indice) const;
    Jogador* getJogadorAtual() const;
    int getTurno() const;
    const std::vector<std::string>& getHistorico() const;
};
