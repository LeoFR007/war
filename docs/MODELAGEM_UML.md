# Modelagem de classes (UML)

## Arquivos

- `UML.png`: diagrama de classes em imagem.
- `UML.dot`: **arquivo-fonte editável** (Graphviz). Para gerar novamente: `make uml` (requer `sudo apt install graphviz`).

## Notação usada

- Seta com triângulo vazio: **herança**.
- Losango preenchido no dono: **composição** (o dono cria e destrói o objeto).
- Losango vazio: **agregação** (aponta para objetos que pertencem a outro).
- Seta simples: **associação**; seta tracejada: **dependência**.

## Camadas (pacotes no diagrama)

- **model**: regras do jogo (todas as classes de antes da interface gráfica).
- **view**: renderização (`MapaView` compõe `Camera`, `RegioesMapa` e `Animacoes`) e interface (`PainelView`, `EventosView`, `BarraSuperiorView`), mais a estrutura `Fontes` e os enums `EstadoVisual`, `AcaoPainel`, `TipoIcone`. Só desenham e consultam o model (dependências «read»).
- **controller**: `ControladorJogo` e o enum `Fase`. Recebe mouse/teclado e chama métodos do model; captura `WarException` («catch»).

## Relações principais

- **Composição:** `Jogo` possui `Mapa`, os jogadores e o baralho de `Carta`; `Mapa` possui os 42 `Territorio` e os 6 `Continente`; `Jogador` possui seu `Objetivo` e as cartas da mão.
- **Agregação:** `Continente` guarda ponteiros para territórios que pertencem ao `Mapa`.
- **Associação (com multiplicidade):** `Jogador` controla `0..*` territórios; `Territorio` tem `0..1` dono e `1..*` vizinhos; `Jogador` guarda `0..1` `eliminadoPor`; `Jogo` guarda `0..1` vencedor; `ObjetivoContinentes` aponta para 2 continentes e `ObjetivoEliminar` para 1 jogador alvo.
- **Enumeração:** `TipoCarta` (Infantaria, Cavalaria, Artilharia), usada por `Carta`.
- **Herança e polimorfismo:**
  - `Objetivo` (abstrata) → `ObjetivoTerritorios`, `ObjetivoContinentes`, `ObjetivoEliminar`.
  - `Jogador` (abstrata) → `JogadorHumano`, `JogadorBot`.
  - `WarException` → `std::runtime_error`.
- **Dependência:** `Jogo::atacar()` retorna `ResultadoAtaque` e lança `WarException`.
