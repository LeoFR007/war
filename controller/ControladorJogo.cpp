#include "ControladorJogo.h"
#include "MapaView.h"
#include "PainelView.h"
#include "Territorio.h"
#include "WarException.h"
#include <algorithm>

ControladorJogo::ControladorJogo(Jogo& jogo)
    : jogo(jogo), fase(Fase::Reforco), selecionado(nullptr), quantidade(1),
      mensagemErro(false), temAtaque(false), esperaBot(0), botaoPressionado(false), arrastando(false) {
    comecarTurno();
}

// ---------------- Entrada do usuario ----------------

void ControladorJogo::tratarEvento(const sf::Event& evento, sf::Vector2f mouse, MapaView& mapa,
                                   const PainelView& painel) {
    if (evento.type == sf::Event::MouseButtonPressed && evento.mouseButton.button == sf::Mouse::Left) {
        int botaoZoom = mapa.botaoZoomEm(mouse);
        if (botaoZoom == 1) mapa.aproximar(1, mouse);
        else if (botaoZoom == 2) mapa.aproximar(-1, mouse);
        else if (botaoZoom == 3) mapa.centralizar();
        else if (mapa.contem(mouse)) {
            // So decide no soltar: se o mouse andou, foi arrasto do mapa; senao, foi clique.
            botaoPressionado = true;
            arrastando = false;
            inicioClique = ultimoMouse = mouse;
        } else {
            AcaoPainel acao = painel.acaoEm(mouse);
            if (acao == AcaoPainel::Menos) mudarQuantidade(-1);
            else if (acao == AcaoPainel::Mais) mudarQuantidade(+1);
            else if (acao == AcaoPainel::Trocar) trocarCartas();
            else if (acao == AcaoPainel::ProximaFase) proximaFase();
        }
    } else if (evento.type == sf::Event::MouseMoved && botaoPressionado) {
        sf::Vector2f andou = mouse - inicioClique;
        // compara o quadrado da distância com 6*6 pra não precisar de raiz quadrada.
        // esses 6 pixels de folga evitam que um clique com a mão tremendo vire arrasto
        if (andou.x * andou.x + andou.y * andou.y > 36) arrastando = true;   // mais de 6 pixels
        if (arrastando) mapa.arrastar(mouse - ultimoMouse);
        ultimoMouse = mouse;
    } else if (evento.type == sf::Event::MouseButtonReleased && evento.mouseButton.button == sf::Mouse::Left) {
        if (botaoPressionado && !arrastando) {
            int id = mapa.territorioEm(mouse);
            if (id > 0) clicarTerritorio(id, false);
        }
        botaoPressionado = false;
        arrastando = false;
    } else if (evento.type == sf::Event::MouseButtonPressed && evento.mouseButton.button == sf::Mouse::Right) {
        int id = mapa.territorioEm(mouse);
        if (id > 0) clicarTerritorio(id, true);
    } else if (evento.type == sf::Event::MouseWheelScrolled && mapa.contem(mouse)) {
        mapa.aproximar(evento.mouseWheelScroll.delta, mouse);
    } else if (evento.type == sf::Event::KeyPressed) {
        sf::Keyboard::Key tecla = evento.key.code;
        if (tecla == sf::Keyboard::Space || tecla == sf::Keyboard::Enter) proximaFase();
        else if (tecla == sf::Keyboard::Add || tecla == sf::Keyboard::Equal) mudarQuantidade(+1);
        else if (tecla == sf::Keyboard::Subtract || tecla == sf::Keyboard::Hyphen) mudarQuantidade(-1);
        else if (tecla == sf::Keyboard::T) trocarCartas();
        else if (tecla == sf::Keyboard::Escape) limparSelecao();
        else if (tecla == sf::Keyboard::R || tecla == sf::Keyboard::Home) mapa.centralizar();
        else if (tecla == sf::Keyboard::Left || tecla == sf::Keyboard::A) mapa.arrastar({60, 0});
        else if (tecla == sf::Keyboard::Right || tecla == sf::Keyboard::D) mapa.arrastar({-60, 0});
        else if (tecla == sf::Keyboard::Up || tecla == sf::Keyboard::W) mapa.arrastar({0, 60});
        else if (tecla == sf::Keyboard::Down || tecla == sf::Keyboard::S) mapa.arrastar({0, -60});
    }
}

void ControladorJogo::atualizar(float segundos) {
    if (!vezDoBot()) return;
    esperaBot += segundos;
    if (esperaBot < 0.8f) return;

    // o bot joga o turno inteiro de uma vez, mas só depois de 0.8s.
    // sem essa espera os bots jogavam todos no mesmo quadro e não dava pra ver nada
    Jogador* bot = jogo.getJogadorAtual();
    jogo.iniciarTurno();
    bot->jogarTurno(jogo);          // polimorfismo: a estrategia esta em JogadorBot
    if (!jogo.terminou()) jogo.finalizarTurno();
    comecarTurno();
}

// ---------------- Acoes ----------------

void ControladorJogo::comecarTurno() {
    selecionado = nullptr;
    quantidade = 1;
    esperaBot = 0;
    if (jogo.terminou()) return;
    Jogador* jogador = jogo.getJogadorAtual();
    if (jogador->ehAutomatico()) {
        avisar("Vez de " + jogador->getNome() + " (automatico)...", false);
        return;
    }
    jogo.iniciarTurno();
    fase = Fase::Reforco;
    temAtaque = false;
    avisar(jogador->getNome() + ": posicione " + std::to_string(jogador->getExercitos()) +
           " exercitos clicando nos seus territorios.", false);
}

void ControladorJogo::clicarTerritorio(int id, bool botaoDireito) {
    if (jogo.terminou() || vezDoBot()) return;
    Territorio* territorio = jogo.getMapa().getTerritorio(id);
    try {
        if (fase == Fase::Reforco) cliqueReforco(territorio, botaoDireito);
        else if (fase == Fase::Ataque) cliqueAtaque(territorio);
        else cliqueMovimento(territorio);
    } catch (const WarException& erro) {
        avisar(erro.what(), true);   // a regra foi validada pelo model
    }
}

void ControladorJogo::cliqueReforco(Territorio* territorio, bool todos) {
    selecionado = territorio;
    Jogador* jogador = jogo.getJogadorAtual();
    int colocar = todos ? jogador->getExercitos() : quantidade;
    jogador->colocarExercitos(territorio, colocar);
    if (jogador->getExercitos() == 0)
        avisar("Reforcos posicionados. Clique em PROXIMA FASE para atacar.", false);
    else
        avisar("+" + std::to_string(colocar) + " em " + territorio->getNome() + ". Restam " +
               std::to_string(jogador->getExercitos()) + ".", false);
}

void ControladorJogo::cliqueAtaque(Territorio* territorio) {
    Jogador* jogador = jogo.getJogadorAtual();
    if (jogador->possui(territorio)) {
        selecionado = territorio;
        avisar("Origem: " + territorio->getNome() + ". Clique em um vizinho destacado em vermelho.", false);
        return;
    }
    if (selecionado == nullptr) {
        avisar("Primeiro selecione um territorio seu como origem do ataque.", true);
        return;
    }
    int dados = std::min(3, selecionado->getTropas() - 1);
    ultimoAtaque = jogo.atacar(jogador, selecionado, territorio, dados);
    temAtaque = true;
    if (ultimoAtaque.conquistou) {
        avisar("Conquistou " + territorio->getNome() + "!", false);
    } else {
        avisar("Ataque a " + territorio->getNome() + ": perdeu " + std::to_string(ultimoAtaque.perdasAtaque) +
               ", defensor perdeu " + std::to_string(ultimoAtaque.perdasDefesa) + ".", false);
        jogo.registrarEvento(jogador->getNome() + " atacou " + territorio->getNome() + " (-" +
                             std::to_string(ultimoAtaque.perdasAtaque) + "/-" +
                             std::to_string(ultimoAtaque.perdasDefesa) + ")");
    }
    if (selecionado->getTropas() < 2) selecionado = nullptr;
}

void ControladorJogo::cliqueMovimento(Territorio* territorio) {
    Jogador* jogador = jogo.getJogadorAtual();
    if (selecionado == nullptr || !jogador->possui(selecionado) || !jogador->possui(territorio) ||
        !selecionado->ehVizinho(territorio)) {
        selecionado = territorio;
        avisar("Origem: " + territorio->getNome() + ". Clique em um vizinho destacado em verde.", false);
        return;
    }
    jogador->moverTropas(selecionado, territorio, quantidade);
    avisar("Moveu " + std::to_string(quantidade) + " tropa(s) para " + territorio->getNome() + ".", false);
    jogo.registrarEvento(jogador->getNome() + " moveu " + std::to_string(quantidade) + ": " +
                         selecionado->getNome() + " -> " + territorio->getNome());
}

void ControladorJogo::proximaFase() {
    if (jogo.terminou() || vezDoBot()) return;
    Jogador* jogador = jogo.getJogadorAtual();
    selecionado = nullptr;
    quantidade = 1;
    if (fase == Fase::Reforco) {
        if (jogador->getExercitos() > 0) {
            avisar("Posicione todos os exercitos antes de atacar (botao direito coloca todos).", true);
            return;
        }
        fase = Fase::Ataque;
        avisar("Ataque: clique na origem e depois no alvo. Usa ate 3 dados automaticamente.", false);
    } else if (fase == Fase::Ataque) {
        fase = Fase::Movimento;
        avisar("Movimentacao: clique na origem e depois no destino (uma vez por tropa).", false);
    } else {
        jogo.finalizarTurno();
        comecarTurno();
    }
}

void ControladorJogo::trocarCartas() {
    if (jogo.terminou() || vezDoBot()) return;
    if (fase != Fase::Reforco) {
        avisar("Cartas so podem ser trocadas na fase de reforco.", true);
        return;
    }
    try {
        int bonus = jogo.trocarCartas(jogo.getJogadorAtual());
        avisar("Troca feita: +" + std::to_string(bonus) + " exercitos.", false);
    } catch (const WarException& erro) {
        avisar(erro.what(), true);
    }
}

void ControladorJogo::mudarQuantidade(int diferenca) {
    quantidade = std::max(1, std::min(99, quantidade + diferenca));
}

void ControladorJogo::limparSelecao() { selecionado = nullptr; }

void ControladorJogo::avisar(const std::string& texto, bool erro) {
    mensagem = texto;
    mensagemErro = erro;
}

// ---------------- Consultas ----------------

Fase ControladorJogo::getFase() const { return fase; }

const Territorio* ControladorJogo::getSelecionado() const { return selecionado; }

int ControladorJogo::getQuantidade() const { return quantidade; }

const std::string& ControladorJogo::getMensagem() const { return mensagem; }

bool ControladorJogo::mensagemEhErro() const { return mensagemErro; }

bool ControladorJogo::temUltimoAtaque() const { return temAtaque; }

const ResultadoAtaque& ControladorJogo::getUltimoAtaque() const { return ultimoAtaque; }

bool ControladorJogo::vezDoBot() const { return !jogo.terminou() && jogo.getJogadorAtual()->ehAutomatico(); }
