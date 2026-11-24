#pragma once
#include <stdexcept>
#include <string>

// Excecao lancada quando uma jogada desrespeita as regras do WAR.
// Herda de std::runtime_error, uma classe da biblioteca padrao.
class WarException : public std::runtime_error {
public:
    explicit WarException(const std::string& mensagem) : std::runtime_error(mensagem) {}
};
