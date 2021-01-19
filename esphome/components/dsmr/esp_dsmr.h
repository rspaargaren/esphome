#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "dsmr.h"

namespace esphome {
namespace esp_dsmr {

class esp_dsmr : public PollingComponent, public uart::UARTDevice {
 public:
 protected:
}  // namespace esp_dsmr
}  // namespace esphome
