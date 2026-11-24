#pragma once
#include <string>

enum class TipoCarta { Infantaria, Cavalaria, Artilharia };

// Carta de territorio, recebida ao conquistar pelo menos um territorio no turno.
class Carta {
private:
    std::string territorio;
    TipoCarta tipo;

public:
    Carta(const std::string& territorio, TipoCarta tipo);

    std::string getTerritorio() const;
    TipoCarta getTipo() const;
    std::string descricao() const;
};
