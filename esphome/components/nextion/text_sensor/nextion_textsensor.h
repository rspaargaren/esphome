#pragma once

#include "esphome/core/component.h"
#include "esphome/components/nextion/nextion.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace esphome {
namespace nextion {

class NextionTextSensor : public text_sensor::TextSensor, public uart::UARTDevice, public PollingComponent {
 public:
  NextionTextSensor(Nextion *parent);
  void set_page_id(uint8_t page_id) { page_id_ = page_id; }
  void set_component_id(uint8_t component_id) { component_id_ = component_id; }
  void set_device_id(std::string device_id) { device_id_ = device_id; }
  void process(uint8_t page_id, uint8_t component_id, float state);
  void update() override;

 protected:
  uint8_t page_id_;
  uint8_t component_id_;
  std::string device_id_;
  Nextion *nextion_;
};
}  // namespace nextion
}  // namespace esphome