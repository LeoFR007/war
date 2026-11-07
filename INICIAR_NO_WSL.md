# Instalar e executar o WAR revisado no WSL / Ubuntu

## 1. Dependências

```bash
sudo apt update
sudo apt install -y build-essential cmake unzip
```

Para regenerar as figuras UML, instale Graphviz (opcional):

```bash
sudo apt install -y graphviz
```

## 2. Extrair a versão final

Se o arquivo `ProjetoWAR_Final_T1_Revisado.zip` estiver no Downloads do Windows, use:

```bash
cd /mnt/c/Users/SEU_USUARIO/Downloads
unzip ProjetoWAR_Final_T1_Revisado.zip
cd ProjetoWAR
```

Substitua `SEU_USUARIO` pelo usuário real do Windows. Execute os comandos a partir da raiz do projeto, onde estão `main.cpp`, `Makefile` e `CMakeLists.txt`.

**Se você já possui a pasta `war` de uma versão anterior**, faça uma cópia de segurança antes de substituir os arquivos. O projeto revisado contém novas assinaturas e testes; copiar só parte dos arquivos pode causar erros de compilação.

## 3. Compilar, testar e jogar

```bash
make
make test
make demo JOGADORES=4 SEED=42 MAX_TURNOS=50
make run JOGADORES=3
```

Para encerrar uma partida durante a execução, pressione `Ctrl+C`. Para remover arquivos compilados e reconstruir:

```bash
make clean
make rebuild
```

## 4. Alternativa usando CMake

```bash
cmake -S . -B build
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/war --jogadores 3
```

## 5. Arquivos importantes para entregar ao professor

- `README.md` e documentação de arquitetura
- `docs/UML.png` (**imagem do diagrama**)
- `docs/UML.dot` (**arquivo-fonte editável**)
- Todos os `src/*.cpp`, `include/*.h`, `main.cpp`, `CMakeLists.txt`, `Makefile`, `tests/`
- Verifique o checklist `docs/CONFERENCIA_T1.md`: link do GitHub, colaboradores, apresentação e aprovação do jogo dependem do grupo
