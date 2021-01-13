#include "nextion_sensor.h"
#include "esphome/core/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nextion {

static const char *TAG = "nextion_sensor";

void NextionSensor::nextion_setup() {
  if (this->nextion_->is_setup_)
    this->update();
#ifdef USE_API
  if (this->hass_name_ != "none")
    subscribe_homeassistant_state(&NextionSensor::on_state_changed, this->hass_name_);
#endif
}

void NextionSensor::on_state_changed(std::string state) {
  if (this->print_debug_)
    ESP_LOGD(TAG, "Received sensor state from Homeassistant: %s", state.c_str());

  auto state_value = parse_float(state);
  if (!state_value.has_value()) {
    ESP_LOGW(TAG, "Can't convert '%s' to number!", state.c_str());
    return;
  }
  this->set_state(*state_value);
  wave_buffer_.push_back(*state_value);
}

void NextionSensor::process_sensor(char *variable_name, int state) {
  if (this->variable_name_ == variable_name) {
    if (this->print_debug_)
      ESP_LOGD(TAG, "NextionSensor process_sensor %s", variable_name);
    this->publish_state(state);
    if (this->print_debug_)
      ESP_LOGD(TAG, "Processed sensor \"%s\" state %d", variable_name, state);
  }
}

void NextionSensor::update() {
  int state = this->nextion_->get_int(this->variable_name_to_send_.c_str());
  this->publish_state(state);
  if (this->print_debug_)
    ESP_LOGD(TAG, "Updated sensor \"%s\" state %d", this->variable_name_.c_str(), state);
  wavetest();
}

void NextionSensor::wavetest() {
  bool ready_to_send;
  ready_to_send = this->nextion_->send_command_printf("addt %d,%u,%u", 10, 1, this->wave_buffer_.size());
  if (ready_to_send) {
    for (int i = 0; i < this->wave_buffer_.size(); i++) {
      this->write_byte(this->wave_buffer_[i]);
    }
    wave_buffer_.clear();
  }
}

void NextionSensor::set_state(float state) {
  if (this->precision_ > 0) {
    double to_multiply = pow(10, this->precision_);
    int state_value = (int) (state * to_multiply);
    this->nextion_->send_command_printf("%s=%d", this->variable_name_to_send_.c_str(), (int) state_value);
  } else {
    this->nextion_->send_command_printf("%s=%d", this->variable_name_to_send_.c_str(), (int) state);
  }
  this->publish_state(state);
  if (this->print_debug_)
    ESP_LOGD(TAG, "Wrote state for sensor \"%s\" state %lf", this->variable_name_.c_str(), state);
}

}  // namespace nextion
}  // namespace esphome
