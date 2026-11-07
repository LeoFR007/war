# Registro de verificacao — interface 2D WAR

## Verificado na entrega

- O jogo original continua compilando via `make` e `cmake` com C++17.
- O alvo `make test` executou com sucesso o conjunto de testes da versao por terminal.
- O CMake compilou o projeto em Debug e os quatro testes CTest passaram.
- O arquivo `gui/main_gui.cpp` passou em verificação de sintaxe de C++17 com `g++ -Wall -Wextra -Wpedantic` usando um cabeçalho local de declarações raylib para análise.
- Os dois diagramas UML foram gerados via Graphviz, e seus fontes `.dot` estão disponíveis.

## Pendente no computador com WSLg

A raylib não estava instalada no ambiente que gerou esta versão, e o acesso externo para baixar o pacote estava indisponível. Por isso **não foi possível compilar/linkar a GUI com a raylib real nem abrir a janela gráfica aqui**. O arquivo gerado pode ser compilado no WSL com o CMake/FetchContent, e deve ser testado visualmente no computador do usuário com a raylib instalada ou acesso ao GitHub.

**Não confundir:** os testes de `war_core` não comprovam que a janela gráfica abre no WSL. Essa validação está pendente.

## Comandos para validar no WSL

```bash
make gui-build
./build-gui/war_gui
```

Se a janela abrir, confira: tela inicial; selecionar jogadores; clique em território; reforço; ataque de 1 a 3 dados; transferência de tropas; movimentação; troca de cartas; objetivo (O); próxima fase e vitória. A janela é redimensionável e fecha pelo X da barra de título.
