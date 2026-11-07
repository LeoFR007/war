#include "Jogador.h"
#include "Exceptions.h"
#include "Objetivo.h"
#include <algorithm>
#include <array>
#include <stdexcept>
#include <utility>

Jogador::Jogador(std::string nome, CorJogador cor) : nome_(std::move(nome)), cor_(cor) {
    if (nome_.empty()) throw std::invalid_argument("O jogador precisa ter nome.");
}
Jogador::~Jogador() = default;
bool Jogador::possui(const Territorio& territorio) const noexcept { return territorio.dono() == this; }
void Jogador::definirObjetivo(std::unique_ptr<Objetivo> objetivo) {
    if (!objetivo) throw std::invalid_argument("Objetivo nao pode ser nulo.");
    objetivo_ = std::move(objetivo);
}
void Jogador::receberExercitos(int quantidade) {
    if (quantidade < 1) throw std::invalid_argument("Reforcos devem ser positivos.");
    exercitosDisponiveis_ += quantidade;
}
void Jogador::distribuirExercitos(Territorio& destino, int quantidade) {
    if (!possui(destino)) throw TerritorioNaoPertenceAoJogadorException("Territorio de reforco nao pertence ao jogador.");
    if (quantidade < 1 || quantidade > exercitosDisponiveis_)
        throw MovimentoInvalidoException("Exercitos insuficientes para o reforco.");
    destino.adicionarTropas(quantidade);
    exercitosDisponiveis_ -= quantidade;
}

void Jogador::conquistarTerritorio(Territorio& territorio) {
    if (territorio.dono_ == this) return;
    if (territorio.tropas_ != 0)
        throw AtaqueInvalidoException("Nao e possivel conquistar territorio ainda ocupado.");
    Jogador* antigoDono = territorio.dono_;
    if (antigoDono) {
        auto& anteriores = antigoDono->territorios_;
        anteriores.erase(std::remove(anteriores.begin(), anteriores.end(), &territorio), anteriores.end());
    }
    territorio.dono_ = this;
    territorios_.push_back(&territorio);
    if (antigoDono && antigoDono->territorios_.empty()) antigoDono->eliminadoPor_ = this;
}

RelatorioCombate Jogador::atacar(Territorio& origem, Territorio& alvo, int dados, std::mt19937& rng) {
    if (!possui(origem)) throw TerritorioNaoPertenceAoJogadorException("Origem do ataque nao pertence ao jogador.");
    if (possui(alvo)) throw AtaqueInvalidoException("Nao e permitido atacar um territorio proprio.");
    auto resultado = origem.atacar(alvo, dados, rng);
    if (resultado.conquistado) {
        Jogador* derrotado = alvo.dono();
        // Garante uma tropa na origem e ocupa o territorio conquistado.
        const int transferir = std::min(dados, origem.tropas() - 1);
        if (transferir < 1) throw std::logic_error("Combate deixou origem sem ocupacao possivel.");
        origem.removerTropas(transferir);
        alvo.conquistar(*this, transferir);
        resultado.tropasTransferidas = transferir;
        // Regra de eliminacao: quem elimina outro jogador recolhe suas cartas.
        if (derrotado && !derrotado->ativo()) {
            resultado.cartasCapturadas = static_cast<int>(derrotado->cartas_.size());
            for (auto& carta : derrotado->cartas_) cartas_.push_back(std::move(carta));
            derrotado->cartas_.clear();
        }
    }
    return resultado;
}

void Jogador::moverTropas(Territorio& origem, Territorio& destino, int quantidade) {
    if (!possui(origem) || !possui(destino))
        throw TerritorioNaoPertenceAoJogadorException("Movimentacao exige dois territorios proprios.");
    if (!origem.ehVizinho(destino))
        throw TerritorioNaoAdjacenteException("Movimentacao apenas entre territorios vizinhos.");
    if (quantidade < 1 || quantidade >= origem.tropas())
        throw MovimentoInvalidoException("E obrigatorio manter pelo menos uma tropa na origem.");
    origem.removerTropas(quantidade);
    destino.adicionarTropas(quantidade);
}

void Jogador::receberCarta(std::unique_ptr<Carta> carta) {
    if (!carta) throw std::invalid_argument("Carta nula.");
    cartas_.push_back(std::move(carta));
}

namespace {
// Encontra a primeira combinacao valida: 3 iguais ou 3 distintos.
std::vector<std::size_t> encontrarTrinca(const std::vector<std::unique_ptr<Carta>>& cartas) {
    for (std::size_t a = 0; a < cartas.size(); ++a)
        for (std::size_t b = a + 1; b < cartas.size(); ++b)
            for (std::size_t c = b + 1; c < cartas.size(); ++c) {
                const auto x = cartas[a]->tipo(), y = cartas[b]->tipo(), z = cartas[c]->tipo();
                if ((x == y && y == z) || (x != y && x != z && y != z)) return {a, b, c};
            }
    return {};
}
}
bool Jogador::podeTrocarCartas() const { return !encontrarTrinca(cartas_).empty(); }
std::vector<std::unique_ptr<Carta>> Jogador::trocarCartas() {
    auto indices = encontrarTrinca(cartas_);
    if (indices.empty()) throw MovimentoInvalidoException("Nao ha tres cartas compativeis para troca.");
    std::vector<std::unique_ptr<Carta>> entregues;
    for (auto i : indices) entregues.push_back(std::move(cartas_[i]));
    for (auto it = indices.rbegin(); it != indices.rend(); ++it)
        cartas_.erase(cartas_.begin() + static_cast<std::ptrdiff_t>(*it));
    return entregues;
}
