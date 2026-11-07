# Projeto WAR — clone educacional em C++17

Versão **2.0 — motor do jogo + interface visual 2D opcional** — Jogo completo por **terminal ou janela raylib**, para **2 a 6 pessoas**, ou simulação automática de jogadores.  
**O motor e o executável de terminal não dependem de bibliotecas gráficas.** A GUI opcional (`make gui`) utiliza **raylib** e preserva a arquitetura C++17.

## Interface visual 2D (opcional)

Para a versão **com janela gráfica**, consulte [docs/INTERFACE_2D.md](docs/INTERFACE_2D.md).

No WSL Ubuntu 24.04:

```bash
sudo apt update
sudo apt install -y build-essential cmake git pkg-config libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev
make gui
```

A raylib será baixada pelo CMake se não estiver instalada. É necessário WSL 2 com WSLg; a janela abre a partir do terminal.


## 1. Como compilar e executar

### Requisitos

- Compilador com suporte a **C++17** (GCC 8+, Clang 7+, MSVC 2019+).
- **CMake 3.16 ou superior**.
- Terminal/PowerShell/Prompt de Comando.

### Windows (PowerShell) com Visual Studio Build Tools ou MinGW

Abra o terminal **na pasta `ProjetoWAR`**:

```powershell
cmake -S . -B build
cmake --build build --config Release
```

Para Visual Studio, o programa geralmente estará em `build\Release\war.exe`:

```powershell
.\build\Release\war.exe --jogadores 3
.\build\Release\war.exe --demo --jogadores 4 --seed 42
```

Para MinGW com gerador Makefiles/Ninja, geralmente estará em `build\war.exe`:

```powershell
.\build\war.exe --jogadores 3
.\build\war.exe --demo --jogadores 4 --seed 42
```

Opcionalmente, execute `executar_windows.bat`, que compila e abre o jogo.

### WSL (Ubuntu/Linux no Windows) — forma recomendada com Makefile

1. Abra o terminal do **Ubuntu/WSL**. Instale as ferramentas se ainda não estiverem instaladas:

   ```bash
   sudo apt update
   sudo apt install -y build-essential git
   ```

2. Clone o repositório e entre na pasta do projeto:

   ```bash
   git clone https://github.com/LeoFR007/war.git
   cd war
   ```

3. Compile o jogo e execute:

   ```bash
   make
   make run
   ```

   Para testar automaticamente, sem informar nomes:

   ```bash
   make demo JOGADORES=4 SEED=42 MAX_TURNOS=100
   make test
   ```

   O executável Linux estará em `build-make/war`. Você também pode iniciá-lo diretamente:

   ```bash
   ./build-make/war --jogadores 3
   ./build-make/war --demo --jogadores 4 --seed 42
   ```

**Observações:** o executável Linux não usa extensão `.exe`. Se aparecer `make: command not found` ou `g++: command not found`, instale `build-essential`. O `Makefile` está na pasta raiz `ProjetoWAR`, portanto execute os comandos dentro dela. A pasta `build-make/` é usada pelo `Makefile` e não interfere com `build/` do CMake.

### Linux/macOS

```bash
cmake -S . -B build
cmake --build build -j
./build/war --jogadores 3
```

### Executar os testes automatizados

```bash
ctest --test-dir build --output-on-failure -C Release
```

Para testes de depuração com as verificações de compilação preservadas:

```bash
cmake -S . -B build-debug -DCMAKE_BUILD_TYPE=Debug
cmake --build build-debug -j
ctest --test-dir build-debug --output-on-failure
```

### Compilar sem CMake (GCC/MinGW, Linux/macOS)

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude main.cpp src/*.cpp -o war
./war --demo --seed 42
```

## 2. Como jogar

Primeiro, inicie o jogo:

```bash
./build/war --jogadores 3
```

Informe o nome de cada jogador quando solicitado. O programa mostra o mapa textual contendo os **IDs de 1 a 42**, território, tropas e dono. Em cada turno há **três fases obrigatórias**, além do encerramento:

1. **Reforço:** distribuir todos os exércitos disponíveis entre seus territórios. O jogador recebe `max(3, numeroDeTerritorios / 2)`, com divisão inteira, mais bônus dos continentes completos; as cartas podem acrescentar reforços.
2. **Ataque:** atacar territórios inimigos adjacentes, quantas vezes desejar e enquanto houver tropa suficiente. A conquista transfere tropas da origem para o destino.
3. **Movimentação:** opcionalmente deslocar tropas entre territórios vizinhos do mesmo jogador, mantendo pelo menos uma unidade em cada origem. A fase permite vários movimentos legais.
4. **Fim:** se conquistou pelo menos um território no turno, o jogador recebe **no máximo uma carta**, mesmo que tenha conquistado vários.

### Comandos

| Fase | Comando | Exemplo | Significado |
|---|---|---|---|
| Qualquer | `mapa` | `mapa` | Exibe todos os territórios e seus IDs. |
| Qualquer | `status` | `status` | Exibe os seus territórios, exércitos e cartas. |
| Reforço e ataque | `vizinhos ID` | `vizinhos 8` | Consulta adjacências do território indicado. |
| Reforço | `r ID QTD` | `r 8 5` | Acrescenta 5 exércitos seus ao território 8. |
| Reforço | `trocar` | `trocar` | Troca uma trinca válida de cartas por reforços. |
| Ataque | `a ORIGEM ALVO DADOS` | `a 8 9 3` | Ataca do ID 8 para o ID 9 com 3 dados. |
| Ataque | `passar` | `passar` | Encerra a fase de ataque. |
| Movimentação | `m ORIGEM DESTINO QTD` | `m 8 9 2` | Move duas tropas entre territórios próprios adjacentes. |
| Movimentação | `fim` | `fim` | Finaliza o turno. |
| Qualquer | `sair` | `sair` | Termina a partida sem vencedor. |

**Importante:** os números do exemplo são meramente ilustrativos; confirme que o território pertence ao seu jogador e é vizinho do destino. O jogo apresenta o erro e continua rodando quando a ação é inválida.

### Simulação automática (sem digitar comandos)

```bash
./build/war --demo --jogadores 4 --seed 42 --max-turnos 400
```

Os jogadores automáticos escolhem fronteiras para reforçar, atacam quando há vantagem e movem tropas de apoio. Eles não são IAs competitivas: servem para demonstrar as mecânicas e executar partidas completas. A **semente** permite repetir o mesmo sorteio em testes.

| Argumento | Padrão | Descrição |
|---|---:|---|
| `--jogadores N` | 3 | De 2 a 6 participantes. |
| `--demo` | desativado | Todos os jogadores são automáticos. |
| `--seed N` | aleatória | Semente do gerador de dados e distribuição. |
| `--max-turnos N` | 400 | Limite de turnos da simulação. |
| `--ajuda` | — | Exibe os parâmetros. |

## 3. Regras implementadas

### Mapa

- **42 territórios**, distribuídos em **6 continentes**: América do Norte (9), América do Sul (4), Europa (7), África (6), Ásia (12) e Oceania (4).
- Ligações por fronteiras do tabuleiro clássico, incluindo rotas entre continentes (Alasca ↔ Kamtchatka, Brasil ↔ Norte da África, etc.).
- Bônus de continentes: América do Norte +5, América do Sul +2, Europa +5, África +3, Ásia +7, Oceania +2.
- Proprietários, tropas e adjacências são armazenados em `Territorio`.

### Combate

1. Origem deve ser do atacante; alvo deve ser inimigo, adjacente e possuir pelo menos uma tropa.
2. Devem existir **duas ou mais tropas na origem**, pois uma fica de guarda.
3. Atacante escolhe de **1 a 3 dados**, limitado a `tropasOrigem - 1`.
4. Defensor lança automaticamente `min(3, tropasDefesa)` dados.
5. Ambos os vetores são ordenados **decrescentemente com lambda** (`std::sort`).
6. Compara-se o maior com o maior, o segundo com o segundo, até o menor tamanho das duas listas. **Em caso de empate, o defensor vence.**
7. Aplica-se uma baixa ao perdedor de cada comparação.
8. Se o defensor ficar com zero tropas, a posse é transferida ao atacante, que ocupa o alvo com `min(dadosUtilizados, tropasRestantesNaOrigem - 1)` unidades.

**Exemplo:** ataque [6,5,3], defesa [6,4] → o atacante perde 1 tropa (6=6), o defensor perde 1 (5>4).

### Objetivos

A implementação possui os três tipos polimórficos solicitados:

- **Conquistar territórios:** atingir um número mínimo, ajustado à quantidade inicial de jogadores (24 para dois, 22 para três ou 18 para quatro a seis).
- **Conquistar continentes:** ocupar totalmente os dois continentes atribuídos ao jogador.
- **Eliminar um adversário:** ser o jogador responsável pela perda do último território do alvo. Se o alvo for eliminado por terceiro, valerá conquistar o mínimo de territórios do objetivo alternativo.

A partida também termina quando **apenas um jogador ainda possuir territórios**. A atribuição dos tipos de objetivos é determinística pela ordem dos jogadores (para demonstrar as classes); a distribuição inicial dos territórios e o baralho são aleatórios. São utilizadas condições de vitória simplificadas, não uma reprodução completa de todas as cartas-objetivo comerciais.

### Cartas

- Baralho de 42 cartas de território, representadas por **`CartaInfantaria`**, **`CartaCavalaria`** e **`CartaArtilharia`**, herdando de **`Carta` abstrata**.
- Uma carta no fim de um turno com conquista, enquanto houver cartas disponíveis.
- Troca de **3 iguais** ou **1 de cada tipo**.
- Bônus progressivo por troca global: 4, 6, 8, 10, 12 etc.
- Cartas entregues voltam ao descarte e o baralho é recomposto quando termina.
- Jogador eliminado entrega suas cartas ao jogador que o eliminou.
- Com **cinco ou mais cartas**, a troca passa a ser obrigatória antes de sair da fase de reforço.

### Simplificações intencionais

Esta é uma **adaptação jogável do WAR/Risk**, não uma reprodução exata de cada edição comercial. A interface é textual; a defesa utiliza automaticamente o máximo de três dados; as movimentações são locais (territórios adjacentes), múltiplas durante a fase; os jogadores começam com uma tropa por território e oito tropas adicionais aleatoriamente distribuídas. Não existem negociações, missões/cartas secretas impressas, coringas ou mapa gráfico interativo.

## 4. Arquitetura de classes (UML)

Veja o diagrama UML completo em:

- [`docs/UML.png`](docs/UML.png) — diagrama completo com atributos, métodos, classes e relações.
- [`docs/UML_resumo.png`](docs/UML_resumo.png) — visão geral das relações entre as principais classes.
- [`docs/UML.svg`](docs/UML.svg) — versão vetorial com zoom sem perda de qualidade.
- [`docs/UML.dot`](docs/UML.dot) — **código-fonte Graphviz editável** (exigência expressa do PDF).
- [`docs/MODELAGEM_UML.md`](docs/MODELAGEM_UML.md) — descrição de associações, composições, agregação, herança e enumerações.

Para regenerar as figuras no WSL, instale `graphviz` e execute `make uml`.

A classe `Jogo` agora fornece uma **API de ações independente da entrada do terminal**: `iniciarTurnoAtual`, `reforcar`, `trocarCartas`, `atacarTerritorio`, `moverTropas` e `avancarFase`. Esse é o ponto de integração sugerido para a futura interface gráfica em C++.

### Responsabilidades e relacionamentos

| Classe | Responsabilidade |
|---|---|
| `Jogo` | Controla partida, turnos, fases, cartas e vitória. |
| `Mapa` | **Possui** continentes e territórios (`unique_ptr`), monta o mapa e verifica vizinhos. |
| `Continente` | **Agrega** referências a seus territórios e calcula bônus de domínio. |
| `Territorio` | Nome, dono, exércitos, vizinhos, combate e conquista. |
| `Jogador` | Nome, cor, territórios controlados, cartas, objetivo, reforço e ações. |
| `Objetivo` | **Classe abstrata**, contrato para `foiConcluido` e `descricao`. |
| `ObjetivoConquistaTerritorios` | Objetivo por contagem de territórios. |
| `ObjetivoConquistarContinente` | Objetivo por controle de continentes. |
| `ObjetivoEliminarJogador` | Objetivo de eliminação com alternativa. |
| `Carta` | **Classe abstrata**, interface de tipo e descrição. |
| `CartaInfantaria` / `CartaCavalaria` / `CartaArtilharia` | Especializações de `Carta`. |
| `WarException` e derivadas | Exceções personalizadas de regras. |

**Composição:** `Jogo` possui `Mapa` e jogadores; `Mapa` possui continentes e territórios; `Jogador` possui seu objetivo e suas cartas.  
**Agregação:** `Continente` mantém ponteiros não proprietários para territórios criados pelo mapa.  
**Associação:** `Jogador` controla territórios sem ter a propriedade da memória deles; território referencia o jogador dono e outros territórios vizinhos.  
**Herança e polimorfismo:** as classes de objetivo e de carta sobrescrevem métodos virtuais abstratos; as exceções herdam de `WarException`.

### Ponteiros e gerenciamento de memória

- `std::unique_ptr` indica posse exclusiva; destruição de objetos é automática (RAII).
- Ponteiros `Jogador*`, `Territorio*` e `Continente*` não são proprietários e só são utilizados enquanto `Jogo` e seu `Mapa` existem.
- `Mapa` mantém objetos em `vector<unique_ptr<...>>`, de modo que o endereço de cada objeto criado não muda ao reorganizar o vetor de ponteiros.
- `Jogador` transfere o controle de um território retirando o ponteiro da lista do antigo dono e incluindo-o na sua lista.

## 5. Tratamento de exceções

Arquivo: `include/Exceptions.h`.

| Exceção | Exemplo |
|---|---|
| `TerritorioNaoPertenceAoJogadorException` | Tentar atacar a partir de um território adversário. |
| `AtaqueInvalidoException` | Atacar território aliado ou usar dados indevidos. |
| `MovimentoInvalidoException` | Tentar mover todas as tropas da origem. |
| `TerritorioNaoAdjacenteException` | Atacar/mover entre territórios sem ligação. |

Os comandos interativos usam `try/catch` nas ações críticas, informando a falha sem encerrar toda a partida. Construtores e invariantes também podem lançar `std::invalid_argument` e `std::out_of_range`.

## 6. Conceitos de C++/POO demonstrados

- **Encapsulamento:** atributos privados e métodos públicos com validação.
- **Abstração:** interfaces `Objetivo` e `Carta`, métodos virtuais puros.
- **Herança:** três subclasses de objetivos; três subclasses de cartas; quatro exceções derivadas.
- **Polimorfismo dinâmico:** `unique_ptr<Objetivo>` e `unique_ptr<Carta>` com chamadas virtuais.
- **Composição e agregação:** ownership claro com `unique_ptr` e associações com ponteiros crus não proprietários.
- **Enums fortemente tipados:** `CorJogador`, `TipoCarta`, `FaseTurno` e `ResultadoCombate`.
- **Expressões lambda:** ordenação de dados (`Territorio.cpp`) e critérios em `std::all_of`, `std::find_if`, `std::any_of`.
- **Exceções customizadas e `try/catch`:** separação entre regras e interface.
- **Biblioteca padrão moderna:** `std::vector`, `std::deque`, `<random>`, `<memory>`, `<algorithm>`.

## 7. Estrutura do projeto

```text
war/
├── CMakeLists.txt
├── README.md
├── Makefile
├── .gitignore
├── main.cpp
├── executar_windows.bat
├── include/
│   ├── Carta.h
│   ├── Continente.h
│   ├── Enums.h
│   ├── Exceptions.h
│   ├── Jogador.h
│   ├── Jogo.h
│   ├── Mapa.h
│   ├── Objetivo.h
│   └── Territorio.h
├── src/
│   ├── Carta.cpp
│   ├── Continente.cpp
│   ├── Jogador.cpp
│   ├── Jogo.cpp
│   ├── Mapa.cpp
│   ├── Objetivo.cpp
│   └── Territorio.cpp
├── docs/
│   ├── UML.dot
│   ├── UML.png
│   ├── UML_resumo.png
│   ├── UML_resumo.dot
│   ├── EXEMPLO_EXECUCAO.txt
│   ├── UML.svg
│   ├── RELATORIO.md
│   ├── MODELAGEM_UML.md
│   └── EVOLUCAO_2D.md
└── tests/
    └── testes.cpp
```

## 8. Como validar o projeto

- **Compilação:** executar `cmake --build build --config Release`.
- **Testes automatizados:** executar `ctest --test-dir build --output-on-failure -C Release`.
- **Partida automática:** executar `war --demo --jogadores 4 --seed 42` para observar turnos, ataques e vitória sem interação.
- **Partida humana:** executar `war --jogadores 3` e testar `mapa`, distribuição, `passar`, `fim`, ataques e erros de ações inválidas.

Os testes incluem quantidade de territórios e continentes, simetria de adjacências, conexão intercontinental, posse, movimentações inválidas, reforço, ataque, conquista, cartas, eliminação com transferência de cartas, troca obrigatória com cinco cartas, fases da API do motor, partidas com 2 a 6 jogadores e os três objetivos.

## 9. Créditos

Implementação original e didática inspirada nas mecânicas gerais de jogos de conquista territorial. Não utiliza artes, logotipos, textos de cartas ou outros materiais licenciados de jogos comerciais. O projeto é disponibilizado para estudo e modificação.
