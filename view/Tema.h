#pragma once
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <string>

// =====================================================================
// Tema visual: paleta, tamanhos de fonte e posicoes das areas da tela.
// So aparencia: nenhuma regra do jogo fica aqui.
// =====================================================================

// ----- Paleta principal (interface) -----
const sf::Color COR_FUNDO(10, 14, 19);
const sf::Color COR_PAINEL(18, 25, 33);
const sf::Color COR_PAINEL_ALTO(27, 36, 47);     // cartoes dentro dos paineis
const sf::Color COR_LINHA(46, 60, 74);
const sf::Color COR_TEXTO(234, 231, 222);
const sf::Color COR_TEXTO_SUAVE(140, 153, 167);
const sf::Color COR_LATAO(205, 166, 82);         // cor de destaque (estilo militar)

// ----- Paleta auxiliar (mapa) -----
const sf::Color COR_OCEANO_CLARO(24, 52, 74);
const sf::Color COR_OCEANO_ESCURO(9, 22, 34);
const sf::Color COR_COSTA(6, 11, 16);
const sf::Color COR_FRONTEIRA(12, 17, 22);

// ----- Alertas e acoes -----
const sf::Color COR_ATAQUE(214, 84, 74);
const sf::Color COR_MOVIMENTO(84, 178, 138);
const sf::Color COR_REFORCO(205, 166, 82);
const sf::Color COR_CONQUISTA(240, 200, 110);
const sf::Color COR_ALERTA(232, 168, 72);
const sf::Color COR_ERRO(226, 96, 88);

// ----- Tamanhos de fonte (hierarquia visual) -----
const unsigned int FONTE_DESTAQUE = 30;   // numeros grandes
const unsigned int FONTE_TITULO = 20;
const unsigned int FONTE_TEXTO = 14;
const unsigned int FONTE_PEQUENA = 12;
const unsigned int FONTE_MINIMA = 11;

// ----- Areas da tela (tamanho logico 1440 x 860) -----
const float LARGURA_TELA = 1440;
const float ALTURA_TELA = 860;
const sf::FloatRect AREA_BARRA(0, 0, 1440, 64);
const sf::FloatRect AREA_MAPA(16, 76, 1040, 768);
const sf::FloatRect AREA_PAINEL(1072, 76, 352, 492);
const sf::FloatRect AREA_EVENTOS(1072, 580, 352, 264);

// ----- Cores dos jogadores: tons sobrios, um emblema para cada cor -----
// Indice: 0 Vermelho (estrela), 1 Azul (escudo), 2 Verde (divisa),
//         3 Amarelo (losango), 4 Roxo (hexagono), 5 Preto (cruz).
inline int indiceDoJogador(const std::string& cor) {
    if (cor == "Vermelho") return 0;
    if (cor == "Azul") return 1;
    if (cor == "Verde") return 2;
    if (cor == "Amarelo") return 3;
    if (cor == "Roxo") return 4;
    return 5;
}

inline sf::Color corDoJogador(const std::string& cor) {
    const sf::Color cores[6] = {sf::Color(196, 82, 62),  sf::Color(66, 126, 180), sf::Color(102, 146, 80),
                                sf::Color(212, 164, 66), sf::Color(132, 96, 168), sf::Color(104, 114, 128)};
    return cores[indiceDoJogador(cor)];
}

// ----- Ajustes de cor -----
inline sf::Color misturar(sf::Color a, sf::Color b, float t) {
    return sf::Color(static_cast<sf::Uint8>(a.r + (b.r - a.r) * t), static_cast<sf::Uint8>(a.g + (b.g - a.g) * t),
                     static_cast<sf::Uint8>(a.b + (b.b - a.b) * t), static_cast<sf::Uint8>(a.a + (b.a - a.a) * t));
}
inline sf::Color clarear(sf::Color cor, float t) { return misturar(cor, sf::Color(255, 255, 255, cor.a), t); }
inline sf::Color escurecer(sf::Color cor, float t) { return misturar(cor, sf::Color(0, 0, 0, cor.a), t); }
inline sf::Color comAlfa(sf::Color cor, float alfa) {
    cor.a = static_cast<sf::Uint8>(alfa < 0 ? 0 : (alfa > 255 ? 255 : alfa));
    return cor;
}
