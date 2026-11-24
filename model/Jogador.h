#pragma once
#include "Carta.h"
#include <memory>
#include <string>
#include <vector>

class Jogo;
class Objetivo;
class Territorio;

// Classe abstrata: o jogo chama jogarTurno() sem saber se o jogador
// e humano ou bot (polimorfismo). Ver JogadorHumano e JogadorBot.
class Jogador {
private:
    std::string nome;
    std::string cor;
    int exercitos;                          // reforcos ainda nao posicionados
    std::vector<Territorio*> territorios;
    std::vector<Carta> cartas;
    std::unique_ptr<Objetivo> objetivo;     // cada jogador tem o seu objetivo
    const Jogador* eliminadoPor;

    Carta removerCartaDoTipo(TipoCarta tipo);

public:
    Jogador(const std::string& nome, const std::string& cor);
    virtual ~Jogador();

    virtual void jogarTurno(Jogo& jogo) = 0;
    virtual bool ehAutomatico() const;  // false para humanos; JogadorBot sobrescreve

    std::string getNome() const;
    std::string getCor() const;
    int getExercitos() const;
    int getQuantidadeTerritorios() const;
    const std::vector<Territorio*>& getTerritorios() const;
    const std::vector<Carta>& getCartas() const;
    const Objetivo* getObjetivo() const;
    const Jogador* getEliminadoPor() const;
    bool estaAtivo() const;
    bool possui(const Territorio* territorio) const;
    int getTotalTropas() const;
    bool podeAtacar(const Territorio* origem, const Territorio* alvo) const;
    bool podeMover(const Territorio* origem, const Territorio* destino) const;

    void setObjetivo(Objetivo* novoObjetivo);
    void setEliminadoPor(const Jogador* jogador);

    void receberExercitos(int quantidade);
    void colocarExercitos(Territorio* territorio, int quantidade);
    void moverTropas(Territorio* origem, Territorio* destino, int quantidade);
    void conquistar(Territorio* territorio);

    void receberCarta(const Carta& carta);
    bool podeTrocarCartas() const;
    std::vector<Carta> trocarCartas();
    void entregarCartasPara(Jogador* outro);
};
