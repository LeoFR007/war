#include "Jogo.h"
#include "Exceptions.h"
#include "Objetivo.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace {
std::vector<CorJogador> coresDisponiveis() {
    return {CorJogador::Vermelho, CorJogador::Azul, CorJogador::Verde,
            CorJogador::Amarelo, CorJogador::Roxo, CorJogador::Preto};
}
std::string formatarDados(const std::vector<int>& dados) {
    std::ostringstream texto;
    for (std::size_t i = 0; i < dados.size(); ++i) texto << (i ? "," : "") << dados[i];
    return texto.str();
}
std::size_t posicao(const std::vector<Territorio*>& todos, const Territorio& territorio) {
    auto it = std::find(todos.begin(), todos.end(), &territorio);
    if (it == todos.end()) throw std::logic_error("Territorio nao pertence ao mapa.");
    return static_cast<std::size_t>(std::distance(todos.begin(), it)) + 1;
}
Territorio& porIndice(const Mapa& mapa, int indice) {
    auto todos = mapa.todosTerritorios();
    if (indice < 1 || indice > static_cast<int>(todos.size()))
        throw std::out_of_range("ID invalido. Use 'mapa' para consultar os IDs.");
    return *todos[static_cast<std::size_t>(indice - 1)];
}
}

Jogo::Jogo(std::vector<std::string> nomes, std::uint32_t semente)
    : mapa_(Mapa::criarClassico()), rng_(semente) { configurar(std::move(nomes)); }

void Jogo::configurar(std::vector<std::string> nomes) {
    if (nomes.size() < 2 || nomes.size() > 6)
        throw std::invalid_argument("O WAR exige de 2 a 6 jogadores.");
    auto cores = coresDisponiveis();
    for (std::size_t i = 0; i < nomes.size(); ++i) {
        if (nomes[i].empty()) throw std::invalid_argument("Nome de jogador vazio.");
        if (std::find(nomes.begin(), nomes.begin() + static_cast<std::ptrdiff_t>(i), nomes[i]) != nomes.begin() + static_cast<std::ptrdiff_t>(i))
            throw std::invalid_argument("Nomes de jogadores devem ser diferentes.");
        jogadores_.push_back(std::make_unique<Jogador>(nomes[i], cores[i]));
    }
    auto territorios = mapa_.todosTerritorios();
    std::shuffle(territorios.begin(), territorios.end(), rng_);
    for (std::size_t i = 0; i < territorios.size(); ++i) {
        auto& jogador = *jogadores_[i % jogadores_.size()];
        jogador.conquistarTerritorio(*territorios[i]);
        territorios[i]->adicionarTropas(1);
    }
    // Reserva inicial simplificada de oito tropas adicionais por jogador.
    for (auto& jogador : jogadores_) {
        std::uniform_int_distribution<std::size_t> sorteio(0, jogador->territorios().size() - 1);
        for (int n = 0; n < 8; ++n) jogador->territorios()[sorteio(rng_)]->adicionarTropas(1);
    }
    criarBaralho();
    atribuirObjetivos();
}

void Jogo::criarBaralho() {
    std::vector<std::unique_ptr<Carta>> cartas;
    auto territorios = mapa_.todosTerritorios();
    for (std::size_t i = 0; i < territorios.size(); ++i) {
        const auto& nome = territorios[i]->nome();
        if (i % 3 == 0) cartas.push_back(std::make_unique<CartaInfantaria>(nome));
        else if (i % 3 == 1) cartas.push_back(std::make_unique<CartaCavalaria>(nome));
        else cartas.push_back(std::make_unique<CartaArtilharia>(nome));
    }
    std::shuffle(cartas.begin(), cartas.end(), rng_);
    for (auto& carta : cartas) baralho_.push_back(std::move(carta));
}

void Jogo::atribuirObjetivos() {
    auto continentes = mapa_.todosContinentes();
    const int meta = jogadores_.size() >= 4 ? 18 : (jogadores_.size() == 3 ? 22 : 24);
    for (std::size_t i = 0; i < jogadores_.size(); ++i) {
        if (i % 3 == 0)
            jogadores_[i]->definirObjetivo(std::make_unique<ObjetivoConquistaTerritorios>(meta));
        else if (i % 3 == 1) {
            // Combinacoes distintas de continentes para cada posicao.
            const auto primeira = (i == 1 ? 1U : 5U);
            const auto segunda = (i == 1 ? 5U : 3U);
            jogadores_[i]->definirObjetivo(std::make_unique<ObjetivoConquistarContinente>(
                std::vector<const Continente*>{continentes[primeira], continentes[segunda]}));
        } else {
            auto& alvo = *jogadores_[(i + 1) % jogadores_.size()];
            jogadores_[i]->definirObjetivo(std::make_unique<ObjetivoEliminarJogador>(alvo, meta));
        }
    }
}

void Jogo::recomporBaralho() {
    if (!baralho_.empty() || descarte_.empty()) return;
    std::shuffle(descarte_.begin(), descarte_.end(), rng_);
    for (auto& carta : descarte_) baralho_.push_back(std::move(carta));
    descarte_.clear();
}
void Jogo::comprarCarta(Jogador& jogador) {
    recomporBaralho();
    if (baralho_.empty()) return;
    jogador.receberCarta(std::move(baralho_.front()));
    baralho_.pop_front();
}
int Jogo::exercitosPorTroca() { return 4 + 2 * numeroTrocas_++; }
int Jogo::reforcosPorTurno(const Jogador& jogador) const {
    int total = std::max(3, static_cast<int>(jogador.territorios().size()) / 2);
    for (const auto* continente : mapa_.todosContinentes()) total += continente->calcularBonus(jogador);
    return total;
}
bool Jogo::verificarVitoria() {
    for (auto& jogador : jogadores_) {
        if (jogador->ativo() && jogador->objetivo() && jogador->objetivo()->foiConcluido(jogador.get())) {
            vencedor_ = jogador.get();
            encerrado_ = true;
            return true;
        }
    }
    std::vector<Jogador*> ativos;
    for (const auto& jogador : jogadores_) if (jogador->ativo()) ativos.push_back(jogador.get());
    if (ativos.size() == 1) {
        vencedor_ = ativos[0];
        encerrado_ = true;
        return true;
    }
    return false;
}
void Jogo::proximoJogador() {
    do { jogadorAtual_ = (jogadorAtual_ + 1) % jogadores_.size(); }
    while (!jogadores_[jogadorAtual_]->ativo());
}
int Jogo::iniciarTurno() {
    fase_ = FaseTurno::Reforco;
    conquistouNesteTurno_ = false;
    auto& jogador = *jogadores_[jogadorAtual_];
    const int bonus = reforcosPorTurno(jogador);
    jogador.receberExercitos(bonus);
    return bonus;
}
void Jogo::exigirFase(FaseTurno fase) const {
    if (encerrado_ || !turnoAtivo_ || fase_ != fase)
        throw MovimentoInvalidoException("Acao indisponivel na fase atual ou turno nao iniciado.");
}
int Jogo::iniciarTurnoAtual() {
    if (encerrado_ || turnoAtivo_) throw MovimentoInvalidoException("Turno encerrado ou ja iniciado.");
    const int bonus = iniciarTurno();
    turnoAtivo_ = true;
    return bonus;
}
void Jogo::reforcar(int idTerritorio, int quantidade) {
    exigirFase(FaseTurno::Reforco);
    jogadorAtual().distribuirExercitos(porIndice(mapa_, idTerritorio), quantidade);
}
RelatorioCombate Jogo::atacarTerritorio(int idOrigem, int idAlvo, int dados) {
    if (encerrado_ || !turnoAtivo_ || fase_ != FaseTurno::Ataque)
        throw AtaqueInvalidoException("O ataque so e permitido na fase de ataque.");
    auto relatorio = jogadorAtual().atacar(porIndice(mapa_, idOrigem), porIndice(mapa_, idAlvo), dados, rng_);
    if (relatorio.conquistado) {
        conquistouNesteTurno_ = true;
        verificarVitoria();
    }
    return relatorio;
}
void Jogo::moverTropas(int idOrigem, int idDestino, int quantidade) {
    exigirFase(FaseTurno::Movimentacao);
    jogadorAtual().moverTropas(porIndice(mapa_, idOrigem), porIndice(mapa_, idDestino), quantidade);
}
int Jogo::trocarCartas() {
    exigirFase(FaseTurno::Reforco);
    auto& jogador = jogadorAtual();
    auto cartas = jogador.trocarCartas();
    for (auto& carta : cartas) descarte_.push_back(std::move(carta));
    const int bonus = exercitosPorTroca();
    jogador.receberExercitos(bonus);
    return bonus;
}
void Jogo::avancarFase() {
    if (encerrado_ || !turnoAtivo_)
        throw MovimentoInvalidoException("Nao e possivel avancar um turno inativo ou encerrado.");
    switch (fase_) {
        case FaseTurno::Reforco:
            if (jogadorAtual().exercitosDisponiveis() > 0)
                throw MovimentoInvalidoException("Distribua todos os reforcos antes de atacar.");
            if (jogadorAtual().totalCartas() >= 5)
                throw MovimentoInvalidoException("Com cinco ou mais cartas, a troca e obrigatoria.");
            fase_ = FaseTurno::Ataque;
            break;
        case FaseTurno::Ataque:
            fase_ = FaseTurno::Movimentacao;
            break;
        case FaseTurno::Movimentacao:
            fase_ = FaseTurno::Fim;
            if (conquistouNesteTurno_) comprarCarta(jogadorAtual());
            verificarVitoria();
            break;
        case FaseTurno::Fim:
            proximoJogador();
            turnoAtivo_ = false;
            break;
    }
}
void Jogo::trocarCartasSePossivel(Jogador& jogador, bool automatico) {
    if (!jogador.podeTrocarCartas()) {
        if (!automatico) std::cout << "Nenhuma trinca disponivel.\n";
        return;
    }
    do {
        const int bonus = trocarCartas();
        std::cout << jogador.nome() << " trocou tres cartas e ganhou +" << bonus << " exercitos.\n";
    } while (automatico && jogador.totalCartas() >= 5 && jogador.podeTrocarCartas());
}

void Jogo::mostrarMapa() const {
    auto todos = mapa_.todosTerritorios();
    std::cout << "\n===== MAPA (IDs para comandos) =====\n";
    for (const auto* continente : mapa_.todosContinentes()) {
        std::cout << "\n" << continente->nome() << " [bonus +" << continente->bonusExercitos() << "]\n";
        for (const auto* territorio : continente->territorios()) {
            std::cout << std::setw(2) << posicao(todos, *territorio) << ") "
                      << std::left << std::setw(22) << territorio->nome() << std::right
                      << " " << std::setw(2) << territorio->tropas() << " tropas | "
                      << (territorio->dono() ? territorio->dono()->nome() : "Sem dono") << "\n";
        }
    }
}
void Jogo::mostrarEstado(const Jogador& jogador) const {
    auto todos = mapa_.todosTerritorios();
    std::cout << "\n" << jogador.nome() << " | Tropas para distribuir: "
              << jogador.exercitosDisponiveis() << "\nTerritorios:\n";
    for (const auto* t : jogador.territorios())
        std::cout << "  #" << posicao(todos, *t) << " " << t->nome() << " (" << t->tropas() << ")\n";
    std::cout << "Cartas:\n";
    if (jogador.cartas().empty()) std::cout << "  Nenhuma\n";
    for (const auto& carta : jogador.cartas()) std::cout << "  - " << carta->descricao() << "\n";
}

void Jogo::executarTurnoHumano() {
    const int bonus = iniciarTurnoAtual();
    auto& jogador = jogadorAtual();
    std::cout << "\n===== TURNO DE " << jogador.nome() << " [" << nomeCor(jogador.cor()) << "] =====\n"
              << "Territorios: " << jogador.territorios().size()
              << " | Reforcos: +" << bonus << " | Cartas: " << jogador.totalCartas() << "\n";
    if (jogador.objetivo()) std::cout << "Objetivo: " << jogador.objetivo()->descricao() << "\n";
    std::cout << "Comandos gerais: mapa | status | vizinhos ID | ajuda | sair\n";
    auto obterLinha = [&]() -> std::string {
        std::string linha;
        if (!std::getline(std::cin, linha)) { encerrado_ = true; return ""; }
        return linha;
    };
    auto informarVizinhos = [&](int id) {
        for (auto* t : porIndice(mapa_, id).vizinhos()) std::cout << "  " << t->nome() << "\n";
    };
    while (!encerrado_ && (jogador.exercitosDisponiveis() > 0 || jogador.totalCartas() >= 5)) {
        std::cout << "[REFORCO: r ID QTD | trocar] > ";
        const auto linha = obterLinha();
        if (encerrado_) return;
        std::istringstream entrada(linha);
        std::string comando;
        entrada >> comando;
        try {
            if (comando == "r") {
                int id, quantidade;
                if (!(entrada >> id >> quantidade)) throw MovimentoInvalidoException("Uso: r ID QTD");
                reforcar(id, quantidade);
            } else if (comando == "trocar") trocarCartasSePossivel(jogador, false);
            else if (comando == "mapa") mostrarMapa();
            else if (comando == "status") mostrarEstado(jogador);
            else if (comando == "vizinhos") {
                int id;
                if (!(entrada >> id)) throw std::invalid_argument("Uso: vizinhos ID");
                informarVizinhos(id);
            } else if (comando == "sair") { encerrado_ = true; return; }
            else std::cout << "Exemplo: r 3 5 | trocar | mapa | status | vizinhos 3 | sair\n";
        } catch (const std::exception& erro) { std::cout << "Acao recusada: " << erro.what() << "\n"; }
    }
    avancarFase();
    while (!encerrado_) {
        std::cout << "[ATAQUE: a ORIGEM ALVO DADOS | passar] > ";
        const auto linha = obterLinha();
        if (encerrado_) return;
        std::istringstream entrada(linha);
        std::string comando;
        entrada >> comando;
        try {
            if (comando == "passar") break;
            if (comando == "a") {
                int origem, alvo, dados;
                if (!(entrada >> origem >> alvo >> dados))
                    throw AtaqueInvalidoException("Uso: a ORIGEM ALVO DADOS(1-3)");
                auto rel = atacarTerritorio(origem, alvo, dados);
                std::cout << "Ataque [" << formatarDados(rel.dadosAtacante) << "] vs defesa ["
                          << formatarDados(rel.dadosDefensor) << "] | perdas: atacante -"
                          << rel.baixasAtacante << ", defensor -" << rel.baixasDefensor << "\n";
                if (rel.conquistado) {
                    std::cout << "Territorio conquistado! " << rel.tropasTransferidas << " tropas transferidas.\n";
                    if (rel.cartasCapturadas)
                        std::cout << "Cartas recebidas do jogador eliminado: " << rel.cartasCapturadas << "\n";
                    if (encerrado_) return;
                }
            } else if (comando == "mapa") mostrarMapa();
            else if (comando == "status") mostrarEstado(jogador);
            else if (comando == "vizinhos") {
                int id;
                if (!(entrada >> id)) throw std::invalid_argument("Uso: vizinhos ID");
                informarVizinhos(id);
            } else if (comando == "sair") { encerrado_ = true; return; }
            else std::cout << "Exemplo: a 5 7 3 | passar | mapa | status | vizinhos 3 | sair\n";
        } catch (const std::exception& erro) { std::cout << "Acao recusada: " << erro.what() << "\n"; }
    }
    avancarFase();
    while (!encerrado_) {
        std::cout << "[MOVIMENTO: m ORIGEM DESTINO QTD | fim] > ";
        const auto linha = obterLinha();
        if (encerrado_) return;
        std::istringstream entrada(linha);
        std::string comando;
        entrada >> comando;
        try {
            if (comando == "fim") break;
            if (comando == "m") {
                int origem, destino, quantidade;
                if (!(entrada >> origem >> destino >> quantidade))
                    throw MovimentoInvalidoException("Uso: m ORIGEM DESTINO QTD");
                moverTropas(origem, destino, quantidade);
                std::cout << "Tropas movimentadas.\n";
            } else if (comando == "mapa") mostrarMapa();
            else if (comando == "status") mostrarEstado(jogador);
            else if (comando == "vizinhos") {
                int id;
                if (!(entrada >> id)) throw std::invalid_argument("Uso: vizinhos ID");
                informarVizinhos(id);
            } else if (comando == "sair") { encerrado_ = true; return; }
            else std::cout << "Exemplo: m 1 2 3 | fim | mapa | status | vizinhos 3 | sair\n";
        } catch (const std::exception& erro) { std::cout << "Acao recusada: " << erro.what() << "\n"; }
    }
    const auto cartasAntes = jogador.totalCartas();
    avancarFase();
    if (jogador.totalCartas() > cartasAntes)
        std::cout << jogador.nome() << " recebeu uma carta de territorio.\n";
}

void Jogo::executarTurnoBot() {
    const int bonus = iniciarTurnoAtual();
    auto& jogador = jogadorAtual();
    std::cout << "\n===== TURNO DE " << jogador.nome() << " [" << nomeCor(jogador.cor()) << "] =====\n"
              << "Territorios: " << jogador.territorios().size()
              << " | Reforcos: +" << bonus << " | Cartas: " << jogador.totalCartas() << "\n";
    trocarCartasSePossivel(jogador, true);
    Territorio* frente = nullptr;
    double melhor = -1e9;
    for (auto* t : jogador.territorios()) {
        int inimigos = 0, minimo = 10000;
        for (const auto* v : t->vizinhos()) if (!jogador.possui(*v)) {
            ++inimigos;
            minimo = std::min(minimo, v->tropas());
        }
        if (inimigos == 0) continue;
        const double nota = t->tropas() * 1.2 - minimo + inimigos * .1;
        if (nota > melhor) { melhor = nota; frente = t; }
    }
    if (!frente) frente = jogador.territorios().front();
    reforcar(static_cast<int>(posicao(mapa_.todosTerritorios(), *frente)), jogador.exercitosDisponiveis());
    avancarFase();
    int conquistas = 0;
    for (int tentativa = 0; tentativa < 28 && !encerrado_; ++tentativa) {
        Territorio* origem = nullptr;
        Territorio* destino = nullptr;
        int maiorVantagem = std::numeric_limits<int>::min();
        for (auto* t : jogador.territorios()) {
            if (t->tropas() < 3) continue;
            for (auto* vizinho : t->vizinhos()) {
                if (jogador.possui(*vizinho)) continue;
                const int vantagem = t->tropas() - vizinho->tropas();
                if (vantagem > maiorVantagem) {
                    maiorVantagem = vantagem;
                    origem = t;
                    destino = vizinho;
                }
            }
        }
        if (!origem || maiorVantagem < 3) break;
        try {
            auto combate = atacarTerritorio(
                static_cast<int>(posicao(mapa_.todosTerritorios(), *origem)),
                static_cast<int>(posicao(mapa_.todosTerritorios(), *destino)),
                std::min(3, origem->tropas() - 1));
            if (combate.conquistado) ++conquistas;
        } catch (const WarException& erro) {
            std::cerr << "Ataque automatico cancelado: " << erro.what() << "\n";
            break;
        }
    }
    if (encerrado_) return;
    avancarFase();
    bool moveu = false;
    for (auto* t : jogador.territorios()) {
        if (t->tropas() < 4) continue;
        bool temInimigo = false;
        for (auto* vizinho : t->vizinhos()) if (!jogador.possui(*vizinho)) temInimigo = true;
        if (temInimigo) continue;
        for (auto* vizinho : t->vizinhos()) {
            if (!jogador.possui(*vizinho)) continue;
            bool fronteira = false;
            for (auto* prox : vizinho->vizinhos()) if (!jogador.possui(*prox)) fronteira = true;
            if (fronteira) {
                moverTropas(static_cast<int>(posicao(mapa_.todosTerritorios(), *t)),
                    static_cast<int>(posicao(mapa_.todosTerritorios(), *vizinho)), t->tropas() - 1);
                moveu = true;
                break;
            }
        }
        if (moveu) break;
    }
    avancarFase();
    std::cout << "Conquistas neste turno: " << conquistas << "\n";
}

void Jogo::executar() {
    std::cout << "=== WAR ORIENTADO A OBJETOS / C++17 ===\n"
              << "Os comandos usam IDs numericos do mapa. Digite 'mapa' e 'status'.\n";
    mostrarMapa();
    while (!encerrado_) {
        executarTurnoHumano();
        if (!encerrado_) avancarFase(); // Fim -> proximo jogador
    }
    if (vencedor_) std::cout << "\n*** VITORIA DE " << vencedor_->nome() << "! ***\n";
    else std::cout << "Partida encerrada sem vencedor.\n";
}

void Jogo::executarDemo(int maxTurnos) {
    if (maxTurnos < 1) throw std::invalid_argument("Limite de turnos invalido.");
    std::cout << "=== SIMULACAO AUTOMATICA WAR (" << jogadores_.size() << " jogadores) ===\n";
    int numeroTurno = 0;
    while (!encerrado_ && numeroTurno < maxTurnos) {
        ++numeroTurno;
        executarTurnoBot();
        if (!encerrado_) avancarFase(); // Fim -> proximo jogador
    }
    if (!vencedor_) std::cout << "Limite de " << maxTurnos << " turnos alcancado sem vencedor.\n";
    else std::cout << "Vencedor: " << vencedor_->nome() << ". Partida concluida em " << numeroTurno << " turnos.\n";
}
