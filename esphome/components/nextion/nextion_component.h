#pragma once
#include "esphome/core/component.h"
namespace esphome {
namespace nextion {
class NextionComponent;

class NextionComponent {
 public:
  virtual void process(uint8_t page_id, uint8_t component_id, bool on){};
  virtual void process_bool(char *variable_name, bool on){};
  virtual void process_sensor(char *variable_name, float state){};
  virtual void process_text(char *variable_name, char *text_value){};
  virtual void nextion_setup() = 0;

  void set_page_id(uint8_t page_id) { page_id_ = page_id; }
  void set_component_id(uint8_t component_id) { component_id_ = component_id; }
  void set_variable_name(std::string variable_name, std::string variable_name_to_send = "") {
    variable_name_ = variable_name;
    if (variable_name_to_send == "") {
      variable_name_to_send_ = variable_name_;
    } else {
      variable_name_to_send_ = variable_name_to_send;
    }
  }

 protected:
  uint8_t page_id_;
  uint8_t component_id_;
  std::string variable_name_;
  std::string variable_name_to_send_;
};
}  // namespace nextion
}  // namespace esphome
