#include "esphome/components/nextion/nextion_sensor.h"
#include "esphome/core/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nextion {

static const char *TAG = "nextion_sensor";

void NextionSensor::nextion_setup() {
  if (this->get_update_interval() == -1)
    this->update();
}

void NextionSensor::process_sensor(char *variable_name, float state) {
  if (this->variable_name_ == variable_name) {
    this->publish_state(state);
    ESP_LOGD(TAG, "Processed sensor \"%s\" state %f", variable_name, state);
  }
}

void NextionSensor::update() {
  uint32_t state = this->nextion_->getn(this->variable_name_to_send_.c_str());
  this->publish_state(state);
  ESP_LOGD(TAG, "Updated sensor \"%s\" state %d", this->variable_name_.c_str(), state);
}

void NextionSensor::write_state(uint32_t state) {
  this->nextion_->send_command_printf("%s=%d", this->variable_name_to_send_.c_str(), state);
  this->publish_state(state);
  ESP_LOGD(TAG, "Wrote state for sensor \"%s\" state %d", this->variable_name_.c_str(), state);
}

}  // namespace nextion
}  // namespace esphome