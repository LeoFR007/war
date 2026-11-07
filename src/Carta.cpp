#include "Carta.h"
#include <stdexcept>
#include <utility>

Carta::Carta(std::string territorio) : territorio_(std::move(territorio)) {
    if (territorio_.empty()) throw std::invalid_argument("Carta sem territorio.");
}
std::string CartaInfantaria::descricao() const { return "Infantaria - " + territorio(); }
std::string CartaCavalaria::descricao() const { return "Cavalaria - " + territorio(); }
std::string CartaArtilharia::descricao() const { return "Artilharia - " + territorio(); }
