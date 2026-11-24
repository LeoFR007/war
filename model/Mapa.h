#pragma once
#include "Continente.h"
#include "Territorio.h"
#include <memory>
#include <string>
#include <vector>

// O mapa classico do WAR: 6 continentes e 42 territorios.
// Composicao: o Mapa cria e e dono dos territorios e continentes
// (std::unique_ptr libera a memoria automaticamente).
class Mapa {
private:
    std::vector<std::unique_ptr<Territorio>> territorios;
    std::vector<std::unique_ptr<Continente>> continentes;

    void criarContinente(const std::string& nome, int bonus, const std::vector<std::string>& nomesTerritorios);
    void conectar(const std::string& primeiro, const std::string& segundo);
    Territorio* buscarPorNome(const std::string& nome) const;

public:
    Mapa();

    int getQuantidadeTerritorios() const;
    Territorio* getTerritorio(int id) const;
    int getQuantidadeContinentes() const;
    Continente* getContinente(int indice) const;

    void mostrar() const;
};
