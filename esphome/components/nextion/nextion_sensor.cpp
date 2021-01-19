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
  if (this->wave_comp_id_ == 0) {
    this->set_state(*state_value);
  } else {
    if (this->print_debug_)
      ESP_LOGD(TAG, "Received sensor state written in buffer");
    wave_buffer_.push_back(*state_value);
  }
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
  if (this->wave_buffer_.size() != 0) {
    wave_update();
  };
}

void NextionSensor::wave_update() {
  this->nextion_->send_command_printf("addt %d,%u,%u", this->wave_comp_id_, this->wave_chan_id_,
                                      this->wave_buffer_.size());
  if (wait_for_data_ready_()) {
    if (this->print_debug_)
      ESP_LOGD(TAG, "Send %i value(s) to wave nextion", this->wave_buffer_.size());
    this->nextion_->send_array(this->wave_buffer_.data(), this->wave_buffer_.size());
    wave_buffer_.clear();
  }
}

bool NextionSensor::wait_for_data_ready_() {
  uint32_t start = millis();
  while (!this->nextion_->datatransmit_ready_) {
    if (millis() - start > 100) {
      ESP_LOGW(TAG, "Waiting for data ready timed out!");
      return false;
    }
  }
  return true;
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
