#pragma once
#include <SFML/Graphics.hpp>

// Camera do mapa: zoom suave e deslocamento (pan).
// "Mundo" = coordenadas do desenho do mapa; "tela" = coordenadas da janela.
class Camera {
private:
    sf::FloatRect area;          // onde o mapa aparece na janela
    sf::Vector2f tamanhoMundo;
    sf::Vector2f centro, centroAlvo;
    float zoom, zoomAlvo;        // pixels de tela por unidade do mundo
    float zoomMinimo, zoomMaximo;

    void limitar();

public:
    Camera(sf::FloatRect area, sf::Vector2f tamanhoMundo);

    void atualizar(float segundos);                       // aproxima o valor atual do alvo (suavizacao)
    void aproximar(float passos, sf::Vector2f pontoTela);  // roda do mouse: zoom em direcao ao cursor
    void arrastar(sf::Vector2f deslocamentoTela);
    void centralizar();

    sf::Vector2f paraMundo(sf::Vector2f pontoTela) const;
    sf::Vector2f paraTela(sf::Vector2f pontoMundo) const;
    sf::View visao() const;      // sf::View que desenha o mundo dentro da area do mapa
    bool contem(sf::Vector2f pontoTela) const;
    float getZoomRelativo() const;  // 1.0 = mapa inteiro na tela
};
