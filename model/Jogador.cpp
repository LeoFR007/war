#include "Jogador.h"
#include "Objetivo.h"
#include "Territorio.h"
#include "WarException.h"

Jogador::Jogador(const std::string& nome, const std::string& cor)
    : nome(nome), cor(cor), exercitos(0), eliminadoPor(nullptr) {}

Jogador::~Jogador() {}

std::string Jogador::getNome() const { return nome; }

std::string Jogador::getCor() const { return cor; }

int Jogador::getExercitos() const { return exercitos; }

int Jogador::getQuantidadeTerritorios() const { return static_cast<int>(territorios.size()); }

const std::vector<Territorio*>& Jogador::getTerritorios() const { return territorios; }

const std::vector<Carta>& Jogador::getCartas() const { return cartas; }

const Objetivo* Jogador::getObjetivo() const { return objetivo.get(); }

const Jogador* Jogador::getEliminadoPor() const { return eliminadoPor; }

bool Jogador::estaAtivo() const { return !territorios.empty(); }

bool Jogador::possui(const Territorio* territorio) const { return territorio->getDono() == this; }

bool Jogador::ehAutomatico() const { return false; }

int Jogador::getTotalTropas() const {
    int total = 0;
    for (Territorio* territorio : territorios) total += territorio->getTropas();
    return total;
}

// Mesmas condicoes que Jogo::atacar exige (a interface usa para destacar alvos).
bool Jogador::podeAtacar(const Territorio* origem, const Territorio* alvo) const {
    return possui(origem) && !possui(alvo) && origem->ehVizinho(alvo) && origem->getTropas() >= 2;
}

// Mesmas condicoes que moverTropas exige para mover pelo menos 1 tropa.
bool Jogador::podeMover(const Territorio* origem, const Territorio* destino) const {
    int livres = origem->getTropas() - 1 - origem->getTropasRecebidas();
    return possui(origem) && possui(destino) && origem->ehVizinho(destino) && livres > 0;
}

void Jogador::setObjetivo(Objetivo* novoObjetivo) { objetivo.reset(novoObjetivo); }

void Jogador::setEliminadoPor(const Jogador* jogador) { eliminadoPor = jogador; }

void Jogador::receberExercitos(int quantidade) { exercitos += quantidade; }

void Jogador::colocarExercitos(Territorio* territorio, int quantidade) {
    if (!possui(territorio)) throw WarException("Esse territorio nao e seu.");
    if (quantidade < 1 || quantidade > exercitos) throw WarException("Voce nao tem essa quantidade de exercitos.");
    territorio->adicionarTropas(quantidade);
    exercitos -= quantidade;
}

void Jogador::moverTropas(Territorio* origem, Territorio* destino, int quantidade) {
    if (!possui(origem) || !possui(destino)) throw WarException("So e possivel mover entre territorios seus.");
    if (!origem->ehVizinho(destino)) throw WarException("Os territorios nao sao vizinhos.");
    // exemplo: tem 6 tropas e 3 chegaram aqui neste turno -> só 6 - 1 - 3 = 2 podem sair
    // Regra do WAR: cada exercito so se desloca uma vez por turno e 1 tropa fica na origem.
    int livres = origem->getTropas() - 1 - origem->getTropasRecebidas();
    if (quantidade < 1 || quantidade > livres)
        throw WarException("So " + std::to_string(livres > 0 ? livres : 0) +
                           " tropa(s) podem sair daqui (1 fica e as que ja se moveram neste turno nao movem de novo).");
    origem->removerTropas(quantidade);
    destino->adicionarTropas(quantidade);
    destino->registrarTropasRecebidas(quantidade);
}

void Jogador::conquistar(Territorio* territorio) {
    Jogador* antigoDono = territorio->getDono();
    // cuidado: o território fica guardado em dois lugares (no dono dele e na lista do jogador).
    // se não tirar da lista do dono antigo, os dois jogadores iam achar que são donos
    if (antigoDono != nullptr) {
        // Remove o territorio da lista do antigo dono.
        std::vector<Territorio*>& lista = antigoDono->territorios;
        for (size_t i = 0; i < lista.size(); i++) {
            if (lista[i] == territorio) {
                lista.erase(lista.begin() + i);
                break;
            }
        }
    }
    territorio->setDono(this);
    territorios.push_back(territorio);
}

void Jogador::receberCarta(const Carta& carta) { cartas.push_back(carta); }

// Troca valida: 3 cartas do mesmo tipo ou 1 de cada tipo.
bool Jogador::podeTrocarCartas() const {
    int infantaria = 0, cavalaria = 0, artilharia = 0;
    for (const Carta& carta : cartas) {
        if (carta.getTipo() == TipoCarta::Infantaria) infantaria++;
        else if (carta.getTipo() == TipoCarta::Cavalaria) cavalaria++;
        else artilharia++;
    }
    bool tresIguais = infantaria >= 3 || cavalaria >= 3 || artilharia >= 3;
    bool umDeCada = infantaria >= 1 && cavalaria >= 1 && artilharia >= 1;
    return tresIguais || umDeCada;
}

Carta Jogador::removerCartaDoTipo(TipoCarta tipo) {
    for (size_t i = 0; i < cartas.size(); i++) {
        if (cartas[i].getTipo() == tipo) {
            Carta carta = cartas[i];
            cartas.erase(cartas.begin() + i);
            return carta;
        }
    }
    throw WarException("Carta nao encontrada.");
}

std::vector<Carta> Jogador::trocarCartas() {
    if (!podeTrocarCartas()) throw WarException("Voce nao tem tres cartas validas para trocar.");
    std::vector<TipoCarta> tipos = {TipoCarta::Infantaria, TipoCarta::Cavalaria, TipoCarta::Artilharia};
    std::vector<Carta> entregues;

    // podeTrocarCartas() já garantiu que existe uma troca válida, então se não
    // achar 3 iguais aqui com certeza tem uma de cada tipo lá embaixo
    // Primeiro tenta tres cartas iguais.
    for (TipoCarta tipo : tipos) {
        int quantidade = 0;
        for (const Carta& carta : cartas) {
            if (carta.getTipo() == tipo) quantidade++;
        }
        if (quantidade >= 3) {
            for (int i = 0; i < 3; i++) entregues.push_back(removerCartaDoTipo(tipo));
            return entregues;
        }
    }
    // Senao, uma carta de cada tipo.
    for (TipoCarta tipo : tipos) entregues.push_back(removerCartaDoTipo(tipo));
    return entregues;
}

void Jogador::entregarCartasPara(Jogador* outro) {
    for (const Carta& carta : cartas) outro->receberCarta(carta);
    cartas.clear();
}
