#include "PainelView.h"
#include "ControladorJogo.h"
#include "Jogo.h"
#include "Objetivo.h"
#include "Tema.h"
#include "Territorio.h"
#include <sstream>

// Margem interna e largura util do painel.
const float X = AREA_PAINEL.left + 16;
const float LARGURA = AREA_PAINEL.width - 32;
const float LINHA_BOTOES = 452;

PainelView::PainelView(const Fontes& fontes)
    : fontes(fontes), botaoMenos(X, LINHA_BOTOES, 30, 38), botaoMais(X + 78, LINHA_BOTOES, 30, 38),
      botaoTrocar(X + 116, LINHA_BOTOES, 92, 38), botaoProxima(X + 216, LINHA_BOTOES, 104, 38) {}

void PainelView::desenhar(sf::RenderTarget& alvo, const Jogo& jogo, const ControladorJogo& controlador,
                          sf::Vector2f mouse) const {
    sombra(alvo, AREA_PAINEL, 10);
    caixa(alvo, AREA_PAINEL, 10, COR_PAINEL, COR_LINHA, 1);
    desenharCabecalho(alvo, jogo);
    desenharContinentes(alvo, jogo, 300);
    linha(alvo, {X, 386}, {X + LARGURA, 386}, 1, COR_LINHA);
    desenharAcoes(alvo, jogo, controlador, mouse, 394);
}

// Jogador da vez, numeros principais e objetivo secreto.
void PainelView::desenharCabecalho(sf::RenderTarget& alvo, const Jogo& jogo) const {
    const Jogador* jogador = jogo.getJogadorAtual();
    sf::Color cor = corDoJogador(jogador->getCor());

    escrever(alvo, fontes.medio, "COMANDANTE", X, 89, FONTE_MINIMA, COR_TEXTO_SUAVE, Alinhamento::Esquerda, 3.0f);
    std::string tipo = jogador->ehAutomatico() ? "AUTOMÁTICO" : "HUMANO";
    float larguraTipo = larguraTexto(fontes.medio, tipo, 10, 1.5f) + 16;
    caixa(alvo, sf::FloatRect(X + LARGURA - larguraTipo, 86, larguraTipo, 18), 9, comAlfa(cor, 60), comAlfa(cor, 160), 1);
    escrever(alvo, fontes.medio, tipo, X + LARGURA - larguraTipo / 2, 88, 10, clarear(cor, 0.4f), Alinhamento::Centro, 1.5f);

    circulo(alvo, {X + 15, 128}, 16, escurecer(cor, 0.55f), cor, 1.5f);
    emblema(alvo, {X + 15, 128}, 9.5f, indiceDoJogador(jogador->getCor()), clarear(cor, 0.25f));
    escrever(alvo, fontes.negrito, jogador->getNome(), X + 40, 114, FONTE_TITULO + 2, COR_TEXTO);
    caixa(alvo, sf::FloatRect(X, 152, LARGURA, 3), 1.5f, cor);

    // tres numeros grandes
    const std::string rotulos[3] = {"RESERVA", "TERRITÓRIOS", "TROPAS"};
    int valores[3] = {jogador->getExercitos(), jogador->getQuantidadeTerritorios(), jogador->getTotalTropas()};
    float larguraCartao = (LARGURA - 16) / 3;
    for (int i = 0; i < 3; i++) {
        sf::FloatRect cartao(X + i * (larguraCartao + 8), 164, larguraCartao, 58);
        bool alerta = i == 0 && valores[0] > 0;      // ainda ha exercitos para posicionar
        caixa(alvo, cartao, 6, COR_PAINEL_ALTO, alerta ? COR_REFORCO : COR_LINHA, alerta ? 1.5f : 1);
        escrever(alvo, fontes.medio, rotulos[i], cartao.left + 10, cartao.top + 7, 10, COR_TEXTO_SUAVE, Alinhamento::Esquerda, 1.4f);
        escrever(alvo, fontes.titulo, std::to_string(valores[i]), cartao.left + 10, cartao.top + 19, FONTE_DESTAQUE,
                 alerta ? COR_REFORCO : COR_TEXTO);
    }

    // objetivo secreto
    sf::FloatRect cartaoObjetivo(X, 230, LARGURA, 60);
    caixa(alvo, cartaoObjetivo, 6, COR_PAINEL_ALTO);
    caixa(alvo, sf::FloatRect(X, 230, 3, 60), 1.5f, COR_LATAO);
    escrever(alvo, fontes.medio, "OBJETIVO SECRETO", X + 14, 236, 10, COR_LATAO, Alinhamento::Esquerda, 2.0f);
    float y = 252;
    for (const std::string& trecho : quebrarLinhas(jogador->getObjetivo()->descricao(), fontes.normal, LARGURA - 26, 13)) {
        if (y > 270) break;
        escrever(alvo, fontes.normal, trecho, X + 14, y, 13, COR_TEXTO);
        y += 17;
    }
}

// Seis "pilulas": cor do dono se o continente estiver dominado, com marca para o jogador da vez.
void PainelView::desenharContinentes(sf::RenderTarget& alvo, const Jogo& jogo, float y) const {
    escrever(alvo, fontes.medio, "CONTINENTES", X, y, FONTE_MINIMA, COR_TEXTO_SUAVE, Alinhamento::Esquerda, 3.0f);
    const std::string curtos[6] = {"AM. NORTE", "AM. SUL", "EUROPA", "ÁFRICA", "ÁSIA", "OCEANIA"};
    const Mapa& mapa = jogo.getMapa();
    float larguraPilula = (LARGURA - 12) / 3;
    for (int c = 0; c < mapa.getQuantidadeContinentes(); c++) {
        const Continente* continente = mapa.getContinente(c);
        const Jogador* dono = nullptr;
        for (int i = 0; i < jogo.getQuantidadeJogadores(); i++)
            if (continente->controladoPor(jogo.getJogador(i))) dono = jogo.getJogador(i);

        sf::FloatRect area(X + (c % 3) * (larguraPilula + 6), y + 18 + (c / 3) * 32, larguraPilula, 26);
        sf::Color fundo = COR_PAINEL_ALTO, borda = COR_LINHA, texto = COR_TEXTO_SUAVE;
        if (dono != nullptr) {
            sf::Color cor = corDoJogador(dono->getCor());
            fundo = escurecer(cor, 0.5f);
            borda = dono == jogo.getJogadorAtual() ? COR_LATAO : cor;
            texto = COR_TEXTO;
        }
        caixa(alvo, area, 13, fundo, borda, dono == jogo.getJogadorAtual() ? 1.5f : 1);
        if (dono != nullptr) emblema(alvo, {area.left + 13, area.top + 13}, 5.5f, indiceDoJogador(dono->getCor()),
                                     clarear(corDoJogador(dono->getCor()), 0.3f));
        escrever(alvo, fontes.medio, curtos[c], area.left + (dono ? 24 : 11), area.top + 6, FONTE_MINIMA, texto);
        escrever(alvo, fontes.titulo, "+" + std::to_string(continente->getBonus()), area.left + area.width - 9, area.top + 4,
                 14, dono ? COR_LATAO : COR_TEXTO_SUAVE, Alinhamento::Direita);
    }
}

// Fase, instrucao, botoes e ultima batalha.
void PainelView::desenharAcoes(sf::RenderTarget& alvo, const Jogo& jogo, const ControladorJogo& controlador,
                               sf::Vector2f mouse, float y) const {
    const Jogador* jogador = jogo.getJogadorAtual();
    bool humano = !jogador->ehAutomatico() && !jogo.terminou();
    Fase fase = controlador.getFase();
    sf::Color corFase = fase == Fase::Ataque ? COR_ATAQUE : (fase == Fase::Movimento ? COR_MOVIMENTO : COR_REFORCO);
    int numero = fase == Fase::Reforco ? 1 : (fase == Fase::Ataque ? 2 : 3);
    std::string nomeFase = fase == Fase::Reforco ? "REFORÇO" : (fase == Fase::Ataque ? "ATAQUE" : "MOVIMENTO");

    if (humano) escrever(alvo, fontes.titulo, "FASE " + std::to_string(numero) + "/3  ·  " + nomeFase, X, y, 17, corFase,
                         Alinhamento::Esquerda, 1.5f);
    else escrever(alvo, fontes.titulo, "AGUARDANDO ADVERSÁRIO", X, y, 17, COR_TEXTO_SUAVE, Alinhamento::Esquerda, 1.5f);

    // instrucao / resultado (erros das excecoes do model aparecem em vermelho)
    float linhaY = y + 24;
    sf::Color corMensagem = controlador.mensagemEhErro() ? COR_ERRO : COR_TEXTO;
    for (const std::string& trecho : quebrarLinhas(controlador.getMensagem(), fontes.normal, LARGURA, 13)) {
        if (linhaY > y + 42) break;
        escrever(alvo, fontes.normal, trecho, X, linhaY, 13, corMensagem);
        linhaY += 17;
    }

    // quantidade (reforco e movimento) ou informacao dos dados (ataque)
    if (fase == Fase::Ataque) {
        caixa(alvo, sf::FloatRect(X, LINHA_BOTOES, 108, 38), 6, COR_PAINEL_ALTO, COR_LINHA, 1);
        escrever(alvo, fontes.medio, "DADOS", X + 54, LINHA_BOTOES + 4, 10, COR_TEXTO_SUAVE, Alinhamento::Centro, 1.5f);
        escrever(alvo, fontes.titulo, "até 3", X + 54, LINHA_BOTOES + 15, 16, COR_TEXTO, Alinhamento::Centro);
    } else {
        desenharBotao(alvo, botaoMenos, "−", humano, COR_PAINEL_ALTO, mouse);
        caixa(alvo, sf::FloatRect(X + 32, LINHA_BOTOES, 44, 38), 4, COR_FUNDO, COR_LINHA, 1);
        escrever(alvo, fontes.titulo, std::to_string(controlador.getQuantidade()), X + 54, LINHA_BOTOES + 7, 20, COR_TEXTO,
                 Alinhamento::Centro);
        desenharBotao(alvo, botaoMais, "+", humano, COR_PAINEL_ALTO, mouse);
    }
    std::string cartas = "CARTAS " + std::to_string(jogador->getCartas().size());
    desenharBotao(alvo, botaoTrocar, cartas, humano && fase == Fase::Reforco && jogador->podeTrocarCartas(),
                  sf::Color(70, 62, 40), mouse);
    std::string proxima = fase == Fase::Reforco ? "ATACAR  »" : (fase == Fase::Ataque ? "MOVER  »" : "FIM DO TURNO");
    desenharBotao(alvo, botaoProxima, proxima, humano, escurecer(corFase, 0.35f), mouse);

    // ultima batalha: dados do atacante x dados do defensor
    float yBatalha = LINHA_BOTOES + 50;
    escrever(alvo, fontes.medio, "ÚLTIMA BATALHA", X, yBatalha, FONTE_MINIMA, COR_TEXTO_SUAVE, Alinhamento::Esquerda, 3.0f);
    if (controlador.temUltimoAtaque()) {
        const ResultadoAtaque& ataque = controlador.getUltimoAtaque();
        for (size_t i = 0; i < ataque.dadosAtaque.size(); i++)
            dado(alvo, {X + i * 34.0f, yBatalha + 20}, 28, ataque.dadosAtaque[i], escurecer(COR_ATAQUE, 0.15f));
        escrever(alvo, fontes.titulo, "×", X + 108, yBatalha + 22, 20, COR_TEXTO_SUAVE);
        for (size_t i = 0; i < ataque.dadosDefesa.size(); i++)
            dado(alvo, {X + 128 + i * 34.0f, yBatalha + 20}, 28, ataque.dadosDefesa[i], sf::Color(58, 98, 140));
        if (ataque.conquistou)
            escrever(alvo, fontes.titulo, "CONQUISTA!", X + LARGURA, yBatalha + 25, 17, COR_CONQUISTA, Alinhamento::Direita, 1.2f);
        else
            escrever(alvo, fontes.titulo, "−" + std::to_string(ataque.perdasAtaque) + " / −" + std::to_string(ataque.perdasDefesa),
                     X + LARGURA, yBatalha + 25, 17, COR_TEXTO, Alinhamento::Direita);
    } else {
        escrever(alvo, fontes.normal, "Nenhum ataque neste turno.", X, yBatalha + 26, 13, COR_TEXTO_SUAVE);
    }
}

// Estados do botao: normal, mouse em cima, pressionado e desabilitado.
void PainelView::desenharBotao(sf::RenderTarget& alvo, const sf::FloatRect& area, const std::string& texto, bool ativo,
                               sf::Color cor, sf::Vector2f mouse) const {
    bool emCima = ativo && area.contains(mouse);
    bool pressionado = emCima && sf::Mouse::isButtonPressed(sf::Mouse::Left);
    sf::Color fundo = !ativo ? comAlfa(cor, 70) : (pressionado ? escurecer(cor, 0.2f) : (emCima ? clarear(cor, 0.12f) : cor));
    sf::Color borda = !ativo ? comAlfa(COR_LINHA, 120) : (emCima ? COR_LATAO : clarear(cor, 0.2f));
    float desce = pressionado ? 1 : 0;
    if (ativo && !pressionado) caixa(alvo, sf::FloatRect(area.left, area.top + 2, area.width, area.height), 6, sf::Color(0, 0, 0, 90));
    caixa(alvo, sf::FloatRect(area.left, area.top + desce, area.width, area.height), 6, fundo, borda, 1);
    escrever(alvo, fontes.negrito, texto, area.left + area.width / 2, area.top + area.height / 2 - 9 + desce, 13,
             ativo ? COR_TEXTO : comAlfa(COR_TEXTO_SUAVE, 110), Alinhamento::Centro, 1.2f);
}

AcaoPainel PainelView::acaoEm(sf::Vector2f ponto) const {
    if (botaoMenos.contains(ponto)) return AcaoPainel::Menos;
    if (botaoMais.contains(ponto)) return AcaoPainel::Mais;
    if (botaoTrocar.contains(ponto)) return AcaoPainel::Trocar;
    if (botaoProxima.contains(ponto)) return AcaoPainel::ProximaFase;
    return AcaoPainel::Nenhuma;
}

// Tela de vitoria: escurece tudo e mostra um cartao central.
void PainelView::desenharVitoria(sf::RenderTarget& alvo, const Jogo& jogo) const {
    caixa(alvo, sf::FloatRect(0, 0, LARGURA_TELA, ALTURA_TELA), 0, comAlfa(COR_FUNDO, 205));
    sf::FloatRect cartao(LARGURA_TELA / 2 - 270, 250, 540, 330);
    sombra(alvo, cartao, 14);
    caixa(alvo, cartao, 14, COR_PAINEL, COR_LATAO, 1.5f);
    float centro = LARGURA_TELA / 2;
    icone(alvo, {centro, 290}, 34, TipoIcone::Vitoria, COR_LATAO);
    escrever(alvo, fontes.titulo, "VITÓRIA", centro, 310, 52, COR_LATAO, Alinhamento::Centro, 6.0f);

    const Jogador* vencedor = jogo.getVencedor();
    if (vencedor == nullptr) {
        escrever(alvo, fontes.medio, "Partida encerrada sem vencedor.", centro, 410, 18, COR_TEXTO, Alinhamento::Centro);
        return;
    }
    sf::Color cor = corDoJogador(vencedor->getCor());
    circulo(alvo, {centro, 404}, 24, escurecer(cor, 0.5f), cor, 2);
    emblema(alvo, {centro, 404}, 14, indiceDoJogador(vencedor->getCor()), clarear(cor, 0.25f));
    escrever(alvo, fontes.negrito, vencedor->getNome(), centro, 436, 26, clarear(cor, 0.3f), Alinhamento::Centro);
    escrever(alvo, fontes.normal, vencedor->getObjetivo()->descricao(), centro, 474, 14, COR_TEXTO_SUAVE, Alinhamento::Centro);
    std::string resumo = "TURNOS " + std::to_string(jogo.getTurno()) + "   ·   TERRITÓRIOS " +
                         std::to_string(vencedor->getQuantidadeTerritorios()) + "   ·   TROPAS " +
                         std::to_string(vencedor->getTotalTropas());
    escrever(alvo, fontes.medio, resumo, centro, 512, 13, COR_TEXTO, Alinhamento::Centro, 1.5f);
    escrever(alvo, fontes.normal, "Feche a janela para sair.", centro, 548, FONTE_PEQUENA, COR_TEXTO_SUAVE, Alinhamento::Centro);
}

// Divide um texto em linhas que cabem na largura informada.
std::vector<std::string> PainelView::quebrarLinhas(const std::string& texto, const sf::Font& fonte, float largura,
                                                   unsigned int tamanho) const {
    std::vector<std::string> linhas;
    std::istringstream palavras(texto);
    std::string palavra, atual;
    while (palavras >> palavra) {
        std::string tentativa = atual.empty() ? palavra : atual + " " + palavra;
        if (!atual.empty() && larguraTexto(fonte, tentativa, tamanho) > largura) {
            linhas.push_back(atual);
            atual = palavra;
        } else {
            atual = tentativa;
        }
    }
    if (!atual.empty()) linhas.push_back(atual);
    return linhas;
}
