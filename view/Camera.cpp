#include "Camera.h"
#include "Tema.h"
#include <algorithm>
#include <cmath>

Camera::Camera(sf::FloatRect area, sf::Vector2f tamanhoMundo) : area(area), tamanhoMundo(tamanhoMundo) {
    zoomMinimo = std::min(area.width / tamanhoMundo.x, area.height / tamanhoMundo.y);
    zoomMaximo = zoomMinimo * 3.0f;
    centralizar();
    zoom = zoomAlvo;
    centro = centroAlvo;
}

void Camera::centralizar() {
    zoomAlvo = zoomMinimo;
    centroAlvo = tamanhoMundo / 2.0f;
}

// Suavizacao exponencial: a cada quadro anda uma fracao do caminho ate o alvo.
void Camera::atualizar(float segundos) {
    // não pula direto pro zoom novo: a cada quadro anda um pedaço do caminho.
    // usar exp() deixa a velocidade igual mesmo se o FPS mudar
    float fator = 1 - std::exp(-segundos * 12);
    zoom += (zoomAlvo - zoom) * fator;
    centro += (centroAlvo - centro) * fator;
}

void Camera::aproximar(float passos, sf::Vector2f pontoTela) {
    // O ponto do mundo sob o cursor deve continuar sob o cursor depois do zoom.
    // ideia: descobre qual ponto do mapa está embaixo do mouse, muda o zoom e depois
    // recalcula o centro pra esse mesmo ponto continuar embaixo do mouse.
    // é isso que faz o zoom "ir na direção" do cursor em vez de ir pro meio
    sf::Vector2f sobCursor = centroAlvo + (pontoTela - sf::Vector2f(area.left + area.width / 2, area.top + area.height / 2)) / zoomAlvo;
    zoomAlvo = std::max(zoomMinimo, std::min(zoomMaximo, zoomAlvo * std::pow(1.2f, passos)));
    centroAlvo = sobCursor - (pontoTela - sf::Vector2f(area.left + area.width / 2, area.top + area.height / 2)) / zoomAlvo;
    limitar();
}

void Camera::arrastar(sf::Vector2f deslocamentoTela) {
    centroAlvo -= deslocamentoTela / zoomAlvo;
    centro -= deslocamentoTela / zoom;   // arrastar responde na hora, sem atraso
    limitar();
}

// Nao deixa a camera sair do mapa.
void Camera::limitar() {
    float meiaLargura = area.width / 2 / zoomAlvo;
    float meiaAltura = area.height / 2 / zoomAlvo;
    if (meiaLargura * 2 >= tamanhoMundo.x) centroAlvo.x = tamanhoMundo.x / 2;
    else centroAlvo.x = std::max(meiaLargura, std::min(tamanhoMundo.x - meiaLargura, centroAlvo.x));
    if (meiaAltura * 2 >= tamanhoMundo.y) centroAlvo.y = tamanhoMundo.y / 2;
    else centroAlvo.y = std::max(meiaAltura, std::min(tamanhoMundo.y - meiaAltura, centroAlvo.y));
}

sf::Vector2f Camera::paraMundo(sf::Vector2f pontoTela) const {
    return centro + (pontoTela - sf::Vector2f(area.left + area.width / 2, area.top + area.height / 2)) / zoom;
}

sf::Vector2f Camera::paraTela(sf::Vector2f pontoMundo) const {
    return sf::Vector2f(area.left + area.width / 2, area.top + area.height / 2) + (pontoMundo - centro) * zoom;
}

sf::View Camera::visao() const {
    sf::View v(centro, sf::Vector2f(area.width / zoom, area.height / zoom));
    v.setViewport(sf::FloatRect(area.left / LARGURA_TELA, area.top / ALTURA_TELA,
                                area.width / LARGURA_TELA, area.height / ALTURA_TELA));
    return v;
}

bool Camera::contem(sf::Vector2f pontoTela) const { return area.contains(pontoTela); }

float Camera::getZoomRelativo() const { return zoom / zoomMinimo; }
