#include "Desenho.h"
#include "Tema.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

const float PI = 3.14159265f;

void Fontes::carregar(const std::string& pasta) {
    std::string arquivo = pasta + "/Ubuntu.ttf";
    if (!normal.loadFromFile(arquivo) || !medio.loadFromFile(arquivo) ||
        !negrito.loadFromFile(arquivo) || !titulo.loadFromFile(arquivo))
        throw std::runtime_error("Fontes nao encontradas em " + pasta + ". Execute a partir da pasta do projeto.");
}

// ---------------------------- Texto ----------------------------

void escrever(sf::RenderTarget& alvo, const sf::Font& fonte, const std::string& texto, float x, float y,
              unsigned int tamanho, sf::Color cor, Alinhamento alinhamento, float espacamento) {
    sf::Text t(sf::String::fromUtf8(texto.begin(), texto.end()), fonte, tamanho);  // UTF-8: aceita acentos
    t.setLetterSpacing(espacamento);
    t.setFillColor(cor);
    float largura = t.getLocalBounds().width + t.getLocalBounds().left;
    if (alinhamento == Alinhamento::Centro) x -= largura / 2;
    if (alinhamento == Alinhamento::Direita) x -= largura;
    t.setPosition(std::round(x), std::round(y));
    alvo.draw(t);
}

float larguraTexto(const sf::Font& fonte, const std::string& texto, unsigned int tamanho, float espacamento) {
    sf::Text t(sf::String::fromUtf8(texto.begin(), texto.end()), fonte, tamanho);
    t.setLetterSpacing(espacamento);
    return t.getLocalBounds().width + t.getLocalBounds().left;
}

// ---------------------------- Formas ----------------------------

// Retangulo de cantos arredondados (cada canto vira um arco de 6 pontos).
void caixa(sf::RenderTarget& alvo, sf::FloatRect area, float raio, sf::Color fundo, sf::Color borda, float espessura) {
    raio = std::min(raio, std::min(area.width, area.height) / 2);
    sf::Vector2f centros[4] = {{area.left + area.width - raio, area.top + raio},
                               {area.left + area.width - raio, area.top + area.height - raio},
                               {area.left + raio, area.top + area.height - raio},
                               {area.left + raio, area.top + raio}};
    sf::ConvexShape forma(24);
    int indice = 0;
    for (int canto = 0; canto < 4; canto++) {
        for (int i = 0; i < 6; i++) {
            float angulo = (-90 + canto * 90 + i * 18) * PI / 180;
            forma.setPoint(indice++, centros[canto] + sf::Vector2f(std::cos(angulo) * raio, std::sin(angulo) * raio));
        }
    }
    forma.setFillColor(fundo);
    forma.setOutlineColor(borda);
    forma.setOutlineThickness(borda.a > 0 ? -espessura : 0);
    alvo.draw(forma);
}

// Sombra suave: tres caixas escuras e transparentes, cada vez maiores.
void sombra(sf::RenderTarget& alvo, sf::FloatRect area, float raio) {
    for (int i = 3; i >= 1; i--) {
        sf::FloatRect maior(area.left - i * 2, area.top - i * 2 + 3, area.width + i * 4, area.height + i * 4);
        caixa(alvo, maior, raio + i * 2, sf::Color(0, 0, 0, 28));
    }
}

void circulo(sf::RenderTarget& alvo, sf::Vector2f centro, float raio, sf::Color fundo, sf::Color borda, float espessura) {
    sf::CircleShape forma(raio, 40);
    forma.setOrigin(raio, raio);
    forma.setPosition(centro);
    forma.setFillColor(fundo);
    forma.setOutlineColor(borda);
    forma.setOutlineThickness(espessura);
    alvo.draw(forma);
}

void linha(sf::RenderTarget& alvo, sf::Vector2f a, sf::Vector2f b, float espessura, sf::Color cor) {
    sf::Vector2f d = b - a;
    float comprimento = std::sqrt(d.x * d.x + d.y * d.y);
    sf::RectangleShape forma(sf::Vector2f(comprimento, espessura));
    forma.setOrigin(0, espessura / 2);
    forma.setPosition(a);
    forma.setRotation(std::atan2(d.y, d.x) * 180 / PI);
    forma.setFillColor(cor);
    alvo.draw(forma);
}

void seta(sf::RenderTarget& alvo, sf::Vector2f a, sf::Vector2f b, float espessura, sf::Color cor) {
    sf::Vector2f d = b - a;
    float comprimento = std::sqrt(d.x * d.x + d.y * d.y);
    if (comprimento < 1) return;
    sf::Vector2f direcao = d / comprimento;
    sf::Vector2f normal(-direcao.y, direcao.x);
    float ponta = espessura * 3.2f;
    linha(alvo, a, b - direcao * ponta, espessura, cor);
    sf::ConvexShape triangulo(3);
    triangulo.setPoint(0, b);
    triangulo.setPoint(1, b - direcao * ponta + normal * ponta * 0.7f);
    triangulo.setPoint(2, b - direcao * ponta - normal * ponta * 0.7f);
    triangulo.setFillColor(cor);
    alvo.draw(triangulo);
}

// ---------------------------- Emblemas dos jogadores ----------------------------

// Cada jogador tem um simbolo proprio, alem da cor (ajuda daltonicos e projetores).
void emblema(sf::RenderTarget& alvo, sf::Vector2f centro, float raio, int indiceJogador, sf::Color cor) {
    std::vector<sf::Vector2f> pontos;
    if (indiceJogador == 0) {                      // estrela de 5 pontas
        for (int i = 0; i < 10; i++) {
            float r = (i % 2 == 0) ? raio : raio * 0.45f;
            float angulo = (-90 + i * 36) * PI / 180;
            pontos.push_back({std::cos(angulo) * r, std::sin(angulo) * r});
        }
    } else if (indiceJogador == 1) {               // escudo
        pontos = {{-raio * 0.85f, -raio * 0.85f}, {raio * 0.85f, -raio * 0.85f}, {raio * 0.85f, raio * 0.1f},
                  {0, raio}, {-raio * 0.85f, raio * 0.1f}};
    } else if (indiceJogador == 2) {               // divisa (chevron) militar
        pontos = {{0, -raio}, {raio, raio * 0.1f}, {raio, raio * 0.75f}, {0, -raio * 0.15f},
                  {-raio, raio * 0.75f}, {-raio, raio * 0.1f}};
    } else if (indiceJogador == 3) {               // losango
        pontos = {{0, -raio}, {raio * 0.8f, 0}, {0, raio}, {-raio * 0.8f, 0}};
    } else if (indiceJogador == 4) {               // hexagono
        for (int i = 0; i < 6; i++) {
            float angulo = (30 + i * 60) * PI / 180;
            pontos.push_back({std::cos(angulo) * raio, std::sin(angulo) * raio});
        }
    } else {                                       // cruz
        float b = raio * 0.36f;
        pontos = {{-b, -raio}, {b, -raio}, {b, -b}, {raio, -b}, {raio, b}, {b, b},
                  {b, raio}, {-b, raio}, {-b, b}, {-raio, b}, {-raio, -b}, {-b, -b}};
    }
    // A estrela, a divisa e a cruz nao sao convexas: desenha como leque a partir do centro.
    sf::VertexArray leque(sf::TriangleFan);
    leque.append(sf::Vertex(centro, cor));
    for (const sf::Vector2f& p : pontos) leque.append(sf::Vertex(centro + p, cor));
    leque.append(sf::Vertex(centro + pontos[0], cor));
    if (indiceJogador == 2) {                      // divisa: dois quadrilateros
        sf::ConvexShape lado(4);
        lado.setFillColor(cor);
        lado.setPoint(0, centro + pontos[0]); lado.setPoint(1, centro + pontos[1]);
        lado.setPoint(2, centro + pontos[2]); lado.setPoint(3, centro + pontos[3]);
        alvo.draw(lado);
        lado.setPoint(0, centro + pontos[0]); lado.setPoint(1, centro + pontos[3]);
        lado.setPoint(2, centro + pontos[4]); lado.setPoint(3, centro + pontos[5]);
        alvo.draw(lado);
    } else {
        alvo.draw(leque);
    }
    // contorno escuro para destacar o simbolo sobre qualquer fundo
    sf::VertexArray contorno(sf::LineStrip);
    for (const sf::Vector2f& p : pontos) contorno.append(sf::Vertex(centro + p, sf::Color(0, 0, 0, 120)));
    contorno.append(sf::Vertex(centro + pontos[0], sf::Color(0, 0, 0, 120)));
    alvo.draw(contorno);
}

// ---------------------------- Dado com pontos ----------------------------

void dado(sf::RenderTarget& alvo, sf::Vector2f posicao, float lado, int valor, sf::Color cor) {
    caixa(alvo, sf::FloatRect(posicao.x, posicao.y, lado, lado), lado * 0.2f, cor, clarear(cor, 0.35f), 1);
    // posicoes dos pontos numa grade 3x3 (0 = esquerda/topo, 1 = meio, 2 = direita/baixo)
    const int pontos[7][6][2] = {{},
                                 {{1, 1}},
                                 {{0, 0}, {2, 2}},
                                 {{0, 0}, {1, 1}, {2, 2}},
                                 {{0, 0}, {2, 0}, {0, 2}, {2, 2}},
                                 {{0, 0}, {2, 0}, {1, 1}, {0, 2}, {2, 2}},
                                 {{0, 0}, {2, 0}, {0, 1}, {2, 1}, {0, 2}, {2, 2}}};
    for (int i = 0; i < valor && i < 6; i++) {
        float px = posicao.x + lado * (0.25f + 0.25f * pontos[valor][i][0]);
        float py = posicao.y + lado * (0.25f + 0.25f * pontos[valor][i][1]);
        circulo(alvo, sf::Vector2f(px, py), lado * 0.085f, sf::Color(245, 242, 235));
    }
}

// ---------------------------- Icones de eventos ----------------------------

void icone(sf::RenderTarget& alvo, sf::Vector2f c, float t, TipoIcone tipo, sf::Color cor) {
    float h = t / 2;
    if (tipo == TipoIcone::Ataque) {               // duas espadas cruzadas
        linha(alvo, c + sf::Vector2f(-h, -h), c + sf::Vector2f(h, h), 2, cor);
        linha(alvo, c + sf::Vector2f(h, -h), c + sf::Vector2f(-h, h), 2, cor);
        linha(alvo, c + sf::Vector2f(-h * 0.9f, h * 0.2f), c + sf::Vector2f(-h * 0.2f, h * 0.9f), 2, cor);
        linha(alvo, c + sf::Vector2f(h * 0.9f, h * 0.2f), c + sf::Vector2f(h * 0.2f, h * 0.9f), 2, cor);
    } else if (tipo == TipoIcone::Conquista) {     // bandeira
        linha(alvo, c + sf::Vector2f(-h * 0.6f, -h), c + sf::Vector2f(-h * 0.6f, h), 2, cor);
        sf::ConvexShape pano(3);
        pano.setPoint(0, c + sf::Vector2f(-h * 0.5f, -h));
        pano.setPoint(1, c + sf::Vector2f(h, -h * 0.5f));
        pano.setPoint(2, c + sf::Vector2f(-h * 0.5f, 0));
        pano.setFillColor(cor);
        alvo.draw(pano);
    } else if (tipo == TipoIcone::Movimento) {     // seta
        seta(alvo, c + sf::Vector2f(-h, 0), c + sf::Vector2f(h, 0), 2.2f, cor);
    } else if (tipo == TipoIcone::Reforco) {       // sinal de mais
        linha(alvo, c + sf::Vector2f(-h, 0), c + sf::Vector2f(h, 0), 2.5f, cor);
        linha(alvo, c + sf::Vector2f(0, -h), c + sf::Vector2f(0, h), 2.5f, cor);
    } else if (tipo == TipoIcone::Cartas) {        // duas cartas
        caixa(alvo, sf::FloatRect(c.x - h, c.y - h * 0.8f, h * 1.1f, h * 1.6f), 2, sf::Color::Transparent, cor, 1.5f);
        caixa(alvo, sf::FloatRect(c.x - h * 0.2f, c.y - h, h * 1.1f, h * 1.6f), 2, comAlfa(cor, 90), cor, 1.5f);
    } else if (tipo == TipoIcone::Eliminacao) {    // X dentro do circulo
        circulo(alvo, c, h, sf::Color::Transparent, cor, 1.5f);
        linha(alvo, c + sf::Vector2f(-h * 0.5f, -h * 0.5f), c + sf::Vector2f(h * 0.5f, h * 0.5f), 2, cor);
        linha(alvo, c + sf::Vector2f(h * 0.5f, -h * 0.5f), c + sf::Vector2f(-h * 0.5f, h * 0.5f), 2, cor);
    } else if (tipo == TipoIcone::Vitoria) {       // coroa
        sf::VertexArray coroa(sf::TriangleFan);
        sf::Vector2f p[7] = {{-h, h * 0.6f}, {-h, -h * 0.5f}, {-h * 0.5f, 0}, {0, -h}, {h * 0.5f, 0}, {h, -h * 0.5f}, {h, h * 0.6f}};
        coroa.append(sf::Vertex(c + sf::Vector2f(0, h * 0.3f), cor));
        for (int i = 0; i < 7; i++) coroa.append(sf::Vertex(c + p[i], cor));
        alvo.draw(coroa);
    } else {                                       // turno: ampulheta
        sf::ConvexShape ampulheta(4);
        ampulheta.setPoint(0, c + sf::Vector2f(-h * 0.7f, -h));
        ampulheta.setPoint(1, c + sf::Vector2f(h * 0.7f, -h));
        ampulheta.setPoint(2, c + sf::Vector2f(-h * 0.7f, h));
        ampulheta.setPoint(3, c + sf::Vector2f(h * 0.7f, h));
        sf::VertexArray v(sf::Triangles);
        v.append(sf::Vertex(ampulheta.getPoint(0), cor)); v.append(sf::Vertex(ampulheta.getPoint(1), cor)); v.append(sf::Vertex(c, cor));
        v.append(sf::Vertex(ampulheta.getPoint(2), cor)); v.append(sf::Vertex(ampulheta.getPoint(3), cor)); v.append(sf::Vertex(c, cor));
        alvo.draw(v);
    }
}
