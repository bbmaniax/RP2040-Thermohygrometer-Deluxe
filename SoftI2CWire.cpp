// SoftI2CWire

#include <Arduino.h>
#include <SoftI2C.h>

#include "DebugSerial.h"
#include "SoftI2CWire.h"

SoftI2CWire::SoftI2CWire(SoftI2C& softI2C) : softI2C(softI2C) {}

void SoftI2CWire::begin() {
  softI2C.begin();
}

void SoftI2CWire::end() {
  softI2C.end();
}

void SoftI2CWire::beginTransmission(uint8_t address) {
  softI2C.beginTransmission(address);
}

void SoftI2CWire::beginTransmission(int address) {
  softI2C.beginTransmission(address);
}

uint8_t SoftI2CWire::endTransmission() {
  return softI2C.endTransmission();
}

uint8_t SoftI2CWire::endTransmission(uint8_t sendStop) {
  return softI2C.endTransmission(sendStop);
}

uint8_t SoftI2CWire::requestFrom(uint8_t address, uint8_t quantity) {
  return softI2C.requestFrom(address, quantity);
}

uint8_t SoftI2CWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop) {
  return softI2C.requestFrom(address, quantity, sendStop);
}

uint8_t SoftI2CWire::requestFrom(uint8_t address, uint8_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop) {
  // Not supported in SoftI2C
  return 0xFF;
}

uint8_t SoftI2CWire::requestFrom(int address, int quantity) {
  return softI2C.requestFrom(address, quantity);
}

uint8_t SoftI2CWire::requestFrom(int address, int quantity, int sendStop) {
  return softI2C.requestFrom(address, quantity, sendStop);
}

size_t SoftI2CWire::write(uint8_t data) {
  return softI2C.write(data);
}

size_t SoftI2CWire::write(const uint8_t* data, size_t quantity) {
  return softI2C.write(data, quantity);
}

size_t SoftI2CWire::write(unsigned long n) {
  return softI2C.write((uint8_t)n);
}

size_t SoftI2CWire::write(long n) {
  return softI2C.write((uint8_t)n);
}

size_t SoftI2CWire::write(unsigned int n) {
  return softI2C.write((uint8_t)n);
}

size_t SoftI2CWire::write(int n) {
  return softI2C.write((uint8_t)n);
}

int SoftI2CWire::available() {
  return softI2C.available();
}

int SoftI2CWire::read() {
  return softI2C.read();
}

int SoftI2CWire::peek() {
  return softI2C.peek();
}

void SoftI2CWire::flush() {
  // Not supported in SoftI2C
}
