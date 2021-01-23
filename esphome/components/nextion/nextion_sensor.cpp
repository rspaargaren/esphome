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
  if (this->wave_mode_ == 0) {
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
    if (this->wave_mode_ == 0) {
      this->publish_state(state);
    } else {
      wave_buffer_.push_back(state);
      if (this->wave_buffer_.size() > 255) {
        this->wave_buffer_.erase(this->wave_buffer_.begin(),
                                 this->wave_buffer_.begin() + (this->wave_buffer_size() - 255));
      }
    }
    if (this->print_debug_)
      ESP_LOGD(TAG, "Processed sensor \"%s\" state %d", variable_name, state);
  }
}

void NextionSensor::update() {
  if (this->wave_mode_ == 0) {
    int state = this->nextion_->get_int(this->variable_name_to_send_.c_str());
    this->publish_state(state);
    if (this->print_debug_)
      ESP_LOGD(TAG, "Updated sensor \"%s\" state %d", this->variable_name_.c_str(), state);
  } else {
    if (this->wave_buffer_.size() != 0) {
      wave_update();
    }
  }
}

void NextionSensor::wave_update() {
  if (this->wave_mode_ == 1) {
    this->nextion_->send_command_printf("addt %d,%u,%u", this->wave_comp_id_, this->wave_chan_id_,
                                        this->wave_buffer_.size());
    if (wait_for_data_ready_()) {
      if (this->print_debug_)
        ESP_LOGD(TAG, "Send %i value(s) to wave nextion", this->wave_buffer_.size());
      this->nextion_->send_array(this->wave_buffer_.data(), this->wave_buffer_.size());
      wave_buffer_.clear();
    }
  } else {
    uint8_t last_value_;
    last_value_ = this->wave_buffer_.back();
    this->wave_buffer_.clear();
    this->wave_buffer_.push_back(last_value_);
    this->wave_que_.push_back(last_value_);
    if (this->wave_que_.size() > 255) {
      this->wave_que_.erase(this->wave_que_.begin(), this->wave_que_.begin() + (this->wave_que_size() - 255));
    }
    this->nextion_->send_command_printf("addt %d,%u,%u", this->wave_comp_id_, this->wave_chan_id_,
                                        this->wave_que_.size());
    if (wait_for_data_ready_()) {
      if (this->print_debug_)
        ESP_LOGD(TAG, "Send %i value(s) to wave nextion", this->wave_que_.size());
      this->nextion_->send_array(this->wave_que_.data(), this->wave_que_.size());
      wave_que_.clear();
    }
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
