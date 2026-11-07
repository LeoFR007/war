#pragma once
#include <string>

enum class CorJogador { Vermelho, Azul, Verde, Amarelo, Roxo, Preto };
enum class TipoCarta { Infantaria, Cavalaria, Artilharia };
enum class FaseTurno { Reforco, Ataque, Movimentacao, Fim };
enum class ResultadoCombate { AvancoAtacante, ResistenciaDefensor, PerdasMutuas, Conquista };

inline std::string nomeCor(CorJogador cor) {
    switch (cor) {
        case CorJogador::Vermelho: return "Vermelho";
        case CorJogador::Azul: return "Azul";
        case CorJogador::Verde: return "Verde";
        case CorJogador::Amarelo: return "Amarelo";
        case CorJogador::Roxo: return "Roxo";
        case CorJogador::Preto: return "Preto";
    }
    return "Desconhecida";
}
