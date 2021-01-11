#include "nextion_textsensor.h"
#include "esphome/core/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nextion {
static const char *TAG = "nextion_textsensor";

void NextionTextSensor::nextion_setup() {
  if (this->nextion_->is_setup_)
    this->update();
#ifdef USE_API
  if (this->hass_name_ != "none")
    subscribe_homeassistant_state(&NextionTextSensor::on_state_changed, this->hass_name_);
#endif
}

void NextionTextSensor::on_state_changed(std::string state) {
  if (this->print_debug_)
    ESP_LOGD(TAG, "Received sensor state from Homeassistant: %s", state.c_str());
  this->set_state(state);
}

void NextionTextSensor::process_text(char *variable_name, char *text_value) {
  if (this->variable_name_ == variable_name) {
    this->publish_state(text_value);
    if (this->print_debug_)
      ESP_LOGD(TAG, "Processed text_sensor \"%s\" state \"%s\"", variable_name, text_value);
  }
}

void NextionTextSensor::update() {
  char buffer[256];
  if (this->nextion_->get_string(this->variable_name_to_send_.c_str(), buffer)) {
    this->publish_state(buffer);
    if (this->print_debug_)
      ESP_LOGD(TAG, "Updated text_sensor \"%s\" state \"%s\"", this->variable_name_.c_str(), buffer);
  } else {
    this->publish_state("");
    ESP_LOGD(TAG, "ERROR: Updated text_sensor \"%s\" state \"\"", this->variable_name_.c_str());
    return;
  }
}

void NextionTextSensor::set_state(std::string state) {
  this->nextion_->send_command_printf("%s=\"%s\"", this->variable_name_to_send_.c_str(), state.c_str());
  this->publish_state(state);
  if (this->print_debug_)
    ESP_LOGD(TAG, "Wrote state for text_sensor \"%s\" state \"%s\"", this->variable_name_.c_str(), state.c_str());
}

}  // namespace nextion
}  // namespace esphome
