#pragma once
#include "Enums.h"
#include <string>

class Carta {
    std::string territorio_;
public:
    explicit Carta(std::string territorio);
    virtual ~Carta() = default;
    const std::string& territorio() const noexcept { return territorio_; }
    virtual TipoCarta tipo() const noexcept = 0;
    virtual std::string descricao() const = 0;
};
class CartaInfantaria final : public Carta {
public:
    using Carta::Carta;
    TipoCarta tipo() const noexcept override { return TipoCarta::Infantaria; }
    std::string descricao() const override;
};
class CartaCavalaria final : public Carta {
public:
    using Carta::Carta;
    TipoCarta tipo() const noexcept override { return TipoCarta::Cavalaria; }
    std::string descricao() const override;
};
class CartaArtilharia final : public Carta {
public:
    using Carta::Carta;
    TipoCarta tipo() const noexcept override { return TipoCarta::Artilharia; }
    std::string descricao() const override;
};
