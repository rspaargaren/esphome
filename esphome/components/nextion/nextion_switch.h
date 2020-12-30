#pragma once

#include "esphome/core/component.h"
#include "esphome/components/nextion/nextion.h"
#include "esphome/components/sensor/sensor.h"

namespace esphome {
namespace nextion {
class NextionSwitch;

class NextionSwitch : public NextionComponent, public switch_::Switch, public Component, public uart::UARTDevice {
 public:
  NextionSwitch(Nextion *nextion) { this->nextion_ = nextion; }
  void process(uint8_t page_id, uint8_t component_id, bool on) override;

 protected:
  void send_command_no_ack(const char *command);
  bool send_command_printf(const char *format, ...);

  void write_state(bool state) override;
  Nextion *nextion_;
};
}  // namespace nextion
}  // namespace esphome