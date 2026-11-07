# Modelagem de classes e notação UML

## Arquivos da UML

- `UML.png`: diagrama completo em imagem, exigido no enunciado.
- `UML.svg`: mesma imagem vetorial, útil para leitura com zoom.
- `UML.dot`: **arquivo-fonte editável** do diagrama (ferramenta Graphviz).
- `UML_resumo.png` e `UML_resumo.dot`: visão de arquitetura mais simples para apresentações.

O arquivo `.dot` utiliza nós no formato *record* para representar classes com atributos e métodos, setas triangulares ocas para herança, diamante preenchido para composição, diamante vazio para agregação e associações direcionadas para as referências sem posse. A legenda fica expressa nos rótulos das relações.

## Relações principais

- **Composição:** `Jogo` possui `Mapa` e `Jogador`; `Mapa` possui `Continente` e `Territorio`; `Jogador` possui `Objetivo` e cartas da mão. Uso de `std::unique_ptr` e valores diretamente encapsulados.
- **Agregação:** `Continente` contém ponteiros não proprietários para os territórios do `Mapa`.
- **Associação:** `Jogador` referencia os territórios controlados; `Territorio` referencia dono e vizinhos; os objetivos podem referenciar jogadores e continentes.
- **Herança/polimorfismo:** três especializações de `Objetivo` e três de `Carta` implementam métodos virtuais puros; há, adicionalmente, quatro tipos específicos de exceção derivados de `WarException`.
- **Enumerações:** `CorJogador`, `TipoCarta`, `FaseTurno` e `ResultadoCombate` aparecem individualmente no diagrama principal.
- **Dependência:** `Territorio::atacar()` devolve `RelatorioCombate` com dados, perdas, posse e cartas capturadas.

## Gerar os diagramas novamente no WSL

```bash
sudo apt install -y graphviz
make uml
```

O código editável `.dot` deve ser versionado junto com as imagens. Gere novamente após alterações nas classes e corrija o `.dot` para que corresponda ao código.
