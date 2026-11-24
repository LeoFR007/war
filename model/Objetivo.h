#pragma once
#include <string>

class Continente;
class Jogador;

// Classe abstrata: cada tipo de objetivo verifica a vitoria do seu jeito.
class Objetivo {
public:
    virtual ~Objetivo() {}
    virtual bool foiCumprido(const Jogador* jogador) const = 0;
    virtual std::string descricao() const = 0;
};

// Conquistar uma quantidade minima de territorios.
class ObjetivoTerritorios : public Objetivo {
private:
    int quantidade;

public:
    explicit ObjetivoTerritorios(int quantidade);
    bool foiCumprido(const Jogador* jogador) const override;
    std::string descricao() const override;
};

// Dominar dois continentes inteiros.
class ObjetivoContinentes : public Objetivo {
private:
    const Continente* primeiro;
    const Continente* segundo;

public:
    ObjetivoContinentes(const Continente* primeiro, const Continente* segundo);
    bool foiCumprido(const Jogador* jogador) const override;
    std::string descricao() const override;
};

// Eliminar um adversario. Se outro jogador elimina-lo antes,
// vale conquistar uma quantidade de territorios.
class ObjetivoEliminar : public Objetivo {
private:
    const Jogador* alvo;
    int territoriosAlternativo;

public:
    ObjetivoEliminar(const Jogador* alvo, int territoriosAlternativo);
    bool foiCumprido(const Jogador* jogador) const override;
    std::string descricao() const override;
};
