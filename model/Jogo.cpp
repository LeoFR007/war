#include "Jogo.h"
#include "JogadorBot.h"
#include "JogadorHumano.h"
#include "Objetivo.h"
#include "WarException.h"
#include <algorithm>
#include <iostream>

Jogo::Jogo(const std::vector<std::string>& nomes, int quantidadeHumanos, unsigned int semente)
    : gerador(semente), jogadorAtual(0), turno(0), trocasRealizadas(0), conquistouNoTurno(false),
      fim(false), vencedor(nullptr) {
    if (nomes.size() < 2 || nomes.size() > 6) throw WarException("O WAR precisa de 2 a 6 jogadores.");
    if (quantidadeHumanos < 0 || quantidadeHumanos > static_cast<int>(nomes.size()))
        throw WarException("Quantidade de jogadores humanos invalida.");

    std::vector<std::string> cores = {"Vermelho", "Azul", "Verde", "Amarelo", "Roxo", "Preto"};
    // os primeiros da lista são humanos e o resto vira bot.
    // como os dois herdam de Jogador, dá pra guardar todo mundo no mesmo vetor
    for (size_t i = 0; i < nomes.size(); i++) {
        if (static_cast<int>(i) < quantidadeHumanos)
            jogadores.push_back(std::make_unique<JogadorHumano>(nomes[i], cores[i]));
        else
            jogadores.push_back(std::make_unique<JogadorBot>(nomes[i], cores[i]));
    }
    distribuirTerritorios();
    criarBaralho();
    sortearObjetivos();
}

// Sorteia os 42 territorios entre os jogadores (1 tropa cada) e
// espalha mais 8 tropas aleatorias por jogador.
void Jogo::distribuirTerritorios() {
    std::vector<Territorio*> lista;
    for (int id = 1; id <= mapa.getQuantidadeTerritorios(); id++) lista.push_back(mapa.getTerritorio(id));
    std::shuffle(lista.begin(), lista.end(), gerador);

    for (size_t i = 0; i < lista.size(); i++) {
        // o i % jogadores.size() faz o rodízio (0, 1, 2, 0, 1, 2...), igual a dar as cartas na mesa
        jogadores[i % jogadores.size()]->conquistar(lista[i]);
        lista[i]->adicionarTropas(1);
    }
    for (auto& jogador : jogadores) {
        std::uniform_int_distribution<int> sorteio(0, jogador->getQuantidadeTerritorios() - 1);
        for (int i = 0; i < 8; i++) jogador->getTerritorios()[sorteio(gerador)]->adicionarTropas(1);
    }
}

// Uma carta por territorio, alternando os tres tipos.
void Jogo::criarBaralho() {
    std::vector<TipoCarta> tipos = {TipoCarta::Infantaria, TipoCarta::Cavalaria, TipoCarta::Artilharia};
    for (int id = 1; id <= mapa.getQuantidadeTerritorios(); id++)
        baralho.push_back(Carta(mapa.getTerritorio(id)->getNome(), tipos[id % 3]));
    std::shuffle(baralho.begin(), baralho.end(), gerador);
}

// Embaralha 6 objetivos diferentes e entrega um para cada jogador.
// A meta de territorios fica bem acima da posse inicial
// (com 2 jogadores cada um ja comeca com 21 territorios).
void Jogo::sortearObjetivos() {
    int meta = (jogadores.size() == 2) ? 30 : 24;
    // cada número é uma "carta" de objetivo. embaralha e o jogador i fica com a carta i,
    // assim dois jogadores nunca recebem o mesmo objetivo
    std::vector<int> objetivos = {0, 1, 2, 3, 4, 5};
    std::shuffle(objetivos.begin(), objetivos.end(), gerador);

    // Indices dos continentes: 0 America do Norte, 1 America do Sul, 2 Europa,
    // 3 Africa, 4 Asia, 5 Oceania.
    for (size_t i = 0; i < jogadores.size(); i++) {
        Objetivo* objetivo = nullptr;
        switch (objetivos[i]) {
            case 0: objetivo = new ObjetivoTerritorios(meta); break;
            case 1: objetivo = new ObjetivoContinentes(mapa.getContinente(0), mapa.getContinente(3)); break;
            case 2: objetivo = new ObjetivoContinentes(mapa.getContinente(4), mapa.getContinente(1)); break;
            case 3: objetivo = new ObjetivoContinentes(mapa.getContinente(0), mapa.getContinente(5)); break;
            case 4: objetivo = new ObjetivoContinentes(mapa.getContinente(4), mapa.getContinente(3)); break;
            default: {
                // o alvo é sempre o próximo da lista, então ninguém tem que eliminar a si mesmo
                Jogador* alvo = jogadores[(i + 1) % jogadores.size()].get();
                objetivo = new ObjetivoEliminar(alvo, meta);
            }
        }
        jogadores[i]->setObjetivo(objetivo); // o jogador passa a ser o dono do objetivo
    }
}

void Jogo::executar(int maxTurnos) {
    // laço da versão de terminal. o turno foi quebrado em iniciar/jogar/finalizar porque
    // a versão com janela não pode ficar travada esperando o teclado: lá o controller
    // chama iniciarTurno() e finalizarTurno() separado e cuida das fases pelos cliques
    while (!fim && turno < maxTurnos) {
        Jogador* jogador = getJogadorAtual();
        iniciarTurno();
        jogador->jogarTurno(*this); // polimorfismo: JogadorHumano ou JogadorBot
        if (!fim) finalizarTurno();
    }
    if (vencedor != nullptr)
        std::cout << "\n*** VITORIA DE " << vencedor->getNome() << " apos " << turno << " turnos! ***\n";
    else
        std::cout << "\nPartida encerrada sem vencedor (" << turno << " turnos).\n";
}

int Jogo::iniciarTurno() {
    Jogador* jogador = getJogadorAtual();
    turno++;
    conquistouNoTurno = false;
    // zera quem já andou no turno anterior, senão a regra de "cada tropa move uma vez"
    // ia valer pro resto do jogo
    for (int id = 1; id <= mapa.getQuantidadeTerritorios(); id++) mapa.getTerritorio(id)->zerarTropasRecebidas();
    int reforcos = calcularReforcos(jogador);
    jogador->receberExercitos(reforcos);
    std::cout << "\n===== TURNO DE " << jogador->getNome() << " [" << jogador->getCor() << "] =====\n"
              << "Territorios: " << jogador->getQuantidadeTerritorios() << " | Reforcos: +" << reforcos
              << " | Cartas: " << jogador->getCartas().size() << "\n";
    registrarEvento("Turno " + std::to_string(turno) + ": " + jogador->getNome() + " recebe +" +
                    std::to_string(reforcos) + " exercitos");

    // Com 5 cartas ou mais a troca e obrigatoria (5 cartas sempre formam uma troca valida).
    while (jogador->getCartas().size() >= 5) {
        std::cout << "Troca obrigatoria de cartas: +" << trocarCartas(jogador) << " exercitos.\n";
    }
    return reforcos;
}

void Jogo::finalizarTurno() {
    Jogador* jogador = getJogadorAtual();
    if (conquistouNoTurno && !baralho.empty()) {
        jogador->receberCarta(baralho.back());
        baralho.pop_back();
        std::cout << jogador->getNome() << " recebeu uma carta.\n";
    }
    verificarVitoria(jogador);
    if (fim) return;
    // Passa a vez para o proximo jogador que ainda tem territorios.
    // do-while porque tem que avançar pelo menos uma vez, e continua pulando quem já foi eliminado
    do {
        jogadorAtual = (jogadorAtual + 1) % static_cast<int>(jogadores.size());
    } while (!jogadores[jogadorAtual]->estaAtivo());
}

// Reforcos = metade dos territorios (minimo 3) + bonus dos continentes dominados.
int Jogo::calcularReforcos(const Jogador* jogador) const {
    int total = std::max(3, jogador->getQuantidadeTerritorios() / 2);
    for (int i = 0; i < mapa.getQuantidadeContinentes(); i++) {
        if (mapa.getContinente(i)->controladoPor(jogador)) total += mapa.getContinente(i)->getBonus();
    }
    return total;
}

// So o jogador da vez pode vencer durante o seu turno.
void Jogo::verificarVitoria(Jogador* jogador) {
    if (fim) return;
    int ativos = 0;
    for (auto& outro : jogadores) {
        if (outro->estaAtivo()) ativos++;
    }
    if (jogador->getObjetivo()->foiCumprido(jogador) || ativos == 1) {
        vencedor = jogador;
        fim = true;
        registrarEvento(jogador->getNome() + " VENCEU! " + jogador->getObjetivo()->descricao());
    }
}

std::vector<int> Jogo::rolarDados(int quantidade) {
    std::uniform_int_distribution<int> dado(1, 6);
    std::vector<int> valores;
    for (int i = 0; i < quantidade; i++) valores.push_back(dado(gerador));
    // Lambda: ordena do maior para o menor, para comparar os dados em pares.
    std::sort(valores.begin(), valores.end(), [](int a, int b) { return a > b; });
    return valores;
}

ResultadoAtaque Jogo::atacar(Jogador* atacante, Territorio* origem, Territorio* alvo, int dados) {
    if (!atacante->possui(origem)) throw WarException("O territorio de origem nao e seu.");
    if (atacante->possui(alvo)) throw WarException("Voce nao pode atacar o proprio territorio.");
    if (!origem->ehVizinho(alvo)) throw WarException("Os territorios nao sao vizinhos.");
    if (dados < 1 || dados > 3 || dados >= origem->getTropas())
        throw WarException("Use de 1 a 3 dados e deixe pelo menos 1 tropa na origem.");

    ResultadoAtaque resultado;
    resultado.dadosAtaque = rolarDados(dados);
    resultado.dadosDefesa = rolarDados(std::min(3, alvo->getTropas()));

    // exemplo: ataque [6, 4, 2] contra defesa [5, 4]
    //   6 > 5 -> defesa perde 1
    //   4 = 4 -> empate, quem perde é o ataque
    //   o 2 sobra porque a defesa só tinha 2 dados
    // Compara os maiores dados entre si; empate favorece a defesa.
    size_t pares = std::min(resultado.dadosAtaque.size(), resultado.dadosDefesa.size());
    for (size_t i = 0; i < pares; i++) {
        if (resultado.dadosAtaque[i] > resultado.dadosDefesa[i]) resultado.perdasDefesa++;
        else resultado.perdasAtaque++;
    }
    if (resultado.perdasAtaque > 0) origem->removerTropas(resultado.perdasAtaque);
    if (resultado.perdasDefesa > 0) alvo->removerTropas(resultado.perdasDefesa);

    if (alvo->getTropas() == 0) {
        Jogador* derrotado = alvo->getDono();
        // entra no território conquistado com o número de dados usados,
        // mas sempre sobrando pelo menos 1 tropa na origem
        int tropasMovidas = std::min(dados, origem->getTropas() - 1);
        origem->removerTropas(tropasMovidas);
        atacante->conquistar(alvo);
        alvo->adicionarTropas(tropasMovidas);
        resultado.conquistou = true;
        conquistouNoTurno = true;
        registrarEvento(atacante->getNome() + " conquistou " + alvo->getNome() + " de " + derrotado->getNome());

        // ficou sem nenhum território = foi eliminado agora. guarda quem eliminou
        // (o ObjetivoEliminar depende disso) e as cartas dele vão pro atacante
        if (!derrotado->estaAtivo()) {
            derrotado->setEliminadoPor(atacante);
            derrotado->entregarCartasPara(atacante);
            std::cout << derrotado->getNome() << " foi eliminado por " << atacante->getNome() << "!\n";
            registrarEvento(derrotado->getNome() + " foi eliminado por " + atacante->getNome() + "!");
        }
        verificarVitoria(atacante);
    }
    return resultado;
}

// Trocas valem 4, 6, 8, 10... exercitos. As cartas voltam para o fundo do baralho.
int Jogo::trocarCartas(Jogador* jogador) {
    std::vector<Carta> devolvidas = jogador->trocarCartas();
    baralho.insert(baralho.begin(), devolvidas.begin(), devolvidas.end());
    // o contador é da partida inteira, não de cada jogador. então quanto mais o jogo
    // avança, mais vale trocar (4, 6, 8, 10...)
    trocasRealizadas++;
    int bonus = 4 + 2 * (trocasRealizadas - 1);
    jogador->receberExercitos(bonus);
    registrarEvento(jogador->getNome() + " trocou cartas: +" + std::to_string(bonus) + " exercitos");
    return bonus;
}

void Jogo::encerrar() { fim = true; }

void Jogo::registrarEvento(const std::string& texto) { historico.push_back(texto); }

Jogador* Jogo::getJogadorAtual() const { return jogadores[jogadorAtual].get(); }

int Jogo::getTurno() const { return turno; }

const std::vector<std::string>& Jogo::getHistorico() const { return historico; }

Mapa& Jogo::getMapa() { return mapa; }

const Mapa& Jogo::getMapa() const { return mapa; }

bool Jogo::terminou() const { return fim; }

const Jogador* Jogo::getVencedor() const { return vencedor; }

int Jogo::getQuantidadeJogadores() const { return static_cast<int>(jogadores.size()); }

Jogador* Jogo::getJogador(int indice) const { return jogadores.at(indice).get(); }
