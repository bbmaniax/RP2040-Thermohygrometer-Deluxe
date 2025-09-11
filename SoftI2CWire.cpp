/*
  SoftI2CWire Library
*/

#include "SoftI2CWire.h"

#include <Arduino.h>
#include <SoftI2C.h>

#include "DebugSerial.h"

SoftI2CWire::SoftI2CWire(uint8_t sdaPin, uint8_t sclPin) : i2c(sdaPin, sclPin) {}

SoftI2CWire::~SoftI2CWire() {}

void SoftI2CWire::begin() {
  i2c.begin();
}

void SoftI2CWire::begin(uint8_t address) {
  // Not supported in SoftI2C
}

void SoftI2CWire::begin(int address) {
  // Not supported in SoftI2C
}

void SoftI2CWire::end() {
  i2c.end();
}

void SoftI2CWire::setClock(uint32_t clock) {
  // Not supported in SoftI2C
}

void SoftI2CWire::setWireTimeout(uint32_t timeout, bool reset_with_timeout) {
  // Not supported in SoftI2C
}

bool SoftI2CWire::getWireTimeoutFlag() {
  // Not supported in SoftI2C
  return false;
}

void SoftI2CWire::clearWireTimeoutFlag() {
  // Not supported in SoftI2C
}

void SoftI2CWire::beginTransmission(uint8_t address) {
  i2c.beginTransmission(address);
}

void SoftI2CWire::beginTransmission(int address) {
  i2c.beginTransmission(address);
}

uint8_t SoftI2CWire::endTransmission() {
  return i2c.endTransmission();
}

uint8_t SoftI2CWire::endTransmission(uint8_t sendStop) {
  return i2c.endTransmission(sendStop);
}

uint8_t SoftI2CWire::requestFrom(uint8_t address, uint8_t quantity) {
  return i2c.requestFrom(address, quantity);
}

uint8_t SoftI2CWire::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop) {
  return i2c.requestFrom(address, quantity, sendStop);
}

uint8_t SoftI2CWire::requestFrom(uint8_t address, uint8_t quantity, uint32_t iaddress, uint8_t isize, uint8_t sendStop) {
  // Not supported in SoftI2C
  return 0xFF;
}

uint8_t SoftI2CWire::requestFrom(int address, int quantity) {
  return i2c.requestFrom(address, quantity);
}

uint8_t SoftI2CWire::requestFrom(int address, int quantity, int sendStop) {
  return i2c.requestFrom(address, quantity, sendStop);
}

size_t SoftI2CWire::write(uint8_t data) {
  return i2c.write(data);
}

size_t SoftI2CWire::write(const uint8_t* data, size_t quantity) {
  return i2c.write(data, quantity);
}

size_t SoftI2CWire::write(unsigned long n) {
  return i2c.write((uint8_t)n);
}

size_t SoftI2CWire::write(long n) {
  return i2c.write((uint8_t)n);
}

size_t SoftI2CWire::write(unsigned int n) {
  return i2c.write((uint8_t)n);
}

size_t SoftI2CWire::write(int n) {
  return i2c.write((uint8_t)n);
}

int SoftI2CWire::available() {
  return i2c.available();
}

int SoftI2CWire::read() {
  return i2c.read();
}

int SoftI2CWire::peek() {
  return i2c.peek();
}

void SoftI2CWire::flush() {
  // Not supported in SoftI2C
}

void SoftI2CWire::onReceive(void (*callback)(int)) {
  // Not supported in SoftI2C
}

void SoftI2CWire::onRequest(void (*callback)()) {
  // Not supported in SoftI2C
}
