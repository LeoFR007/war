#include "Continente.h"
#include "Jogador.h"
#include "Territorio.h"
#include <algorithm>
#include <stdexcept>
#include <utility>

Continente::Continente(std::string nome, int bonus)
    : nome_(std::move(nome)), bonusExercitos_(bonus) {
    if (nome_.empty() || bonus <= 0) throw std::invalid_argument("Continente invalido.");
}
void Continente::adicionarTerritorio(Territorio& territorio) {
    if (std::find(territorios_.begin(), territorios_.end(), &territorio) == territorios_.end())
        territorios_.push_back(&territorio);
}
bool Continente::verificarControle(const Jogador& jogador) const {
    return !territorios_.empty() && std::all_of(territorios_.begin(), territorios_.end(),
        [&jogador](const Territorio* t) { return t->dono() == &jogador; });
}
int Continente::calcularBonus(const Jogador& jogador) const {
    return verificarControle(jogador) ? bonusExercitos_ : 0;
}
