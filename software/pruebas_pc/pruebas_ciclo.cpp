// Pruebas del ciclo de compactador_v3.ino en un PC, con un modelo simple del actuador.
// Compilar y ejecutar:  g++ -std=c++17 -o pruebas pruebas_ciclo.cpp && ./pruebas
#include "Arduino.h"
#include <cassert>
#include <iostream>

namespace sim {
  unsigned long t = 0;
  int pinVal[20]; int pinModeArr[20];
  bool botonVerde = false, botonRojo = false;
  std::function<double(unsigned long)> corriente = [](unsigned long) { return 0.0; };
  double sensibilidad = 0.100, cero = 2.50;
  std::vector<std::string> log;
  int ambosActivos = 0;
  int writesAntesDeOutput = 0;
}
SerialSim Serial;

#include "../src/compactador_v3/compactador_v3.ino"   // el programa real, sin copiarlo

// ---------------- modelo del actuador
struct Modelo {
  double x = 0;              // mm: 0 = arriba (retraído), 300 = abajo (extendido)
  double contacto = 1e9;     // mm donde la placa toca las latas
  double rigidez = 0.10;     // A por mm de compresión después del contacto
  double obstruccionSubida = -1; // mm donde algo traba la subida (-1 = nada)
  int dirPrev = 0; unsigned long arranque = 0;
  int dir() const {
    bool av = sim::pinVal[4] == LOW, rt = sim::pinVal[5] == LOW;
    if (av && !rt) return +1;
    if (rt && !av) return -1;
    return 0;
  }
  double I(unsigned long t) const {
    int d = dir();
    if (d == 0) return 0;
    if (d > 0 && x >= 300) return 0;     // fin de carrera interno inferior
    if (d < 0 && x <= 0) return 0;       // fin de carrera interno superior
    double i = 1.0;                      // corriente en vacío
    if (d > 0 && x > contacto) i += rigidez * (x - contacto);
    if (d < 0 && obstruccionSubida >= 0 && x <= obstruccionSubida + 0.5) i = 4.5;
    if (t - arranque < 200) i = std::max(i, 6.0);   // pico de arranque
    return i;
  }
  void paso(double dt_s) {
    int d = dir();
    if (d != dirPrev) { arranque = sim::t; dirPrev = d; }
    if (d == 0) return;
    double v = 6.0;                      // mm/s
    double i = I(sim::t);
    if (d > 0 && x > contacto && i >= 3.5) v = 0;   // la placa ya no avanza
    if (d < 0 && obstruccionSubida >= 0 && x <= obstruccionSubida + 0.5) v = 0;
    x += d * v * dt_s;
    if (x < 0) x = 0;
    if (x > 300) x = 300;
  }
};

Modelo M;
const char *nombreEstado() {
  static const char *n[] = {"REPOSO", "AVANCE", "INVERSION", "RETORNO", "EMERGENCIA"};
  return n[estado];
}
void correr(double segundos) {
  unsigned long fin = sim::t + (unsigned long)(segundos * 1000);
  while (sim::t < fin) { loop(); sim::t += 3; M.paso(0.003); }
}
void pulsar(bool &b, double ms = 150) { b = true; correr(ms / 1000.0); b = false; correr(0.1); }

void reiniciar(double contacto) {
  sim::t = 0; sim::log.clear(); sim::ambosActivos = 0;
  for (int i = 0; i < 20; i++) { sim::pinVal[i] = -1; sim::pinModeArr[i] = 0; }
  M = Modelo(); M.contacto = contacto;
  sim::corriente = [](unsigned long t) { return M.I(t); };
  estado = REPOSO; verdeEstable = verdeLecturaAnterior = false;
  setup();
  assert(sim::pinVal[4] == HIGH && sim::pinVal[5] == HIGH);   // relés apagados al encender
}
void imprimirEventos() {
  for (auto &l : sim::log) if (l.find(">>") != std::string::npos || l.find("maxima del ciclo") != std::string::npos) std::cout << "   " << l << "\n";
}

int main() {
  // ---- A: 5 latas paradas; la placa las toca a 180 mm y la corriente sube 0,1 A por mm
  reiniciar(180);
  pulsar(sim::botonVerde);
  assert(estado == AVANCE);
  double maxReal = 0;                 // corriente real máxima con el relé de avance activo
  unsigned long tUmbral = 0, tCorte = 0;
  for (int i = 0; i < 20000 && estado == AVANCE; i++) {
    loop();
    if (sim::pinVal[4] == LOW) {
      double ireal = M.I(sim::t);
      if (sim::t - M.arranque > 300) maxReal = std::max(maxReal, ireal);
      if (tUmbral == 0 && sim::t - M.arranque > 400 && ireal >= 2.9) tUmbral = sim::t;
    } else if (tCorte == 0) tCorte = sim::t;
    sim::t += 3; M.paso(0.003);
  }
  double xLimite = M.x;
  assert(estado == INVERSION);
  assert(maxReal < 3.0);                         // nunca llega a 3 A
  // El ADC del UNO mide en escalones de ~49 mA con el ACS712-20A: el corte ocurre dentro de
  // un escalón sobre 2,9 A, siempre bajo 3 A.
  assert(maxReal < 2.95);
  (void)tUmbral; (void)tCorte;
  unsigned long tInicioInversion = sim::t;
  while (estado == INVERSION) { loop(); sim::t += 3; M.paso(0.003); }
  assert(estado == RETORNO && sim::pinVal[5] == LOW);
  unsigned long msHastaRetroceso = sim::t - tInicioInversion;
  assert(msHastaRetroceso <= 60);
  correr(70); assert(estado == REPOSO); assert(M.x == 0);
  assert(sim::ambosActivos == 0);
  std::cout << std::unitbuf << "A) 5 latas: corte en x = " << xLimite << " mm; corriente real maxima = " << maxReal
            << " A (< 3 A); retroceso a los " << msHastaRetroceso << " ms del corte. OK\n";
  imprimirEventos();

  // ---- B: sin latas: llega al fin de carrera inferior (300 mm) y vuelve
  reiniciar(1e9);
  pulsar(sim::botonVerde);
  for (int i = 0; i < 30000 && estado == AVANCE; i++) { loop(); sim::t += 3; M.paso(0.003); }
  assert(estado == INVERSION && M.x > 299);
  correr(60); assert(estado == REPOSO && M.x == 0);
  assert(sim::ambosActivos == 0);
  std::cout << "B) sin latas: fin de carrera inferior -> retorno -> reposo. OK\n";
  imprimirEventos();

  // ---- C: paro de emergencia a los 10 s
  reiniciar(180);
  pulsar(sim::botonVerde);
  correr(10);
  sim::botonRojo = true; loop();
  assert(estado == EMERGENCIA && sim::pinVal[4] == HIGH && sim::pinVal[5] == HIGH);
  double xParo = M.x;
  correr(1); pulsar(sim::botonVerde);          // verde con el rojo aún presionado: no hace nada
  assert(estado == EMERGENCIA && M.x == xParo);
  sim::botonRojo = false; correr(0.2);
  pulsar(sim::botonVerde);                      // ahora el verde solo hace subir
  assert(estado == RETORNO);
  correr(70); assert(estado == REPOSO && M.x == 0);
  assert(sim::ambosActivos == 0);
  std::cout << "C) paro de emergencia en x = " << xParo << " mm: reles apagados en el mismo ciclo; retorno con verde. OK\n";
  imprimirEventos();

  // ---- D: el pico de arranque de 6 A (200 ms) no debe detener el avance
  reiniciar(1e9);
  pulsar(sim::botonVerde);
  correr(2); assert(estado == AVANCE);
  std::cout << "D) pico de arranque ignorado. OK\n";

  // ---- E: algo traba la subida a 100 mm -> se detiene y queda en emergencia
  reiniciar(1e9);
  M.obstruccionSubida = 100;
  pulsar(sim::botonVerde);
  correr(55); correr(70);
  assert(estado == EMERGENCIA && sim::pinVal[4] == HIGH && sim::pinVal[5] == HIGH);
  std::cout << "E) sobrecorriente al subir -> detenido en x = " << M.x << " mm. OK\n";
  imprimirEventos();

  std::cout << "\nTodas las pruebas pasaron. Ambos reles activos a la vez: " << sim::ambosActivos << " veces.\n";
  return 0;
}
