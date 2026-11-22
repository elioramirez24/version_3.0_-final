// Simulación mínima de Arduino para probar compactador_v3.ino en un PC.
#pragma once
#include <cstdio>
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <functional>

#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define A0 14

struct __FlashStringHelper;
#define F(s) (reinterpret_cast<const __FlashStringHelper *>(s))

namespace sim {
  extern unsigned long t;              // ms simulados
  extern int pinVal[20];
  extern int pinModeArr[20];
  extern bool botonVerde, botonRojo;   // true = presionado
  extern std::function<double(unsigned long)> corriente;  // A reales según el escenario
  extern double sensibilidad, cero;
  extern std::vector<std::string> log;
  extern int ambosActivos;             // veces que se vio a los 2 relés activos
  extern int writesAntesDeOutput;
}

inline unsigned long millis() { return sim::t; }
inline void delay(unsigned long ms) { sim::t += ms; }
inline void pinMode(int p, int m) { sim::pinModeArr[p] = m; }
inline void digitalWrite(int p, int v) {
  sim::pinVal[p] = v;
  if (sim::pinVal[4] == LOW && sim::pinVal[5] == LOW && sim::pinModeArr[4] == OUTPUT && sim::pinModeArr[5] == OUTPUT)
    sim::ambosActivos++;
}
inline int digitalRead(int p) {
  if (p == 6) return sim::botonVerde ? LOW : HIGH;
  if (p == 3) return sim::botonRojo ? LOW : HIGH;
  return HIGH;
}
inline int analogRead(int p) {
  (void)p;
  double v = sim::cero + sim::sensibilidad * sim::corriente(sim::t);
  int adc = (int)std::lround(v / 5.0 * 1023.0);
  return adc < 0 ? 0 : (adc > 1023 ? 1023 : adc);
}

struct SerialSim {
  std::string linea;
  void begin(long) {}
  void print(const char *s) { linea += s; }
  void print(const __FlashStringHelper *s) { linea += reinterpret_cast<const char *>(s); }
  void print(float f, int d = 2) { char b[32]; snprintf(b, sizeof b, "%.*f", d, f); linea += b; }
  void print(double f, int d = 2) { print((float)f, d); }
  void println(const char *s) { linea += s; flush(); }
  void println(const __FlashStringHelper *s) { linea += reinterpret_cast<const char *>(s); flush(); }
  void flush() {
    char b[24]; snprintf(b, sizeof b, "[%7.2fs] ", sim::t / 1000.0);
    sim::log.push_back(std::string(b) + linea);
    linea.clear();
  }
};
extern SerialSim Serial;
