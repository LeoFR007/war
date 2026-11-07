#include "Territorio.h"
#include "Exceptions.h"
#include "Jogador.h"
#include <algorithm>
#include <stdexcept>
#include <utility>

Territorio::Territorio(std::string nome) : nome_(std::move(nome)) {
    if (nome_.empty()) throw std::invalid_argument("O territorio precisa ter nome.");
}

void Territorio::adicionarVizinho(Territorio& outro) {
    if (this == &outro) throw std::invalid_argument("Territorio nao pode ser vizinho dele mesmo.");
    if (!ehVizinho(outro)) vizinhos_.push_back(&outro);
}

bool Territorio::ehVizinho(const Territorio& outro) const noexcept {
    return std::find(vizinhos_.begin(), vizinhos_.end(), &outro) != vizinhos_.end();
}

void Territorio::adicionarTropas(int quantidade) {
    if (quantidade <= 0) throw std::invalid_argument("Quantidade de tropas deve ser positiva.");
    tropas_ += quantidade;
}
void Territorio::removerTropas(int quantidade) {
    if (quantidade <= 0 || quantidade > tropas_)
        throw std::invalid_argument("Quantidade invalida de tropas para remocao.");
    tropas_ -= quantidade;
}

RelatorioCombate Territorio::atacar(Territorio& alvo, int quantidadeDados, std::mt19937& gerador) {
    if (!dono_ || !alvo.dono_ || dono_ == alvo.dono_)
        throw AtaqueInvalidoException("Ataque exige jogadores adversarios com territorios ocupados.");
    if (!ehVizinho(alvo)) throw TerritorioNaoAdjacenteException("Territorios nao sao adjacentes.");
    if (tropas_ < 2 || quantidadeDados < 1 || quantidadeDados > 3 || quantidadeDados >= tropas_)
        throw AtaqueInvalidoException("Ataque exige 2+ tropas na origem e 1 a 3 dados validos.");
    if (alvo.tropas_ < 1) throw AtaqueInvalidoException("O territorio adversario nao possui tropas.");

    std::uniform_int_distribution<int> dado(1, 6);
    RelatorioCombate relatorio;
    const int dadosDefesa = std::min(3, alvo.tropas_);
    for (int i = 0; i < quantidadeDados; ++i) relatorio.dadosAtacante.push_back(dado(gerador));
    for (int i = 0; i < dadosDefesa; ++i) relatorio.dadosDefensor.push_back(dado(gerador));
    // Lambda obrigatoria: comparar dados do maior para o menor.
    const auto decrescente = [](int a, int b) { return a > b; };
    std::sort(relatorio.dadosAtacante.begin(), relatorio.dadosAtacante.end(), decrescente);
    std::sort(relatorio.dadosDefensor.begin(), relatorio.dadosDefensor.end(), decrescente);
    const auto total = std::min(relatorio.dadosAtacante.size(), relatorio.dadosDefensor.size());
    for (std::size_t i = 0; i < total; ++i) {
        if (relatorio.dadosAtacante[i] > relatorio.dadosDefensor[i]) ++relatorio.baixasDefensor;
        else ++relatorio.baixasAtacante; // Empate favorece o defensor.
    }
    if (relatorio.baixasAtacante) removerTropas(relatorio.baixasAtacante);
    if (relatorio.baixasDefensor) alvo.removerTropas(relatorio.baixasDefensor);
    relatorio.conquistado = alvo.tropas_ == 0;
    if (relatorio.conquistado) relatorio.resultado = ResultadoCombate::Conquista;
    else if (relatorio.baixasAtacante && relatorio.baixasDefensor)
        relatorio.resultado = ResultadoCombate::PerdasMutuas;
    else if (relatorio.baixasDefensor) relatorio.resultado = ResultadoCombate::AvancoAtacante;
    else relatorio.resultado = ResultadoCombate::ResistenciaDefensor;
    return relatorio;
}

void Territorio::conquistar(Jogador& novoDono, int quantidadeTropas) {
    if (quantidadeTropas < 1 || tropas_ != 0)
        throw AtaqueInvalidoException("Conquista exige territorio derrotado e tropas de ocupacao.");
    novoDono.conquistarTerritorio(*this);
    adicionarTropas(quantidadeTropas);
}
