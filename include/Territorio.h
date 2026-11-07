#pragma once
#include "Enums.h"
#include <random>
#include <string>
#include <vector>

class Jogador;

struct RelatorioCombate {
    std::vector<int> dadosAtacante;
    std::vector<int> dadosDefensor;
    int baixasAtacante = 0;
    int baixasDefensor = 0;
    ResultadoCombate resultado = ResultadoCombate::ResistenciaDefensor;
    bool conquistado = false;
    int tropasTransferidas = 0;
    int cartasCapturadas = 0;
};

class Territorio {
    std::string nome_;
    int tropas_ = 0;
    Jogador* dono_ = nullptr;             // Referencia nao proprietaria
    std::vector<Territorio*> vizinhos_;   // Referencias nao proprietarias
    friend class Jogador;
public:
    explicit Territorio(std::string nome);
    const std::string& nome() const noexcept { return nome_; }
    int tropas() const noexcept { return tropas_; }
    Jogador* dono() const noexcept { return dono_; }
    const std::vector<Territorio*>& vizinhos() const noexcept { return vizinhos_; }
    void adicionarVizinho(Territorio& outro);
    bool ehVizinho(const Territorio& outro) const noexcept;
    void adicionarTropas(int quantidade);
    void removerTropas(int quantidade);
    RelatorioCombate atacar(Territorio& alvo, int quantidadeDados, std::mt19937& gerador);
    void conquistar(Jogador& novoDono, int quantidadeTropas);
};
