#include "Jogo.h"
#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    try {
        bool demo = false;
        int numeroJogadores = 3;
        std::uint32_t semente = std::random_device{}();
        int maxTurnos = 400;
        for (int i = 1; i < argc; ++i) {
            const std::string arg = argv[i];
            if (arg == "--demo") demo = true;
            else if (arg == "--jogadores" && i + 1 < argc) numeroJogadores = std::stoi(argv[++i]);
            else if (arg == "--seed" && i + 1 < argc)
                semente = static_cast<std::uint32_t>(std::stoul(argv[++i]));
            else if (arg == "--max-turnos" && i + 1 < argc) maxTurnos = std::stoi(argv[++i]);
            else if (arg == "--ajuda" || arg == "-h") {
                std::cout << "Uso: war [--demo] [--jogadores N] [--seed N] [--max-turnos N]\n"
                          << "  Sem --demo: jogo interativo por terminal.\n"
                          << "  --demo: jogadores automaticos.\n"
                          << "  --jogadores: de 2 a 6 (padrao 3).\n";
                return 0;
            } else throw std::invalid_argument("Parametro nao reconhecido: " + arg);
        }
        if (numeroJogadores < 2 || numeroJogadores > 6)
            throw std::invalid_argument("Use de 2 a 6 jogadores.");
        std::vector<std::string> nomes;
        for (int i = 0; i < numeroJogadores; ++i) {
            if (demo) nomes.push_back("Bot" + std::to_string(i + 1));
            else {
                std::string nome;
                std::cout << "Nome do jogador " << (i + 1) << ": ";
                if (!std::getline(std::cin, nome)) return 0;
                if (nome.empty()) nome = "Jogador" + std::to_string(i + 1);
                nomes.push_back(nome);
            }
        }
        Jogo jogo(std::move(nomes), semente);
        if (demo) jogo.executarDemo(maxTurnos);
        else jogo.executar();
        return 0;
    } catch (const std::exception& erro) {
        std::cerr << "Erro fatal: " << erro.what() << "\n";
        return 1;
    }
}
