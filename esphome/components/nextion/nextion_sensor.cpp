#include "nextion_sensor.h"
#include "esphome/core/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nextion {

static const char *TAG = "nextion_sensor";

void NextionSensor::nextion_setup() {
  if (this->nextion_->is_setup_)
    this->update();
#ifdef USE_API
  if (this->hass_name_ != "none")
    subscribe_homeassistant_state(&NextionSensor::on_state_changed, this->hass_name_);
#endif
}

void NextionSensor::on_state_changed(std::string state) {
  if (this->print_debug_)
    ESP_LOGD(TAG, "Received sensor state from Homeassistant: %s", state.c_str());
  this->set_state(string_to_int(state));
}

int NextionSensor::string_to_int(std::string state) {
  int pos = state.find('.');
  char *p_end;
  if (pos != 0) {
    state.erase(std::remove(state.begin(), state.end(), '.'), state.end());
    long int num = std::strtol(state.c_str(), &p_end, 10);
    return num;
  } else {
    long int num = std::strtol(state.c_str(), &p_end, 10);
    return num;
  }
}

void NextionSensor::process_sensor(char *variable_name, int state) {
  if (this->variable_name_ == variable_name) {
    if (this->print_debug_)
      ESP_LOGD(TAG, "NextionSensor process_sensor %s", variable_name);
    this->publish_state(state);
    if (this->print_debug_)
      ESP_LOGD(TAG, "Processed sensor \"%s\" state %d", variable_name, state);
  }
}

void NextionSensor::update() {
  int state = this->nextion_->get_int(this->variable_name_to_send_.c_str());
  this->publish_state(state);
  if (this->print_debug_)
    ESP_LOGD(TAG, "Updated sensor \"%s\" state %d", this->variable_name_.c_str(), state);
}

void NextionSensor::set_state(int state) {
  this->nextion_->send_command_printf("%s=%d", this->variable_name_to_send_.c_str(), state);
  this->publish_state(state);
  if (this->print_debug_)
    ESP_LOGD(TAG, "Wrote state for sensor \"%s\" state %d", this->variable_name_.c_str(), state);
}

}  // namespace nextion
}  // namespace esphome
