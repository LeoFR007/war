#include "Mapa.h"
#include <algorithm>
#include <stdexcept>
#include <utility>

Continente& Mapa::adicionarContinente(const std::string& nome, int bonus) {
    if (std::any_of(continentes_.begin(), continentes_.end(), [&nome](const auto& c) { return c->nome() == nome; }))
        throw std::invalid_argument("Continente duplicado: " + nome);
    continentes_.push_back(std::make_unique<Continente>(nome, bonus));
    return *continentes_.back();
}
Territorio& Mapa::adicionarTerritorio(const std::string& nome, Continente& continente) {
    if (std::any_of(territorios_.begin(), territorios_.end(), [&nome](const auto& t) { return t->nome() == nome; }))
        throw std::invalid_argument("Territorio duplicado: " + nome);
    territorios_.push_back(std::make_unique<Territorio>(nome));
    continente.adicionarTerritorio(*territorios_.back());
    return *territorios_.back();
}
Territorio& Mapa::buscarTerritorio(const std::string& nome) const {
    const auto iter = std::find_if(territorios_.begin(), territorios_.end(),
        [&nome](const auto& t) { return t->nome() == nome; });
    if (iter == territorios_.end()) throw std::out_of_range("Territorio nao encontrado: " + nome);
    return **iter;
}
bool Mapa::saoAdjacentes(const Territorio& primeiro, const Territorio& segundo) const noexcept {
    return primeiro.ehVizinho(segundo);
}
void Mapa::conectar(const std::string& primeiro, const std::string& segundo) {
    auto& a = buscarTerritorio(primeiro);
    auto& b = buscarTerritorio(segundo);
    a.adicionarVizinho(b);
    b.adicionarVizinho(a);
}
std::vector<Territorio*> Mapa::todosTerritorios() const {
    std::vector<Territorio*> saida;
    for (const auto& t : territorios_) saida.push_back(t.get());
    return saida;
}
std::vector<Continente*> Mapa::todosContinentes() const {
    std::vector<Continente*> saida;
    for (const auto& c : continentes_) saida.push_back(c.get());
    return saida;
}

Mapa Mapa::criarClassico() {
    Mapa mapa;
    struct Definicao { const char* nome; int bonus; std::vector<const char*> regioes; };
    const std::vector<Definicao> continentes = {
        {"America do Norte", 5, {"Alasca", "Noroeste", "Groenlandia", "Alberta", "Ontario", "Quebec", "Oeste dos EUA", "Leste dos EUA", "America Central"}},
        {"America do Sul", 2, {"Venezuela", "Peru", "Brasil", "Argentina"}},
        {"Europa", 5, {"Islandia", "Escandinavia", "Ucrania", "Gra-Bretanha", "Norte da Europa", "Oeste da Europa", "Sul da Europa"}},
        {"Africa", 3, {"Norte da Africa", "Egito", "Africa Oriental", "Congo", "Africa do Sul", "Madagascar"}},
        {"Asia", 7, {"Ural", "Siberia", "Iacutia", "Kamtchatka", "Irkutsk", "Mongolia", "Japao", "Afeganistao", "Oriente Medio", "India", "China", "Siao"}},
        {"Oceania", 2, {"Indonesia", "Nova Guine", "Australia Ocidental", "Australia Oriental"}}
    };
    for (const auto& definicao : continentes) {
        auto& continente = mapa.adicionarContinente(definicao.nome, definicao.bonus);
        for (const auto* nome : definicao.regioes) mapa.adicionarTerritorio(nome, continente);
    }
    // Arestas nao direcionadas do mapa classico, inclusive conexoes intercontinentais.
    const std::vector<std::pair<const char*, const char*>> adjacencias = {
        {"Alasca", "Noroeste"}, {"Alasca", "Alberta"}, {"Alasca", "Kamtchatka"},
        {"Noroeste", "Alberta"}, {"Noroeste", "Ontario"}, {"Noroeste", "Groenlandia"},
        {"Groenlandia", "Ontario"}, {"Groenlandia", "Quebec"}, {"Groenlandia", "Islandia"},
        {"Alberta", "Ontario"}, {"Alberta", "Oeste dos EUA"},
        {"Ontario", "Quebec"}, {"Ontario", "Oeste dos EUA"}, {"Ontario", "Leste dos EUA"},
        {"Quebec", "Leste dos EUA"}, {"Oeste dos EUA", "Leste dos EUA"},
        {"Oeste dos EUA", "America Central"}, {"Leste dos EUA", "America Central"},
        {"America Central", "Venezuela"},
        {"Venezuela", "Peru"}, {"Venezuela", "Brasil"},
        {"Peru", "Brasil"}, {"Peru", "Argentina"}, {"Brasil", "Argentina"},
        {"Brasil", "Norte da Africa"},
        {"Islandia", "Escandinavia"}, {"Islandia", "Gra-Bretanha"},
        {"Escandinavia", "Gra-Bretanha"}, {"Escandinavia", "Norte da Europa"},
        {"Escandinavia", "Ucrania"}, {"Gra-Bretanha", "Norte da Europa"},
        {"Gra-Bretanha", "Oeste da Europa"}, {"Norte da Europa", "Oeste da Europa"},
        {"Norte da Europa", "Sul da Europa"}, {"Norte da Europa", "Ucrania"},
        {"Oeste da Europa", "Sul da Europa"}, {"Oeste da Europa", "Norte da Africa"},
        {"Sul da Europa", "Ucrania"}, {"Sul da Europa", "Norte da Africa"},
        {"Sul da Europa", "Egito"}, {"Sul da Europa", "Oriente Medio"},
        {"Ucrania", "Ural"}, {"Ucrania", "Afeganistao"}, {"Ucrania", "Oriente Medio"},
        {"Norte da Africa", "Egito"}, {"Norte da Africa", "Africa Oriental"},
        {"Norte da Africa", "Congo"}, {"Egito", "Africa Oriental"},
        {"Egito", "Oriente Medio"}, {"Africa Oriental", "Congo"},
        {"Africa Oriental", "Africa do Sul"}, {"Africa Oriental", "Madagascar"},
        {"Africa Oriental", "Oriente Medio"}, {"Congo", "Africa do Sul"},
        {"Africa do Sul", "Madagascar"},
        {"Ural", "Siberia"}, {"Ural", "China"}, {"Ural", "Afeganistao"},
        {"Siberia", "Iacutia"}, {"Siberia", "Irkutsk"}, {"Siberia", "Mongolia"},
        {"Siberia", "China"}, {"Iacutia", "Irkutsk"}, {"Iacutia", "Kamtchatka"},
        {"Kamtchatka", "Irkutsk"}, {"Kamtchatka", "Mongolia"},
        {"Kamtchatka", "Japao"}, {"Irkutsk", "Mongolia"},
        {"Mongolia", "Japao"}, {"Mongolia", "China"},
        {"Afeganistao", "China"}, {"Afeganistao", "India"},
        {"Afeganistao", "Oriente Medio"}, {"Oriente Medio", "India"},
        {"India", "China"}, {"India", "Siao"}, {"China", "Siao"},
        {"Siao", "Indonesia"}, {"Indonesia", "Nova Guine"},
        {"Indonesia", "Australia Ocidental"}, {"Nova Guine", "Australia Ocidental"},
        {"Nova Guine", "Australia Oriental"}, {"Australia Ocidental", "Australia Oriental"}
    };
    for (const auto& a : adjacencias) mapa.conectar(a.first, a.second);
    return mapa;
}
