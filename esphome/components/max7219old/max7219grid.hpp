

#include "esphome/core/log.h"
#include "max7219grid.h"

namespace display {

static const char *TAG = "display.max7219";
static const uint8_t MAX7219_REGISTER_NOOP = 0x00;
static const uint8_t MAX7219_REGISTER_DECODE_MODE = 0x09;
static const uint8_t MAX7219_REGISTER_INTENSITY = 0x0A;
static const uint8_t MAX7219_REGISTER_SCAN_LIMIT = 0x0B;
static const uint8_t MAX7219_REGISTER_SHUTDOWN = 0x0C;
static const uint8_t MAX7219_REGISTER_DISPLAY_TEST = 0x0F;
constexpr uint8_t MAX7219_NO_SHUTDOWN = 0x00;
constexpr uint8_t MAX7219_SHUTDOWN = 0x01;
constexpr uint8_t MAX7219_NO_DISPLAY_TEST = 0x00;
constexpr uint8_t MAX7219_DISPLAY_TEST = 0x01;

MAX7219GridComponent::MAX7219GridComponent(SPIComponent *parent, GPIOPin *cs, uint32_t update_interval)
  : PollingComponent(update_interval), SPIDevice(parent, cs) {}

float MAX7219GridComponent::get_setup_priority() const { return setup_priority::POST_HARDWARE; }
void MAX7219GridComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MAX7219...");
  this->spi_setup();
  this->buffer_ = new uint8_t[this->num_chips_ * 8];
  for (uint8_t i = 0; i < this->num_chips_ * 8; i++)
    this->buffer_[i] = 0;
  // let's assume the user has all 8 digits connected, only important in daisy chained setups anyway
  this->send_to_all_(MAX7219_REGISTER_SHUTDOWN, MAX7219_NO_SHUTDOWN);
  this->send_to_all_(MAX7219_REGISTER_DISPLAY_TEST, MAX7219_NO_DISPLAY_TEST);
  //this->send_to_all_(MAX7219_REGISTER_SHUTDOWN, MAX7219_NO_SHUTDOWN);
  // let's assume the user has all 8 digits connected, only important in daisy chained setups anyway
  this->send_to_all_(MAX7219_REGISTER_SCAN_LIMIT, 7);
  // let's use our own ASCII -> led pattern encoding
  this->send_to_all_(MAX7219_REGISTER_DECODE_MODE, 0);
  this->send_to_all_(MAX7219_REGISTER_INTENSITY, this->intensity_);
  delay(250);
  this->display();
  // power up
  this->send_to_all_(MAX7219_REGISTER_SHUTDOWN, 1);
}
void MAX7219GridComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "MAX7219:");
  ESP_LOGCONFIG(TAG, " Number of Chips: %u", this->num_chips_);
  ESP_LOGCONFIG(TAG, " Intensity: %u", this->intensity_);
  LOG_PIN(" CS Pin: ", this->cs_);
  LOG_UPDATE_INTERVAL(this);
}

void MAX7219GridComponent::display() {

  for (uint8_t i = 0; i < 8; i++) {
    this->enable();
      for (uint8_t j = 0; j < this->num_chips_; j++) {
        this->send_byte_(8 - i, this->buffer_[j * 8 + i]);
      }
    this->disable();
  }
}
void MAX7219GridComponent::send_byte_(uint8_t a_register, uint8_t data) {
  // ESP_LOGW(TAG, "R(%x)<- %x", a_register, data);
  this->write_byte(a_register);
  this->write_byte(data);
}
void MAX7219GridComponent::send_to_all_(uint8_t a_register, uint8_t data) {
  this->enable();
  for (uint8_t i = 0; i < this->num_chips_; i++)
    this->send_byte_(a_register, data);
  this->disable();
}
bool MAX7219GridComponent::is_device_msb_first() { return true; }
void MAX7219GridComponent::update() {
  for (uint8_t i = 0; i < this->num_chips_ * 8; i++){
    this->buffer_[i] = 0;
  }
  if (this->writer_.has_value())
    (*this->writer_)(*this);
  this->display();
}
uint8_t MAX7219GridComponent::print(uint8_t start_pos, const char *str) {
  // uint8_t pos = start_pos;
  // for (; *str != '\0'; str++) {
  //   uint8_t data = MAX7219_UNKNOWN_CHAR;
  //   if (*str >= ' ' && *str <= '}')
  //     data = pgm_read_byte(&MAX7219_ASCII_TO_RAW[*str - ' ']);
  //
  //   if (data == MAX7219_UNKNOWN_CHAR) {
  //     ESP_LOGW(TAG, "Encountered character '%c' with no MAX7219 representation while translating string!", *str);
  //   }
  //   if (*str == '.') {
  //     if (pos != start_pos)
  //       pos--;
  //     this->buffer_[pos] |= 0b10000000;
  //   } else {
  //     if (pos >= this->num_chips_ * 8) {
  //       ESP_LOGE(TAG, "MAX7219 String is too long for the display!");
  //       break;
  //     }
  //     this->buffer_[pos] = data;
  //   }
  //   pos++;
  // }
  // return pos - start_pos;
  return 0;
}
uint8_t MAX7219GridComponent::print(const char *str) {return this->print(0, str);}
uint8_t MAX7219GridComponent::printf(uint8_t pos, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char buffer[64];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0)
    return this->print(pos, buffer);
  return 0;
}

uint8_t MAX7219GridComponent::printf(const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char buffer[64];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0)
    return this->print(buffer);
  return 0;
}
void MAX7219GridComponent::set_writer(max7219_writer_t &&writer) {this->writer_ = writer;}
void MAX7219GridComponent::set_intensity(uint8_t intensity) {this->intensity_ = intensity;}
void MAX7219GridComponent::set_num_chips(uint8_t num_chips) {this->num_chips_ = num_chips;}

#ifdef USE_TIME
uint8_t MAX7219GridComponent::strftime(uint8_t pos, const char *format, time::ESPTime time) {
  char buffer[64];
  size_t ret = time.strftime(buffer, sizeof(buffer), format);
  if (ret > 0)
    return this->print(pos, buffer);
  return 0;
}
uint8_t MAX7219GridComponent::strftime(const char *format, time::ESPTime time) {
  return this->strftime(0, format, time);
}
#endif

} // namespace display
