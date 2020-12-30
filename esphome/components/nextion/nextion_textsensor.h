#pragma once

#include "esphome/core/component.h"
#include "esphome/components/nextion/nextion.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace nextion {
class NextionTextSensor;
class NextionTextSensor : public NextionComponent, public text_sensor::TextSensor, public PollingComponent {
 public:
  NextionTextSensor(Nextion *nextion) { this->nextion_ = nextion; }
  void process_text(uint8_t page_id, uint8_t component_id, std::string state) override;
  void update() override;

 protected:
  Nextion *nextion_;
};
}  // namespace nextion
}  // namespace esphome