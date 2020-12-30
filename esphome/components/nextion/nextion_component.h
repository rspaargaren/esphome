#pragma once
#include "esphome/core/component.h"
namespace esphome {
namespace nextion {
class NextionComponent;

class NextionComponent {
 public:
  virtual void process(uint8_t page_id, uint8_t component_id, bool on){};
  virtual void process_sensor(uint8_t page_id, uint8_t component_id, float state){};
  virtual void process_text(uint8_t page_id, uint8_t component_id, std::string state){};

  virtual void restore_from_nextion(){};
  void set_restore_from_nextion(bool restore_from_nextion) { restore_from_nextion_ = restore_from_nextion; }
  void set_page_id(uint8_t page_id) { page_id_ = page_id; }
  void set_component_id(uint8_t component_id) { component_id_ = component_id; }
  void set_variable_id(std::string variable_id) { variable_id_ = variable_id; }

 protected:
  uint8_t page_id_;
  uint8_t component_id_;
  bool restore_from_nextion_;
  std::string variable_id_;
};
}  // namespace nextion
}  // namespace esphome