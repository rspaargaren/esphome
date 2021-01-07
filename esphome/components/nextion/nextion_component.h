#pragma once
#include "Arduino.h"
namespace esphome {
namespace nextion {
class NextionComponent;

class NextionComponent {
 public:
  // For Nextion Touch Component
  virtual void process(uint8_t page_id, uint8_t component_id, bool on){};
  // For Nextion Switch Component
  virtual void process_bool(char *variable_name, bool on){};
  // For Nextion Sensor Component
  virtual void process_sensor(char *variable_name, int state){};
  // For Nextion Text Sensor Component
  virtual void process_text(char *variable_name, char *text_value){};

  // Component publishes its state
  virtual void update_component() = 0;
  // Component setup routine, triggered from the Nextion display
  virtual void nextion_setup() = 0;
  // Set the components variable name
  void set_variable_name(std::string variable_name, std::string variable_name_to_send = "") {
    variable_name_ = variable_name;
    if (variable_name_to_send == "") {
      variable_name_to_send_ = variable_name_;
    } else {
      variable_name_to_send_ = variable_name_to_send;
    }
  }
  // Set the components page id for Nextion Touch Component
  void set_page_id(uint8_t page_id) { page_id_ = page_id; }
  // Set the components component id for Nextion Touch Component
  void set_component_id(uint8_t component_id) { component_id_ = component_id; }

  void set_print_debug(bool print_debug) { this->print_debug_ = print_debug; }

  void set_hass_name(std::string hass_name) { this->hass_name_ = hass_name; }

 protected:
  uint8_t page_id_;
  uint8_t component_id_;
  std::string variable_name_;
  std::string variable_name_to_send_;
  std::string hass_name_;
  bool print_debug_ = true;
};
}  // namespace nextion
}  // namespace esphome
