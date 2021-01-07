#pragma once

namespace esphome {
namespace nextion {
class NextionBase;

class NextionBase {
 public:
  virtual int get_int(const char *component_id) { return 0; };
  virtual bool get_string(const char *component_id, char *string_buffer) { return false; };
  virtual bool send_command_printf(const char *format, ...) __attribute__((format(printf, 2, 3))) { return false; };
};
}  // namespace nextion
}  // namespace esphome
