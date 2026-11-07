#include "Jogo.h"
#include "Objetivo.h"
#include "Exceptions.h"
#include "raylib.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <exception>
#include <initializer_list>
#include <sstream>
#include <memory>
#include <string>
#include <vector>

// Interface 2D: somente invoca a API publica de Jogo. A logica do WAR
// permanece em war_core, compartilhada com o jogo de terminal.
namespace {
constexpr int LARGURA = 1580;
constexpr int ALTURA = 920;
constexpr float MAP_X = 32.f;
constexpr float MAP_Y = 143.f;
constexpr float MAP_W = 1110.f;
constexpr float MAP_H = 655.f;
constexpr float SIDE_X = 1165.f;
constexpr float SIDE_W = 382.f;

const Color FUNDO      = {12, 22, 34, 255};
const Color PAINEL     = {22, 36, 53, 255};
const Color PAINEL2    = {29, 46, 64, 255};
const Color PAINEL3    = {37, 58, 77, 255};
const Color TINTA      = {236, 240, 233, 255};
const Color MUTED      = {159, 179, 191, 255};
const Color DOURADO    = {236, 188, 99, 255};
const Color VERDE      = {76, 202, 166, 255};
const Color VERMELHO   = {249, 118, 111, 255};
const Color BORDA      = {63, 89, 105, 255};
bool entradaHabilitada = true;
Vector2 mouseVirtual = {0,0};

struct Pos { float x, y; };
// Coordenadas de uma representacao esquematica do tabuleiro classico.
// Ordem identica a Mapa::criarClassico(), por isso os IDs sao estaveis.
const std::array<Pos, 42> PONTOS {{
    {90,225}, {192,200}, {358,167}, {165,273}, {283,271}, {379,256},
    {178,357}, {305,358}, {253,448},
    {318,510}, {302,598}, {383,555}, {344,695},
    {541,210}, {622,202}, {744,222}, {566,293}, {661,284}, {571,365}, {683,365},
    {540,451}, {640,443}, {706,519}, {555,550}, {639,642}, {725,645},
    {810,252}, {897,188}, {968,138}, {1110,160}, {969,253}, {1031,335},
    {1112,259}, {830,388}, {783,470}, {881,473}, {946,408}, {1006,509},
    {1003,612}, {1108,614}, {1027,712}, {1123,715}
}};
const std::array<const char*,42> CURTOS {{
    "ALASKA", "NW", "GRO", "ALB", "ONT", "QUE", "US-W", "US-E", "CENT",
    "VEN", "PER", "BRA", "ARG",
    "ISL", "SCAN", "UKR", "UK", "N-EUR", "W-EUR", "S-EUR",
    "N-AFR", "EGY", "E-AFR", "CON", "S-AFR", "MAD",
    "URAL", "SIB", "YAK", "KAM", "IRK", "MON", "JPN", "AFG", "M-EAST", "IND", "CHN", "SIAM",
    "INDO", "GUIN", "AUS-W", "AUS-E"
}};

Color comAlpha(Color c, unsigned char a) { c.a = a; return c; }
Color corJogador(CorJogador cor) {
    switch (cor) {
        case CorJogador::Vermelho: return {225, 89, 90, 255};
        case CorJogador::Azul: return {93, 161, 237, 255};
        case CorJogador::Verde: return {77, 190, 148, 255};
        case CorJogador::Amarelo: return {236, 197, 91, 255};
        case CorJogador::Roxo: return {178, 126, 228, 255};
        case CorJogador::Preto: return {142, 158, 177, 255};
    }
    return MUTED;
}
const char* nomeFase(FaseTurno fase) {
    switch (fase) {
        case FaseTurno::Reforco: return "REFORCOS";
        case FaseTurno::Ataque: return "ATAQUES";
        case FaseTurno::Movimentacao: return "MOVIMENTACAO";
        case FaseTurno::Fim: return "FIM DO TURNO";
    }
    return "";
}
void texto(const std::string& s, float x, float y, float tamanho = 19, Color cor = TINTA) {
    DrawTextEx(GetFontDefault(), s.c_str(), {x,y}, tamanho, 1.f, cor);
}
float largura(const std::string& s, float tamanho) {
    return MeasureTextEx(GetFontDefault(), s.c_str(), tamanho, 1.f).x;
}
void centro(const std::string& s, float x, float y, float tamanho=19, Color cor=TINTA) {
    texto(s, x - largura(s, tamanho)/2.f, y, tamanho, cor);
}
void painel(Rectangle rec, Color fundo = PAINEL, Color borda = BORDA, float round = .08f) {
    DrawRectangleRounded(rec, round, 9, fundo);
    DrawRectangleRoundedLinesEx(rec, round, 9, 1.f, borda);
}
bool botao(Rectangle r, const std::string& titulo, bool ativo=true, Color cor = PAINEL3,
           float fonte = 19.f) {
    Vector2 mouse = mouseVirtual;
    bool hover = ativo && entradaHabilitada && CheckCollisionPointRec(mouse, r);
    Color fill = !ativo ? comAlpha(cor, 90) : hover ? ColorBrightness(cor, .14f) : cor;
    DrawRectangleRounded(r, .18f, 8, fill);
    DrawRectangleRoundedLinesEx(r, .18f, 8, 1.5f, hover ? DOURADO : comAlpha(BORDA,175));
    centro(titulo, r.x+r.width*.5f, r.y+(r.height-fonte)*.5f - 1, fonte, ativo?TINTA:comAlpha(MUTED,140));
    return hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}
void etiqueta(const std::string& titulo, float x, float y, Color c=MUTED) {
    texto(titulo, x, y, 16.f, c);
}
void poligono(std::initializer_list<Vector2> vertices, Color cor) {
    std::vector<Vector2> ps(vertices);
    if (ps.size()<3) return;
    DrawTriangleFan(ps.data(), static_cast<int>(ps.size()), cor);
    for (std::size_t i=0;i<ps.size();++i)
        DrawLineEx(ps[i], ps[(i+1)%ps.size()], 2.f, comAlpha(BORDA, 65));
}
void continenteFundo() {
    // Desenho artistico esquematico, nao projecao geografica nem mapa oficial.
    poligono({{63,187},{178,157},{317,145},{398,172},{412,268},{366,315},
              {330,380},{270,408},{210,437},{207,369},{147,336},{100,294}}, {48,75,90,255});
    poligono({{269,461},{337,446},{407,487},{406,568},{376,620},{366,694},
              {342,751},{299,711},{283,630},{291,551}}, {43,86,79,255});
    poligono({{515,190},{566,161},{655,170},{735,195},{771,238},{732,303},
              {726,390},{644,407},{571,395},{525,317}}, {67,79,97,255});
    poligono({{526,420},{624,409},{712,438},{765,525},{743,606},{724,674},
              {653,715},{611,659},{560,626},{524,542}}, {109,79,72,255});
    poligono({{767,184},{849,119},{975,105},{1090,121},{1130,170},{1132,298},
              {1101,398},{1049,453},{1002,538},{934,513},{900,443},
              {822,462},{762,390}}, {90,84,71,255});
    poligono({{963,588},{1059,566},{1134,590},{1140,656},{1121,751},{1028,747},
              {975,684}}, {70,80,105,255});
    // Grade sutil para leitura espacial.
    for (int x=73;x<=1120;x+=74) DrawLine(x,150,x,780,comAlpha(MUTED,17));
    for (int y=174;y<=775;y+=65) DrawLine(50,y,1140,y,comAlpha(MUTED,17));
    etiqueta("AMERICA DO NORTE", 96,169,comAlpha(TINTA,110));
    etiqueta("AMERICA DO SUL", 279,476,comAlpha(TINTA,110));
    etiqueta("EUROPA", 572,186,comAlpha(TINTA,110));
    etiqueta("AFRICA", 552,428,comAlpha(TINTA,110));
    etiqueta("ASIA", 844,160,comAlpha(TINTA,110));
    etiqueta("OCEANIA", 990,568,comAlpha(TINTA,110));
}
void planoFundo() {
    ClearBackground(FUNDO);
    DrawRectangleGradientV(0,0,LARGURA,ALTURA,{17,30,46,255},FUNDO);
    DrawCircleGradient({750.0f,270.0f},670.0f,comAlpha({32,70,89,255},65),comAlpha(FUNDO,0));
    DrawLine(0,110,LARGURA,110,BORDA);
    texto("WAR",36,25,53,DOURADO);
    texto("CONQUISTA GLOBAL",171,41,24,TINTA);
    etiqueta("JOGO DE ESTRATEGIA  |  C++17 + raylib", 172,71);
    etiqueta("PUCRS / POO", 1390,43);
    painel({MAP_X,MAP_Y,MAP_W,MAP_H}, {23,44,61,255});
    BeginScissorMode(static_cast<int>(MAP_X+2),static_cast<int>(MAP_Y+2),
                     static_cast<int>(MAP_W-4),static_cast<int>(MAP_H-4));
    continenteFundo();
    EndScissorMode();
    painel({SIDE_X,MAP_Y,SIDE_W,MAP_H});
    painel({MAP_X,817,1515,73}, PAINEL, BORDA, .09f);
}

struct Ui {
    int quantidadeJogadores = 3;
    std::unique_ptr<Jogo> jogo;
    int origem = -1;
    int destino = -1;
    int quantidade = 1;
    int dados = 3;
    int turno = 1;
    std::string mensagem = "Clique num territorio para selecionar a origem.";
    Color corMensagem = VERDE;
    double expira = 0.0;
    RelatorioCombate ultimoCombate;
    bool temCombate = false;
    bool mostrarObjetivo = false;
    void notificar(const std::string& msg, Color cor=VERDE) {
        mensagem = msg; corMensagem = cor; expira = GetTime() + 7.0;
    }
    void iniciar() {
        std::vector<std::string> nomes;
        for (int i=0;i<quantidadeJogadores;++i) nomes.push_back("Jogador " + std::to_string(i+1));
        jogo = std::make_unique<Jogo>(std::move(nomes));
        jogo->iniciarTurnoAtual();
        origem=-1; destino=-1; quantidade=1; dados=3; turno=1; temCombate=false; mostrarObjetivo=false;
        notificar("Partida iniciada. Escolha o territorio de reforco.");
    }
    void avancar() {
        jogo->avancarFase();
        origem=-1; destino=-1; quantidade=1;
        if (jogo->encerrado()) return;
        if (!jogo->turnoAtivo()) {
            ++turno;
            jogo->iniciarTurnoAtual();
            notificar("Novo turno: distribua seus reforcos.");
        } else notificar(std::string("Fase: ") + nomeFase(jogo->fase()));
    }
    void selecionar(int id) {
        if (jogo->encerrado()) return;
        const auto todos = jogo->mapa().todosTerritorios();
        if (id<1 || id>static_cast<int>(todos.size())) return;
        if (jogo->fase()==FaseTurno::Reforco) {
            origem=id; destino=-1;
            return;
        }
        if (origem==id) {origem=-1; destino=-1;return;}
        if (origem<0 || destino>=0) {origem=id;destino=-1;return;}
        destino=id;
    }
    void acao() {
        if (!jogo || jogo->encerrado()) return;
        auto fase=jogo->fase();
        if (origem<1 || (fase!=FaseTurno::Reforco && destino<1)) {
            notificar("Selecione origem e destino no mapa.",VERMELHO);return;
        }
        if (fase==FaseTurno::Reforco) {
            jogo->reforcar(origem,quantidade);
            notificar("Exercitos posicionados no territorio.");
        } else if (fase==FaseTurno::Ataque) {
            ultimoCombate=jogo->atacarTerritorio(origem,destino,dados);
            temCombate=true;
            std::string s="Dados ";
            for (int i:ultimoCombate.dadosAtacante) s+=std::to_string(i);
            s+=" x ";
            for (int i:ultimoCombate.dadosDefensor) s+=std::to_string(i);
            s+=" | -"+std::to_string(ultimoCombate.baixasAtacante)+" / -"+
                std::to_string(ultimoCombate.baixasDefensor);
            if (ultimoCombate.conquistado) s+=" | CONQUISTA!";
            notificar(s,ultimoCombate.conquistado?DOURADO:VERDE);
            if (ultimoCombate.conquistado) {origem=-1; destino=-1;}
        } else if (fase==FaseTurno::Movimentacao) {
            jogo->moverTropas(origem,destino,quantidade);
            notificar("Tropas movimentadas com sucesso.");
        } else avancar();
    }
};

const Territorio* pegar(const Ui& ui, int id) {
    if (!ui.jogo || id<1) return nullptr;
    auto todos=ui.jogo->mapa().todosTerritorios();
    return id<=static_cast<int>(todos.size()) ? todos[static_cast<std::size_t>(id-1)] : nullptr;
}
int idMouse(Vector2 mouse) {
    // Prioriza os centros dos territorios em vez de grandes areas de clique.
    int id=-1; float distancia=25.f;
    for (int i=0;i<42;++i) {
        const float dx=mouse.x-PONTOS[static_cast<std::size_t>(i)].x;
        const float dy=mouse.y-PONTOS[static_cast<std::size_t>(i)].y;
        const float d=std::sqrt(dx*dx+dy*dy);
        if (d<distancia) {distancia=d;id=i+1;}
    }
    return id;
}
void desenharMapa(Ui& ui) {
    const auto territorios=ui.jogo->mapa().todosTerritorios();
    // Topologia: conexoes discretas; apenas a selecao realca vizinhos.
    for (std::size_t i=0;i<territorios.size();++i) {
        for (const auto* vizinho : territorios[i]->vizinhos()) {
            const auto it=std::find(territorios.begin(),territorios.end(),vizinho);
            if (it==territorios.end()) continue;
            const auto j=static_cast<std::size_t>(std::distance(territorios.begin(),it));
            if (j<=i) continue;
            const bool ligada=ui.origem==static_cast<int>(i)+1 || ui.origem==static_cast<int>(j)+1;
            DrawLineEx({PONTOS[i].x,PONTOS[i].y},{PONTOS[j].x,PONTOS[j].y},
                ligada?2.6f:.95f, ligada?comAlpha(DOURADO,170):comAlpha(MUTED,28));
        }
    }
    Vector2 mouse=GetMousePosition();
    int hover=idMouse(mouse);
    if (!ui.mostrarObjetivo && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hover>0) ui.selecionar(hover);
    for (std::size_t i=0;i<territorios.size();++i) {
        const auto* t=territorios[i];
        const Vector2 pos={PONTOS[i].x,PONTOS[i].y};
        const bool selected=ui.origem==static_cast<int>(i)+1;
        const bool target=ui.destino==static_cast<int>(i)+1;
        const bool over=hover==static_cast<int>(i)+1;
        Color c = t->dono()?corJogador(t->dono()->cor()):MUTED;
        if (selected || target || over) DrawCircleV(pos,26.f,comAlpha(selected?DOURADO:(target?VERMELHO:TINTA),70));
        DrawCircleV(pos,18.f,comAlpha(FUNDO,255));
        DrawCircleV(pos,15.8f,c);
        DrawCircleLines(static_cast<int>(pos.x),static_cast<int>(pos.y),18.4f,
            selected?DOURADO:target?VERMELHO:comAlpha(TINTA,155));
        centro(std::to_string(t->tropas()),pos.x,pos.y-9.f,20.f,
            (t->dono() && t->dono()->cor()==CorJogador::Amarelo)?FUNDO:TINTA);
        centro(CURTOS[i],pos.x,pos.y+20.f,12.f,comAlpha(TINTA,215));
    }
    if (hover>0) {
        const Territorio& t=*territorios[static_cast<std::size_t>(hover-1)];
        std::string title=t.nome();
        float width=std::max(190.f,largura(title,18)+20.f);
        const Vector2 p=PONTOS[static_cast<std::size_t>(hover-1)].x > 890 ?
            Vector2{mouse.x-width-19,mouse.y-71}: Vector2{mouse.x+19,mouse.y-71};
        painel({p.x,p.y,width,60},PAINEL2,DOURADO);
        texto(title,p.x+11,p.y+7,18.f);
        texto(std::to_string(t.tropas())+" tropas  |  "+(t.dono()?t.dono()->nome():"Sem dono"),
              p.x+11,p.y+35,14.f,MUTED);
    }
}
void tituloSecao(const std::string& nome, float y) {
    texto(nome,SIDE_X+22,y,23,DOURADO);
    DrawLine(static_cast<int>(SIDE_X+22),static_cast<int>(y+36),
             static_cast<int>(SIDE_X+SIDE_W-20),static_cast<int>(y+36),BORDA);
}
void desenharPainel(Ui& ui) {
    auto& j=*ui.jogo;
    const auto& atual=j.jogadorAtual();
    const float x=SIDE_X+23;
    DrawCircleV({x+10,MAP_Y+28},10.f,corJogador(atual.cor()));
    texto(atual.nome(),x+30,MAP_Y+13,24,TINTA);
    etiqueta("TURNO " + std::to_string(ui.turno),SIDE_X+SIDE_W-108,MAP_Y+21);
    etiqueta(nomeFase(j.fase()),x,MAP_Y+66,DOURADO);
    texto(std::to_string(atual.territorios().size())+" territorios", x,MAP_Y+104,18.f);
    texto(std::to_string(atual.exercitosDisponiveis())+" reforcos", x+173,MAP_Y+104,18.f);
    texto(std::to_string(atual.totalCartas())+" cartas", x,MAP_Y+133,17.f,MUTED);
    if (botao({x+158, MAP_Y+130, 178, 30}, "VER OBJETIVO [O]", true, PAINEL3, 14.f))
        ui.mostrarObjetivo = true;
    DrawLine(static_cast<int>(x),static_cast<int>(MAP_Y+167),
             static_cast<int>(SIDE_X+SIDE_W-20),static_cast<int>(MAP_Y+167),BORDA);

    tituloSecao("TERRITORIOS", MAP_Y+187);
    const Territorio* o=pegar(ui,ui.origem);
    const Territorio* d=pegar(ui,ui.destino);
    etiqueta("ORIGEM / REFORCO",x,MAP_Y+235);
    texto(o?o->nome():"Clique no mapa...", x,MAP_Y+258,19.f,o?TINTA:MUTED);
    if (o) etiqueta(std::to_string(o->tropas())+" tropas  |  "+(o->dono()?o->dono()->nome():"Sem dono"),x,MAP_Y+286);
    etiqueta("ALVO / DESTINO",x,MAP_Y+323);
    texto(d?d->nome():"Clique no proximo...", x,MAP_Y+346,19.f,d?TINTA:MUTED);
    if (d) etiqueta(std::to_string(d->tropas())+" tropas  |  "+(d->dono()?d->dono()->nome():"Sem dono"),x,MAP_Y+375);

    const bool ataque=j.fase()==FaseTurno::Ataque;
    const bool fim=j.fase()==FaseTurno::Fim;
    const bool reforco=j.fase()==FaseTurno::Reforco;
    etiqueta(ataque?"DADOS DE ATAQUE":"QUANTIDADE DE EXERCITOS",x,MAP_Y+408);
    if (botao({x,MAP_Y+435,54,45},"-",!fim,PAINEL3,25.f)) {
        if (ataque) ui.dados=std::max(1,ui.dados-1);
        else ui.quantidade=std::max(1,ui.quantidade-1);
    }
    painel({x+66,MAP_Y+435,165,45},PAINEL2,BORDA,.14f);
    centro(std::to_string(ataque?ui.dados:ui.quantidade),x+148,MAP_Y+444,24.f,TINTA);
    if (botao({x+244,MAP_Y+435,54,45},"+",!fim,PAINEL3,25.f)) {
        if (ataque) ui.dados=std::min(3,ui.dados+1);
        else ui.quantidade=std::min(999,ui.quantidade+1);
    }
    const char* acao=reforco?"POSICIONAR TROPAS":ataque?"ATACAR AGORA":
                     fim?"ENCERRAR TURNO":"MOVER EXERCITOS";
    if (botao({x,MAP_Y+501,336,47},acao,true,ataque?VERMELHO:VERDE,20.f)) {
        try { ui.acao(); } catch(const std::exception& e) { ui.notificar(e.what(),VERMELHO); }
    }
    if (botao({x,MAP_Y+559,159,41},"TROCAR CARTAS",
              reforco && atual.podeTrocarCartas(),PAINEL3,15.f)) {
        try { ui.notificar("Troca realizada: +"+std::to_string(j.trocarCartas())+" tropas."); }
        catch(const std::exception& e) {ui.notificar(e.what(),VERMELHO);}
    }
    if (botao({x+170,MAP_Y+559,166,41},"PROXIMA FASE",true,PAINEL3,15.f)) {
        try {ui.avancar();}catch(const std::exception& e){ui.notificar(e.what(),VERMELHO);}
    }
    etiqueta("[ESPACO] fase   [ENTER] acao   [+/-] qtd",x,MAP_Y+625);
}
void desenharRodape(Ui& ui) {
    texto("SITUACAO",MAP_X+18,831,16.f,DOURADO);
    texto(ui.mensagem,MAP_X+18,854,19.f,ui.corMensagem);
    const auto& jogadores=ui.jogo->jogadores();
    float x=836;
    for (const auto& p:jogadores) {
        if (!p->ativo()) continue;
        DrawCircleV({x,843},7,corJogador(p->cor()));
        texto(std::to_string(p->territorios().size()),x+14,832,17,TINTA);
        x+=66;
    }
    etiqueta("[ESC] limpar selecao",1364,851);
}
std::vector<std::string> quebrarLinhas(const std::string& frase, float maxWidth, float fonte) {
    std::istringstream input(frase);
    std::vector<std::string> linhas;
    std::string linha, palavra;
    while (input >> palavra) {
        const std::string candidata = linha.empty() ? palavra : linha + " " + palavra;
        if (!linha.empty() && largura(candidata, fonte) > maxWidth) {
            linhas.push_back(linha);
            linha = palavra;
        } else linha = candidata;
    }
    if (!linha.empty()) linhas.push_back(linha);
    return linhas;
}
void desenharObjetivo(Ui& ui) {
    if (!ui.mostrarObjetivo) return;
    DrawRectangle(0,0,LARGURA,ALTURA,comAlpha(FUNDO,208));
    painel({350,273,880,341},PAINEL2,DOURADO,.06f);
    centro("OBJETIVO DA PARTIDA",790,314,29,DOURADO);
    centro("Jogador: " + ui.jogo->jogadorAtual().nome(),790,365,19,MUTED);
    std::string objetivo = ui.jogo->jogadorAtual().objetivo()
                           ? ui.jogo->jogadorAtual().objetivo()->descricao()
                           : "Nao definido";
    auto linhas=quebrarLinhas(objetivo, 750.f, 25.f);
    float y=425.f;
    for (const auto& linha : linhas) {
        centro(linha,790,y,25,TINTA);
        y+=32.f;
    }
    entradaHabilitada=true;
    if (botao({635,548,310,48},"FECHAR OBJETIVO [O]",true,VERDE,19.f))
        ui.mostrarObjetivo = false;
}
void desenharVitoria(Ui& ui) {
    if (!ui.jogo->encerrado())return;
    DrawRectangle(0,0,LARGURA,ALTURA,comAlpha(FUNDO,205));
    painel({412,232,760,417},PAINEL2,DOURADO,.045f);
    centro("OBJETIVO CONCLUIDO",792,292,36,DOURADO);
    centro("VITORIA!",792,374,63,TINTA);
    centro(ui.jogo->vencedor()?ui.jogo->vencedor()->nome():"Partida encerrada",792,468,32,VERDE);
    if (botao({640,543,300,63},"NOVA PARTIDA",true,VERDE,22)) {
        ui.jogo.reset();ui.origem=-1;ui.destino=-1;
    }
}
void telaMenu(Ui& ui) {
    planoFundo();
    DrawRectangle(0,104,LARGURA,792,comAlpha(FUNDO,160));
    painel({396,174,786,580},PAINEL2,DOURADO,.04f);
    centro("COMANDO GLOBAL",789,215,20,DOURADO);
    centro("WAR",789,263,98,TINTA);
    centro("CONQUISTE TERRITORIOS. DOMINE CONTINENTES.",789,388,21,MUTED);
    centro("Uma partida local para 2 a 6 jogadores",789,426,18,MUTED);
    centro("QUANTIDADE DE JOGADORES",789,482,20,DOURADO);
    if (botao({590,529,70,61},"-",ui.quantidadeJogadores>2,PAINEL3,32))
        --ui.quantidadeJogadores;
    painel({679,528,220,62},PAINEL);
    centro(std::to_string(ui.quantidadeJogadores),789,538,38,TINTA);
    if (botao({918,529,70,61},"+",ui.quantidadeJogadores<6,PAINEL3,32))
        ++ui.quantidadeJogadores;
    if (botao({575,632,430,71},"INICIAR PARTIDA",true,VERDE,25)) {
        try {ui.iniciar();}catch(const std::exception& e){ui.notificar(e.what(),VERMELHO);}
    }
    centro("Mapa esquematico  |  Regras no motor C++ do projeto",789,717,15,MUTED);
}
} // namespace

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(1280, 750, "WAR | Conquista Global - Interface 2D");
    SetWindowMinSize(950, 570);
    RenderTexture2D quadro=LoadRenderTexture(LARGURA,ALTURA);
    SetExitKey(KEY_NULL); // ESC limpa a selecao; fechar no X da janela.
    SetTargetFPS(60);
    Ui ui;
    while (!WindowShouldClose()) {
        const float escala=std::min(static_cast<float>(GetScreenWidth())/LARGURA,
                                    static_cast<float>(GetScreenHeight())/ALTURA);
        const float dx=(GetScreenWidth()-LARGURA*escala)*.5f;
        const float dy=(GetScreenHeight()-ALTURA*escala)*.5f;
        const Vector2 mouseReal=GetMousePosition();
        mouseVirtual={(mouseReal.x-dx)/escala,(mouseReal.y-dy)/escala};
        BeginDrawing();
        BeginTextureMode(quadro);
        if (!ui.jogo) telaMenu(ui);
        else {
            planoFundo();
            entradaHabilitada = !ui.mostrarObjetivo;
            desenharMapa(ui);
            if (!ui.jogo->encerrado()) {
                desenharPainel(ui);
                desenharRodape(ui);
                if (IsKeyPressed(KEY_O)) ui.mostrarObjetivo = !ui.mostrarObjetivo;
                if (IsKeyPressed(KEY_ESCAPE)) {
                    if (ui.mostrarObjetivo) ui.mostrarObjetivo = false;
                    else {ui.origem=-1;ui.destino=-1;}
                }
                if (!ui.mostrarObjetivo && (IsKeyPressed(KEY_EQUAL) || IsKeyPressed(KEY_KP_ADD))) {
                    if(ui.jogo->fase()==FaseTurno::Ataque) ui.dados=std::min(3,ui.dados+1);
                    else ui.quantidade=std::min(999,ui.quantidade+1);
                }
                if (!ui.mostrarObjetivo && (IsKeyPressed(KEY_MINUS) || IsKeyPressed(KEY_KP_SUBTRACT))) {
                    if(ui.jogo->fase()==FaseTurno::Ataque) ui.dados=std::max(1,ui.dados-1);
                    else ui.quantidade=std::max(1,ui.quantidade-1);
                }
                if (!ui.mostrarObjetivo && IsKeyPressed(KEY_ENTER)) {
                    try {ui.acao();}catch(const std::exception& e){ui.notificar(e.what(),VERMELHO);}
                }
                if (!ui.mostrarObjetivo && IsKeyPressed(KEY_SPACE)) {
                    try {ui.avancar();}catch(const std::exception& e){ui.notificar(e.what(),VERMELHO);}
                }
                if (!ui.mostrarObjetivo && IsKeyPressed(KEY_E) && ui.jogo->fase()==FaseTurno::Reforco) {
                    try{ui.notificar("Troca: +"+std::to_string(ui.jogo->trocarCartas())+" tropas.");}
                    catch(const std::exception& e){ui.notificar(e.what(),VERMELHO);}
                }
                desenharObjetivo(ui);
            } else {
                entradaHabilitada = true;
                desenharRodape(ui);
                desenharVitoria(ui);
            }
        }
        EndTextureMode();
        ClearBackground(FUNDO);
        DrawTexturePro(quadro.texture,
            {0,0,static_cast<float>(LARGURA),-static_cast<float>(ALTURA)},
            {dx,dy,LARGURA*escala,ALTURA*escala},{0,0},0.f,WHITE);
        EndDrawing();
    }
    UnloadRenderTexture(quadro);
    CloseWindow();
    return 0;
}
