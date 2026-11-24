#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// Fontes do jogo (familia Ubuntu, em assets/fonts).
struct Fontes {
    sf::Font normal;      // textos corridos
    sf::Font medio;       // rotulos
    sf::Font negrito;     // destaques
    sf::Font titulo;      // condensada: titulos e numeros (visual militar)
    void carregar(const std::string& pasta);
};

enum class Alinhamento { Esquerda, Centro, Direita };

// Icones vetoriais simples usados no painel de eventos.
enum class TipoIcone { Ataque, Conquista, Movimento, Reforco, Cartas, Eliminacao, Vitoria, Turno };

// ---------- Funcoes de desenho reutilizadas por todas as telas ----------
void escrever(sf::RenderTarget& alvo, const sf::Font& fonte, const std::string& texto, float x, float y,
              unsigned int tamanho, sf::Color cor, Alinhamento alinhamento = Alinhamento::Esquerda,
              float espacamento = 1.0f);
float larguraTexto(const sf::Font& fonte, const std::string& texto, unsigned int tamanho, float espacamento = 1.0f);

void caixa(sf::RenderTarget& alvo, sf::FloatRect area, float raio, sf::Color fundo,
           sf::Color borda = sf::Color::Transparent, float espessura = 1);
void sombra(sf::RenderTarget& alvo, sf::FloatRect area, float raio);
void circulo(sf::RenderTarget& alvo, sf::Vector2f centro, float raio, sf::Color fundo,
             sf::Color borda = sf::Color::Transparent, float espessura = 0);
void linha(sf::RenderTarget& alvo, sf::Vector2f a, sf::Vector2f b, float espessura, sf::Color cor);
void seta(sf::RenderTarget& alvo, sf::Vector2f a, sf::Vector2f b, float espessura, sf::Color cor);

void emblema(sf::RenderTarget& alvo, sf::Vector2f centro, float raio, int indiceJogador, sf::Color cor);
void dado(sf::RenderTarget& alvo, sf::Vector2f posicao, float lado, int valor, sf::Color cor);
void icone(sf::RenderTarget& alvo, sf::Vector2f centro, float tamanho, TipoIcone tipo, sf::Color cor);
