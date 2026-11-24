#pragma once
#include "Animacoes.h"
#include "Camera.h"
#include "Desenho.h"
#include "RegioesMapa.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class ControladorJogo;
class Territorio;
class Jogo;
class Mapa;

// Estados visuais de um territorio no mapa.
enum class EstadoVisual { Normal, Proibido, Possivel, Selecionado };

// Desenha o mapa (regioes, fronteiras, marcadores de tropas, setas e dicas).
// Nao contem regras: pergunta ao model (Jogador::podeAtacar / podeMover) e ao controller.
class MapaView {
private:
    const Fontes& fontes;
    Camera camera;
    RegioesMapa regioes;
    Animacoes animacoes;
    sf::VertexArray rotasMaritimas;                 // tracejados criados uma vez (cache)
    std::vector<sf::Vector2f> centros;              // [id] centro do territorio no mundo
    std::vector<int> continenteDe;                  // [id] indice do continente
    std::vector<std::string> nomesContinentes;
    std::vector<std::string> rotulosContinentes;
    std::vector<sf::Vector2f> posicoesRotulos;
    const Territorio* selecionadoAntes;
    std::vector<EstadoVisual> estados;             // [id] estado visual do quadro atual
    sf::RenderTexture camadaMapa;                  // cache: fundo + territorios + fronteiras
    std::vector<sf::Color> coresDesenhadas;        // cores usadas no ultimo redesenho do cache

    void montarLayout();
    void criarRotasMaritimas(const Mapa& mapa);
    EstadoVisual estadoDe(const Jogo& jogo, const ControladorJogo& controlador, const Territorio* territorio) const;
    sf::Color corDaAcao(const ControladorJogo& controlador) const;

    sf::Color corDoTerritorio(const Jogo& jogo, const ControladorJogo& controlador, int id, int idMouse) const;
    void redesenharCamadaSeMudou(const Jogo& jogo, const ControladorJogo& controlador, int idMouse);
    void desenharContornos(sf::RenderTarget& alvo, const ControladorJogo& controlador, int idMouse) const;
    void desenharSeta(sf::RenderTarget& alvo, const Jogo& jogo, const ControladorJogo& controlador, int idMouse) const;
    void desenharRotulos(sf::RenderTarget& alvo) const;
    void desenharMarcadores(sf::RenderTarget& alvo, const Jogo& jogo, const ControladorJogo& controlador, int idMouse) const;
    void desenharMoldura(sf::RenderTarget& alvo, sf::Vector2f mouse) const;
    void desenharDica(sf::RenderTarget& alvo, const Jogo& jogo, const ControladorJogo& controlador, int idMouse,
                      sf::Vector2f mouse) const;

public:
    MapaView(const Fontes& fontes, const Mapa& mapa);

    void atualizar(const Jogo& jogo, const ControladorJogo& controlador, sf::Vector2f mouse, float segundos);
    void desenhar(sf::RenderTarget& alvo, const Jogo& jogo, const ControladorJogo& controlador, sf::Vector2f mouse) const;

    int territorioEm(sf::Vector2f pontoTela) const;   // 0 = nenhum
    bool contem(sf::Vector2f pontoTela) const;
    int botaoZoomEm(sf::Vector2f pontoTela) const;    // 0 nenhum, 1 aproximar, 2 afastar, 3 centralizar
    void aproximar(float passos, sf::Vector2f pontoTela);
    void arrastar(sf::Vector2f deslocamentoTela);
    void centralizar();
};
