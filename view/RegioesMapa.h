#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

// Gera, uma unica vez, o desenho dos territorios como regioes de verdade.
//
// Ideia (diagrama de Voronoi): cada ponto de um continente pertence ao
// territorio mais proximo daquele continente. Uma pequena ondulacao deixa
// as fronteiras com aspecto organico. O resultado vira texturas guardadas
// em memoria (cache), que sao apenas coloridas a cada quadro.
class RegioesMapa {
public:
    static const int ESCALA = 2;    // resolucao das texturas: 2 pixels por unidade do mundo

private:
    int largura, altura;                       // tamanho das imagens (mundo x ESCALA)
    std::vector<int> idPorPixel;               // 0 = oceano, 1..42 = territorio
    std::vector<sf::Texture> preenchimentos;   // [id] area do territorio (branca, com sombreado)
    std::vector<sf::Texture> contornos;        // [id] borda interna do territorio (branca)
    std::vector<sf::Vector2f> cantos;          // [id] canto superior esquerdo de cada textura (mundo)
    std::vector<std::vector<bool>> tocam;      // tocam[a][b]: as regioes a e b se encostam em terra
    sf::Texture fundo;                         // oceano, grade, sombra e halo da costa
    sf::Texture fronteiras;                    // linhas entre territorios e linha da costa

    int idEm(int px, int py) const;
    std::vector<int> distanciaAteBorda(bool paraTerra) const;

public:
    // centros[id] e continenteDe[id] em coordenadas do mundo; formas = poligono de cada continente.
    void gerar(sf::Vector2f tamanhoMundo, const std::vector<sf::Vector2f>& centros,
               const std::vector<int>& continenteDe, const std::vector<std::vector<sf::Vector2f>>& formas);

    int territorioEm(sf::Vector2f pontoMundo) const;
    bool tocamEmTerra(int a, int b) const;

    void desenharFundo(sf::RenderTarget& alvo) const;
    void desenharTerritorio(sf::RenderTarget& alvo, int id, sf::Color cor) const;
    void desenharContorno(sf::RenderTarget& alvo, int id, sf::Color cor) const;
    void desenharFronteiras(sf::RenderTarget& alvo) const;
};
