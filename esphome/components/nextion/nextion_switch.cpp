#include "esphome/components/nextion/nextion_switch.h"
#include "esphome/core/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nextion {

static const char *TAG = "nextion_switch";

void NextionSwitch::nextion_setup() {
  if (this->get_update_interval() == -1)
    this->update();
}

void NextionSwitch::process_bool(char *variable_name, bool on) {
  if (this->variable_name_ == variable_name) {
    this->publish_state(on);
    ESP_LOGD(TAG, "Processed switch \"%s\" state %s", variable_name, state ? "ON" : "OFF");
  }
}

void NextionSwitch::update() {
  uint32_t state = this->nextion_->getn(this->variable_name_to_send_.c_str());
  this->publish_state(state == 0 ? false : true);
  ESP_LOGD(TAG, "Updated switch \"%s\" state %s", this->variable_name_.c_str(), state ? "ON" : "OFF");
}

void NextionSwitch::write_state(bool state) {
  this->publish_state(state);
  this->nextion_->send_command_printf("%s=%d", this->variable_name_to_send_.c_str(), state);
  ESP_LOGD(TAG, "Updated switch \"%s\" state %s", this->variable_name_.c_str(), state ? "ON" : "OFF");
}

}  // namespace nextion
}  // namespace esphome