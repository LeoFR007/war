# Conferencia de aderencia — Trabalho T1 (2026/2)

Base da conferência: **Enunciado T1 - Jogo Livre.pdf**, disciplina Programação Orientada a Objetos, professor Anderson Domingues. Esta conferência foi feita com base no PDF oficial fornecido, e não apenas no prompt inicial de desenvolvimento.

## Matriz dos critérios de avaliação

| Critério oficial | Evidências na versão entregue | Situação |
|---|---|---|
| 1 ponto — modelagem e descrição do padrão UML | `docs/UML.png`, `docs/UML.svg`, `docs/UML.dot`, `docs/MODELAGEM_UML.md` | Preparado para avaliação |
| 1 ponto — documentação e organização do repositório | `README.md`, `docs/RELATORIO.md`, `.gitignore`, `include/`, `src/`, `tests/` | Preparado para avaliação |
| 1 ponto — apresentação em aula | `docs/ROTEIRO_APRESENTACAO.md`; apresentação ainda deve ser realizada pelo grupo | **Pendente de execução** |
| 5 pontos — implementação das mecânicas principais | Jogo terminal para 2–6 jogadores, 42 territórios, reforços, ataques, cartas, objetivos, deslocamento, continentes, turnos; `make test` | Implementado e testado |
| 1 ponto — ao menos 1 lambda e 2 exemplos de herança | `src/Territorio.cpp` (`std::sort` com lambda); heranças de `Objetivo` e `Carta`; ainda há herança de exceções | Implementado |
| 1 ponto — tratamento de exceções | `include/Exceptions.h` e `try/catch` em `src/Jogo.cpp`, `main.cpp` | Implementado |
| 2 pontos extras — interface gráfica | **Implementada como opção** em `gui/main_gui.cpp` com raylib, CMake (`WAR_BUILD_GUI`), `make gui`, mapa clicável, reforços, ataques, movimentos, objetivos e menus. | **Código implementado; validação visual no WSLg do usuário pendente** |

## Exigências administrativas que o código não consegue comprovar

- [ ] **Jogo WAR previamente combinado/aprovado pelo professor**.
- [ ] **Criar e publicar repositório GitHub, GitLab ou Bitbucket** (o ZIP não substitui o link solicitado).
- [ ] **Todos os integrantes do grupo cadastrados como colaboradores** e com suas contribuições registradas no histórico.
- [ ] **Publicar a imagem do UML e também o arquivo-fonte editável `docs/UML.dot`** no repositório.
- [ ] Conferir a entrega do link **até 01/11/2026 às 23h50** (prazo indicado no PDF).
- [ ] Preparar e apresentar o projeto em aula.

## Observações técnicas / regras simplificadas

O professor não obriga reproduzir cada regra específica de uma edição comercial, mas exige demonstrar **as principais mecânicas do jogo escolhido**. Assim, este projeto usa o mapa clássico de 42 territórios, distribuição inicial simplificada de 8 tropas extras por jogador, defesa automática com até 3 dados, recompensas de cartas por conquista, bônus por continentes, objetivos de controle e eliminação e trocas progressivas `4,6,8,...`. Não há arte comercial nem modo em rede. Essas escolhas constam no README e devem ser explicadas na apresentação.

**Atenção:** a matriz indica evidências e estado de implementação; não garante nota. A atribuição de pontos cabe exclusivamente ao professor.
