#pragma once
#include "Jogo.h"
#include <SFML/Window/Event.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>

class MapaView;
class PainelView;

// Fases do turno de um jogador humano na interface grafica.
enum class Fase { Reforco, Ataque, Movimento };

// Controller: transforma cliques e teclas em chamadas as classes do jogo (model).
// As regras continuam no model; aqui so ficam a selecao e a ordem das fases.
class ControladorJogo {
private:
    Jogo& jogo;
    Fase fase;
    Territorio* selecionado;
    int quantidade;
    std::string mensagem;
    bool mensagemErro;
    ResultadoAtaque ultimoAtaque;
    bool temAtaque;
    float esperaBot;            // segundos esperando antes de o bot jogar
    bool botaoPressionado;      // botao esquerdo segurado (pode virar clique ou arrasto do mapa)
    bool arrastando;
    sf::Vector2f inicioClique, ultimoMouse;

    void comecarTurno();
    void cliqueReforco(Territorio* territorio, bool todos);
    void cliqueAtaque(Territorio* territorio);
    void cliqueMovimento(Territorio* territorio);
    void avisar(const std::string& texto, bool erro);

public:
    explicit ControladorJogo(Jogo& jogo);

    // Entrada do usuario
    void tratarEvento(const sf::Event& evento, sf::Vector2f mouse, MapaView& mapa, const PainelView& painel);
    void atualizar(float segundos);   // faz os bots jogarem com uma pequena pausa

    // Acoes
    void clicarTerritorio(int id, bool botaoDireito);
    void proximaFase();
    void trocarCartas();
    void mudarQuantidade(int diferenca);
    void limparSelecao();

    // Consultas usadas pela view
    Fase getFase() const;
    const Territorio* getSelecionado() const;
    int getQuantidade() const;
    const std::string& getMensagem() const;
    bool mensagemEhErro() const;
    bool temUltimoAtaque() const;
    const ResultadoAtaque& getUltimoAtaque() const;
    bool vezDoBot() const;
};
