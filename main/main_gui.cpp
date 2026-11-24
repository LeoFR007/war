#include "BarraSuperiorView.h"
#include "ControladorJogo.h"
#include "Desenho.h"
#include "EventosView.h"
#include "Jogo.h"
#include "MapaView.h"
#include "PainelView.h"
#include "Tema.h"
#include "WarException.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>

// Interface grafica (SFML). Uso: ./war_gui [--jogadores N] [--humanos H] [--seed N]
//   --jogadores  total de jogadores, de 2 a 6 (padrao 4)
//   --humanos    quantos sao pessoas; os demais sao bots (padrao 1; use 0 para so assistir)
int main(int argc, char* argv[]) {
    try {
        int quantidade = 4;
        int humanos = 1;
        unsigned int semente = std::random_device{}();
        for (int i = 1; i < argc; i++) {
            std::string argumento = argv[i];
            if (argumento == "--jogadores" && i + 1 < argc) quantidade = std::stoi(argv[++i]);
            else if (argumento == "--humanos" && i + 1 < argc) humanos = std::stoi(argv[++i]);
            else if (argumento == "--seed" && i + 1 < argc) semente = std::stoul(argv[++i]);
            else throw WarException("Parametro desconhecido: " + argumento +
                                    "\nUso: ./war_gui [--jogadores N] [--humanos H] [--seed N]");
        }
        std::vector<std::string> nomes;
        for (int i = 1; i <= quantidade; i++) {
            if (i <= humanos) nomes.push_back("Jogador " + std::to_string(i));
            else nomes.push_back("Bot " + std::to_string(i - humanos));
        }

        // ---------- Model ----------
        Jogo jogo(nomes, humanos, semente);

        // ---------- Janela ----------
        // A interface e desenhada em 1440 x 860 e a janela se ajusta a tela do computador.
        sf::VideoMode monitor = sf::VideoMode::getDesktopMode();
        float escala = std::min(1.0f, std::min(monitor.width * 0.94f / LARGURA_TELA, monitor.height * 0.88f / ALTURA_TELA));
        sf::ContextSettings configuracao;
        configuracao.antialiasingLevel = 4;   // bordas suaves nas formas
        sf::RenderWindow janela(sf::VideoMode(static_cast<unsigned int>(LARGURA_TELA * escala),
                                              static_cast<unsigned int>(ALTURA_TELA * escala)),
                                "WAR - Conquista Global", sf::Style::Titlebar | sf::Style::Close, configuracao);
        janela.setView(sf::View(sf::FloatRect(0, 0, LARGURA_TELA, ALTURA_TELA)));
        janela.setVerticalSyncEnabled(true);
        janela.setFramerateLimit(60);

        // ---------- View ----------
        Fontes fontes;
        fontes.carregar("assets/fonts");
        MapaView mapaView(fontes, jogo.getMapa());      // gera as regioes do mapa uma vez
        PainelView painelView(fontes);
        EventosView eventosView(fontes);
        BarraSuperiorView barraView(fontes);

        // ---------- Controller ----------
        ControladorJogo controlador(jogo);

        sf::Clock relogio;
        while (janela.isOpen()) {
            sf::Vector2f mouse = janela.mapPixelToCoords(sf::Mouse::getPosition(janela));
            sf::Event evento;
            while (janela.pollEvent(evento)) {
                if (evento.type == sf::Event::Closed) janela.close();
                controlador.tratarEvento(evento, mouse, mapaView, painelView);
            }
            float segundos = std::min(relogio.restart().asSeconds(), 0.1f);
            controlador.atualizar(segundos);
            mapaView.atualizar(jogo, controlador, mouse, segundos);
            barraView.atualizar(jogo, segundos);

            janela.clear(COR_FUNDO);
            mapaView.desenhar(janela, jogo, controlador, mouse);
            painelView.desenhar(janela, jogo, controlador, mouse);
            eventosView.desenhar(janela, jogo);
            barraView.desenhar(janela, jogo, controlador);
            barraView.desenharFaixaDeTurno(janela, jogo);
            if (jogo.terminou()) painelView.desenharVitoria(janela, jogo);
            janela.display();
        }
        return 0;
    } catch (const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << "\n";
        return 1;
    }
}
