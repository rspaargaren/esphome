#include "nextion.h"
#include "esphome/core/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nextion {
static const char *TAG = "nextion_textsensor";

NextionTextSensor::NextionTextSensor(Nextion *nextion) { this->nextion_ = nextion; }

void NextionTextSensor::process(uint8_t page_id, uint8_t component_id, float state) {
  if (this->page_id_ == page_id && this->component_id_ == component_id) {
    this->publish_state(state);
    ESP_LOGD(TAG, "Sensor state published");
  }
}

void NextionTextSensor::update() {
  char buffer[256];
  if (value = this->nextion_->gets(this->device_id_.c_str(), buffer)) {
    this->publish_state(buffer);
    ESP_LOGD(TAG, "Sensor state restored from Nextion");
  } else {
    this->publish_state("");
    return;
  }
}

}  // namespace nextion
}  // namespace esphome