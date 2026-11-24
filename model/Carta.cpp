#include "Carta.h"

Carta::Carta(const std::string& territorio, TipoCarta tipo) : territorio(territorio), tipo(tipo) {}

std::string Carta::getTerritorio() const { return territorio; }

TipoCarta Carta::getTipo() const { return tipo; }

std::string Carta::descricao() const { 
    if (tipo == TipoCarta::Infantaria) return "Infantaria - " + territorio;
    if (tipo == TipoCarta::Cavalaria) return "Cavalaria - " + territorio;
    return "Artilharia - " + territorio;
}
