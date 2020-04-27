#include "esphome/core/defines.h"

#include "esphome/core/helpers.h"
#include "esphome/components/spi/spi.h"

#ifdef USE_TIME
#include "esphome/components/time/real_time_clock.h"
#endif

namespace display {

extern const uint8_t MAX7219_ASCII_TO_RAW[94] PROGMEM;

class MAX7219GridComponent;

using max7219_writer_t = std::function<void(MAX7219GridComponent &)>;

class MAX7219GridComponent : public PollingComponent, public SPIDevice {
public:
  MAX7219GridComponent(SPIComponent *parent, GPIOPin *cs, uint32_t update_interval = 1000);

  void set_writer(max7219_writer_t &&writer);

  void setup() override;

  void dump_config() override;

  void update() override;

  float get_setup_priority() const override;

  void display();

  void set_intensity(uint8_t intensity);
  void set_num_chips(uint8_t num_chips);

  uint8_t printf(uint8_t pos, const char *format, ...) __attribute__((format(printf, 3, 4)));
  uint8_t printf(const char *format, ...) __attribute__((format(printf, 2, 3)));

  uint8_t print(uint8_t pos, const char *str);
  uint8_t print(const char *str);
  uint16_t width(){ return this->num_chips_ * 8;}
  uint16_t height(){ return 8;}
  void fill_vertical_line(uint16_t line, uint8_t val){
    if(line < width()){
      this->buffer_[line] = val;
    }
  }

#ifdef USE_TIME
  uint8_t strftime(uint8_t pos, const char *format, time::ESPTime time) __attribute__((format(strftime, 3, 0)));

  uint8_t strftime(const char *format, time::ESPTime time) __attribute__((format(strftime, 2, 0)));
#endif

protected:
  void send_byte_(uint8_t a_register, uint8_t data);
  void send_to_all_(uint8_t a_register, uint8_t data);
  bool is_device_msb_first() override;

  uint8_t intensity_{15};
  uint8_t num_chips_{1};
  uint8_t *buffer_;
  optional<max7219_writer_t> writer_{};
};

} // namespace display