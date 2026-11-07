> Atualizacao da versao 2.0: a primeira interface raylib foi implementada. Veja `INTERFACE_2D.md`. As ideias abaixo ficam registradas como evolucoes futuras para o projeto.

# Preparação da próxima etapa: interface visual 2D

O objetivo é adicionar uma biblioteca gráfica **sem reescrever regras nem duplicar a lógica de combate**.

## API existente do motor (`include/Jogo.h`)

- `iniciarTurnoAtual()` inicia o turno atual e informa os reforços.
- `jogadorAtual()`, `mapa()`, `fase()`, `turnoAtivo()`, `encerrado()` e `vencedor()` permitem consultar o estado.
- `reforcar(id, quantidade)` e `trocarCartas()` implementam a fase de distribuição.
- `atacarTerritorio(origem, alvo, dados)` devolve `RelatorioCombate` com números dos dados, perdas e conquista.
- `moverTropas(origem, destino, quantidade)` executa movimentos com validação.
- `avancarFase()` passa pelas fases `Reforco → Ataque → Movimentacao → Fim → proximo jogador`.

O terminal permanece implementado como frontend simples em `Jogo.cpp`. Para a etapa 2D, mover seus métodos de interface (`mostrarMapa`, `mostrarEstado`, `executarTurnoHumano`) para uma classe `InterfaceTerminal`, mantendo as operações de regras no `Jogo`. Essa separação final é recomendada antes de implementar a GUI.

## Plano incremental sugerido

1. Escolher uma biblioteca (SFML, raylib ou SDL2) conforme facilidade de compilação no WSL e disponibilidade de ambiente gráfico.
2. Criar módulo independente de apresentação `visual/` e um executável `war_2d` opcional no CMake.
3. Desenhar um mapa educacional original (sem reutilizar arte comercial), com posições e hitboxes para seleção dos 42 territórios.
4. Clicar em territórios para selecionar origem e alvo; mostrar adjacências e tropas; acionar a API do motor.
5. Exibir painel de jogadores/objetivos, dados de ataque animados e mensagem de vitória.
6. Manter `make`, `make test` e o modo terminal funcionando para quem não tiver biblioteca gráfica.

**Estado:** a primeira versão da interface 2D (raylib) já foi implementada; os itens acima ficam como referência para evoluções futuras.
