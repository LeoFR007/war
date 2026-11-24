#include "EventosView.h"
#include "Jogo.h"
#include "Tema.h"

EventosView::EventosView(const Fontes& fontes) : fontes(fontes) {}

// Classifica o texto do historico (gerado pelo model) so para escolher icone e cor.
TipoIcone EventosView::tipoDoEvento(const std::string& texto) const {
    if (texto.find("VENCEU") != std::string::npos) return TipoIcone::Vitoria;
    if (texto.find("eliminado") != std::string::npos) return TipoIcone::Eliminacao;
    if (texto.find("conquistou") != std::string::npos) return TipoIcone::Conquista;
    if (texto.find("atacou") != std::string::npos) return TipoIcone::Ataque;
    if (texto.find("moveu") != std::string::npos) return TipoIcone::Movimento;
    if (texto.find("trocou") != std::string::npos) return TipoIcone::Cartas;
    if (texto.rfind("Turno ", 0) == 0) return TipoIcone::Turno;
    return TipoIcone::Reforco;
}

void EventosView::desenhar(sf::RenderTarget& alvo, const Jogo& jogo) const {
    const sf::FloatRect& area = AREA_EVENTOS;
    sombra(alvo, area, 10);
    caixa(alvo, area, 10, COR_PAINEL, COR_LINHA, 1);
    const std::vector<std::string>& historico = jogo.getHistorico();
    float x = area.left + 16, largura = area.width - 32;
    escrever(alvo, fontes.medio, "EVENTOS DA PARTIDA", x, area.top + 13, FONTE_MINIMA, COR_TEXTO_SUAVE, Alinhamento::Esquerda, 3.0f);
    escrever(alvo, fontes.medio, std::to_string(historico.size()), x + largura, area.top + 12, FONTE_PEQUENA, COR_TEXTO_SUAVE,
             Alinhamento::Direita);
    linha(alvo, {x, area.top + 36}, {x + largura, area.top + 36}, 1, COR_LINHA);

    float y = area.top + 44;
    int mostrados = 0;
    for (int i = static_cast<int>(historico.size()) - 1; i >= 0 && y + 24 <= area.top + area.height - 6; i--, mostrados++) {
        const std::string& texto = historico[i];
        TipoIcone tipo = tipoDoEvento(texto);
        float alfa = mostrados == 0 ? 255 : std::max(120.0f, 245.0f - mostrados * 12);   // mais antigos ficam mais apagados

        if (tipo == TipoIcone::Turno) {          // separador entre turnos
            std::string rotulo = texto.substr(0, texto.find(':'));
            for (char& c : rotulo) c = static_cast<char>(toupper(c));
            std::string resto = texto.substr(texto.find(':') + 2);
            linha(alvo, {x, y + 11}, {x + largura, y + 11}, 1, comAlfa(COR_LINHA, alfa));
            std::string etiqueta = rotulo + "  ·  " + resto;
            float larguraEtiqueta = larguraTexto(fontes.medio, etiqueta, 10, 1.2f) + 16;
            caixa(alvo, sf::FloatRect(x + largura / 2 - larguraEtiqueta / 2, y + 3, larguraEtiqueta, 17), 8, COR_PAINEL_ALTO);
            escrever(alvo, fontes.medio, etiqueta, x + largura / 2, y + 4, 10, comAlfa(COR_TEXTO_SUAVE, alfa), Alinhamento::Centro, 1.2f);
            y += 24;
            continue;
        }

        sf::Color corTipo = COR_TEXTO_SUAVE;
        if (tipo == TipoIcone::Ataque) corTipo = COR_ATAQUE;
        else if (tipo == TipoIcone::Conquista) corTipo = COR_CONQUISTA;
        else if (tipo == TipoIcone::Movimento) corTipo = COR_MOVIMENTO;
        else if (tipo == TipoIcone::Cartas || tipo == TipoIcone::Vitoria) corTipo = COR_LATAO;
        else if (tipo == TipoIcone::Eliminacao) corTipo = COR_ERRO;

        // barra com a cor de quem fez a acao (o texto comeca com o nome do jogador)
        for (int j = 0; j < jogo.getQuantidadeJogadores(); j++) {
            const Jogador* jogador = jogo.getJogador(j);
            if (texto.rfind(jogador->getNome() + " ", 0) == 0)
                caixa(alvo, sf::FloatRect(x, y + 2, 3, 20), 1.5f, comAlfa(corDoJogador(jogador->getCor()), alfa));
        }
        if (mostrados == 0) caixa(alvo, sf::FloatRect(x + 6, y, largura - 6, 24), 5, COR_PAINEL_ALTO);
        icone(alvo, {x + 20, y + 12}, 11, tipo, comAlfa(corTipo, alfa));
        std::string curto = texto;
        while (curto.size() > 4 && larguraTexto(fontes.normal, curto, FONTE_PEQUENA) > largura - 40) curto = curto.substr(0, curto.size() - 4) + "...";
        escrever(alvo, fontes.normal, curto, x + 34, y + 4, FONTE_PEQUENA, comAlfa(COR_TEXTO, alfa));
        y += 24;
    }
    if (historico.empty())
        escrever(alvo, fontes.normal, "A partida acabou de começar.", x, y + 4, FONTE_PEQUENA, COR_TEXTO_SUAVE);
}
