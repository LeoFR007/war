#include "Jogo.h"
#include "WarException.h"
#include <iostream>
#include <random>
#include <string>
#include <vector>

// Uso: ./war [--demo] [--jogadores N] [--seed N]
//   --demo       partida automatica entre bots
//   --jogadores  de 2 a 6 (padrao 3)
//   --seed       numero para repetir exatamente a mesma partida
int main(int argc, char* argv[]) {
    try {
        bool demo = false;
        int quantidade = 3;
        unsigned int semente = std::random_device{}();

        for (int i = 1; i < argc; i++) {
            std::string argumento = argv[i];
            if (argumento == "--demo") demo = true;
            else if (argumento == "--jogadores" && i + 1 < argc) quantidade = std::stoi(argv[++i]);
            else if (argumento == "--seed" && i + 1 < argc) semente = std::stoul(argv[++i]);
            else throw WarException("Parametro desconhecido: " + argumento +
                                    "\nUso: ./war [--demo] [--jogadores N] [--seed N]");
        }

        std::vector<std::string> nomes;
        for (int i = 1; i <= quantidade; i++) {
            std::string nome = "Bot" + std::to_string(i);
            if (!demo) {
                std::cout << "Nome do jogador " << i << ": ";
                std::getline(std::cin, nome);
                if (nome.empty()) nome = "Jogador" + std::to_string(i);
            }
            nomes.push_back(nome);
        }

        Jogo jogo(nomes, demo ? 0 : quantidade, semente);
        std::cout << "=== WAR - Programacao Orientada a Objetos em C++ ===\n";
        if (!demo) jogo.getMapa().mostrar();
        jogo.executar(demo ? 500 : 100000);
        return 0;
    } catch (const std::exception& erro) {
        std::cerr << "Erro: " << erro.what() << "\n";
        return 1;
    }
}
