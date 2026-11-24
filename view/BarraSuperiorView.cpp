#include "BarraSuperiorView.h"
#include "ControladorJogo.h"
#include "Jogo.h"
#include "Tema.h"
#include <cmath>

BarraSuperiorView::BarraSuperiorView(const Fontes& fontes)
    : fontes(fontes), turnoVisto(-1), tempoFaixa(99), duracaoFaixa(1), relogio(0) {}

void BarraSuperiorView::atualizar(const Jogo& jogo, float segundos) {
    relogio += segundos;
    tempoFaixa += segundos;
    if (jogo.getTurno() != turnoVisto && !jogo.terminou()) {     // a vez mudou: mostra a faixa
        turnoVisto = jogo.getTurno();
        tempoFaixa = 0;
        duracaoFaixa = jogo.getJogadorAtual()->ehAutomatico() ? 0.75f : 1.8f;
    }
}

void BarraSuperiorView::desenhar(sf::RenderTarget& alvo, const Jogo& jogo, const ControladorJogo& controlador) const {
    // fundo com leve degrade e linha cor de latao embaixo
    sf::VertexArray fundo(sf::Quads, 4);
    fundo[0] = sf::Vertex({0, 0}, sf::Color(22, 30, 40));
    fundo[1] = sf::Vertex({LARGURA_TELA, 0}, sf::Color(22, 30, 40));
    fundo[2] = sf::Vertex({LARGURA_TELA, AREA_BARRA.height}, sf::Color(13, 18, 24));
    fundo[3] = sf::Vertex({0, AREA_BARRA.height}, sf::Color(13, 18, 24));
    alvo.draw(fundo);
    linha(alvo, {0, AREA_BARRA.height - 0.5f}, {LARGURA_TELA, AREA_BARRA.height - 0.5f}, 1, comAlfa(COR_LATAO, 110));

    // logotipo: insignia + nome
    circulo(alvo, {36, 32}, 17, COR_PAINEL_ALTO, COR_LATAO, 1.5f);
    emblema(alvo, {36, 32}, 10, 0, COR_LATAO);
    escrever(alvo, fontes.titulo, "WAR", 62, 9, 34, COR_LATAO, Alinhamento::Esquerda, 2.0f);
    escrever(alvo, fontes.medio, "CONQUISTA", 150, 15, FONTE_MINIMA, COR_TEXTO_SUAVE, Alinhamento::Esquerda, 3.0f);
    escrever(alvo, fontes.medio, "GLOBAL", 150, 31, FONTE_MINIMA, COR_TEXTO_SUAVE, Alinhamento::Esquerda, 3.0f);

    // contador de turno
    caixa(alvo, sf::FloatRect(300, 14, 110, 36), 6, COR_PAINEL_ALTO, COR_LINHA, 1);
    escrever(alvo, fontes.medio, "TURNO", 312, 25, FONTE_MINIMA, COR_TEXTO_SUAVE, Alinhamento::Esquerda, 2.0f);
    escrever(alvo, fontes.titulo, std::to_string(jogo.getTurno()), 398, 15, 26, COR_TEXTO, Alinhamento::Direita);

    desenharEtapas(alvo, jogo, controlador);
    desenharJogadores(alvo, jogo);
}

// Etapas do turno: 1 Reforco - 2 Ataque - 3 Movimento (a atual fica acesa).
void BarraSuperiorView::desenharEtapas(sf::RenderTarget& alvo, const Jogo& jogo, const ControladorJogo& controlador) const {
    if (jogo.terminou()) {
        escrever(alvo, fontes.titulo, "PARTIDA ENCERRADA", 440, 20, 20, COR_LATAO, Alinhamento::Esquerda, 2.0f);
        return;
    }
    if (jogo.getJogadorAtual()->ehAutomatico()) {
        int pontos = static_cast<int>(relogio * 3) % 4;
        std::string texto = jogo.getJogadorAtual()->getNome() + " está jogando" + std::string(pontos, '.');
        circulo(alvo, {450, 32}, 5, comAlfa(COR_ALERTA, 150 + 100 * std::sin(relogio * 6)));
        escrever(alvo, fontes.medio, texto, 464, 22, FONTE_TEXTO, COR_TEXTO_SUAVE, Alinhamento::Esquerda, 1.2f);
        return;
    }
    const std::string nomes[3] = {"REFORÇO", "ATAQUE", "MOVIMENTO"};
    const sf::Color cores[3] = {COR_REFORCO, COR_ATAQUE, COR_MOVIMENTO};
    int atual = controlador.getFase() == Fase::Reforco ? 0 : (controlador.getFase() == Fase::Ataque ? 1 : 2);
    for (int i = 0; i < 3; i++) {
        float x = 440 + i * 128;
        sf::Color cor = i == atual ? cores[i] : (i < atual ? comAlfa(cores[i], 150) : COR_LINHA);
        if (i > 0) linha(alvo, {x - 26, 32}, {x - 8, 32}, 2, i <= atual ? comAlfa(cores[i - 1], 160) : COR_LINHA);
        circulo(alvo, {x + 6, 32}, 12, i == atual ? cor : COR_PAINEL_ALTO, cor, 2);
        if (i < atual) {        // etapa concluida: sinal de "ok"
            linha(alvo, {x + 0, 32}, {x + 4.5f, 37}, 2.2f, cor);
            linha(alvo, {x + 4, 37}, {x + 12, 27}, 2.2f, cor);
        } else {
            escrever(alvo, fontes.titulo, std::to_string(i + 1), x + 6, 22, 16, i == atual ? COR_FUNDO : COR_TEXTO_SUAVE, Alinhamento::Centro);
        }
        escrever(alvo, i == atual ? fontes.negrito : fontes.medio, nomes[i], x + 24, 24, FONTE_PEQUENA,
                 i == atual ? COR_TEXTO : COR_TEXTO_SUAVE, Alinhamento::Esquerda, 1.5f);
    }
}

// Um cartao por jogador: emblema, nome, territorios e tropas.
void BarraSuperiorView::desenharJogadores(sf::RenderTarget& alvo, const Jogo& jogo) const {
    int quantidade = jogo.getQuantidadeJogadores();
    float largura = 92, espaco = 6;
    float x = LARGURA_TELA - 16 - quantidade * (largura + espaco) + espaco;
    for (int i = 0; i < quantidade; i++, x += largura + espaco) {
        const Jogador* jogador = jogo.getJogador(i);
        bool daVez = jogador == jogo.getJogadorAtual() && !jogo.terminou();
        float alfa = jogador->estaAtivo() ? 255 : 90;
        sf::Color cor = corDoJogador(jogador->getCor());
        caixa(alvo, sf::FloatRect(x, 11, largura, 42), 6, daVez ? COR_PAINEL_ALTO : comAlfa(COR_PAINEL, 220),
              daVez ? COR_LATAO : COR_LINHA, daVez ? 1.5f : 1);
        caixa(alvo, sf::FloatRect(x + 1, 12, 3, 40), 1.5f, comAlfa(cor, alfa));
        emblema(alvo, {x + 17, 32}, 7, indiceDoJogador(jogador->getCor()), comAlfa(clarear(cor, 0.15f), alfa));

        std::string nome = jogador->getNome();
        while (nome.size() > 3 && larguraTexto(fontes.medio, nome, FONTE_MINIMA) > largura - 34) nome.pop_back();
        escrever(alvo, fontes.medio, nome, x + 29, 15, FONTE_MINIMA, comAlfa(COR_TEXTO_SUAVE, alfa));
        if (jogador->estaAtivo()) {
            escrever(alvo, fontes.titulo, std::to_string(jogador->getQuantidadeTerritorios()), x + 29, 29, 16, COR_TEXTO);
            escrever(alvo, fontes.normal, "·  " + std::to_string(jogador->getTotalTropas()), x + 51, 31, FONTE_PEQUENA,
                     COR_TEXTO_SUAVE);
        } else {
            escrever(alvo, fontes.titulo, "ELIMINADO", x + 29, 31, FONTE_PEQUENA, comAlfa(COR_ERRO, 160), Alinhamento::Esquerda, 1.2f);
        }
        if (daVez) {     // seta indicando quem joga
            sf::ConvexShape marca(3);
            marca.setPoint(0, {x + largura / 2 - 6, 56});
            marca.setPoint(1, {x + largura / 2 + 6, 56});
            marca.setPoint(2, {x + largura / 2, 61});
            marca.setFillColor(COR_LATAO);
            alvo.draw(marca);
        }
    }
}

// Faixa que desce no topo do mapa quando o turno muda (entra, espera e some).
void BarraSuperiorView::desenharFaixaDeTurno(sf::RenderTarget& alvo, const Jogo& jogo) const {
    if (tempoFaixa >= duracaoFaixa || jogo.terminou()) return;
    float entrada = std::min(1.0f, tempoFaixa / 0.2f);
    float saida = std::min(1.0f, (duracaoFaixa - tempoFaixa) / 0.3f);
    float visivel = std::min(entrada, saida);
    const Jogador* jogador = jogo.getJogadorAtual();
    sf::Color cor = corDoJogador(jogador->getCor());

    float largura = 420, altura = 66;
    float x = AREA_MAPA.left + AREA_MAPA.width / 2 - largura / 2;
    float y = AREA_MAPA.top + 18 - 16 * (1 - entrada);
    sf::Uint8 alfa = static_cast<sf::Uint8>(245 * visivel);
    caixa(alvo, sf::FloatRect(x + 2, y + 4, largura, altura), 10, comAlfa(sf::Color::Black, 110 * visivel));
    caixa(alvo, sf::FloatRect(x, y, largura, altura), 10, comAlfa(COR_PAINEL, alfa), comAlfa(COR_LATAO, alfa), 1.5f);
    caixa(alvo, sf::FloatRect(x, y, 7, altura), 3, comAlfa(cor, alfa));
    emblema(alvo, {x + 40, y + altura / 2}, 16, indiceDoJogador(jogador->getCor()), comAlfa(clarear(cor, 0.15f), alfa));
    escrever(alvo, fontes.titulo, "TURNO " + std::to_string(jogo.getTurno()), x + 70, y + 7, 26, comAlfa(COR_LATAO, alfa),
             Alinhamento::Esquerda, 2.0f);
    std::string legenda = jogador->getNome() + (jogador->ehAutomatico() ? "  ·  automático" : "  ·  sua vez");
    escrever(alvo, fontes.medio, legenda, x + 71, y + 39, FONTE_TEXTO, comAlfa(COR_TEXTO, alfa));
}
