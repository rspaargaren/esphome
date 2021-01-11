#pragma once
#include "Arduino.h"
#include "nextion_base.h"
namespace esphome {
namespace nextion {
class NextionComponent;

class NextionComponent {
 public:
  // Component publishes its state
  virtual void update_component() = 0;
  // Component setup routine, triggered from the Nextion display
  virtual void nextion_setup() = 0;
  // Set the components variable name
  void set_variable_name(std::string variable_name, std::string variable_name_to_send = "") {
    variable_name_ = variable_name;
    if (variable_name_to_send.empty()) {
      variable_name_to_send_ = variable_name_;
    } else {
      variable_name_to_send_ = variable_name_to_send;
    }
  }

  void set_print_debug(bool print_debug) { this->print_debug_ = print_debug; }
  void set_hass_name(std::string hass_name) {
    this->hass_name_ = hass_name;
    get_entity_type(hass_name);
  }

  void get_entity_type(std::string hass_name) {
    std::string::size_type pos = hass_name.find('.');
    if (pos != std::string::npos) {
      this->hass_entity_type_ = hass_name.substr(0, hass_name.find('.'));
    } else {
      this->hass_entity_type_ = "switch";  // Maybe change this to error message and set hass_name_ to "none"
    }
  }

 protected:
  NextionBase* nextion_;

  std::string variable_name_;
  std::string variable_name_to_send_;
  std::string hass_name_;
  std::string hass_entity_type_;
  bool print_debug_ = true;
};
}  // namespace nextion
}  // namespace esphome
