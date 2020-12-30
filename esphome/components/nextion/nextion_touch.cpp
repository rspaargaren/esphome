#include "esphome/components/nextion/nextion_touch.h"
#include "esphome/core/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nextion {

static const char *TAG = "nextion_touch";
class NextionTouch;

void NextionTouchComponent::process(uint8_t page_id, uint8_t component_id, bool on) {
  if (this->page_id_ == page_id && this->component_id_ == component_id) {
    this->publish_state(on);
  }
}

}  // namespace nextion
}  // namespace esphome