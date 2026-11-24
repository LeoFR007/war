#include "JogadorHumano.h"
#include "Jogo.h"
#include "Objetivo.h"
#include "Territorio.h"
#include "WarException.h"
#include <iostream>

JogadorHumano::JogadorHumano(const std::string& nome, const std::string& cor) : Jogador(nome, cor) {}

void JogadorHumano::jogarTurno(Jogo& jogo) {
    std::cout << "Objetivo: " << getObjetivo()->descricao() << "\n"
              << "Comandos em qualquer fase: mapa | status | vizinhos ID | sair\n";
    faseReforco(jogo);
    if (!jogo.terminou()) faseAtaque(jogo);
    if (!jogo.terminou()) faseMovimento(jogo);
}

// Le uma linha do teclado. Se a entrada acabar, age como "sair".
std::string JogadorHumano::lerLinha(const std::string& prompt) const {
    std::cout << prompt;
    std::string linha;
    if (!std::getline(std::cin, linha)) return "sair";
    return linha;
}

// Comandos que valem em todas as fases. Retorna true se reconheceu o comando.
bool JogadorHumano::comandoGeral(Jogo& jogo, const std::string& comando, std::istringstream& entrada) const {
    if (comando == "mapa") {
        jogo.getMapa().mostrar();
    } else if (comando == "status") {
        mostrarStatus();
    } else if (comando == "vizinhos") {
        int id = 0;
        entrada >> id;
        for (Territorio* vizinho : jogo.getMapa().getTerritorio(id)->getVizinhos())
            std::cout << "  " << vizinho->getId() << ") " << vizinho->getNome() << "\n";
    } else if (comando == "sair") {
        jogo.encerrar();
    } else {
        return false;
    }
    return true;
}

void JogadorHumano::mostrarStatus() const {
    std::cout << getNome() << " | Exercitos para posicionar: " << getExercitos() << "\nTerritorios:\n";
    for (Territorio* territorio : getTerritorios())
        std::cout << "  " << territorio->getId() << ") " << territorio->getNome() << " (" << territorio->getTropas() << ")\n";
    std::cout << "Cartas:\n";
    for (const Carta& carta : getCartas()) std::cout << "  - " << carta.descricao() << "\n";
}

void JogadorHumano::faseReforco(Jogo& jogo) {
    while (getExercitos() > 0 && !jogo.terminou()) {
        std::istringstream entrada(lerLinha("[REFORCO] r ID QTD | trocar > "));
        std::string comando;
        entrada >> comando;
        try {
            if (comando == "r") {
                int id = 0, quantidade = 0;
                entrada >> id >> quantidade;
                colocarExercitos(jogo.getMapa().getTerritorio(id), quantidade);
            } else if (comando == "trocar") {
                std::cout << "Troca feita: +" << jogo.trocarCartas(this) << " exercitos.\n";
            } else if (!comandoGeral(jogo, comando, entrada)) {
                std::cout << "Exemplo: r 3 5 (coloca 5 exercitos no territorio 3)\n";
            }
        } catch (const WarException& erro) {
            std::cout << "Acao recusada: " << erro.what() << "\n";
        }
    }
}

void JogadorHumano::faseAtaque(Jogo& jogo) {
    while (!jogo.terminou()) {
        std::istringstream entrada(lerLinha("[ATAQUE] a ORIGEM ALVO DADOS | passar > "));
        std::string comando;
        entrada >> comando;
        if (comando == "passar") return;
        try {
            if (comando == "a") {
                int origem = 0, alvo = 0, dados = 0;
                entrada >> origem >> alvo >> dados;
                ResultadoAtaque r = jogo.atacar(this, jogo.getMapa().getTerritorio(origem),
                                                jogo.getMapa().getTerritorio(alvo), dados);
                std::cout << "Ataque [";
                for (int valor : r.dadosAtaque) std::cout << " " << valor;
                std::cout << " ] x Defesa [";
                for (int valor : r.dadosDefesa) std::cout << " " << valor;
                std::cout << " ] | perdas: atacante -" << r.perdasAtaque << ", defensor -" << r.perdasDefesa << "\n";
                if (r.conquistou) std::cout << "Territorio conquistado!\n";
            } else if (!comandoGeral(jogo, comando, entrada)) {
                std::cout << "Exemplo: a 5 7 3 (ataca do 5 para o 7 com 3 dados)\n";
            }
        } catch (const WarException& erro) {
            std::cout << "Acao recusada: " << erro.what() << "\n";
        }
    }
}

void JogadorHumano::faseMovimento(Jogo& jogo) {
    while (!jogo.terminou()) {
        std::istringstream entrada(lerLinha("[MOVIMENTO] m ORIGEM DESTINO QTD | fim > "));
        std::string comando;
        entrada >> comando;
        if (comando == "fim") return;
        try {
            if (comando == "m") {
                int origem = 0, destino = 0, quantidade = 0;
                entrada >> origem >> destino >> quantidade;
                moverTropas(jogo.getMapa().getTerritorio(origem), jogo.getMapa().getTerritorio(destino), quantidade);
                std::cout << "Tropas movidas.\n";
            } else if (!comandoGeral(jogo, comando, entrada)) {
                std::cout << "Exemplo: m 1 2 3 (move 3 tropas do 1 para o 2)\n";
            }
        } catch (const WarException& erro) {
            std::cout << "Acao recusada: " << erro.what() << "\n";
        }
    }
}
