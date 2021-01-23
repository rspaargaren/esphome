#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "dsmr.h"
#include "parser.h"

namespace esphome {
namespace dsmr {

const uint16_t Max_Telegraph_Length = 1500;

class Dsmr : public PollingComponent, public uart::UARTDevice {
 public:
  void setup() override;
  void loop() override;
  void update() override;
  float get_setup_priority() const override;
  void dump_config() override;

 protected:
  bool header_found_;
  bool footer_found_;
  uint16_t telegramlen_;
  uint8_t bytes_read_;
  char telegram_[Max_Telegraph_Length];
  char tel_char_;

}  // namespace dsmr
}  // namespace esphome
