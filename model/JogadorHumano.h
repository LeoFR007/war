#pragma once
#include "Jogador.h"
#include <sstream>
#include <string>

// Jogador controlado por uma pessoa, digitando comandos no terminal.
class JogadorHumano : public Jogador {
private:
    std::string lerLinha(const std::string& prompt) const;
    bool comandoGeral(Jogo& jogo, const std::string& comando, std::istringstream& entrada) const;
    void mostrarStatus() const;
    void faseReforco(Jogo& jogo);
    void faseAtaque(Jogo& jogo);
    void faseMovimento(Jogo& jogo);

public:
    JogadorHumano(const std::string& nome, const std::string& cor);
    void jogarTurno(Jogo& jogo) override;
};
