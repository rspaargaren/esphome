#include "nextion_switch.h"
#include "esphome/core/util.h"
#include "esphome/core/log.h"
#include "esphome.h"

namespace esphome {
namespace nextion {

static const char *TAG = "nextion_switch";

void NextionSwitch::nextion_setup() {
  if (this->nextion_->is_setup_)
    this->update();
}

void NextionSwitch::process_bool(char *variable_name, bool on) {
  if (this->variable_name_ == variable_name) {
    this->publish_state(on);
    if (this->print_debug_)
      ESP_LOGD(TAG, "Processed switch \"%s\" state %s", variable_name, state ? "ON" : "OFF");
    if ((on) & (this->hass_name_ != = "none")) {
      call_homeassistant_service("switch.turn_on", {
                                                       {"entity_id", this->hass_name_},
                                                   });
    } else {
      call_homeassistant_service("switch.turn_off", {
                                                        {"entity_id", this->hass_name_},
                                                    });
    }
  }
}

void NextionSwitch::update() {
  uint32_t state = this->nextion_->get_int(this->variable_name_to_send_.c_str());
  this->publish_state(state == 0 ? false : true);
  if (this->print_debug_)
    ESP_LOGD(TAG, "Updated switch \"%s\" state %s", this->variable_name_.c_str(), state ? "ON" : "OFF");
}

void NextionSwitch::set_state(bool state) { this->write_state(state); }

void NextionSwitch::write_state(bool state) {
  this->publish_state(state);
  this->nextion_->send_command_printf("%s=%d", this->variable_name_to_send_.c_str(), state);
  if (this->print_debug_)
    ESP_LOGD(TAG, "Updated switch \"%s\" state %s", this->variable_name_.c_str(), state ? "ON" : "OFF");
}

}  // namespace nextion
}  // namespace esphome
