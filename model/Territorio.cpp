#include "Territorio.h"
#include "WarException.h"

Territorio::Territorio(int id, const std::string& nome) : id(id), nome(nome), tropas(0), tropasRecebidas(0), dono(nullptr) {}

int Territorio::getId() const { return id; }

std::string Territorio::getNome() const { return nome; }

int Territorio::getTropas() const { return tropas; }

int Territorio::getTropasRecebidas() const { return tropasRecebidas; }

Jogador* Territorio::getDono() const { return dono; }

const std::vector<Territorio*>& Territorio::getVizinhos() const { return vizinhos; }

void Territorio::setDono(Jogador* novoDono) { dono = novoDono; }

void Territorio::adicionarTropas(int quantidade) {
    if (quantidade <= 0) throw WarException("A quantidade de tropas deve ser positiva.");
    tropas += quantidade;
}

void Territorio::removerTropas(int quantidade) {
    if (quantidade <= 0 || quantidade > tropas) throw WarException("Quantidade invalida de tropas.");
    tropas -= quantidade;
}

void Territorio::registrarTropasRecebidas(int quantidade) { tropasRecebidas += quantidade; }

void Territorio::zerarTropasRecebidas() { tropasRecebidas = 0; }

void Territorio::adicionarVizinho(Territorio* outro) {
    if (!ehVizinho(outro)) vizinhos.push_back(outro);
}

bool Territorio::ehVizinho(const Territorio* outro) const {
    for (Territorio* vizinho : vizinhos) {
        if (vizinho == outro) return true;
    }
    return false;
}
