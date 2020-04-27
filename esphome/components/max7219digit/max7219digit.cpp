#include "max7219digit.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "max7219font.h"

namespace esphome {
namespace max7219digit {

static const char *TAG = "max7219";

static const uint8_t MAX7219_REGISTER_NOOP = 0x00;
static const uint8_t MAX7219_REGISTER_DECODE_MODE = 0x09;
static const uint8_t MAX7219_REGISTER_INTENSITY = 0x0A;
static const uint8_t MAX7219_REGISTER_SCAN_LIMIT = 0x0B;
static const uint8_t MAX7219_REGISTER_SHUTDOWN = 0x0C;
static const uint8_t MAX7219_REGISTER_DISPLAY_TEST = 0x0F;
//static const uint8_t MAX7219_UNKNOWN_CHAR = 0b11111111;
constexpr uint8_t MAX7219_NO_SHUTDOWN = 0x00;
constexpr uint8_t MAX7219_SHUTDOWN = 0x01;
constexpr uint8_t MAX7219_NO_DISPLAY_TEST = 0x00;
constexpr uint8_t MAX7219_DISPLAY_TEST = 0x01;


float MAX7219Component::get_setup_priority() const { return setup_priority::PROCESSOR; }

void MAX7219Component::setup() {
  ESP_LOGCONFIG(TAG, "Setting up MAX7219...");
  this->spi_setup();
  this->buffer_ = new uint8_t[this->num_chips_ * 8];
  for (uint8_t i = 0; i < this->num_chips_ * 8; i++)
    this->buffer_[i] = 0;

  // let's assume the user has all 8 digits connected, only important in daisy chained setups anyway
  this->send_to_all_(MAX7219_REGISTER_SCAN_LIMIT, 7);
  
  //this->send_to_all_(MAX7219_REGISTER_SHUTDOWN, MAX7219_NO_SHUTDOWN);
  //this->send_to_all_(MAX7219_REGISTER_DISPLAY_TEST, MAX7219_NO_DISPLAY_TEST);
  
  
  // let's use our own ASCII -> led pattern encoding
  this->send_to_all_(MAX7219_REGISTER_DECODE_MODE, 0);
  this->send_to_all_(MAX7219_REGISTER_INTENSITY, this->intensity_);
  this->display();
  // power up
  this->send_to_all_(MAX7219_REGISTER_SHUTDOWN, 1);
}
void MAX7219Component::dump_config() {
  ESP_LOGCONFIG(TAG, "MAX7219:");
  ESP_LOGCONFIG(TAG, "  Number of Chips: %u", this->num_chips_);
  ESP_LOGCONFIG(TAG, "  Intensity: %u", this->intensity_);
  LOG_PIN("  CS Pin: ", this->cs_);
  LOG_UPDATE_INTERVAL(this);
}

void MAX7219Component::display() {
  for (uint8_t i = 0; i < 8; i++) {
    this->enable();
    for (uint8_t j = 0; j < this->num_chips_; j++) {
      this->send_byte_(8 - i, this->buffer_[j * 8 + i]);
    }
    this->disable();
  }
}
void MAX7219Component::send_byte_(uint8_t a_register, uint8_t data) {
  this->write_byte(a_register);
  this->write_byte(data);
}
void MAX7219Component::send_to_all_(uint8_t a_register, uint8_t data) {
  this->enable();
  for (uint8_t i = 0; i < this->num_chips_; i++)
    this->send_byte_(a_register, data);
  this->disable();
}
void MAX7219Component::update() {
  for (uint8_t i = 0; i < this->num_chips_ * 8; i++)
    this->buffer_[i] = 0;
  if (this->writer_.has_value())
    (*this->writer_)(*this);
  this->display();
}
//uint8_t MAX7219Component::print(uint8_t start_pos, const char *str) {
//  uint8_t pos = start_pos;          // start positie
//  for (; *str != '\0'; str++) {     // Doorloop de string van start to eind /0
//    uint8_t data = MAX7219_UNKNOWN_CHAR; //set data op niet gevonden
//    if (*str >= ' ' && *str <= '~') // als de string in een bepaalde range is lees de waarde uit
//      data = pgm_read_byte(&MAX7219_ASCII_TO_RAW[*str - ' ']);
//
//    if (data == MAX7219_UNKNOWN_CHAR) {
//      ESP_LOGW(TAG, "Encountered character '%c' with no MAX7219 representation while translating string!", *str);
//    }
//    if (*str == '.') {
//      if (pos != start_pos)
//        pos--;
//      this->buffer_[pos] |= 0b10000000;
//    } else {
//      if (pos >= this->num_chips_ * 8) {
//        ESP_LOGE(TAG, "MAX7219 String is too long for the display!");
//        break;
//      }
//      this->buffer_[pos] = data;
//    }
//    pos++;
//  }
//  return pos - start_pos;
//}

// write an entire null-terminated string to the LEDs
uint8_t MAX7219Component::print(uint8_t start_pos, const char *s)
{
  byte chip;

  for (chip = 0; chip < this->num_chips_ && *s; chip++)
    sendChar (chip, *s++);

 // space out rest
  while (chip < (this->num_chips_))
    sendChar (chip++, ' ');

}  // end of sendString


void MAX7219Component::sendChar (const byte chip, const byte data)
  {
  // get this character from PROGMEM
  byte pixels [8];
  for (byte i = 0; i < 8; i++)
     pixels [i] = pgm_read_byte (&MAX7219_Dot_Matrix_font [data] [i]);

  this->send64pixels (chip, pixels);
  }  // end of sendChar

// send one character (data) to position (chip)
void MAX7219Component::send64pixels (const byte chip, const byte pixels [8])
  {
  for (byte col = 0; col < 8; col++)
    {
    // start sending
    //digitalWrite (load_, LOW);
    this->enable();
    // send extra NOPs to push the pixels out to extra displays
    for (byte i = 0; i < chip; i++)
      this->send_byte_ (MAX7219_REGISTER_NOOP, MAX7219_REGISTER_NOOP);
    // rotate pixels 90 degrees
    byte b = 0;
    for (byte i = 0; i < 8; i++)
      b |= bitRead (pixels [i], col) << (7 - i);
    this->send_byte_(col + 1, b);
    // end with enough NOPs so later chips don't update
    for (int i = 0; i < this->num_chips_ - chip - 1; i++)
      this->send_byte_ (MAX7219_REGISTER_NOOP, MAX7219_REGISTER_NOOP);
    // all done!
    this->disable();
    //digitalWrite (load_, HIGH);
    }   // end of for each column
  }  // end of sendChar











uint8_t MAX7219Component::print(const char *str) { return this->print(0, str); }
uint8_t MAX7219Component::printf(uint8_t pos, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char buffer[64];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0)
    return this->print(pos, buffer);
  return 0;
}
uint8_t MAX7219Component::printf(const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char buffer[64];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0)
    return this->print(buffer);
  return 0;
}
void MAX7219Component::set_writer(max7219_writer_t &&writer) { this->writer_ = writer; }
void MAX7219Component::set_intensity(uint8_t intensity) { this->intensity_ = intensity; }
void MAX7219Component::set_num_chips(uint8_t num_chips) { this->num_chips_ = num_chips; }

#ifdef USE_TIME
uint8_t MAX7219Component::strftime(uint8_t pos, const char *format, time::ESPTime time) {
  char buffer[64];
  size_t ret = time.strftime(buffer, sizeof(buffer), format);
  if (ret > 0)
    return this->print(pos, buffer);
  return 0;
}
uint8_t MAX7219Component::strftime(const char *format, time::ESPTime time) { return this->strftime(0, format, time); }
#endif

}  // namespace max7219
}  // namespace esphome
