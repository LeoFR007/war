#include "Animacoes.h"
#include "Jogo.h"
#include "Tema.h"
#include "Territorio.h"
#include <algorithm>
#include <cmath>

Animacoes::Animacoes() : relogio(0) {}

void Animacoes::observar(const Jogo& jogo, const std::vector<sf::Vector2f>& centros) {
    const Mapa& mapa = jogo.getMapa();
    int quantidade = mapa.getQuantidadeTerritorios();
    bool primeiraVez = tropasAntes.empty();
    if (primeiraVez) {
        tropasAntes.assign(quantidade + 1, 0);
        donosAntes.assign(quantidade + 1, nullptr);
        brilhos.assign(quantidade + 1, 0);
    }
    std::vector<int> diferenca(quantidade + 1, 0);
    for (int id = 1; id <= quantidade; id++) {
        const Territorio* territorio = mapa.getTerritorio(id);
        diferenca[id] = territorio->getTropas() - tropasAntes[id];
        if (!primeiraVez) {
            sf::Color corDono = corDoJogador(territorio->getDono()->getCor());
            if (territorio->getDono() != donosAntes[id]) {             // conquista
                brilhos[id] = 1;
                criarOnda(centros[id], COR_CONQUISTA, 46, 0.7f);
            } else if (diferenca[id] != 0) {                           // reforco, perda ou movimento
                std::string sinal = diferenca[id] > 0 ? "+" : "";
                textos.push_back({centros[id], sinal + std::to_string(diferenca[id]),
                                  diferenca[id] > 0 ? clarear(corDono, 0.35f) : COR_ATAQUE, 0});
            }
        }
    }
    // as animações não sabem o que o jogador fez, elas só comparam com o quadro anterior.
    // por isso o movimento é "adivinhado": se um ganhou N e o vizinho perdeu N, foi movimento
    // Movimento: um territorio ganhou N e um vizinho do mesmo dono perdeu exatamente N.
    if (!primeiraVez) {
        for (int id = 1; id <= quantidade; id++) {
            const Territorio* destino = mapa.getTerritorio(id);
            if (diferenca[id] <= 0 || destino->getDono() != donosAntes[id]) continue;
            for (const Territorio* vizinho : destino->getVizinhos()) {
                int v = vizinho->getId();
                if (diferenca[v] == -diferenca[id] && vizinho->getDono() == destino->getDono()) {
                    deslocamentos.push_back({centros[v], centros[id], corDoJogador(destino->getDono()->getCor()),
                                             diferenca[id], 0});
                    break;
                }
            }
        }
    }
    for (int id = 1; id <= quantidade; id++) {
        tropasAntes[id] = mapa.getTerritorio(id)->getTropas();
        donosAntes[id] = mapa.getTerritorio(id)->getDono();
    }
}

void Animacoes::avancar(float segundos) {
    relogio += segundos;
    for (float& brilho : brilhos) brilho = std::max(0.0f, brilho - segundos * 1.4f);
    for (size_t i = 0; i < textos.size(); i++) textos[i].idade += segundos;
    for (size_t i = 0; i < ondas.size(); i++) ondas[i].idade += segundos;
    for (size_t i = 0; i < deslocamentos.size(); i++) deslocamentos[i].idade += segundos;
    // remove os efeitos que ja terminaram
    for (size_t i = textos.size(); i-- > 0;) if (textos[i].idade > 1.3f) textos.erase(textos.begin() + i);
    for (size_t i = ondas.size(); i-- > 0;) if (ondas[i].idade > ondas[i].duracao) ondas.erase(ondas.begin() + i);
    for (size_t i = deslocamentos.size(); i-- > 0;) if (deslocamentos[i].idade > 0.6f) deslocamentos.erase(deslocamentos.begin() + i);
}

void Animacoes::criarOnda(sf::Vector2f mundo, sf::Color cor, float raioFinal, float duracao) {
    ondas.push_back({mundo, cor, 0, duracao, raioFinal});
}

float Animacoes::getRelogio() const { return relogio; }

float Animacoes::brilhoDe(int id) const {
    return id >= 0 && id < static_cast<int>(brilhos.size()) ? brilhos[id] : 0;
}

void Animacoes::desenharNoMundo(sf::RenderTarget& alvo) const {
    for (const Onda& onda : ondas) {
        float t = onda.idade / onda.duracao;          // 0 -> 1
        float raio = onda.raioFinal * (0.3f + 0.7f * t);
        circulo(alvo, onda.mundo, raio, sf::Color::Transparent, comAlfa(onda.cor, 230 * (1 - t)), 2.5f * (1 - t) + 0.5f);
    }
}

void Animacoes::desenharNaTela(sf::RenderTarget& alvo, const Camera& camera, const Fontes& fontes) const {
    for (const Deslocamento& d : deslocamentos) {
        float t = d.idade / 0.6f;
        float suave = t * t * (3 - 2 * t);            // comeca e termina devagar
        sf::Vector2f posicao = camera.paraTela(d.de + (d.para - d.de) * suave);
        circulo(alvo, posicao, 11, comAlfa(d.cor, 235 * (1 - t * 0.5f)), comAlfa(COR_TEXTO, 200), 1.5f);
        escrever(alvo, fontes.titulo, std::to_string(d.quantidade), posicao.x, posicao.y - 9, 14, COR_TEXTO, Alinhamento::Centro);
    }
    for (const TextoFlutuante& texto : textos) {
        float t = texto.idade / 1.3f;
        sf::Vector2f posicao = camera.paraTela(texto.mundo) + sf::Vector2f(0, -26 - 34 * t);
        if (!camera.contem(posicao)) continue;
        float alfa = 255 * (t < 0.7f ? 1 : (1 - t) / 0.3f);
        unsigned int tamanho = texto.texto.size() > 4 ? 13 : 17;
        escrever(alvo, fontes.titulo, texto.texto, posicao.x + 1, posicao.y + 1, tamanho, comAlfa(sf::Color::Black, alfa * 0.7f), Alinhamento::Centro, 1.2f);
        escrever(alvo, fontes.titulo, texto.texto, posicao.x, posicao.y, tamanho, comAlfa(texto.cor, alfa), Alinhamento::Centro, 1.2f);
    }
}
