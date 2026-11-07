#pragma once
#include <string>
#include <vector>

class Jogador;
class Continente;

class Objetivo {
public:
    virtual ~Objetivo() = default;
    virtual bool foiConcluido(Jogador* jogador) const = 0;
    virtual std::string descricao() const = 0;
};
class ObjetivoConquistaTerritorios final : public Objetivo {
    int quantidade_;
public:
    explicit ObjetivoConquistaTerritorios(int quantidade);
    bool foiConcluido(Jogador* jogador) const override;
    std::string descricao() const override;
};
class ObjetivoConquistarContinente final : public Objetivo {
    std::vector<const Continente*> continentes_;
public:
    explicit ObjetivoConquistarContinente(std::vector<const Continente*> continentes);
    bool foiConcluido(Jogador* jogador) const override;
    std::string descricao() const override;
};
class ObjetivoEliminarJogador final : public Objetivo {
    Jogador* alvo_;          // Referencia nao proprietaria
    int objetivoAlternativo_;
public:
    ObjetivoEliminarJogador(Jogador& alvo, int objetivoAlternativo);
    bool foiConcluido(Jogador* jogador) const override;
    std::string descricao() const override;
};
