#pragma once
#include "Desenho.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class ControladorJogo;
class Jogo;

// Botoes do painel lateral.
enum class AcaoPainel { Nenhuma, Menos, Mais, Trocar, ProximaFase };

// Painel "Comandante": jogador da vez, reserva, territorios, tropas, objetivo,
// continentes, acoes do turno e ultima batalha. Tambem desenha a tela de vitoria.
class PainelView {
private:
    const Fontes& fontes;
    sf::FloatRect botaoMenos, botaoMais, botaoTrocar, botaoProxima;

    std::vector<std::string> quebrarLinhas(const std::string& texto, const sf::Font& fonte, float largura,
                                           unsigned int tamanho) const;
    void desenharBotao(sf::RenderTarget& alvo, const sf::FloatRect& area, const std::string& texto, bool ativo,
                       sf::Color cor, sf::Vector2f mouse) const;
    void desenharCabecalho(sf::RenderTarget& alvo, const Jogo& jogo) const;
    void desenharContinentes(sf::RenderTarget& alvo, const Jogo& jogo, float y) const;
    void desenharAcoes(sf::RenderTarget& alvo, const Jogo& jogo, const ControladorJogo& controlador, sf::Vector2f mouse,
                       float y) const;

public:
    explicit PainelView(const Fontes& fontes);
    void desenhar(sf::RenderTarget& alvo, const Jogo& jogo, const ControladorJogo& controlador, sf::Vector2f mouse) const;
    void desenharVitoria(sf::RenderTarget& alvo, const Jogo& jogo) const;
    AcaoPainel acaoEm(sf::Vector2f ponto) const;
};
