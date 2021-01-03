#include "nextion_switch.h"
#include "esphome/core/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nextion {

static const char *TAG = "nextion_switch";

void NextionSwitch::nextion_setup() {
  if (this->nextion_->has_setup_)
    this->update();
}

void NextionSwitch::process_bool(char *variable_name, bool on) {
  ESP_LOGD(TAG, "NextionSwitch process_bool");
  if (this->variable_name_ == variable_name) {
    this->publish_state(on);
    if (this->print_debug_)
      ESP_LOGD(TAG, "Processed switch \"%s\" state %s", variable_name, state ? "ON" : "OFF");
  }
}

void NextionSwitch::update() {
  ESP_LOGD(TAG, "NextionSwitch update");
  if (!this->nextion_->has_setup_)
    return;
  uint32_t state = this->nextion_->get_int(this->variable_name_to_send_.c_str());
  this->publish_state(state == 0 ? false : true);
  if (this->print_debug_)
    ESP_LOGD(TAG, "Updated switch \"%s\" state %s", this->variable_name_.c_str(), state ? "ON" : "OFF");
}

void NextionSwitch::set_state(bool state) { this->write_state(state); }

void NextionSwitch::write_state(bool state) {
  if (!this->nextion_->has_setup_ || this->is_readonly_)
    return;
  ESP_LOGD(TAG, "NextionSwitch  write_state");
  this->publish_state(state);
  this->nextion_->send_command_printf("%s=%d", this->variable_name_to_send_.c_str(), state);
  if (this->print_debug_)
    ESP_LOGD(TAG, "Updated switch \"%s\" state %s", this->variable_name_.c_str(), state ? "ON" : "OFF");
}

}  // namespace nextion
}  // namespace esphome
