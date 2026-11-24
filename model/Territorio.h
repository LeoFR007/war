#pragma once
#include <string>
#include <vector>

class Jogador;

// Um territorio do mapa. As tropas so mudam pelos metodos, que validam os valores.
class Territorio {
private:
    int id;                             // numero usado nos comandos (1 a 42)
    std::string nome;
    int tropas;
    int tropasRecebidas;                // chegaram por movimentacao neste turno (nao podem mover de novo)
    Jogador* dono;                      // jogador que controla o territorio
    std::vector<Territorio*> vizinhos;  // territorios ligados a este

public:
    Territorio(int id, const std::string& nome);

    int getId() const;
    std::string getNome() const;
    int getTropas() const;
    int getTropasRecebidas() const;
    Jogador* getDono() const;
    const std::vector<Territorio*>& getVizinhos() const;

    void setDono(Jogador* novoDono);
    void adicionarTropas(int quantidade);
    void removerTropas(int quantidade);
    void registrarTropasRecebidas(int quantidade);
    void zerarTropasRecebidas();
    void adicionarVizinho(Territorio* outro);
    bool ehVizinho(const Territorio* outro) const;
};
