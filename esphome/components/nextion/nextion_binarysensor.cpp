#include "nextion_binarysensor.h"
#include "esphome/core/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nextion {

static const char *TAG = "nextion_binarysensor";

void NextionBinarySensor::nextion_setup() {
  if (this->nextion_->is_setup_)
    this->update();
#ifdef USE_API
  if (this->hass_name_ != "none")
    subscribe_homeassistant_state(&NextionBinarySensor::on_state_changed, this->hass_name_);
#endif
}

void NextionBinarySensor::on_state_changed(std::string state) {
  if (this->print_debug_)
    ESP_LOGD(TAG, "Received switch state from Homeassistant: %s", state.c_str());
  if (state == "on") {
    this->set_state(true);
  } else {
    this->set_state(false);
  }
}

void NextionBinarySensor::process_bool(char *variable_name, bool on) {
  if (this->variable_name_.empty())  // This is a touch component
    return;
  if (this->variable_name_ == variable_name) {
    this->publish_state(on);
    if (this->print_debug_)
      ESP_LOGD(TAG, "Processed binarysensor \"%s\" state %s", variable_name, state ? "ON" : "OFF");
  }
}

void NextionBinarySensor::process_touch(uint8_t page_id, uint8_t component_id, bool on) {
  if (this->page_id_ == page_id && this->component_id_ == component_id) {
    this->publish_state(on);
  }
}

void NextionBinarySensor::update() {
  if (this->variable_name_.empty())  // This is a touch component
    return;
  uint32_t state = this->nextion_->get_int(this->variable_name_to_send_.c_str());
  this->publish_state(state != 0);
  if (this->print_debug_)
    ESP_LOGD(TAG, "Updated binarysensor \"%s\" state %s", this->variable_name_.c_str(), state ? "ON" : "OFF");
}

void NextionBinarySensor::set_state(bool state) {
  if (this->variable_name_.empty())  // This is a touch component
    return;
  this->publish_state(state);
  this->nextion_->send_command_printf("%s=%d", this->variable_name_to_send_.c_str(), (int) state);
  if (this->print_debug_)
    ESP_LOGD(TAG, "Updated binarysensor \"%s\" state %s", this->variable_name_.c_str(), state ? "ON" : "OFF");
}

}  // namespace nextion
}  // namespace esphome
