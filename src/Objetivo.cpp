#include "Objetivo.h"
#include "Continente.h"
#include "Jogador.h"
#include <algorithm>
#include <stdexcept>
#include <utility>

ObjetivoConquistaTerritorios::ObjetivoConquistaTerritorios(int quantidade) : quantidade_(quantidade) {
    if (quantidade <= 0) throw std::invalid_argument("Quantidade invalida para objetivo.");
}
bool ObjetivoConquistaTerritorios::foiConcluido(Jogador* jogador) const {
    return jogador != nullptr && static_cast<int>(jogador->territorios().size()) >= quantidade_;
}
std::string ObjetivoConquistaTerritorios::descricao() const {
    return "Conquistar pelo menos " + std::to_string(quantidade_) + " territorios.";
}
ObjetivoConquistarContinente::ObjetivoConquistarContinente(std::vector<const Continente*> continentes)
    : continentes_(std::move(continentes)) {
    if (continentes_.empty() || std::any_of(continentes_.begin(), continentes_.end(),
        [](const Continente* c) { return c == nullptr; }))
        throw std::invalid_argument("Objetivo de continentes invalido.");
}
bool ObjetivoConquistarContinente::foiConcluido(Jogador* jogador) const {
    return jogador && std::all_of(continentes_.begin(), continentes_.end(),
        [jogador](const Continente* c) { return c->verificarControle(*jogador); });
}
std::string ObjetivoConquistarContinente::descricao() const {
    std::string texto = "Dominar: ";
    for (std::size_t i = 0; i < continentes_.size(); ++i)
        texto += (i ? " + " : "") + continentes_[i]->nome();
    return texto + ".";
}
ObjetivoEliminarJogador::ObjetivoEliminarJogador(Jogador& alvo, int objetivoAlternativo)
    : alvo_(&alvo), objetivoAlternativo_(objetivoAlternativo) {
    if (objetivoAlternativo <= 0) throw std::invalid_argument("Objetivo alternativo invalido.");
}
bool ObjetivoEliminarJogador::foiConcluido(Jogador* jogador) const {
    if (!jogador || jogador == alvo_) return false;
    if (!alvo_->ativo() && alvo_->eliminadoPor() == jogador) return true;
    // Se outro jogador eliminar o alvo, vale o objetivo alternativo.
    if (!alvo_->ativo() && alvo_->eliminadoPor() != jogador)
        return static_cast<int>(jogador->territorios().size()) >= objetivoAlternativo_;
    return false;
}
std::string ObjetivoEliminarJogador::descricao() const {
    return "Eliminar " + alvo_->nome() + " (se outro o eliminar, conquistar " +
        std::to_string(objetivoAlternativo_) + " territorios).";
}
