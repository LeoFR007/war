#include "Mapa.h"
#include "Jogador.h"
#include "WarException.h"
#include <iomanip>
#include <iostream>

Mapa::Mapa() {
    // A ordem de criacao define os ids: Alasca = 1 ... Australia Oriental = 42.
    criarContinente("America do Norte", 5, {"Alasca", "Noroeste", "Groenlandia", "Alberta", "Ontario",
                                            "Quebec", "Oeste dos EUA", "Leste dos EUA", "America Central"});
    criarContinente("America do Sul", 2, {"Venezuela", "Peru", "Brasil", "Argentina"});
    criarContinente("Europa", 5, {"Islandia", "Escandinavia", "Ucrania", "Gra-Bretanha",
                                  "Norte da Europa", "Oeste da Europa", "Sul da Europa"});
    criarContinente("Africa", 3, {"Norte da Africa", "Egito", "Africa Oriental", "Congo",
                                  "Africa do Sul", "Madagascar"});
    criarContinente("Asia", 7, {"Ural", "Siberia", "Iacutia", "Kamtchatka", "Irkutsk", "Mongolia",
                                "Japao", "Afeganistao", "Oriente Medio", "India", "China", "Siao"});
    criarContinente("Oceania", 2, {"Indonesia", "Nova Guine", "Australia Ocidental", "Australia Oriental"});

    // America do Norte
    conectar("Alasca", "Noroeste");             conectar("Alasca", "Alberta");
    conectar("Alasca", "Kamtchatka");           conectar("Noroeste", "Alberta");
    conectar("Noroeste", "Ontario");            conectar("Noroeste", "Groenlandia");
    conectar("Groenlandia", "Ontario");         conectar("Groenlandia", "Quebec");
    conectar("Groenlandia", "Islandia");        conectar("Alberta", "Ontario");
    conectar("Alberta", "Oeste dos EUA");       conectar("Ontario", "Quebec");
    conectar("Ontario", "Oeste dos EUA");       conectar("Ontario", "Leste dos EUA");
    conectar("Quebec", "Leste dos EUA");        conectar("Oeste dos EUA", "Leste dos EUA");
    conectar("Oeste dos EUA", "America Central"); conectar("Leste dos EUA", "America Central");
    conectar("America Central", "Venezuela");
    // America do Sul
    conectar("Venezuela", "Peru");              conectar("Venezuela", "Brasil");
    conectar("Peru", "Brasil");                 conectar("Peru", "Argentina");
    conectar("Brasil", "Argentina");            conectar("Brasil", "Norte da Africa");
    // Europa
    conectar("Islandia", "Escandinavia");       conectar("Islandia", "Gra-Bretanha");
    conectar("Escandinavia", "Gra-Bretanha");   conectar("Escandinavia", "Norte da Europa");
    conectar("Escandinavia", "Ucrania");        conectar("Gra-Bretanha", "Norte da Europa");
    conectar("Gra-Bretanha", "Oeste da Europa"); conectar("Norte da Europa", "Oeste da Europa");
    conectar("Norte da Europa", "Sul da Europa"); conectar("Norte da Europa", "Ucrania");
    conectar("Oeste da Europa", "Sul da Europa"); conectar("Oeste da Europa", "Norte da Africa");
    conectar("Sul da Europa", "Ucrania");       conectar("Sul da Europa", "Norte da Africa");
    conectar("Sul da Europa", "Egito");         conectar("Sul da Europa", "Oriente Medio");
    conectar("Ucrania", "Ural");                conectar("Ucrania", "Afeganistao");
    conectar("Ucrania", "Oriente Medio");
    // Africa
    conectar("Norte da Africa", "Egito");       conectar("Norte da Africa", "Africa Oriental");
    conectar("Norte da Africa", "Congo");       conectar("Egito", "Africa Oriental");
    conectar("Egito", "Oriente Medio");         conectar("Africa Oriental", "Congo");
    conectar("Africa Oriental", "Africa do Sul"); conectar("Africa Oriental", "Madagascar");
    conectar("Africa Oriental", "Oriente Medio"); conectar("Congo", "Africa do Sul");
    conectar("Africa do Sul", "Madagascar");
    // Asia
    conectar("Ural", "Siberia");                conectar("Ural", "China");
    conectar("Ural", "Afeganistao");            conectar("Siberia", "Iacutia");
    conectar("Siberia", "Irkutsk");             conectar("Siberia", "Mongolia");
    conectar("Siberia", "China");               conectar("Iacutia", "Irkutsk");
    conectar("Iacutia", "Kamtchatka");          conectar("Kamtchatka", "Irkutsk");
    conectar("Kamtchatka", "Mongolia");         conectar("Kamtchatka", "Japao");
    conectar("Irkutsk", "Mongolia");            conectar("Mongolia", "Japao");
    conectar("Mongolia", "China");              conectar("Afeganistao", "China");
    conectar("Afeganistao", "India");           conectar("Afeganistao", "Oriente Medio");
    conectar("Oriente Medio", "India");         conectar("India", "China");
    conectar("India", "Siao");                  conectar("China", "Siao");
    conectar("Siao", "Indonesia");
    // Oceania
    conectar("Indonesia", "Nova Guine");        conectar("Indonesia", "Australia Ocidental");
    conectar("Nova Guine", "Australia Ocidental"); conectar("Nova Guine", "Australia Oriental");
    conectar("Australia Ocidental", "Australia Oriental");
}

void Mapa::criarContinente(const std::string& nome, int bonus, const std::vector<std::string>& nomesTerritorios) {
    continentes.push_back(std::make_unique<Continente>(nome, bonus));
    for (const std::string& nomeTerritorio : nomesTerritorios) {
        int novoId = static_cast<int>(territorios.size()) + 1;
        territorios.push_back(std::make_unique<Territorio>(novoId, nomeTerritorio));
        continentes.back()->adicionarTerritorio(territorios.back().get());
    }
}

void Mapa::conectar(const std::string& primeiro, const std::string& segundo) {
    Territorio* a = buscarPorNome(primeiro);
    Territorio* b = buscarPorNome(segundo);
    a->adicionarVizinho(b);
    b->adicionarVizinho(a);
}

Territorio* Mapa::buscarPorNome(const std::string& nome) const {
    for (const auto& territorio : territorios) {
        if (territorio->getNome() == nome) return territorio.get();
    }
    throw WarException("Territorio nao encontrado: " + nome);
}

int Mapa::getQuantidadeTerritorios() const { return static_cast<int>(territorios.size()); }

Territorio* Mapa::getTerritorio(int id) const {
    if (id < 1 || id > getQuantidadeTerritorios())
        throw WarException("ID de territorio invalido: " + std::to_string(id) + ". Digite 'mapa' para ver os IDs.");
    return territorios[id - 1].get();
}

int Mapa::getQuantidadeContinentes() const { return static_cast<int>(continentes.size()); }

Continente* Mapa::getContinente(int indice) const { return continentes.at(indice).get(); }

void Mapa::mostrar() const {
    std::cout << "\n===== MAPA (ID, territorio, tropas, dono) =====\n";
    for (const auto& territorio : territorios) {
        std::string dono = territorio->getDono() ? territorio->getDono()->getNome() : "Sem dono";
        std::cout << std::setw(2) << territorio->getId() << ") " << std::left << std::setw(22)
                  << territorio->getNome() << std::right << std::setw(3) << territorio->getTropas()
                  << " tropas | " << dono << "\n";
    }
}
