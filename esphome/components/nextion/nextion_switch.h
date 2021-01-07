#pragma once
#include "esphome/core/component.h"
#include "esphome/components/nextion/nextion.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/api/custom_api_device.h"

namespace esphome {
namespace nextion {
class NextionSwitch;

class NextionSwitch : public NextionComponent, public switch_::Switch, public PollingComponent, api::CustomAPIDevice {
 public:
  NextionSwitch(Nextion *nextion) { this->nextion_ = nextion; }
  void process_bool(char *variable_name, bool on) override;
  void update_component() override { this->update(); }
  void update() override;
  void nextion_setup() override;
  void set_state(bool state);
  void on_state_changed(std::string state);

 protected:
  Nextion *nextion_;
  void write_state(bool state) override;

  // void send_command_no_ack(const char *command);
  // bool send_command_printf(const char *format, ...);
};
}  // namespace nextion
}  // namespace esphome
