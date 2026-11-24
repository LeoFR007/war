# Diagrama de classes (UML)

O diagrama mostra uma **visão intermediária** do projeto: cada classe aparece com seus **atributos e métodos principais**, junto das enumerações, das multiplicidades mais importantes e das relações essenciais. Getters, métodos auxiliares e detalhes de implementação foram omitidos para que a estrutura continue fácil de ler.

## Arquivos

- `uml/classes.puml`: **arquivo-fonte editável**, escrito em PlantUML.
- `uml/classes.png`: imagem do diagrama.

Para gerar a imagem de novo: `make uml` (requer `sudo apt install plantuml`).

## O que o diagrama mostra

As classes estão agrupadas nas três camadas do projeto:

- **Controller**: `ControladorJogo` recebe os cliques e teclas do jogador e executa as jogadas no `Jogo`.
- **Model**: as regras do jogo. `Jogo` coordena a partida (turnos, ataques, trocas); `Territorio` e `Continente` formam o mapa; `Jogador` e `Objetivo` são classes abstratas (métodos em itálico são virtuais puros).
- **View**: `MapaView`, `PainelView`, `EventosView` e `BarraSuperiorView` só leem o `Jogo` para desenhar a tela; `MapaView` é composta por `Camera` (zoom e pan) e `RegioesMapa` (desenho dos territórios).
- **Enumerações**: `Fase` (etapas do turno na interface) e `TipoCarta` (infantaria, cavalaria, artilharia).
- **Multiplicidades**: um `Jogo` tem 1 `Mapa` e de 2 a 6 jogadores; o `Mapa` tem 42 territórios e 6 continentes; cada `Jogador` tem 1 objetivo.

## Notação usada

- Seta com triângulo vazio: **herança**. `JogadorHumano` e `JogadorBot` herdam de `Jogador`; os três tipos de objetivo herdam de `Objetivo`.
- Losango preenchido no dono: **composição**. O `Jogo` possui o `Mapa`, os jogadores e o baralho de cartas; o `Mapa` possui territórios e continentes; cada `Jogador` possui seu `Objetivo` e as cartas da mão.
- Losango vazio: **agregação**. O `Continente` agrupa territórios que pertencem ao `Mapa`.
- Seta contínua: **associação**. O `Jogador` controla territórios; cada `Territorio` conhece seus vizinhos; o jogador (ator) usa o controller.
- Seta tracejada: **dependência**. O `Jogo` lança `WarException`; as views leem o `Jogo`.
