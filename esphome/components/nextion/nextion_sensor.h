#pragma once
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "nextion_component.h"
#include "esphome/components/uart/uart.h"
#include "nextion_base.h"

namespace esphome {
namespace nextion {
class NextionSensor;

class NextionSensor : public NextionComponent, public sensor::Sensor, public PollingComponent {
 public:
  NextionSensor(NextionBase *nextion) { this->nextion_ = nextion; }
  void update_component() override { this->update(); }
  void update() override;
  void nextion_setup() override;
  void on_state_changed(std::string state);

  void set_precision(uint8_t precision) { this->precision_ = precision; }
  void set_wave_component_id(uint8_t wave_comp_id) { this->wave_comp_id_ = wave_comp_id; }
  void set_wave_channel_id(uint8_t wave_chan_id) { this->wave_chan_id_ = wave_chan_id; }
  void wave_update();
  bool wait_for_data_ready_();
  void process_sensor(char *variable_name, int state);
  void set_state(float state);

 protected:
  uint8_t precision_ = 0;
  uint8_t wave_comp_id_ = 0;
  uint8_t wave_chan_id_ = 0;
  std::vector<uint8_t> wave_buffer_;
};
}  // namespace nextion
}  // namespace esphome
