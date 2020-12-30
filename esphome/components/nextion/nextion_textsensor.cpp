#include "esphome/components/nextion/nextion_textsensor.h"
#include "esphome/core/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nextion {
static const char *TAG = "nextion_textsensor";

void NextionTextSensor::process_text(uint8_t page_id, uint8_t component_id, std::string state) {
  if (this->page_id_ == page_id && this->component_id_ == component_id) {
    this->publish_state(state);
    ESP_LOGD(TAG, "Sensor state process_text");
  }
}

void NextionTextSensor::update() {
  char buffer[256];
  if (this->nextion_->gets(this->variable_id_.c_str(), buffer)) {
    this->publish_state(buffer);
    ESP_LOGD(TAG, "Sensor state update");
  } else {
    this->publish_state("");
    return;
  }
}

}  // namespace nextion
}  // namespace esphome