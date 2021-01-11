#pragma once
#include "esphome/core/component.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/api/custom_api_device.h"
#include "nextion_component.h"
#include "nextion_base.h"

namespace esphome {
namespace nextion {
class NextionTextSensor;

class NextionTextSensor : public NextionComponent,
                          public text_sensor::TextSensor,
                          public PollingComponent,
                          api::CustomAPIDevice {
 public:
  NextionTextSensor(NextionBase *nextion) { this->nextion_ = nextion; }
  void update_component() override { this->update(); }
  void update() override;
  void nextion_setup() override;
  void on_state_changed(std::string state);

  void process_text(char *variable_name, char *text_value);
  void set_state(std::string state);
};
}  // namespace nextion
}  // namespace esphome
