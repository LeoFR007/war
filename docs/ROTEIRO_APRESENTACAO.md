# Roteiro de apresentação — Projeto WAR / POO

Sugestão de demonstração rápida, adaptável à duração e ao número de integrantes.

1. **Visão geral**: escolher WAR, explicar as mecânicas do clone e mostrar o README/repositório.
2. **UML**: abrir `docs/UML.png` e mostrar composição (`Jogo`–`Mapa`), agregação (`Continente`–`Territorio`) e associação (`Jogador`–`Territorio`). Mostrar o fonte `UML.dot`.
3. **POO**: abrir `include/Objetivo.h` e `include/Carta.h` para explicar as **duas hierarquias distintas de herança**. Mencionar `std::unique_ptr` e virtual puro.
4. **Lambda e exceções**: apontar ordenação dos dados em `src/Territorio.cpp`; mostrar `Exceptions.h` e os `try/catch` no controlador do terminal.
5. **Mecânicas em execução**: `make test`, `make demo JOGADORES=4 SEED=42 MAX_TURNOS=50` e, se houver tempo, `make run` para mostrar um reforço, um ataque e uma movimentação.
6. **Próxima etapa**: explicar que há métodos públicos em `Jogo` para conectar uma visualização 2D sem reescrever regras.

## Regras de demonstração

No WSL, a partir da raiz do projeto:

```bash
make
make test
make demo JOGADORES=4 SEED=42 MAX_TURNOS=50
make run JOGADORES=3
```

O código deve estar entendido por **todos** os participantes. A divisão da apresentação e das contribuições deve refletir o histórico verdadeiro do repositório, sem inventar participação.

## Demonstracao visual (extra)

Com WSL 2 + WSLg instalado, iniciar `make gui`. Mostrar o menu inicial, mapa com 42 territorios, clique para reforcar, ataque com ate tres dados, movimentacao e objetivo secreto (tecla O). Explicar que `gui/main_gui.cpp` apenas apresenta a interface; regras e validacoes sao executadas pelas mesmas classes C++17 em `war_core`, sem duplicacao de regras. Caso o computador nao permita WSLg, manter a demonstracao do terminal com `make run`.
