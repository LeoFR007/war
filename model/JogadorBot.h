#pragma once
#include "Jogador.h"

class Territorio;

// Jogador automatico com uma estrategia simples (usado no modo --demo).
class JogadorBot : public Jogador {
private:
    bool ehFronteira(const Territorio* territorio) const;
    void reforcar();
    void atacar(Jogo& jogo);
    void mover();

public:
    JogadorBot(const std::string& nome, const std::string& cor);
    void jogarTurno(Jogo& jogo) override;
    bool ehAutomatico() const override;
};
