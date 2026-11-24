#include "JogadorBot.h"
#include "Jogo.h"
#include "Territorio.h"
#include "WarException.h"
#include <algorithm>

JogadorBot::JogadorBot(const std::string& nome, const std::string& cor) : Jogador(nome, cor) {}

bool JogadorBot::ehAutomatico() const { return true; }

void JogadorBot::jogarTurno(Jogo& jogo) {
    reforcar();
    atacar(jogo);
    if (!jogo.terminou()) mover();
}

bool JogadorBot::ehFronteira(const Territorio* territorio) const {
    for (Territorio* vizinho : territorio->getVizinhos()) {
        if (!possui(vizinho)) return true;
    }
    return false;
}

// Coloca todos os exercitos onde a vantagem sobre o vizinho inimigo mais fraco e maior.
void JogadorBot::reforcar() {
    Territorio* escolhido = getTerritorios()[0];
    // vantagem = minhas tropas - tropas do vizinho inimigo. começa bem negativo pra
    // garantir que algum território seja escolhido mesmo se todos estiverem em desvantagem
    int melhorVantagem = -100000;
    for (Territorio* territorio : getTerritorios()) {
        for (Territorio* vizinho : territorio->getVizinhos()) {
            int vantagem = territorio->getTropas() - vizinho->getTropas();
            if (!possui(vizinho) && vantagem > melhorVantagem) {
                melhorVantagem = vantagem;
                escolhido = territorio;
            }
        }
    }
    colocarExercitos(escolhido, getExercitos());
}

// Leva as tropas paradas no interior para um vizinho que esteja na fronteira.
void JogadorBot::mover() {
    // território "interno" (sem inimigo em volta) não serve pra nada com tropa parada,
    // então manda tudo pra um vizinho que esteja na fronteira
    std::vector<Territorio*> meus = getTerritorios(); // copia: a lista pode mudar
    for (Territorio* territorio : meus) {
        if (territorio->getTropas() < 2 || ehFronteira(territorio)) continue;
        for (Territorio* vizinho : territorio->getVizinhos()) {
            if (ehFronteira(vizinho)) {
                moverTropas(territorio, vizinho, territorio->getTropas() - 1);
                break;
            }
        }
    }
}

// Ataca enquanto tiver mais tropas que algum vizinho inimigo.
void JogadorBot::atacar(Jogo& jogo) {
    // a cada volta procura o melhor ataque de novo, porque as tropas mudam depois de cada
    // combate. o limite de 100 é só pra nunca ficar num laço infinito
    // (antes era só com vantagem >= 3 e as partidas empacavam, por isso agora é > 0)
    for (int tentativa = 0; tentativa < 100 && !jogo.terminou(); tentativa++) {
        Territorio* origem = nullptr;
        Territorio* alvo = nullptr;
        int melhorVantagem = 0;
        for (Territorio* territorio : getTerritorios()) {
            for (Territorio* vizinho : territorio->getVizinhos()) {
                int vantagem = territorio->getTropas() - vizinho->getTropas();
                if (!possui(vizinho) && vantagem > melhorVantagem) {
                    melhorVantagem = vantagem;
                    origem = territorio;
                    alvo = vizinho;
                }
            }
        }
        if (origem == nullptr) return; // nenhum ataque vantajoso
        try {
            jogo.atacar(this, origem, alvo, std::min(3, origem->getTropas() - 1));
        } catch (const WarException&) {
            return;
        }
    }
}
