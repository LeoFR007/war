#pragma once
#include "Continente.h"
#include "Territorio.h"
#include <memory>
#include <string>
#include <vector>

class Mapa {
    std::vector<std::unique_ptr<Continente>> continentes_;
    std::vector<std::unique_ptr<Territorio>> territorios_;
public:
    Mapa() = default;
    Mapa(const Mapa&) = delete;
    Mapa& operator=(const Mapa&) = delete;
    Mapa(Mapa&&) noexcept = default;
    Mapa& operator=(Mapa&&) noexcept = default;
    Continente& adicionarContinente(const std::string& nome, int bonus);
    Territorio& adicionarTerritorio(const std::string& nome, Continente& continente);
    Territorio& buscarTerritorio(const std::string& nome) const;
    bool saoAdjacentes(const Territorio& primeiro, const Territorio& segundo) const noexcept;
    void conectar(const std::string& primeiro, const std::string& segundo);
    std::vector<Territorio*> todosTerritorios() const;
    std::vector<Continente*> todosContinentes() const;
    static Mapa criarClassico();
};
