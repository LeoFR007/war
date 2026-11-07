#pragma once
#include <stdexcept>
#include <string>

class WarException : public std::runtime_error {
public:
    explicit WarException(const std::string& mensagem) : std::runtime_error(mensagem) {}
};
class TerritorioNaoPertenceAoJogadorException : public WarException {
public:
    explicit TerritorioNaoPertenceAoJogadorException(const std::string& m) : WarException(m) {}
};
class AtaqueInvalidoException : public WarException {
public:
    explicit AtaqueInvalidoException(const std::string& m) : WarException(m) {}
};
class MovimentoInvalidoException : public WarException {
public:
    explicit MovimentoInvalidoException(const std::string& m) : WarException(m) {}
};
class TerritorioNaoAdjacenteException : public WarException {
public:
    explicit TerritorioNaoAdjacenteException(const std::string& m) : WarException(m) {}
};
