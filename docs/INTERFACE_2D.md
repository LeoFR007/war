# Interface 2D do WAR com raylib — WSL

A interface 2D é **opcional** e funciona sobre o motor `war_core` já entregue no T1.
A biblioteca usada é [raylib](https://www.raylib.com/), uma API simples para jogos 2D, implementada em C e compatível com C++17.
A janela pode ser **redimensionada** sem perder os cliques no tabuleiro. O programa **é lançado pelo terminal** (`make gui`), mas abre uma **janela gráfica**. Isso requer WSL 2 com WSLg ou outro ambiente Linux com servidor gráfico; não desenha no terminal de texto.

## Ubuntu 24.04 + WSL 2

No terminal do Ubuntu, dentro da pasta do projeto:

```bash
sudo apt update
sudo apt install -y build-essential cmake git pkg-config \
    libasound2-dev libx11-dev libxrandr-dev libxi-dev \
    libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev
make gui
```

Se o Ubuntu oferecer raylib em repositório, instalar `libraylib-dev` também é válido; o CMake tenta detectar a instalação.
Se não houver raylib instalada, **o CMake faz download automaticamente** do código-fonte do release 6.2 no GitHub, portanto a primeira compilação requer internet. As próximas compilam usando o cache em `build-gui/`.

Se ocorrer falha por dependências de áudio no Ubuntu, confira `libasound2-dev`. A interface não exige arquivos extras de áudio nem fontes externas.

**No PowerShell do Windows**, confira a integração gráfica:

```powershell
wsl --list --verbose
wsl --update
wsl --shutdown
```

Abra novamente o Ubuntu e confirme `echo $DISPLAY` (em geral `:0`) antes de `make gui`. É necessário WSL **versão 2** com WSLg e suporte gráfico habilitado. Feche a janela pelo botão X.

## Outros comandos

```bash
make                # WAR original no terminal (sem raylib)
make test           # testes de POO e mecânicas
make run            # interface textual
make gui-build      # somente compila GUI
make gui            # compila e executa GUI
./build-gui/war_gui # executa sem recompilar
```

Ou diretamente via CMake:

```bash
cmake -S . -B build-gui -DWAR_BUILD_GUI=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build-gui --target war_gui -j 2
./build-gui/war_gui
```

## Controles

- No menu inicial, use `-`/`+` para selecionar entre **2 e 6 jogadores**; clique em **INICIAR PARTIDA**.
- Clique em um território para **selecionar a origem**. Na fase de ataque e de movimentação, clique em outro território para **selecionar o destino**. Clique na origem novamente para cancelar.
- Em **Reforços**, selecione território próprio e quantidade; clique **POSICIONAR TROPAS**.
- Em **Ataques**, selecione território próprio, depois inimigo adjacente; escolha de 1 a 3 dados; clique **ATACAR AGORA**. As perdas e conquistas aparecem no rodapé.
- Em **Movimentação**, selecione origem e destino próprios, adjacentes; configure quantas tropas mover.
- **PROXIMA FASE** avança o turno; os reforços precisam ser todos distribuídos antes da fase de ataque. **TROCAR CARTAS** aplica as regras já implementadas no motor.
- Atalhos: `Enter` executar ação, `Espaço` próxima fase, `+`/`-` ajustar quantidade/dados, `E` trocar cartas, `Esc` limpar seleção.
- O **objetivo secreto do jogador atual** pode ser consultado pelo botão **VER OBJETIVO** ou tecla `O`. Vence quem completa o objetivo.

## Organização

- `gui/main_gui.cpp`: desenho vetorial do mapa esquemático, marcadores, estados, entrada via mouse e teclado, mensagens e botões.
- `src/` + `include/`: motor original de jogo independente da raylib.
- `CMakeLists.txt`: `WAR_BUILD_GUI` controla a dependência opcional.
- `Makefile`: mantém compatibilidade com `make`, `make run` e `make test`; adiciona `make gui`.

## Limitações (próximas melhorias)

- O mapa é **esquemático**, não usa contornos geopolíticos oficiais e não é um desenho licenciado do tabuleiro WAR.
- Jogadores locais têm nomes automáticos `Jogador 1`, `Jogador 2` etc.; ainda não há tela de personalização.
- Não há trilha sonora, salvamento, IA visual ou animação elaborada dos dados. O combate é aplicado corretamente pelo motor; os valores são exibidos em texto.
- O resultado final deve ser avaliado em ambiente real **WSLg com raylib instalada/baixada**, pois a compilação gráfica requer dependências de terceiros e servidor gráfico.

## UML

O UML original em `docs/UML.dot` continua válido para o motor do jogo, e `docs/ARQUITETURA_GUI.dot` complementa a apresentação da fronteira entre a GUI e o núcleo. Ambos os fontes são editáveis com Graphviz.
