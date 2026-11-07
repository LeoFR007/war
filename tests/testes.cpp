#include "Carta.h"
#include "Continente.h"
#include "Exceptions.h"
#include "Jogo.h"
#include "Jogador.h"
#include "Mapa.h"
#include "Objetivo.h"
#include "Territorio.h"
#include <algorithm>
#include <cstdlib>

// Verificacoes ativas mesmo quando CMake usa Release (-DNDEBUG).
#define REQUIRE(cond) do { if (!(cond)) { \
    std::cerr << "Falha no teste: " << #cond << " (linha " << __LINE__ << ")\n"; \
    std::exit(EXIT_FAILURE); \
} } while (false)
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>

int main() {
    auto mapa = Mapa::criarClassico();
    auto territorios = mapa.todosTerritorios();
    auto continentes = mapa.todosContinentes();
    REQUIRE(territorios.size() == 42);
    REQUIRE(continentes.size() == 6);
    for (auto* t : territorios) {
        REQUIRE(!t->vizinhos().empty());
        for (auto* v : t->vizinhos()) REQUIRE(v->ehVizinho(*t));
    }
    REQUIRE(mapa.saoAdjacentes(mapa.buscarTerritorio("Alasca"), mapa.buscarTerritorio("Kamtchatka")));
    REQUIRE(!mapa.saoAdjacentes(mapa.buscarTerritorio("Alasca"), mapa.buscarTerritorio("Brasil")));

    Jogador a("Ana", CorJogador::Azul), b("Beto", CorJogador::Vermelho);
    Territorio p("P"), q("Q"), r("R");
    p.adicionarVizinho(q); q.adicionarVizinho(p);
    a.conquistarTerritorio(p); b.conquistarTerritorio(q); a.conquistarTerritorio(r);
    p.adicionarTropas(20); q.adicionarTropas(1); r.adicionarTropas(2);
    REQUIRE(a.territorios().size() == 2);
    try { a.moverTropas(p, q, 1); REQUIRE(false); }
    catch (const TerritorioNaoPertenceAoJogadorException&) {}
    try { a.moverTropas(p, r, 1); REQUIRE(false); }
    catch (const TerritorioNaoAdjacenteException&) {}
    std::mt19937 rngInvalido(1);
    try { b.atacar(p, q, 1, rngInvalido); REQUIRE(false); }
    catch (const TerritorioNaoPertenceAoJogadorException&) {}
    a.receberExercitos(3);
    a.distribuirExercitos(p, 2);
    REQUIRE(a.exercitosDisponiveis() == 1);

    std::mt19937 rng(1234);
    bool conquistado = false;
    for (int n = 0; n < 15 && !conquistado; ++n) {
        auto resultado = a.atacar(p, q, std::min(3, p.tropas() - 1), rng);
        REQUIRE(resultado.dadosAtacante.size() >= 1 && resultado.dadosAtacante.size() <= 3);
        REQUIRE(std::is_sorted(resultado.dadosAtacante.begin(), resultado.dadosAtacante.end(), std::greater<int>()));
        conquistado = resultado.conquistado;
    }
    REQUIRE(conquistado);
    REQUIRE(a.possui(q) && b.territorios().empty() && b.eliminadoPor() == &a);
    REQUIRE(p.tropas() >= 1 && q.tropas() >= 1);
    ObjetivoEliminarJogador objetivoEliminar(b, 10);
    REQUIRE(objetivoEliminar.foiConcluido(&a));
    ObjetivoConquistaTerritorios objetivoTerritorios(3);
    REQUIRE(objetivoTerritorios.foiConcluido(&a));
    a.moverTropas(p, q, 1);

    a.receberCarta(std::make_unique<CartaInfantaria>("I"));
    a.receberCarta(std::make_unique<CartaCavalaria>("C"));
    a.receberCarta(std::make_unique<CartaArtilharia>("A"));
    REQUIRE(a.podeTrocarCartas());
    const auto trocadas = a.trocarCartas();
    REQUIRE(trocadas.size() == 3 && a.totalCartas() == 0);
    a.receberCarta(std::make_unique<CartaInfantaria>("1"));
    a.receberCarta(std::make_unique<CartaInfantaria>("2"));
    a.receberCarta(std::make_unique<CartaInfantaria>("3"));
    REQUIRE(a.podeTrocarCartas());
    REQUIRE(a.trocarCartas().size() == 3);

    // Objetivo de continentes: controla toda a America do Sul.
    Jogador c("Caio", CorJogador::Verde);
    for (auto* t : continentes[1]->territorios()) { c.conquistarTerritorio(*t); t->adicionarTropas(1); }
    ObjetivoConquistarContinente objetivoContinente({continentes[1]});
    REQUIRE(objetivoContinente.foiConcluido(&c));

    // Valida sistematicamente o comparador, as baixas e a regra de desempate.
    Territorio dadosA("DadosA"), dadosB("DadosB");
    dadosA.adicionarVizinho(dadosB); dadosB.adicionarVizinho(dadosA);
    Jogador lancador("Lancador", CorJogador::Roxo);
    Jogador resistente("Resistente", CorJogador::Preto);
    lancador.conquistarTerritorio(dadosA);
    resistente.conquistarTerritorio(dadosB);
    dadosA.adicionarTropas(500); dadosB.adicionarTropas(500);
    std::mt19937 diceRng(2026);
    int empates = 0;
    for (int n = 0; n < 50; ++n) {
        const auto resultado = dadosA.atacar(dadosB, 3, diceRng);
        REQUIRE(resultado.dadosAtacante.size() == 3);
        REQUIRE(resultado.dadosDefensor.size() == 3);
        int baixasA = 0, baixasB = 0;
        for (std::size_t z = 0; z < 3; ++z) {
            if (resultado.dadosAtacante[z] <= resultado.dadosDefensor[z]) {
                ++baixasA;
                if (resultado.dadosAtacante[z] == resultado.dadosDefensor[z]) ++empates;
            } else ++baixasB;
        }
        REQUIRE(baixasA == resultado.baixasAtacante);
        REQUIRE(baixasB == resultado.baixasDefensor);
        REQUIRE(baixasA + baixasB == 3);
    }
    REQUIRE(empates > 0);

    // Empate nos dados favorece a defesa: cada par causa exatamente uma baixa.
    Territorio x("X"), y("Y");
    x.adicionarVizinho(y); y.adicionarVizinho(x);
    Jogador d("Dani", CorJogador::Amarelo);
    d.conquistarTerritorio(x); c.conquistarTerritorio(y);
    x.adicionarTropas(2); y.adicionarTropas(2);
    try { d.atacar(x, y, 3, rng); REQUIRE(false); }
    catch (const AtaqueInvalidoException&) {}
    try { y.conquistar(d, 1); REQUIRE(false); }
    catch (const AtaqueInvalidoException&) {}
    // Ao eliminar um adversario, as cartas na mao dele passam ao vencedor.
    Jogador vencedorCartas("Vencedor", CorJogador::Vermelho);
    Jogador eliminadoCartas("Eliminado", CorJogador::Azul);
    Territorio atacante("AtaqueCartas"), defensor("DefesaCartas");
    atacante.adicionarVizinho(defensor);
    defensor.adicionarVizinho(atacante);
    vencedorCartas.conquistarTerritorio(atacante);
    eliminadoCartas.conquistarTerritorio(defensor);
    atacante.adicionarTropas(50);
    defensor.adicionarTropas(1);
    eliminadoCartas.receberCarta(std::make_unique<CartaInfantaria>("Teste1"));
    eliminadoCartas.receberCarta(std::make_unique<CartaCavalaria>("Teste2"));
    std::mt19937 sorteioCartas(42);
    bool ganhou = false;
    for (int tentativa = 0; tentativa < 15 && !ganhou; ++tentativa) {
        auto resultado = vencedorCartas.atacar(atacante, defensor, 3, sorteioCartas);
        if (resultado.conquistado) {
            ganhou = true;
            REQUIRE(resultado.cartasCapturadas == 2);
        }
    }
    REQUIRE(ganhou && vencedorCartas.totalCartas() == 2);
    REQUIRE(eliminadoCartas.totalCartas() == 0 && !eliminadoCartas.ativo());

    // O motor oferece operacoes seguras por fase, sem depender do terminal.
    Jogo partida({"TimeA", "TimeB"}, 2026);
    REQUIRE(!partida.turnoAtivo());
    try { partida.atacarTerritorio(1, 2, 3); REQUIRE(false); }
    catch (const AtaqueInvalidoException&) {}
    const int reforcos = partida.iniciarTurnoAtual();
    REQUIRE(reforcos >= 3 && partida.turnoAtivo());
    REQUIRE(partida.fase() == FaseTurno::Reforco);
    try { partida.iniciarTurnoAtual(); REQUIRE(false); }
    catch (const MovimentoInvalidoException&) {}
    try { partida.avancarFase(); REQUIRE(false); }
    catch (const MovimentoInvalidoException&) {}
    auto mapaPartida = partida.mapa().todosTerritorios();
    auto* proprio = partida.jogadorAtual().territorios().front();
    const auto posicao = std::find(mapaPartida.begin(), mapaPartida.end(), proprio);
    REQUIRE(posicao != mapaPartida.end());
    const int id = static_cast<int>(posicao - mapaPartida.begin()) + 1;
    try { partida.atacarTerritorio(id, id, 1); REQUIRE(false); }
    catch (const AtaqueInvalidoException&) {}
    partida.reforcar(id, reforcos);
    REQUIRE(partida.jogadorAtual().exercitosDisponiveis() == 0);
    partida.avancarFase();
    REQUIRE(partida.fase() == FaseTurno::Ataque);
    try { partida.reforcar(id, 1); REQUIRE(false); }
    catch (const MovimentoInvalidoException&) {}
    try { partida.moverTropas(id, id, 1); REQUIRE(false); }
    catch (const MovimentoInvalidoException&) {}
    partida.avancarFase();
    REQUIRE(partida.fase() == FaseTurno::Movimentacao);
    partida.avancarFase();
    REQUIRE(partida.fase() == FaseTurno::Fim);
    partida.avancarFase();
    REQUIRE(!partida.turnoAtivo());
    REQUIRE(partida.jogadorAtual().nome() == "TimeB");

    // Mao de 5+ cartas exige troca antes de avancar ao ataque.
    Jogo trocaObrigatoria({"Um", "Dois", "Tres"}, 111);
    auto& um = trocaObrigatoria.jogadorAtual();
    um.receberCarta(std::make_unique<CartaInfantaria>("1"));
    um.receberCarta(std::make_unique<CartaInfantaria>("2"));
    um.receberCarta(std::make_unique<CartaCavalaria>("3"));
    um.receberCarta(std::make_unique<CartaCavalaria>("4"));
    um.receberCarta(std::make_unique<CartaArtilharia>("5"));
    const int bonusInicial = trocaObrigatoria.iniciarTurnoAtual();
    const auto listaTroca = trocaObrigatoria.mapa().todosTerritorios();
    const auto posTroca = std::find(listaTroca.begin(), listaTroca.end(), um.territorios().front());
    REQUIRE(posTroca != listaTroca.end());
    const int idTroca = static_cast<int>(posTroca - listaTroca.begin()) + 1;
    trocaObrigatoria.reforcar(idTroca, bonusInicial);
    try { trocaObrigatoria.avancarFase(); REQUIRE(false); }
    catch (const MovimentoInvalidoException&) {}
    const int bonusTroca = trocaObrigatoria.trocarCartas();
    REQUIRE(bonusTroca == 4);
    REQUIRE(um.totalCartas() == 2);
    trocaObrigatoria.reforcar(idTroca, bonusTroca);
    trocaObrigatoria.avancarFase();
    REQUIRE(trocaObrigatoria.fase() == FaseTurno::Ataque);

    // Partidas entre 2 e 6 jogadores mantem 42 territorios de posse unica.
    for (int qtd = 2; qtd <= 6; ++qtd) {
        std::vector<std::string> nomes;
        for (int k = 0; k < qtd; ++k) nomes.push_back("J" + std::to_string(k));
        Jogo jogoDeTeste(nomes, static_cast<std::uint32_t>(qtd));
        int somatorio = 0;
        for (const auto& jogador : jogoDeTeste.jogadores()) {
            REQUIRE(jogador->ativo());
            REQUIRE(jogador->objetivo() != nullptr);
            somatorio += static_cast<int>(jogador->territorios().size());
        }
        REQUIRE(somatorio == 42);
        for (const auto* territorio : jogoDeTeste.mapa().todosTerritorios()) {
            REQUIRE(territorio->dono() != nullptr);
            REQUIRE(territorio->tropas() >= 1);
        }
    }

    std::cout << "TESTES APROVADOS: mapa, combate, cartas, fases, posse, excecoes, objetivos e turnos.\n";
    return 0;
}
