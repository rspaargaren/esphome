#include "dsmr_sensor.h"
#include "esphome/core/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace espdsmr {

static const char *TAG = "dsmr_sensor";

void DsmrSensor::setup() { this->update(); }

void DsmrSensor::process_sensor(char *variable_name, int state) {
  if (this->variable_name_ == variable_name) {
    ESP_LOGD(TAG, "DsmrSensor process_sensor %s", variable_name);
    this->publish_state(state);
    this->laststate_ = state;
  }
}

void DsmrSensor::update() {
  this->publish_state(this->laststate_);
  ESP_LOGD(TAG, "Updated sensor \"%s\" state %d", this->variable_name_.c_str(), this->laststate_);
}

}  // namespace espdsmr
}  // namespace esphome