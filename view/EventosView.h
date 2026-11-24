#pragma once
#include "Desenho.h"
#include <SFML/Graphics.hpp>
#include <string>

class Jogo;

// Painel de eventos: historico da partida com icone e cor por tipo
// (ataque, conquista, movimento, troca de cartas, eliminacao, vitoria e inicio de turno).
class EventosView {
private:
    const Fontes& fontes;
    TipoIcone tipoDoEvento(const std::string& texto) const;

public:
    explicit EventosView(const Fontes& fontes);
    void desenhar(sf::RenderTarget& alvo, const Jogo& jogo) const;
};
