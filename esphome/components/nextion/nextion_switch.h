#pragma once
#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/api/custom_api_device.h"
#include "nextion_component.h"
#include "nextion_base.h"

namespace esphome {
namespace nextion {
class NextionSwitch;

class NextionSwitch : public NextionComponent, public switch_::Switch, public PollingComponent, api::CustomAPIDevice {
 public:
  NextionSwitch(NextionBase *nextion) { this->nextion_ = nextion; }
  void update_component() override { this->update(); }
  void update() override;
  void nextion_setup() override;

  void process_bool(char *variable_name, bool on);
  void set_state(bool state);
  void on_state_changed(std::string state);

 protected:
  void write_state(bool state) override;
};
}  // namespace nextion
}  // namespace esphome
