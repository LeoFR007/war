#pragma once
#include "Desenho.h"
#include <SFML/Graphics.hpp>

class ControladorJogo;
class Jogo;

// Barra superior: logotipo, turno, etapas do turno e resumo de cada jogador.
// Tambem mostra a faixa animada "TURNO N" quando a vez muda.
class BarraSuperiorView {
private:
    const Fontes& fontes;
    int turnoVisto;
    float tempoFaixa;       // segundos desde que a faixa apareceu
    float duracaoFaixa;
    float relogio;

    void desenharEtapas(sf::RenderTarget& alvo, const Jogo& jogo, const ControladorJogo& controlador) const;
    void desenharJogadores(sf::RenderTarget& alvo, const Jogo& jogo) const;

public:
    explicit BarraSuperiorView(const Fontes& fontes);
    void atualizar(const Jogo& jogo, float segundos);
    void desenhar(sf::RenderTarget& alvo, const Jogo& jogo, const ControladorJogo& controlador) const;
    void desenharFaixaDeTurno(sf::RenderTarget& alvo, const Jogo& jogo) const;
};
