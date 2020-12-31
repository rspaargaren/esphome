#pragma once

#include "esphome/core/component.h"
#include "esphome/components/nextion/nextion.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace nextion {
class NextionSwitch;

class NextionSwitch : public NextionComponent, public switch_::Switch, public PollingComponent {
 public:
  NextionSwitch(Nextion *nextion) { this->nextion_ = nextion; }
  void process_bool(char *variable_name, bool on) override;
  void update() override;
  void nextion_setup() override;

 protected:
  Nextion *nextion_;
  void write_state(bool state) override;

  // void send_command_no_ack(const char *command);
  // bool send_command_printf(const char *format, ...);
};
}  // namespace nextion
}  // namespace esphome