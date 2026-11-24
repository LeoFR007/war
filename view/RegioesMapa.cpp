#include "RegioesMapa.h"
#include "Tema.h"
#include <algorithm>
#include <cmath>

// Teste classico "ponto dentro do poligono" (raio horizontal conta quantas arestas cruza).
static bool dentroDoPoligono(const std::vector<sf::Vector2f>& poligono, sf::Vector2f p) {
    bool dentro = false;
    for (size_t i = 0, j = poligono.size() - 1; i < poligono.size(); j = i++) {
        const sf::Vector2f& a = poligono[i];
        const sf::Vector2f& b = poligono[j];
        if ((a.y > p.y) != (b.y > p.y) && p.x < (b.x - a.x) * (p.y - a.y) / (b.y - a.y) + a.x) dentro = !dentro;
    }
    return dentro;
}

// Pequeno "ruido" deterministico para dar textura de papel ao terreno.
static float granulado(int x, int y) {
    unsigned int h = static_cast<unsigned int>(x) * 374761393u + static_cast<unsigned int>(y) * 668265263u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return ((h & 255) / 255.0f) - 0.5f;
}

int RegioesMapa::idEm(int px, int py) const {
    if (px < 0 || py < 0 || px >= largura || py >= altura) return 0;
    return idPorPixel[py * largura + px];
}

// Distancia (em pixels, "quarteirao") ate a borda mais proxima, calculada em duas passadas.
//  paraTerra = true : para cada pixel de terra, distancia ate a fronteira do seu territorio.
//  paraTerra = false: para cada pixel de mar, distancia ate a terra.
std::vector<int> RegioesMapa::distanciaAteBorda(bool paraTerra) const {
    const int INFINITO = 1000000;
    std::vector<int> d(idPorPixel.size(), INFINITO);
    for (int py = 0; py < altura; py++) {
        for (int px = 0; px < largura; px++) {
            int id = idEm(px, py);
            bool origem;
            if (paraTerra) {
                origem = id != 0 && (idEm(px + 1, py) != id || idEm(px - 1, py) != id ||
                                     idEm(px, py + 1) != id || idEm(px, py - 1) != id);
            } else {
                origem = id != 0;
            }
            if (origem) d[py * largura + px] = 0;
        }
    }
    for (int py = 0; py < altura; py++)               // passada de cima para baixo
        for (int px = 0; px < largura; px++) {
            int i = py * largura + px;
            if (px > 0) d[i] = std::min(d[i], d[i - 1] + 1);
            if (py > 0) d[i] = std::min(d[i], d[i - largura] + 1);
        }
    for (int py = altura - 1; py >= 0; py--)          // passada de baixo para cima
        for (int px = largura - 1; px >= 0; px--) {
            int i = py * largura + px;
            if (px < largura - 1) d[i] = std::min(d[i], d[i + 1] + 1);
            if (py < altura - 1) d[i] = std::min(d[i], d[i + largura] + 1);
        }
    return d;
}

void RegioesMapa::gerar(sf::Vector2f tamanhoMundo, const std::vector<sf::Vector2f>& centros,
                        const std::vector<int>& continenteDe, const std::vector<std::vector<sf::Vector2f>>& formas) {
    largura = static_cast<int>(tamanhoMundo.x) * ESCALA;
    altura = static_cast<int>(tamanhoMundo.y) * ESCALA;
    int quantidade = static_cast<int>(centros.size()) - 1;   // centros[0] nao e usado
    idPorPixel.assign(largura * altura, 0);

    // resumo da ideia: o mapa não tem o desenho de cada território, só o "centro" de cada um.
    // então pra cada pixel a gente vê em qual continente ele está e pinta com o território
    // mais perto (isso chama diagrama de Voronoi). demora meio segundo, mas só roda
    // uma vez quando o jogo abre
    // 1) Cada pixel de continente recebe o territorio mais proximo do mesmo continente.
    for (int py = 0; py < altura; py++) {
        for (int px = 0; px < largura; px++) {
            float x = (px + 0.5f) / ESCALA, y = (py + 0.5f) / ESCALA;
            // ondulacao: fronteiras e costas deixam de ser retas
            sf::Vector2f p(x + 7 * std::sin(y * 0.045f + 1.3f) + 3 * std::sin(y * 0.13f + x * 0.03f),
                           y + 7 * std::sin(x * 0.040f + 0.7f) + 3 * std::sin(x * 0.11f - y * 0.05f));
            int continente = -1;
            for (size_t c = 0; c < formas.size() && continente < 0; c++)
                if (dentroDoPoligono(formas[c], p)) continente = static_cast<int>(c);
            if (continente < 0) continue;

            int melhor = 0;
            float menorDistancia = 95 * 95;   // longe demais de qualquer territorio = mar
            for (int id = 1; id <= quantidade; id++) {
                if (continenteDe[id] != continente) continue;
                sf::Vector2f d = p - centros[id];
                float distancia = d.x * d.x + d.y * d.y;
                if (distancia < menorDistancia) { menorDistancia = distancia; melhor = id; }
            }
            idPorPixel[py * largura + px] = melhor;
        }
    }
    // 2) Garante que todo territorio tenha terra em volta do seu centro (ilhas pequenas).
    for (int id = 1; id <= quantidade; id++) {
        int cx = static_cast<int>(centros[id].x * ESCALA), cy = static_cast<int>(centros[id].y * ESCALA);
        int r = 16 * ESCALA;
        for (int py = cy - r; py <= cy + r; py++)
            for (int px = cx - r; px <= cx + r; px++)
                if ((px - cx) * (px - cx) + (py - cy) * (py - cy) <= r * r && px >= 0 && py >= 0 &&
                    px < largura && py < altura && idPorPixel[py * largura + px] == 0)
                    idPorPixel[py * largura + px] = id;
    }

    std::vector<int> distanciaBorda = distanciaAteBorda(true);
    std::vector<int> distanciaTerra = distanciaAteBorda(false);

    // 3) Quais regioes se encostam em terra (as demais ligacoes sao rotas maritimas).
    tocam.assign(quantidade + 1, std::vector<bool>(quantidade + 1, false));
    for (int py = 0; py < altura; py++)
        for (int px = 0; px < largura; px++) {
            int a = idEm(px, py), direita = idEm(px + 1, py), baixo = idEm(px, py + 1);
            if (a && direita && a != direita) tocam[a][direita] = tocam[direita][a] = true;
            if (a && baixo && a != baixo) tocam[a][baixo] = tocam[baixo][a] = true;
        }

    // 4) Imagem de fundo (mar com degrade, grade, sombra dos continentes e agua rasa na costa)
    //    e imagem das fronteiras (linhas escuras entre territorios e contorno da costa).
    sf::Image imagemFundo, imagemFronteiras;
    imagemFundo.create(largura, altura);
    imagemFronteiras.create(largura, altura, sf::Color::Transparent);
    const int sombraX = 7 * ESCALA, sombraY = 9 * ESCALA;
    for (int py = 0; py < altura; py++) {
        for (int px = 0; px < largura; px++) {
            int i = py * largura + px;
            float x = static_cast<float>(px) / largura, y = static_cast<float>(py) / altura;
            float vinheta = std::min(1.0f, std::sqrt((x - 0.5f) * (x - 0.5f) + (y - 0.45f) * (y - 0.45f)) * 1.4f);
            sf::Color mar = misturar(COR_OCEANO_CLARO, COR_OCEANO_ESCURO, 0.25f + 0.75f * vinheta);
            if ((px / ESCALA) % 60 == 0 || (py / ESCALA) % 60 == 0) mar = clarear(mar, 0.05f);   // grade
            int d = distanciaTerra[i];
            if (idPorPixel[i] == 0) {
                if (d >= 3 && d <= 16 * ESCALA) mar = misturar(mar, sf::Color(46, 92, 118), 0.42f * (1 - (d - 3.0f) / (16 * ESCALA)));
                if (idEm(px - sombraX, py - sombraY) != 0) mar = escurecer(mar, 0.45f);        // sombra
                if (d >= 1 && d <= 2 * ESCALA) imagemFronteiras.setPixel(px, py, COR_COSTA);     // linha da costa
            } else {
                mar = sf::Color(30, 34, 32);
                int vizinho = 0;
                if (idEm(px + 1, py) != idPorPixel[i]) vizinho = idEm(px + 1, py);
                if (idEm(px, py + 1) != idPorPixel[i]) vizinho = idEm(px, py + 1);
                if (distanciaBorda[i] == 0 && vizinho != 0) imagemFronteiras.setPixel(px, py, comAlfa(COR_FRONTEIRA, 235));
                else if (distanciaBorda[i] == 1) imagemFronteiras.setPixel(px, py, comAlfa(COR_FRONTEIRA, 90));
            }
            imagemFundo.setPixel(px, py, mar);
        }
    }
    fundo.loadFromImage(imagemFundo);
    fundo.setSmooth(true);
    fronteiras.loadFromImage(imagemFronteiras);
    fronteiras.setSmooth(true);

    // 5) Uma textura de preenchimento e uma de contorno para cada territorio (recortadas no tamanho dele).
    preenchimentos.assign(quantidade + 1, sf::Texture());
    contornos.assign(quantidade + 1, sf::Texture());
    cantos.assign(quantidade + 1, sf::Vector2f(0, 0));
    for (int id = 1; id <= quantidade; id++) {
        int minX = largura, minY = altura, maxX = -1, maxY = -1;
        for (int py = 0; py < altura; py++)
            for (int px = 0; px < largura; px++)
                if (idPorPixel[py * largura + px] == id) {
                    minX = std::min(minX, px); maxX = std::max(maxX, px);
                    minY = std::min(minY, py); maxY = std::max(maxY, py);
                }
        if (maxX < 0) continue;
        int w = maxX - minX + 1, h = maxY - minY + 1;
        sf::Image area, borda;
        area.create(w, h, sf::Color(255, 255, 255, 0));
        borda.create(w, h, sf::Color(255, 255, 255, 0));
        for (int py = minY; py <= maxY; py++)
            for (int px = minX; px <= maxX; px++) {
                int i = py * largura + px;
                if (idPorPixel[i] != id) continue;
                int d = distanciaBorda[i];
                // sombreado: mais escuro perto da borda, mais claro no meio (efeito de relevo)
                float luz = 0.72f + 0.28f * std::min(d, 14 * ESCALA) / (14.0f * ESCALA) + 0.05f * granulado(px, py);
                sf::Uint8 v = static_cast<sf::Uint8>(255 * std::min(1.0f, luz));
                area.setPixel(px - minX, py - minY, sf::Color(v, v, v, 255));
                if (d <= 3 * ESCALA) {
                    sf::Uint8 alfa = d <= 2 * ESCALA ? 255 : 120;
                    borda.setPixel(px - minX, py - minY, sf::Color(255, 255, 255, alfa));
                }
            }
        preenchimentos[id].loadFromImage(area);
        preenchimentos[id].setSmooth(true);
        contornos[id].loadFromImage(borda);
        contornos[id].setSmooth(true);
        cantos[id] = sf::Vector2f(static_cast<float>(minX) / ESCALA, static_cast<float>(minY) / ESCALA);
    }
}

int RegioesMapa::territorioEm(sf::Vector2f pontoMundo) const {
    return idEm(static_cast<int>(pontoMundo.x * ESCALA), static_cast<int>(pontoMundo.y * ESCALA));
}

bool RegioesMapa::tocamEmTerra(int a, int b) const { return tocam[a][b]; }

void RegioesMapa::desenharFundo(sf::RenderTarget& alvo) const {
    sf::Sprite sprite(fundo);
    sprite.setScale(1.0f / ESCALA, 1.0f / ESCALA);
    alvo.draw(sprite);
}

void RegioesMapa::desenharTerritorio(sf::RenderTarget& alvo, int id, sf::Color cor) const {
    sf::Sprite sprite(preenchimentos[id]);
    sprite.setPosition(cantos[id]);
    sprite.setScale(1.0f / ESCALA, 1.0f / ESCALA);
    sprite.setColor(cor);   // a textura branca e "tingida" com a cor do dono
    alvo.draw(sprite);
}

void RegioesMapa::desenharContorno(sf::RenderTarget& alvo, int id, sf::Color cor) const {
    sf::Sprite sprite(contornos[id]);
    sprite.setPosition(cantos[id]);
    sprite.setScale(1.0f / ESCALA, 1.0f / ESCALA);
    sprite.setColor(cor);
    alvo.draw(sprite);
}

void RegioesMapa::desenharFronteiras(sf::RenderTarget& alvo) const {
    sf::Sprite sprite(fronteiras);
    sprite.setScale(1.0f / ESCALA, 1.0f / ESCALA);
    alvo.draw(sprite);
}
