#include "Objetivo.h"
#include "Continente.h"
#include "Jogador.h"

// ----- ObjetivoTerritorios -----
ObjetivoTerritorios::ObjetivoTerritorios(int quantidade) : quantidade(quantidade) {}

bool ObjetivoTerritorios::foiCumprido(const Jogador* jogador) const {
    return jogador->getQuantidadeTerritorios() >= quantidade;
}

std::string ObjetivoTerritorios::descricao() const {
    return "Conquistar pelo menos " + std::to_string(quantidade) + " territorios.";
}

// ----- ObjetivoContinentes -----
ObjetivoContinentes::ObjetivoContinentes(const Continente* primeiro, const Continente* segundo)
    : primeiro(primeiro), segundo(segundo) {}

bool ObjetivoContinentes::foiCumprido(const Jogador* jogador) const {
    return primeiro->controladoPor(jogador) && segundo->controladoPor(jogador);
}

std::string ObjetivoContinentes::descricao() const {
    return "Dominar " + primeiro->getNome() + " e " + segundo->getNome() + ".";
}

// ----- ObjetivoEliminar -----
ObjetivoEliminar::ObjetivoEliminar(const Jogador* alvo, int territoriosAlternativo)
    : alvo(alvo), territoriosAlternativo(territoriosAlternativo) {}

bool ObjetivoEliminar::foiCumprido(const Jogador* jogador) const {
    // enquanto o alvo estiver vivo não tem como cumprir.
    // se fui eu que eliminei, ganhei. se foi outro jogador, aí vale o objetivo
    // alternativo de território (é a regra do WAR original)
    if (alvo->estaAtivo()) return false;
    if (alvo->getEliminadoPor() == jogador) return true;
    return jogador->getQuantidadeTerritorios() >= territoriosAlternativo;
}

std::string ObjetivoEliminar::descricao() const {
    return "Eliminar " + alvo->getNome() + " (se outro jogador o eliminar, conquistar " +
           std::to_string(territoriosAlternativo) + " territorios).";
}
