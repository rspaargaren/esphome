#include "esphome/components/nextion/nextion_sensor.h"
#include "esphome/core/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nextion {
static const char *TAG = "nextion_sensor";

void NextionSensor::process_sensor(uint8_t page_id, uint8_t component_id, float state) {
  if (this->page_id_ == page_id && this->component_id_ == component_id) {
    this->publish_state(state);
    ESP_LOGD(TAG, "Sensor state published");
  }
}

void NextionSensor::update_from_nextion() {
  uint32_t value = this->nextion_->getn(this->device_id_.c_str());
  this->publish_state(value);
  ESP_LOGD(TAG, "Sensor state restored from Nextion");
  this->has_restored_ = true;
}

void NextionSensor::update() { ESP_LOGD(TAG, "Sensor update"); }

void NextionSensor::restore_from_nextion() {
  if (this->restore_from_nextion_ && !this->has_restored_) {
    this->update_from_nextion();
  }
}

}  // namespace nextion
}  // namespace esphome