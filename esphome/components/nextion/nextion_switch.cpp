#include "esphome/components/nextion/nextion_switch.h"
#include "esphome/core/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nextion {

static const char *TAG = "nextion_switch";
class NextionSwitch;

void NextionSwitch::process(uint8_t page_id, uint8_t component_id, bool on) {
  if (this->page_id_ == page_id && this->component_id_ == component_id) {
    this->publish_state(on);
    ESP_LOGW(TAG, "Switch state published");
  }
}

void NextionSwitch::write_state(bool state) {
  this->publish_state(state);
  this->send_command_printf("%s=%d", this->device_id_.c_str(), state);
}

bool NextionSwitch::send_command_printf(const char *format, ...) {
  char buffer[256];
  va_list arg;
  va_start(arg, format);
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret <= 0) {
    ESP_LOGW(TAG, "Building command for format '%s' failed!", format);
    return false;
  }
  this->send_command_no_ack(buffer);
  return true;
}

void NextionSwitch::send_command_no_ack(const char *command) {
  this->write_str(command);
  const uint8_t data[3] = {0xFF, 0xFF, 0xFF};
  this->write_array(data, sizeof(data));
}

}  // namespace nextion
}  // namespace esphome