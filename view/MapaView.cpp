#include "MapaView.h"
#include "ControladorJogo.h"
#include "Jogo.h"
#include "Tema.h"
#include "Territorio.h"
#include <algorithm>
#include <cmath>

// Tamanho do desenho do mapa ("mundo").
const sf::Vector2f TAMANHO_MUNDO(1110, 820);   // mesma proporcao da area do mapa na tela

// Coordenadas desenhadas numa grade de rascunho; o mundo comeca em (40, 20) dessa grade.
static sf::Vector2f noMundo(float x, float y) { return sf::Vector2f(x - 40, y - 20); }

MapaView::MapaView(const Fontes& fontes, const Mapa& mapa)
    : fontes(fontes), camera(AREA_MAPA, TAMANHO_MUNDO), rotasMaritimas(sf::Quads), selecionadoAntes(nullptr) {
    montarLayout();
    std::vector<std::vector<sf::Vector2f>> formas = {
        {noMundo(63, 187), noMundo(178, 157), noMundo(317, 145), noMundo(398, 172), noMundo(412, 268), noMundo(366, 315),
         noMundo(330, 380), noMundo(270, 408), noMundo(210, 437), noMundo(207, 369), noMundo(147, 336), noMundo(100, 294)},
        {noMundo(269, 461), noMundo(337, 446), noMundo(407, 487), noMundo(406, 568), noMundo(376, 620), noMundo(366, 694),
         noMundo(342, 751), noMundo(299, 711), noMundo(283, 630), noMundo(291, 551)},
        {noMundo(515, 190), noMundo(566, 161), noMundo(655, 170), noMundo(735, 195), noMundo(771, 238), noMundo(732, 303),
         noMundo(726, 390), noMundo(644, 407), noMundo(571, 395), noMundo(525, 317)},
        {noMundo(526, 420), noMundo(624, 409), noMundo(712, 438), noMundo(765, 525), noMundo(743, 606), noMundo(724, 674),
         noMundo(653, 715), noMundo(611, 659), noMundo(560, 626), noMundo(524, 542)},
        {noMundo(767, 184), noMundo(849, 119), noMundo(975, 105), noMundo(1090, 121), noMundo(1130, 170), noMundo(1132, 298),
         noMundo(1101, 398), noMundo(1049, 453), noMundo(1002, 538), noMundo(934, 513), noMundo(900, 443),
         noMundo(822, 462), noMundo(762, 390)},
        {noMundo(963, 588), noMundo(1059, 566), noMundo(1134, 590), noMundo(1140, 656), noMundo(1121, 751),
         noMundo(1028, 747), noMundo(975, 684)}};
    regioes.gerar(TAMANHO_MUNDO, centros, continenteDe, formas);   // feito uma vez so (cache)
    criarRotasMaritimas(mapa);
    camadaMapa.create(static_cast<unsigned int>(TAMANHO_MUNDO.x) * RegioesMapa::ESCALA,
                      static_cast<unsigned int>(TAMANHO_MUNDO.y) * RegioesMapa::ESCALA);
    camadaMapa.setView(sf::View(sf::FloatRect(0, 0, TAMANHO_MUNDO.x, TAMANHO_MUNDO.y)));   // desenha em unidades do mundo
    camadaMapa.setSmooth(true);
}

// Dados de layout: centro de cada territorio (mesma ordem dos ids do Mapa) e continentes.
void MapaView::montarLayout() {
    float pontos[42][2] = {
        {90, 225}, {192, 200}, {358, 167}, {165, 273}, {283, 271}, {379, 256}, {178, 357}, {305, 358}, {253, 448},
        {318, 510}, {302, 598}, {383, 555}, {344, 695},
        {541, 210}, {622, 202}, {744, 222}, {566, 293}, {661, 284}, {571, 365}, {683, 365},
        {540, 451}, {640, 443}, {706, 519}, {555, 550}, {639, 642}, {725, 645},
        {810, 252}, {897, 188}, {968, 138}, {1110, 160}, {969, 253}, {1031, 335},
        {1112, 259}, {830, 388}, {783, 470}, {881, 473}, {946, 408}, {1006, 509},
        {1003, 612}, {1108, 614}, {1027, 712}, {1123, 715}};
    int primeiroDeCadaContinente[7] = {1, 10, 14, 21, 27, 39, 43};
    centros.push_back(sf::Vector2f(0, 0));    // id 0 nao e usado
    continenteDe.push_back(-1);
    for (int id = 1; id <= 42; id++) {
        centros.push_back(noMundo(pontos[id - 1][0], pontos[id - 1][1]));
        int continente = 0;
        while (id >= primeiroDeCadaContinente[continente + 1]) continente++;
        continenteDe.push_back(continente);
    }
    nomesContinentes = {"América do Norte", "América do Sul", "Europa", "África", "Ásia", "Oceania"};
    rotulosContinentes = {"AMÉRICA DO NORTE  +5", "AMÉRICA DO SUL  +2", "EUROPA  +5", "ÁFRICA  +3", "ÁSIA  +7", "OCEANIA  +2"};
    posicoesRotulos = {noMundo(205, 132), noMundo(470, 640), noMundo(640, 150), noMundo(640, 742),
                       noMundo(940, 98), noMundo(890, 700)};
}

// Ligacoes entre territorios que nao se encostam em terra viram rotas tracejadas no mar.
void MapaView::criarRotasMaritimas(const Mapa& mapa) {
    sf::Color cor(170, 196, 212, 120);
    for (int a = 1; a <= mapa.getQuantidadeTerritorios(); a++) {
        for (const Territorio* vizinho : mapa.getTerritorio(a)->getVizinhos()) {
            int b = vizinho->getId();
            if (b < a || regioes.tocamEmTerra(a, b)) continue;
            std::vector<std::pair<sf::Vector2f, sf::Vector2f>> trechos;
            if (std::fabs(centros[a].x - centros[b].x) > TAMANHO_MUNDO.x / 2) {   // Alasca-Kamtchatka: volta ao mundo
                sf::Vector2f esquerda = centros[a].x < centros[b].x ? centros[a] : centros[b];
                sf::Vector2f direita = centros[a].x < centros[b].x ? centros[b] : centros[a];
                trechos.push_back({esquerda, sf::Vector2f(0, esquerda.y - 30)});
                trechos.push_back({direita, sf::Vector2f(TAMANHO_MUNDO.x, direita.y - 30)});
            } else {
                trechos.push_back({centros[a], centros[b]});
            }
            for (const auto& trecho : trechos) {
                sf::Vector2f d = trecho.second - trecho.first;
                float comprimento = std::sqrt(d.x * d.x + d.y * d.y);
                sf::Vector2f direcao = d / comprimento, normal(-direcao.y * 0.8f, direcao.x * 0.8f);
                for (float s = 14; s < comprimento - 10; s += 11) {        // tracos de 6 com espaco de 5
                    sf::Vector2f p1 = trecho.first + direcao * s, p2 = trecho.first + direcao * std::min(s + 6, comprimento);
                    rotasMaritimas.append(sf::Vertex(p1 + normal, cor));
                    rotasMaritimas.append(sf::Vertex(p2 + normal, cor));
                    rotasMaritimas.append(sf::Vertex(p2 - normal, cor));
                    rotasMaritimas.append(sf::Vertex(p1 - normal, cor));
                }
            }
        }
    }
}

// ----------------------------- Atualizacao e entrada -----------------------------

void MapaView::atualizar(const Jogo& jogo, const ControladorJogo& controlador, sf::Vector2f mouse, float segundos) {
    camera.atualizar(segundos);
    animacoes.observar(jogo, centros);
    animacoes.avancar(segundos);
    // Estado visual de cada territorio: calculado uma vez por quadro e reutilizado no desenho.
    estados.assign(centros.size(), EstadoVisual::Normal);
    for (int id = 1; id < static_cast<int>(centros.size()); id++)
        estados[id] = estadoDe(jogo, controlador, jogo.getMapa().getTerritorio(id));
    const Territorio* selecionado = controlador.getSelecionado();
    if (selecionado != nullptr && selecionado != selecionadoAntes)
        animacoes.criarOnda(centros[selecionado->getId()], COR_LATAO, 38, 0.45f);   // onda ao selecionar
    selecionadoAntes = selecionado;
    redesenharCamadaSeMudou(jogo, controlador, territorioEm(mouse));
}

// Cor de preenchimento de um territorio conforme dono e estado visual.
sf::Color MapaView::corDoTerritorio(const Jogo& jogo, const ControladorJogo& controlador, int id, int idMouse) const {
    EstadoVisual estado = estados[id];
    sf::Color cor = corDoJogador(jogo.getMapa().getTerritorio(id)->getDono()->getCor());
    if (id % 2 == 0) cor = escurecer(cor, 0.07f);                       // vizinhos do mesmo dono se distinguem
    if (estado == EstadoVisual::Proibido) cor = misturar(cor, sf::Color(26, 32, 40), controlador.getSelecionado() ? 0.62f : 0.35f);
    if (estado == EstadoVisual::Selecionado) cor = clarear(cor, 0.24f);
    if (id == idMouse && estado != EstadoVisual::Proibido) cor = clarear(cor, 0.16f);   // hover
    return misturar(cor, COR_CONQUISTA, 0.6f * animacoes.brilhoDe(id));                  // brilho de conquista
}

// CACHE: fundo + territorios coloridos + fronteiras ficam prontos numa unica textura.
// So redesenha quando alguma cor muda (dono, hover, selecao, brilho); nos outros
// quadros o mapa inteiro e um unico sprite.
void MapaView::redesenharCamadaSeMudou(const Jogo& jogo, const ControladorJogo& controlador, int idMouse) {
    // em vez de desenhar o mapa inteiro todo quadro, guarda pronto numa textura
    // e só refaz quando alguma cor muda
    // (alguém conquistou, passou o mouse, selecionou...). na maioria dos quadros sai daqui
    bool mudou = coresDesenhadas.size() != centros.size();
    std::vector<sf::Color> cores(centros.size(), sf::Color::Transparent);
    for (int id = 1; id < static_cast<int>(centros.size()); id++) {
        cores[id] = corDoTerritorio(jogo, controlador, id, idMouse);
        if (!mudou && cores[id] != coresDesenhadas[id]) mudou = true;
    }
    if (!mudou) return;
    camadaMapa.clear(COR_FUNDO);
    regioes.desenharFundo(camadaMapa);
    camadaMapa.draw(rotasMaritimas);
    for (int id = 1; id < static_cast<int>(centros.size()); id++) regioes.desenharTerritorio(camadaMapa, id, cores[id]);
    regioes.desenharFronteiras(camadaMapa);
    camadaMapa.display();
    camadaMapa.generateMipmap();   // versoes menores prontas: desenhar reduzido fica mais rapido e mais nitido
    coresDesenhadas = cores;
}


int MapaView::territorioEm(sf::Vector2f pontoTela) const {
    if (!camera.contem(pontoTela) || botaoZoomEm(pontoTela) != 0) return 0;
    return regioes.territorioEm(camera.paraMundo(pontoTela));
}

bool MapaView::contem(sf::Vector2f pontoTela) const { return camera.contem(pontoTela); }

int MapaView::botaoZoomEm(sf::Vector2f pontoTela) const {
    for (int i = 0; i < 3; i++) {
        sf::Vector2f centro(AREA_MAPA.left + 34 + i * 40, AREA_MAPA.top + AREA_MAPA.height - 64);
        sf::Vector2f d = pontoTela - centro;
        if (d.x * d.x + d.y * d.y <= 16 * 16) return i + 1;
    }
    return 0;
}

void MapaView::aproximar(float passos, sf::Vector2f pontoTela) { camera.aproximar(passos, pontoTela); }

void MapaView::arrastar(sf::Vector2f deslocamentoTela) { camera.arrastar(deslocamentoTela); }

void MapaView::centralizar() { camera.centralizar(); }

// ----------------------------- Estados visuais -----------------------------

// Decide como o territorio aparece; as condicoes de jogo vem do model (podeAtacar / podeMover).
EstadoVisual MapaView::estadoDe(const Jogo& jogo, const ControladorJogo& controlador, const Territorio* territorio) const {
    const Jogador* atual = jogo.getJogadorAtual();
    if (jogo.terminou() || atual->ehAutomatico()) return EstadoVisual::Normal;
    const Territorio* selecionado = controlador.getSelecionado();
    Fase fase = controlador.getFase();
    bool meu = atual->possui(territorio);
    if (territorio == selecionado) return EstadoVisual::Selecionado;

    // a view não decide regra nenhuma: quem diz se pode atacar/mover é o Jogador
    // (podeAtacar / podeMover). aqui só escolhe como pintar
    if (selecionado != nullptr && fase == Fase::Ataque)
        return atual->podeAtacar(selecionado, territorio) ? EstadoVisual::Possivel : (meu ? EstadoVisual::Normal : EstadoVisual::Proibido);
    if (selecionado != nullptr && fase == Fase::Movimento && atual->possui(selecionado))
        return atual->podeMover(selecionado, territorio) ? EstadoVisual::Possivel : EstadoVisual::Proibido;
    if (fase == Fase::Reforco) return meu ? EstadoVisual::Possivel : EstadoVisual::Proibido;

    // Sem origem escolhida: destaca de onde da para agir.
    if (meu) {
        for (const Territorio* vizinho : territorio->getVizinhos()) {
            if (fase == Fase::Ataque && atual->podeAtacar(territorio, vizinho)) return EstadoVisual::Possivel;
            if (fase == Fase::Movimento && atual->podeMover(territorio, vizinho)) return EstadoVisual::Possivel;
        }
    }
    return EstadoVisual::Normal;
}

sf::Color MapaView::corDaAcao(const ControladorJogo& controlador) const {
    if (controlador.getFase() == Fase::Ataque) return COR_ATAQUE;
    if (controlador.getFase() == Fase::Movimento) return COR_MOVIMENTO;
    return COR_REFORCO;
}

// ----------------------------- Desenho -----------------------------

void MapaView::desenhar(sf::RenderTarget& alvo, const Jogo& jogo, const ControladorJogo& controlador, sf::Vector2f mouse) const {
    int idMouse = territorioEm(mouse);

    // 1) Coisas presas ao mapa: desenhadas com a camera (acompanham zoom e pan).
    sf::View visaoDaTela = alvo.getView();
    alvo.setView(camera.visao());
    sf::Sprite camada(camadaMapa.getTexture());              // mapa ja pronto (cache)
    camada.setScale(1.0f / RegioesMapa::ESCALA, 1.0f / RegioesMapa::ESCALA);
    alvo.draw(camada);
    desenharContornos(alvo, controlador, idMouse);
    animacoes.desenharNoMundo(alvo);
    alvo.setView(visaoDaTela);

    // 2) Coisas de interface sobre o mapa: tamanho fixo na tela (texto sempre nitido).
    desenharRotulos(alvo);
    desenharSeta(alvo, jogo, controlador, idMouse);
    desenharMarcadores(alvo, jogo, controlador, idMouse);
    animacoes.desenharNaTela(alvo, camera, fontes);
    desenharMoldura(alvo, mouse);
    desenharDica(alvo, jogo, controlador, idMouse, mouse);
}

// Contornos animados (pulsam), desenhados por cima da camada em cache.
void MapaView::desenharContornos(sf::RenderTarget& alvo, const ControladorJogo& controlador, int idMouse) const {
    float pulso = 0.5f + 0.5f * std::sin(animacoes.getRelogio() * 5.0f);
    sf::Color acao = corDaAcao(controlador);
    for (int id = 1; id < static_cast<int>(centros.size()); id++) {
        EstadoVisual estado = estados[id];
        if (estado == EstadoVisual::Selecionado) regioes.desenharContorno(alvo, id, comAlfa(COR_LATAO, 170 + 85 * pulso));
        else if (estado == EstadoVisual::Possivel && controlador.getFase() != Fase::Reforco)
            regioes.desenharContorno(alvo, id, comAlfa(acao, controlador.getSelecionado() ? 150 + 105 * pulso : 110));
        if (id == idMouse) {
            sf::Color corHover = estado == EstadoVisual::Proibido ? COR_ERRO
                               : (estado == EstadoVisual::Possivel ? clarear(acao, 0.3f) : COR_TEXTO);
            regioes.desenharContorno(alvo, id, comAlfa(corHover, 230));
        }
        if (animacoes.brilhoDe(id) > 0) regioes.desenharContorno(alvo, id, comAlfa(COR_CONQUISTA, 255 * animacoes.brilhoDe(id)));
    }
}

// Seta da origem selecionada ate o alvo sob o mouse (so quando a acao e possivel).
void MapaView::desenharSeta(sf::RenderTarget& alvo, const Jogo& jogo, const ControladorJogo& controlador, int idMouse) const {
    const Territorio* origem = controlador.getSelecionado();
    if (origem == nullptr || idMouse == 0) return;
    const Territorio* destino = jogo.getMapa().getTerritorio(idMouse);
    if (estados[destino->getId()] != EstadoVisual::Possivel) return;
    sf::Vector2f a = camera.paraTela(centros[origem->getId()]);
    sf::Vector2f b = camera.paraTela(centros[idMouse]);
    sf::Vector2f d = b - a;
    float comprimento = std::sqrt(d.x * d.x + d.y * d.y);
    if (comprimento < 40) return;
    sf::Vector2f direcao = d / comprimento;
    a += direcao * 20.0f;
    b -= direcao * 22.0f;
    seta(alvo, a + sf::Vector2f(2, 3), b + sf::Vector2f(2, 3), 5, sf::Color(0, 0, 0, 110));   // sombra
    seta(alvo, a, b, 5, clarear(corDaAcao(controlador), 0.1f));
}

void MapaView::desenharRotulos(sf::RenderTarget& alvo) const {
    for (size_t c = 0; c < rotulosContinentes.size(); c++) {
        sf::Vector2f p = camera.paraTela(posicoesRotulos[c]);
        float meiaLargura = larguraTexto(fontes.titulo, rotulosContinentes[c], 15, 2.2f) / 2;
        if (p.x - meiaLargura < AREA_MAPA.left + 8 || p.x + meiaLargura > AREA_MAPA.left + AREA_MAPA.width - 8 ||
            p.y < AREA_MAPA.top + 6 || p.y > AREA_MAPA.top + AREA_MAPA.height - 26) continue;
        escrever(alvo, fontes.titulo, rotulosContinentes[c], p.x + 1, p.y + 1, 15, sf::Color(0, 0, 0, 120), Alinhamento::Centro, 2.2f);
        escrever(alvo, fontes.titulo, rotulosContinentes[c], p.x, p.y, 15, comAlfa(COR_TEXTO, 150), Alinhamento::Centro, 2.2f);
    }
}

// Marcador de tropas: emblema do dono + numero, sempre do mesmo tamanho na tela.
void MapaView::desenharMarcadores(sf::RenderTarget& alvo, const Jogo& jogo, const ControladorJogo& controlador,
                                  int idMouse) const {
    const Mapa& mapa = jogo.getMapa();
    bool mostrarNomes = camera.getZoomRelativo() > 1.35f;
    for (int id = 1; id <= mapa.getQuantidadeTerritorios(); id++) {
        const Territorio* territorio = mapa.getTerritorio(id);
        sf::Vector2f p = camera.paraTela(centros[id]);
        sf::FloatRect areaSegura(AREA_MAPA.left + 26, AREA_MAPA.top + 16, AREA_MAPA.width - 52, AREA_MAPA.height - 34);
        if (!areaSegura.contains(p)) continue;
        const Jogador* dono = territorio->getDono();
        sf::Color cor = corDoJogador(dono->getCor());
        EstadoVisual estado = estados[id];
        std::string numero = std::to_string(territorio->getTropas());
        float largura = 30 + 9.0f * numero.size();
        sf::FloatRect pilula(p.x - largura / 2, p.y - 12, largura, 24);

        sf::Color borda = clarear(cor, 0.35f);
        if (estado == EstadoVisual::Selecionado) borda = COR_LATAO;
        else if (id == idMouse) borda = COR_TEXTO;
        float alfa = estado == EstadoVisual::Proibido && controlador.getSelecionado() ? 150 : 245;

        caixa(alvo, pilula, 12, comAlfa(escurecer(cor, 0.62f), alfa), comAlfa(borda, alfa), 2.0f);
        emblema(alvo, sf::Vector2f(pilula.left + 13, p.y), 6.5f, indiceDoJogador(dono->getCor()), clarear(cor, 0.25f));
        escrever(alvo, fontes.titulo, numero, pilula.left + 23, p.y - 10, 17, comAlfa(COR_TEXTO, alfa));

        if (mostrarNomes) {
            std::string nome = territorio->getNome();
            escrever(alvo, fontes.medio, nome, p.x + 1, p.y + 15, FONTE_MINIMA, sf::Color(0, 0, 0, 170), Alinhamento::Centro, 1.1f);
            escrever(alvo, fontes.medio, nome, p.x, p.y + 14, FONTE_MINIMA, comAlfa(COR_TEXTO, 220), Alinhamento::Centro, 1.1f);
        }
    }
}

// Moldura com sombra interna, controles de zoom e dica de navegacao.
void MapaView::desenharMoldura(sf::RenderTarget& alvo, sf::Vector2f mouse) const {
    sf::VertexArray vinheta(sf::Quads);
    sf::Color escuro(0, 0, 0, 140), nada(0, 0, 0, 0);
    float x = AREA_MAPA.left, y = AREA_MAPA.top, l = AREA_MAPA.width, a = AREA_MAPA.height, f = 26;
    sf::Vertex faixas[16] = {
        {{x, y}, escuro}, {{x + l, y}, escuro}, {{x + l, y + f}, nada}, {{x, y + f}, nada},                       // topo
        {{x, y + a - f}, nada}, {{x + l, y + a - f}, nada}, {{x + l, y + a}, escuro}, {{x, y + a}, escuro},       // base
        {{x, y}, escuro}, {{x + f, y}, nada}, {{x + f, y + a}, nada}, {{x, y + a}, escuro},                       // esquerda
        {{x + l - f, y}, nada}, {{x + l, y}, escuro}, {{x + l, y + a}, escuro}, {{x + l - f, y + a}, nada}};      // direita
    for (const sf::Vertex& v : faixas) vinheta.append(v);
    alvo.draw(vinheta);
    caixa(alvo, AREA_MAPA, 10, sf::Color::Transparent, COR_LINHA, 1.5f);

    // botoes de zoom: aproximar, afastar, centralizar
    for (int i = 0; i < 3; i++) {
        sf::Vector2f c(AREA_MAPA.left + 34 + i * 40, AREA_MAPA.top + AREA_MAPA.height - 64);
        bool emCima = botaoZoomEm(mouse) == i + 1;
        circulo(alvo, c + sf::Vector2f(0, 2), 16, sf::Color(0, 0, 0, 100));
        circulo(alvo, c, 16, emCima ? COR_PAINEL_ALTO : comAlfa(COR_PAINEL, 235), emCima ? COR_LATAO : COR_LINHA, 1.5f);
        sf::Color simbolo = emCima ? COR_LATAO : COR_TEXTO;
        if (i < 2) linha(alvo, c - sf::Vector2f(6, 0), c + sf::Vector2f(6, 0), 2, simbolo);
        if (i == 0) linha(alvo, c - sf::Vector2f(0, 6), c + sf::Vector2f(0, 6), 2, simbolo);
        if (i == 2) {
            circulo(alvo, c, 5.5f, sf::Color::Transparent, simbolo, 1.6f);
            circulo(alvo, c, 1.8f, simbolo);
        }
    }
    std::string dica = "Roda: zoom  ·  Arrastar: mover o mapa  ·  R: centralizar";
    float largura = larguraTexto(fontes.normal, dica, FONTE_MINIMA) + 20;
    caixa(alvo, sf::FloatRect(AREA_MAPA.left + 12, AREA_MAPA.top + AREA_MAPA.height - 34, largura, 22), 11, comAlfa(COR_PAINEL, 210));
    escrever(alvo, fontes.normal, dica, AREA_MAPA.left + 22, AREA_MAPA.top + AREA_MAPA.height - 30, FONTE_MINIMA, COR_TEXTO_SUAVE);
}

// Cartao com informacoes do territorio sob o mouse e o que acontece ao clicar.
void MapaView::desenharDica(sf::RenderTarget& alvo, const Jogo& jogo, const ControladorJogo& controlador, int idMouse,
                            sf::Vector2f mouse) const {
    if (idMouse == 0) return;
    const Territorio* territorio = jogo.getMapa().getTerritorio(idMouse);
    const Jogador* dono = territorio->getDono();
    const Jogador* atual = jogo.getJogadorAtual();
    EstadoVisual estado = estados[idMouse];
    bool humano = !jogo.terminou() && !atual->ehAutomatico();
    Fase fase = controlador.getFase();
    const Territorio* origem = controlador.getSelecionado();

    std::string acao;
    sf::Color corAcao = COR_TEXTO_SUAVE;
    if (humano) {
        if (fase == Fase::Reforco && atual->possui(territorio)) {
            acao = "Clique: +" + std::to_string(controlador.getQuantidade()) + "  ·  Botão direito: todos";
            corAcao = COR_REFORCO;
        } else if (fase == Fase::Reforco) {
            acao = "× Só é possível reforçar seus territórios";
            corAcao = COR_ERRO;
        } else if (estado == EstadoVisual::Possivel && origem != nullptr && fase == Fase::Ataque) {
            acao = "Clique para ATACAR com " + std::to_string(std::min(3, origem->getTropas() - 1)) + " dado(s)";
            corAcao = COR_ATAQUE;
        } else if (estado == EstadoVisual::Possivel && origem != nullptr) {
            acao = "Clique para MOVER " + std::to_string(controlador.getQuantidade()) + " tropa(s)";
            corAcao = COR_MOVIMENTO;
        } else if (estado == EstadoVisual::Proibido) {
            acao = "× Ação proibida: fora de alcance";
            corAcao = COR_ERRO;
        } else if (atual->possui(territorio) && estado != EstadoVisual::Selecionado) {
            acao = "Clique para escolher como origem";
            corAcao = COR_LATAO;
        }
    }

    std::string titulo = territorio->getNome();
    std::string linha2 = dono->getNome() + (dono == atual ? "  (você)" : "");
    std::string linha3 = std::to_string(territorio->getTropas()) + " tropas  ·  " + nomesContinentes[continenteDe[idMouse]];
    float largura = std::max(std::max(larguraTexto(fontes.negrito, titulo, 16), larguraTexto(fontes.normal, linha3, FONTE_PEQUENA)),
                             larguraTexto(fontes.medio, acao, FONTE_PEQUENA)) + 44;
    largura = std::max(largura, 200.0f);
    float altura = acao.empty() ? 74 : 98;
    sf::Vector2f p = mouse + sf::Vector2f(18, 18);
    if (p.x + largura > AREA_MAPA.left + AREA_MAPA.width - 8) p.x = mouse.x - largura - 18;
    if (p.y + altura > AREA_MAPA.top + AREA_MAPA.height - 8) p.y = mouse.y - altura - 18;

    sf::Color corDono = corDoJogador(dono->getCor());
    sombra(alvo, sf::FloatRect(p.x, p.y, largura, altura), 8);
    caixa(alvo, sf::FloatRect(p.x, p.y, largura, altura), 8, comAlfa(COR_PAINEL, 245), COR_LINHA, 1);
    caixa(alvo, sf::FloatRect(p.x, p.y, 4, altura), 2, corDono);
    escrever(alvo, fontes.negrito, titulo, p.x + 16, p.y + 9, 16, COR_TEXTO);
    emblema(alvo, sf::Vector2f(p.x + 22, p.y + 41), 6, indiceDoJogador(dono->getCor()), clarear(corDono, 0.2f));
    escrever(alvo, fontes.medio, linha2, p.x + 34, p.y + 33, FONTE_PEQUENA, clarear(corDono, 0.35f));
    escrever(alvo, fontes.normal, linha3, p.x + 16, p.y + 52, FONTE_PEQUENA, COR_TEXTO_SUAVE);
    if (!acao.empty()) {
        linha(alvo, sf::Vector2f(p.x + 14, p.y + 74), sf::Vector2f(p.x + largura - 14, p.y + 74), 1, COR_LINHA);
        escrever(alvo, fontes.medio, acao, p.x + 16, p.y + 78, FONTE_PEQUENA, corAcao);
    }
}
