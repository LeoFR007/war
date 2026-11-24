# Limpeza do projeto: versão final

Cópia otimizada de `~/projetos/war` para demonstração, apresentação e execução. **O projeto original não foi modificado.**

| | Original (`war/`) | Versão final (`war-versao-final/`) |
|---|---|---|
| Tamanho | 3,3 MB | **2,0 MB** (−39%) |
| Arquivos | 53 | 50 (49 do projeto + este relatório) |
| Código-fonte (model, view, controller, main) | igual | **igual, sem nenhuma alteração** |

## Critério usado

Um item só saiu se nenhuma parte da aplicação o usa para compilar ou executar. Itens com dúvida foram **mantidos**. O exemplo é o UML: não é usado para rodar, mas o enunciado da disciplina exige a imagem e o arquivo-fonte no repositório.

## Arquivos removidos

| Arquivo | Por que saiu |
|---|---|
| `tests/testes.cpp` | Testes automáticos não são necessários para compilar nem para rodar o jogo. Continuam no projeto original (`make test` lá). |
| `docs/UML.svg` | Duplicata do `UML.png` em outro formato. |
| `docs/RELATORIO.md` | Texto de documentação que repete o README; não é usado pela aplicação nem pela apresentação. |
| `docs/EXEMPLO_EXECUCAO.txt` | Exemplo de saída do terminal. Qualquer pessoa reproduz com `make demo`. |
| `docs/interface.png` | Substituído por `docs/interface.jpg`: a mesma imagem, de 767 KB para 161 KB. |

## Pastas removidas

| Pasta | Por que saiu |
|---|---|
| `tests/` | Ficou vazia após a remoção do arquivo de testes. |

Não havia arquivos temporários, logs, backups, cache nem executáveis no original (`make clean` já tinha sido rodado). Por isso nada desse tipo precisou ser removido.

## Arquivos ajustados (só nesta cópia)

| Arquivo | Ajuste |
|---|---|
| `Makefile` | Removidos os alvos `war_testes` e `test` e a geração do `.svg` no alvo `uml`; `clean` não cita mais `war_testes`. Comandos que ficam: `make jogar`, `make`, `make run`, `make demo`, `make uml`, `make clean`. |
| `README.md` | A imagem aponta para `docs/interface.jpg`; saíram `make test` e a pasta `tests/`; a estrutura descreve a pasta nova; foi incluído o link para `docs/MODELAGEM_UML.md`. |
| `docs/MODELAGEM_UML.md` | Saiu a menção ao `UML.svg`. |
| `docs/UML.png` | Mesma imagem (4723×3687), salva com paleta indexada: de 1,1 MB para 407 KB, mantendo a legibilidade. |
| `.gitignore` | Saiu a entrada `war_testes`. |

## O que foi mantido e por quê

| Item | Motivo |
|---|---|
| `model/`, `view/`, `controller/`, `main/` | Código da aplicação: tudo é compilado pelo Makefile. Nenhuma classe ou função sem uso foi encontrada além de um getter simples (`Carta::getTerritorio`), mantido por fazer parte do UML. |
| `assets/fonts/Ubuntu.ttf` | Único asset carregado pelo programa (`Fontes::carregar`). Sem ele a janela não abre. |
| `assets/fonts/LICENSE-Ubuntu.txt` | Exigido pela licença da fonte para redistribuí-la. |
| `docs/UML.png`, `docs/UML.dot` | O enunciado exige a imagem do diagrama **e** o arquivo-fonte da ferramenta. |
| `docs/MODELAGEM_UML.md` | Critério "descrição do padrão UML". |
| `docs/interface.jpg` | Mostrada no README. |
| `Makefile`, `.gitignore`, `README.md` | Build, organização do repositório e instruções de uso. |

## Validação

| Verificação | Resultado |
|---|---|
| Compilação com `-Wall -Wextra -Wpedantic` (terminal e janela) | **0 avisos, 0 erros** |
| 25 partidas automáticas (2 a 6 jogadores, 5 sementes) comparadas com o original | **saída idêntica**, byte a byte (10.577 linhas) |
| Telas da interface (reforço, ataque, zoom, jogada inválida, vitória) comparadas com o original | **5 de 5 idênticas**, pixel a pixel |
| `make jogar` | compila e abre a janela |
| Jogada inválida no terminal | recusada com a mensagem da `WarException` |
| `docs/UML.dot` | regenera com Graphviz sem erro |
| Links do README | todos apontam para arquivos existentes |
| Referências a arquivos removidos | nenhuma |

## Estrutura final

```
war-versao-final/
├── Makefile                 make jogar · make · make run · make demo · make uml · make clean
├── README.md                como compilar, jogar, regras e conceitos de POO
├── LIMPEZA_PROJETO.md       este relatório
├── .gitignore
├── assets/fonts/            Ubuntu.ttf + licença
├── controller/              ControladorJogo (entrada do usuário)
├── docs/                    UML.png, UML.dot, MODELAGEM_UML.md, interface.jpg
├── main/                    main.cpp (terminal) · main_gui.cpp (janela)
├── model/                   regras: Jogo, Mapa, Territorio, Continente, Carta,
│                            Jogador → JogadorHumano | JogadorBot,
│                            Objetivo → 3 tipos, WarException
└── view/                    desenho SFML: MapaView, Camera, RegioesMapa, Animacoes,
                             PainelView, EventosView, BarraSuperiorView, Desenho, Tema
```

## Como usar

```bash
cd ~/projetos/war-versao-final
make jogar          # interface gráfica (requer: sudo apt install libsfml-dev)
make run            # partida no terminal
make demo           # partida automática entre bots
```
