#pragma once

#include "esphome/components/nextion/nextion.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace nextion {
class NextionTextSensor;

class NextionTextSensor : public NextionComponent, public text_sensor::TextSensor, public PollingComponent {
 public:
  NextionTextSensor(Nextion *nextion) { this->nextion_ = nextion; }
  void process_text(char *variable_name, char *text_value) override;
  void update_component() override { this->update(); }
  void update() override;
  void set_state(std::string state);
  void nextion_setup() override;

 protected:
  Nextion *nextion_;
};
}  // namespace nextion
}  // namespace esphome
