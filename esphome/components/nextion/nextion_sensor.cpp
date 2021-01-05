#include "nextion_sensor.h"
#include "esphome/core/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nextion {

static const char *TAG = "nextion_sensor";

void NextionSensor::nextion_setup() { this->update(); }

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
