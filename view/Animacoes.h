#pragma once
#include "Camera.h"
#include "Desenho.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class Jogador;
class Jogo;

// Animacoes leves do mapa. A cada quadro compara o jogo com o quadro anterior
// (tropas e donos) e cria efeitos curtos: "+N"/"-N", brilho de conquista e
// marcador deslizando quando tropas sao movidas. Nao altera nada do jogo.
class Animacoes {
private:
    struct TextoFlutuante { sf::Vector2f mundo; std::string texto; sf::Color cor; float idade; };
    struct Onda { sf::Vector2f mundo; sf::Color cor; float idade; float duracao; float raioFinal; };
    struct Deslocamento { sf::Vector2f de, para; sf::Color cor; int quantidade; float idade; };

    std::vector<TextoFlutuante> textos;
    std::vector<Onda> ondas;
    std::vector<Deslocamento> deslocamentos;
    std::vector<float> brilhos;                 // [id] brilho apos conquista (1 -> 0)
    std::vector<int> tropasAntes;
    std::vector<const Jogador*> donosAntes;
    float relogio;

public:
    Animacoes();
    void observar(const Jogo& jogo, const std::vector<sf::Vector2f>& centros);
    void avancar(float segundos);
    void criarOnda(sf::Vector2f mundo, sf::Color cor, float raioFinal, float duracao);

    float getRelogio() const;                   // usado para efeitos pulsantes
    float brilhoDe(int id) const;
    void desenharNoMundo(sf::RenderTarget& alvo) const;                                   // ondas
    void desenharNaTela(sf::RenderTarget& alvo, const Camera& camera, const Fontes& fontes) const;  // textos e marcadores
};
