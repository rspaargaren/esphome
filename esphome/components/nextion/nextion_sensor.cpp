#include "esphome/components/nextion/nextion_sensor.h"
#include "esphome/core/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nextion {
static const char *TAG = "nextion_sensor";

void NextionSensor::process_sensor(uint8_t page_id, uint8_t component_id, float state) {
  if (this->page_id_ == page_id && this->component_id_ == component_id) {
    this->publish_state(state);
    ESP_LOGD(TAG, "Sensor state process_sensor");
  }
}

void NextionSensor::update() {
  uint32_t state = this->nextion_->getn(this->variable_id_.c_str());
  this->publish_state(state);
  ESP_LOGD(TAG, "Sensor state update");
}

void NextionSensor::restore_from_nextion() {
  if (this->restore_from_nextion_ && !this->has_restored_) {
    this->update();
  }
  this->has_restored_ = true;
}

}  // namespace nextion
}  // namespace esphome