# Relatório técnico — Clone educacional de WAR em C++

## 1. Identificação do projeto

**Título:** Jogo WAR: modelagem UML e orientação a objetos em C++17.  
**Tipo:** Implementação de um jogo de tabuleiro de conquista territorial.  
**Forma de execução:** Terminal de texto; 2 a 6 jogadores humanos ou bots demonstrativos.

## 2. Introdução e objetivo

O software recria as mecânicas essenciais de um jogo de conquista territorial, incluindo mapas, continentes, domínios, distribuição de exércitos, turnos, combate com dados, cartas e objetivos pessoais. O foco do projeto é utilizar conceitos de POO de forma verificável: classes com responsabilidades específicas, herança, abstração, composição, agregação, associações, lambdas, enumerações e tratamento de erros.

## 3. Modelagem UML e arquitetura

**Diagrama principal:** `UML.png` ou `UML.svg` (versão com zoom). **Fonte editável obrigatória:** `UML.dot`.  
**Fonte do diagrama:** `UML.dot` (Graphviz).

O padrão geral é separar o motor de regras (`Jogo`, `Mapa`, `Jogador`, `Territorio`, `Continente`) dos objetos de regras polimórficas (`Objetivo`, `Carta`). Cada classe expõe métodos que encapsulam seus invariantes. Referências entre entidades não transferem posse de memória; proprietários usam `std::unique_ptr`.

## 4. Diagrama de turnos (fluxo)

```text
Inicializar mapa -> distribuir territórios -> definir objetivos -> montar baralho
                                      |
                                      v
                   Identificar próximo jogador ativo
                                      |
                                      v
                      Distribuir reforços / trocar cartas
                                      |
                                      v
                            Atacar (opcional)
                         /                    \
               Conquistou território?        Sem conquista
                     |                              |
               Testar vitória                      |
                     |                              |
                     +------------------------------+
                                      |
                                      v
                         Movimentar tropas (opcional)
                                      |
                                      v
                  Comprar 1 carta, se houve conquista
                                      |
                                      v
                       Verificar vitória e avançar
```

## 5. Matriz de aderência aos requisitos

| Requisito | Implementação / evidência |
|---|---|
| Jogo, mapa, jogadores, territórios, continentes | `include/*.h`, `src/*.cpp` |
| Herança 1: classes de objetivo | `include/Objetivo.h`, `src/Objetivo.cpp` |
| Herança 2: classes de carta | `include/Carta.h`, `src/Carta.cpp` |
| Classe abstrata e polimorfismo | `Objetivo::foiConcluido()` e `Carta::tipo()/descricao()` |
| 4 enumerações | `include/Enums.h` |
| Dados até 3 x 3 e empates favoráveis à defesa | `src/Territorio.cpp` |
| Lambda em `std::sort` | `src/Territorio.cpp` |
| Exceções customizadas | `include/Exceptions.h` |
| `try/catch` em ações críticas | `src/Jogo.cpp` |
| Múltiplos jogadores / objetivos / turnos | `src/Jogo.cpp`, `main.cpp` |
| UML com classes e relacionamentos | `docs/UML.png`, `docs/UML.svg` |
| Estrutura modular `.h` e `.cpp` | `include/`, `src/` |
| Testes de regras, fases e troca obrigatória | `tests/testes.cpp` |
| Projeto publicável como repositório | `.gitignore`, `README.md` |
| README com instruções de compilação | `README.md` |

## 6. Demonstração de orientação a objetos

**Encapsulamento:** atributos como `Territorio::tropas_` e `Jogador::exercitosDisponiveis_` são privados e só alterados por operações validadas.

**Abstração:** `Objetivo` e `Carta` não podem ser instanciadas diretamente, pois definem métodos virtuais puros.

**Herança e polimorfismo:** classes derivadas implementam diferentes regras, mas podem ser utilizadas pelos mesmos ponteiros `Objetivo` e `Carta`.

**Composição:** `Jogo` possui seus jogadores e o mapa; o mapa possui territórios e continentes. Destruição automática por RAII.

**Agregação:** `Continente` agrega territórios, que pertencem ao `Mapa`.

**Associação:** `Territorio` aponta para `Jogador` dono e vizinhos; `Jogador` aponta para seus territórios.

**Lambda:** `const auto decrescente = [](int a, int b) { return a > b; };` com `std::sort` no combate.

**Exceções:** operações ilegais geram exceções específicas que são capturadas na interface por `try/catch`.

## 7. Estratégia de testes

1. Construção do tabuleiro com 42 territórios, seis continentes e adjacências simétricas.
2. Atribuição de donos, reforço e atualizações de exércitos.
3. Detecção de ataque inválido e movimento entre territórios que não são vizinhos.
4. Combate e transferência de propriedade após a defesa chegar a zero.
5. Objetivos de conquista territorial, continental e eliminação.
6. Troca de cartas de tipos idênticos e variados, obrigatoriedade quando mão tem 5 ou mais cartas.
7. Transferência de cartas do jogador eliminado ao vencedor da eliminação.
8. Invariantes e restrições de mudança de fase no motor (API pública).
9. Simulação automática em cenário aleatório reproduzível por semente.

**Comandos:** `cmake -S . -B build && cmake --build build && ctest --test-dir build --output-on-failure`.

## 8. Limitações e possíveis evoluções

A versão não implementa interface gráfica, escolhas de dados de defesa, todas as regras especiais de diferentes edições comerciais ou multiplayer em rede. A escolha das cartas-objetivo é determinística e a distribuição inicial é simplificada. O motor agora expõe operações por fase para facilitar um frontend 2D em SFML, raylib ou SDL2; a etapa gráfica ainda depende de implementação e testes específicos. Consulte `EVOLUCAO_2D.md`.

## 9. Conclusão

O projeto consolida os pilares de POO em um domínio concreto com regras não triviais. O modelo de classes permite modificar os tipos de objetivos, cartas e a apresentação visual preservando as regras centrais. A compilação via CMake e a suíte de testes facilitam a reprodução e a validação do projeto.
