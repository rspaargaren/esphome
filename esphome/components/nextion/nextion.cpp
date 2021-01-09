#include "nextion.h"
#include "esphome/core/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nextion {

static const char *TAG = "nextion";

void Nextion::setup() {
  String response = String("");
  this->send_command_no_ack("");
  this->send_command_no_ack("connect");
  this->recv_ret_string_(response);
  if (response.indexOf(F("comok")) == -1) {
    ESP_LOGD(TAG, "display doesn't accept the first connect request");
  } else {
    sscanf(response.c_str(), "%*64[^,],%*64[^,],%64[^,],%64[^,],%*64[^,],%64[^,],%64[^,]", device_model_,
           firmware_version_, serial_number_, flash_size_);
  }
  this->send_command_printf("bkcmd=3");
  this->set_backlight_brightness(static_cast<uint8_t>(brightness_ * 100));
  this->goto_page("0");
  this->is_setup_ = true;
  for (auto *sensortype : this->sensortype_) {
    sensortype->nextion_setup();
  }
  for (auto *binarysensortype : this->binarysensortype_) {
    binarysensortype->nextion_setup();
  }
  for (auto *switchtype : this->switchtype_) {
    switchtype->nextion_setup();
  }
  for (auto *textsensortype : this->textsensortype_) {
    textsensortype->nextion_setup();
  }
}

void Nextion::set_print_debug(bool print_debug) {
  this->print_debug_ = print_debug;
  for (auto *sensortype : this->sensortype_) {
    sensortype->set_print_debug(print_debug);
  }
  for (auto *switchtype : this->switchtype_) {
    switchtype->set_print_debug(print_debug);
  }
  for (auto *textsensortype : this->textsensortype_) {
    textsensortype->set_print_debug(print_debug);
  }
}

void Nextion::dump_config() {
  ESP_LOGCONFIG(TAG, "Nextion:");
  ESP_LOGCONFIG(TAG, "  Baud Rate:        %d", this->parent_->get_baud_rate());
  ESP_LOGCONFIG(TAG, "  Device Model:     %s", this->device_model_);
  ESP_LOGCONFIG(TAG, "  Firmware Version: %s", this->firmware_version_);
  ESP_LOGCONFIG(TAG, "  Serial Number:    %s", this->serial_number_);
  ESP_LOGCONFIG(TAG, "  Flash Size:       %s", this->flash_size_);
}

float Nextion::get_setup_priority() const { return setup_priority::PROCESSOR; }
void Nextion::update() {
  if (this->writer_.has_value()) {
    (*this->writer_)(*this);
  }
}

void Nextion::add_sleep_state_callback(std::function<void(bool)> &&callback) {
  this->sleep_callback_.add(std::move(callback));
}

void Nextion::add_wake_state_callback(std::function<void(bool)> &&callback) {
  this->wake_callback_.add(std::move(callback));
}

void Nextion::send_command_no_ack(const char *command) {
  // Flush RX...
  this->loop();

  this->write_str(command);
  const uint8_t data[3] = {0xFF, 0xFF, 0xFF};
  this->write_array(data, sizeof(data));
}

bool Nextion::ack_() {
  if (!this->wait_for_ack_)
    return true;

  uint32_t start = millis();
  while (!this->read_until_ack_()) {
    if (millis() - start > 100) {
      ESP_LOGW(TAG, "Waiting for ACK timed out!");
      return false;
    }
  }
  return true;
}

void Nextion::update_all_components() {
  for (auto *sensortype : this->sensortype_) {
    sensortype->update_component();
  }
  for (auto *switchtype : this->switchtype_) {
    switchtype->update_component();
  }
  for (auto *textsensortype : this->textsensortype_) {
    textsensortype->update_component();
  }
}

void Nextion::set_component_text(const char *component, const char *text) {
  this->send_command_printf("%s.txt=\"%s\"", component, text);
}
void Nextion::set_component_value(const char *component, int value) {
  this->send_command_printf("%s.val=%d", component, value);
}
void Nextion::display_picture(int picture_id, int x_start, int y_start) {
  this->send_command_printf("pic %d %d %d", x_start, y_start, picture_id);
}
void Nextion::set_component_background_color(const char *component, uint32_t color) {
  this->send_command_printf("%s.bco=%d", component, color);
}
void Nextion::set_component_background_color(const char *component, const char *color) {
  this->send_command_printf("%s.bco=\"%s\"", component, color);
}
void Nextion::set_component_background_color(const char *component, Color color) {
  this->send_command_printf("%s.bco=%d", component, color.to_565());
}
void Nextion::set_component_pressed_background_color(const char *component, uint32_t color) {
  this->send_command_printf("%s.bco2=%d", component, color);
}
void Nextion::set_component_pressed_background_color(const char *component, const char *color) {
  this->send_command_printf("%s.bco2=\"%s\"", component, color);
}
void Nextion::set_component_pressed_background_color(const char *component, Color color) {
  this->send_command_printf("%s.bco2=%d", component, color.to_565());
}
void Nextion::set_component_font_color(const char *component, uint32_t color) {
  this->send_command_printf("%s.pco=%d", component, color);
}
void Nextion::set_component_font_color(const char *component, const char *color) {
  this->send_command_printf("%s.pco=\"%s\"", component, color);
}
void Nextion::set_component_font_color(const char *component, Color color) {
  this->send_command_printf("%s.pco=%d", component, color.to_565());
}
void Nextion::set_component_pressed_font_color(const char *component, uint32_t color) {
  this->send_command_printf("%s.pco2=%d", component, color);
}
void Nextion::set_component_pressed_font_color(const char *component, const char *color) {
  this->send_command_printf("%s.pco2=\"%s\"", component, color);
}
void Nextion::set_component_pressed_font_color(const char *component, Color color) {
  this->send_command_printf("%s.pco2=%d", component, color.to_565());
}
void Nextion::set_component_coordinates(const char *component, int x, int y) {
  this->send_command_printf("%s.xcen=%d", component, x);
  this->send_command_printf("%s.ycen=%d", component, y);
}
void Nextion::set_component_font(const char *component, uint8_t font_id) {
  this->send_command_printf("%s.font=%d", component, font_id);
}
void Nextion::goto_page(const char *page) { this->send_command_printf("page %s", page); }

bool Nextion::send_command_printf(const char *format, ...) {
  char buffer[256];
  va_list arg;
  va_start(arg, format);
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret <= 0) {
    ESP_LOGW(TAG, "Building command for format '%s' failed!", format);
    return false;
  }
  this->send_command_no_ack(buffer);
  return this->ack_();
}
void Nextion::hide_component(const char *component) { this->send_command_printf("vis %s,0", component); }
void Nextion::show_component(const char *component) { this->send_command_printf("vis %s,1", component); }
void Nextion::enable_component_touch(const char *component) { this->send_command_printf("tsw %s,1", component); }
void Nextion::disable_component_touch(const char *component) { this->send_command_printf("tsw %s,0", component); }
void Nextion::add_waveform_data(int component_id, uint8_t channel_number, uint8_t value) {
  this->send_command_printf("add %d,%u,%u", component_id, channel_number, value);
}
void Nextion::fill_area(int x1, int y1, int width, int height, const char *color) {
  this->send_command_printf("fill %d,%d,%d,%d,%s", x1, y1, width, height, color);
}
void Nextion::fill_area(int x1, int y1, int width, int height, Color color) {
  this->send_command_printf("fill %d,%d,%d,%d,%d", x1, y1, width, height, color.to_565());
}
void Nextion::line(int x1, int y1, int x2, int y2, const char *color) {
  this->send_command_printf("line %d,%d,%d,%d,%s", x1, y1, x2, y2, color);
}
void Nextion::line(int x1, int y1, int x2, int y2, Color color) {
  this->send_command_printf("line %d,%d,%d,%d,%d", x1, y1, x2, y2, color.to_565());
}
void Nextion::rectangle(int x1, int y1, int width, int height, const char *color) {
  this->send_command_printf("draw %d,%d,%d,%d,%s", x1, y1, x1 + width, y1 + height, color);
}
void Nextion::rectangle(int x1, int y1, int width, int height, Color color) {
  this->send_command_printf("draw %d,%d,%d,%d,%d", x1, y1, x1 + width, y1 + height, color.to_565());
}
void Nextion::circle(int center_x, int center_y, int radius, const char *color) {
  this->send_command_printf("cir %d,%d,%d,%s", center_x, center_y, radius, color);
}
void Nextion::circle(int center_x, int center_y, int radius, Color color) {
  this->send_command_printf("cir %d,%d,%d,%d", center_x, center_y, radius, color.to_565());
}
void Nextion::filled_circle(int center_x, int center_y, int radius, const char *color) {
  this->send_command_printf("cirs %d,%d,%d,%s", center_x, center_y, radius, color);
}
void Nextion::filled_circle(int center_x, int center_y, int radius, Color color) {
  this->send_command_printf("cirs %d,%d,%d,%d", center_x, center_y, radius, color.to_565());
}
bool Nextion::read_until_ack_() {
  while (this->available() >= 4) {
    // flush preceding filler bytes
    uint8_t temp;
    while (this->available() && this->peek_byte(&temp) && temp == 0xFF)
      this->read_byte(&temp);

    if (!this->available())
      break;

    uint8_t event;
    // event type
    this->read_byte(&event);

    uint8_t data[255];
    // total length of data (including end bytes)
    uint8_t data_length = 0;
    // message is terminated by three consecutive 0xFF
    // this variable keeps track of ohow many of those have
    // been received
    uint8_t end_length = 0;
    while (this->available() && end_length < 3 && data_length < sizeof(data)) {
      uint8_t byte;
      this->read_byte(&byte);
      if (byte == 0xFF) {
        end_length++;
      } else {
        end_length = 0;
      }
      data[data_length++] = byte;
    }

    if (end_length != 3) {
      ESP_LOGW(TAG, "Received unknown filler end bytes from Nextion!");
      continue;
    }

    data_length -= 3;  // remove filler bytes

    bool invalid_data_length = false;
    switch (event) {
      case 0x01:  // successful execution of instruction (ACK)
        return true;
      case 0x00:  // invalid instruction
        ESP_LOGW(TAG, "Nextion reported invalid instruction!");
        break;
      case 0x02:  // component ID invalid
        ESP_LOGW(TAG, "Nextion reported component ID invalid!");
        break;
      case 0x03:  // page ID invalid
        ESP_LOGW(TAG, "Nextion reported page ID invalid!");
        break;
      case 0x04:  // picture ID invalid
        ESP_LOGW(TAG, "Nextion reported picture ID invalid!");
        break;
      case 0x05:  // font ID invalid
        ESP_LOGW(TAG, "Nextion reported font ID invalid!");
        break;
      case 0x11:  // baud rate setting invalid
        ESP_LOGW(TAG, "Nextion reported baud rate invalid!");
        break;
      case 0x12:  // curve control ID number or channel number is invalid
        ESP_LOGW(TAG, "Nextion reported control/channel ID invalid!");
        break;
      case 0x1A:  // variable name invalid
        ESP_LOGW(TAG, "Nextion reported variable name invalid!");
        break;
      case 0x1B:  // variable operation invalid
        ESP_LOGW(TAG, "Nextion reported variable operation invalid!");
        break;
      case 0x1C:  // failed to assign
        ESP_LOGW(TAG, "Nextion reported failed to assign variable!");
        break;
      case 0x1D:  // operate EEPROM failed
        ESP_LOGW(TAG, "Nextion reported operating EEPROM failed!");
        break;
      case 0x1E:  // parameter quantity invalid
        ESP_LOGW(TAG, "Nextion reported parameter quantity invalid!");
        break;
      case 0x1F:  // IO operation failed
        ESP_LOGW(TAG, "Nextion reported component I/O operation invalid!");
        break;
      case 0x20:  // undefined escape characters
        ESP_LOGW(TAG, "Nextion reported undefined escape characters!");
        break;
      case 0x23:  // too long variable name
        ESP_LOGW(TAG, "Nextion reported too long variable name!");
        break;
      case 0x65: {  // touch event return data
        if (data_length != 3) {
          invalid_data_length = true;
          break;
        }
        uint8_t page_id = data[0];
        uint8_t component_id = data[1];
        uint8_t touch_event = data[2];  // 0 -> release, 1 -> press
        ESP_LOGD(TAG, "Got touch page=%u component=%u type=%s", page_id, component_id,
                 touch_event ? "PRESS" : "RELEASE");
        for (auto *touch : this->touch_) {
          touch->process_touch(page_id, component_id, touch_event == 0 ? false : true);
        }
        break;
      }
      case 0x67:
      case 0x68: {  // touch coordinate data
        if (data_length != 5) {
          invalid_data_length = true;
          break;
        }
        uint16_t x = (uint16_t(data[0]) << 8) | data[1];
        uint16_t y = (uint16_t(data[2]) << 8) | data[3];
        uint8_t touch_event = data[4];  // 0 -> release, 1 -> press
        ESP_LOGD(TAG, "Got touch at x=%u y=%u type=%s", x, y, touch_event ? "PRESS" : "RELEASE");
        break;
      }
      case 0x66:    // sendme page id
      case 0x70:    // string variable data return
      case 0x71:    // numeric variable data return
      case 0x86: {  // device automatically enters into sleep mode
        this->sleep_callback_.call(true);
        break;
      }
      case 0x87:  // device automatically wakes up
      {
        this->wake_callback_.call(true);
        break;
      }
      case 0x88:  // system successful start up
      case 0x89:  // start SD card upgrade
      // Data from nextion is
      // 0x90 - Start
      // variable length of 0x70 return formatted data (bytes) that contain the variable name: prints "temp1",0
      // 00 - NULL
      // 00/01 - Single byte for on/off
      // FF FF FF - End
      case 0x90: {  // Switched component
        char variable_name[64];
        uint8_t variable_name_end = 0;
        uint8_t index = 0;

        // Get variable name
        for (index = 0; index < data_length; ++index) {
          variable_name[index] = data[index];
          if (data[index] == 0) {  // First Null
            variable_name_end = index;
            break;
          }
        }
        if (variable_name_end == 0) {
          invalid_data_length = true;
          break;
        }
        ++index;

        ESP_LOGD(TAG, "Got Switch variable_name=%s value=%d", variable_name, data[index] == 0 ? false : true);
        for (auto *switchtype : this->switchtype_) {
          switchtype->process_bool(&variable_name[0], data[index] == 0 ? false : true);
        }
        break;
      }
      // Data from nextion is
      // 0x91 - Start
      // variable length of 0x70 return formatted data (bytes) that contain the variable name: prints "temp1",0
      // 00 - NULL
      // variable length of 0x71 return data: prints temp1.val,0
      // FF FF FF - End
      case 0x91: {  // Sensor component
        char variable_name[64];
        uint8_t variable_name_end = 0;
        uint8_t index = 0;

        // Get variable name
        for (index = 0; index < data_length; ++index) {
          variable_name[index] = data[index];
          if (data[index] == 0) {  // First Null
            variable_name_end = index;
            break;
          }
        }
        if (variable_name_end == 0) {
          invalid_data_length = true;
          break;
        }

        // Get variable data
        for (int i = index + 1; i < data_length; ++i)
          ESP_LOGD(TAG, "Received get_int response %d %d", i, data[i]);

        int value = 0;
        int dataindex = 0;
        for (int i = 0; i < data_length - index - 1; ++i) {
          value += data[i + index + 1] << (8 * i);
          ++dataindex;
        }

        // if the length is < 4 than its a negative.
        // fill in any missing bytes
        if (dataindex < 4) {
          for (int i = dataindex; i < 4; ++i) {
            value += 255 << (8 * i);
          }
        }

        ESP_LOGD(TAG, "Got sensor variable_name=%s value=%d", variable_name, value);
        for (auto *sensor : this->sensortype_) {
          sensor->process_sensor(&variable_name[0], value);
        }
        break;
      }
      // Data from nextion is
      // 0x92 - Start
      // variable length of 0x70 return formatted data (bytes) that contain the variable name: prints "temp1",0
      // 00 - NULL
      // variable length of 0x70 return formatted data (bytes) that contain the text prints temp1.txt,0
      // 00 - NULL
      // FF FF FF - End
      case 0x92: {  // Text Sensor Component
        char variable_name[64];
        char text_value[128];
        uint8_t variable_name_end = 0;
        uint8_t index = 0;

        // Get variable name
        for (index = 0; index < data_length; ++index) {
          variable_name[index] = data[index];
          if (data[index] == 0) {  // First Null
            variable_name_end = index;
            break;
          }
        }
        if (variable_name_end == 0) {
          invalid_data_length = true;
          break;
        }

        variable_name_end = 0;
        int data_index = 0;
        for (int i = index + 1; i < data_length; ++i) {
          text_value[data_index++] = data[i];
          if (data[i] == 0) {  // Second Null
            variable_name_end = index;
            break;
          }
        }
        if (variable_name_end == 0) {
          invalid_data_length = true;
          break;
        }

        ESP_LOGD(TAG, "Got Text Sensor variable_name=%s value=%s", variable_name, text_value);
        for (auto *textsensortype : this->textsensortype_) {
          textsensortype->process_text(&variable_name[0], &text_value[0]);
        }
        break;
      }
      // Data from nextion is
      // 0x90 - Start
      // variable length of 0x70 return formatted data (bytes) that contain the variable name: prints "temp1",0
      // 00 - NULL
      // 00/01 - Single byte for on/off
      // FF FF FF - End
      case 0x93: {  // Binary Sensor component
        char variable_name[64];
        uint8_t variable_name_end = 0;
        uint8_t index = 0;

        // Get variable name
        for (index = 0; index < data_length; ++index) {
          variable_name[index] = data[index];
          if (data[index] == 0) {  // First Null
            variable_name_end = index;
            break;
          }
        }
        if (variable_name_end == 0) {
          invalid_data_length = true;
          break;
        }
        ++index;

        ESP_LOGD(TAG, "Got Binary Sensor variable_name=%s value=%d", variable_name, data[index] == 0 ? false : true);
        for (auto *binarysensortype : this->binarysensortype_) {
          binarysensortype->process_bool(&variable_name[0], data[index] == 0 ? false : true);
        }
        break;
      }
      case 0xFD:  // data transparent transmit finished
      case 0xFE:  // data transparent transmit ready
        break;
      default:
        ESP_LOGW(TAG, "Received unknown event from nextion: 0x%02X", event);
        break;
    }
    if (invalid_data_length) {
      ESP_LOGW(TAG, "Event: %d , invalid data length (%d) from nextion!", event, data_length);
      for (int index = 0; index < data_length; ++index) {
        ESP_LOGD("nextion loop invalid_data_length", "data[%d] %d", index, data[index]);
      }
    }
  }

  return false;
}  // namespace nextion
void Nextion::loop() {
  while (this->available() >= 4 && !this->is_updating_) {
    this->read_until_ack_();
  }
}
#ifdef USE_TIME
void Nextion::set_nextion_rtc_time(time::ESPTime time) {
  this->send_command_printf("rtc0=%u", time.year);
  this->send_command_printf("rtc1=%u", time.month);
  this->send_command_printf("rtc2=%u", time.day_of_month);
  this->send_command_printf("rtc3=%u", time.hour);
  this->send_command_printf("rtc4=%u", time.minute);
  this->send_command_printf("rtc5=%u", time.second);
}
#endif

//  0x70 0x61 0x62 0x31 0x32 0x33 0xFF 0xFF 0xFF
//  Returned when using get command for a string.
//  Each byte is converted to char.
//  data: ab123
bool Nextion::get_string(const char *component_id, char *string_buffer) {
  char command[64];
  sprintf(command, "get %s", component_id);
  String response = "";
  this->send_command_no_ack(command);
  this->recv_ret_string_(response);

  if (response[0] == 0x70) {
    response.remove(0, 1);
    strcpy(string_buffer, response.c_str());
    if (this->print_debug_)
      ESP_LOGD(TAG, "Received get_string response \"%s\" for component id %s", string_buffer, component_id);
    return true;
  } else {
    ESP_LOGD(TAG, "Received unknown get_string response \"%s\" for component id %s", response.c_str(), component_id);
  }
  return false;
}

uint16_t Nextion::recv_ret_string_(String &response, uint32_t timeout, bool recv_flag) {
#if defined ESP8266
  yield();
#endif

  uint16_t ret = 0;
  uint8_t c = 0;
  uint8_t nr_of_ff_bytes = 0;
  long start;
  bool exit_flag = false;
  bool ff_flag = false;
  if (timeout != 500)
    ESP_LOGD(TAG, "timeout serial read: %d", timeout);

  start = millis();

  while (millis() - start <= timeout) {
    while (this->available()) {
      this->read_byte(&c);
      if (c == 0) {
        continue;
      }

      if (c == 0xFF)
        nr_of_ff_bytes++;
      else {
        nr_of_ff_bytes = 0;
        ff_flag = false;
      }

      if (nr_of_ff_bytes >= 3)
        ff_flag = true;

      response += (char) c;

      if (recv_flag) {
        if (response.indexOf(0x05) != -1) {
          exit_flag = true;
        }
      }
    }
    if (exit_flag || ff_flag) {
      break;
    }
  }

  if (ff_flag)
    response = response.substring(0, response.length() - 3);  // Remove last 3 0xFF

  ret = response.length();
  return ret;
}

//  0x71 0x01 0x02 0x03 0x04 0xFF 0xFF 0xFF
//  Returned when get command to return a number
//  4 byte 32-bit value in little endian order.
//  (0x01+0x02*256+0x03*65536+0x04*16777216)
//  data: 67305985
int Nextion::get_int(const char *component_id) {
  char command[64];
  String response = "";
  int value = 0;

  sprintf(command, "get %s", component_id);
  this->send_command_no_ack(command);

  this->recv_ret_string_(response);

  if (response[0] == 0x71) {
    response.remove(0, 1);

    uint8_t num_byte_index = 0;

    int dataindex = 0;
    for (int i = 0; i < response.length(); ++i) {
      value += response[i] << (8 * i);
      ++dataindex;
    }

    // if the length is < 4 than its a negative. 2s complement conversion is needed and
    // fill in any missing bytes and then flip the bits
    if (dataindex < 4) {
      for (int i = dataindex; i < 4; ++i) {
        value += 255 << (8 * i);
      }
    }

  } else if (response[0] == 0x02) {
    if (this->print_debug_)
      ESP_LOGD(TAG, "Received invalid variable name for component id %s", component_id);
  } else {
    if (this->print_debug_)
      ESP_LOGD(TAG, "Received unknown get_int response, length: \"%d\"  first_value: \"%d\" for component id %s",
               response.length(), response.c_str()[0], component_id);
  }
  return value;
}

void Nextion::soft_reset() { this->send_command_no_ack("rest"); }

void Nextion::set_backlight_brightness(uint8_t brightness) { this->send_command_printf("dim=%u", brightness); }
void Nextion::set_touch_sleep_timeout(uint16_t timeout) { this->send_command_printf("thsp=%u", timeout); }
void Nextion::set_wake_up_page(uint8_t page_id) { this->send_command_printf("wup=%u", page_id); }
void Nextion::set_auto_wake_on_touch(bool auto_wake) {
  auto_wake ? this->send_command_no_ack("thup=1") : this->send_command_no_ack("thup=0");
}
void Nextion::sleep(bool sleep) { sleep ? this->send_command_no_ack("sleep=1") : this->send_command_no_ack("sleep=0"); }
void Nextion::set_writer(const nextion_writer_t &writer) { this->writer_ = writer; }
void Nextion::set_component_text_printf(const char *component, const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  char buffer[256];
  int ret = vsnprintf(buffer, sizeof(buffer), format, arg);
  va_end(arg);
  if (ret > 0)
    this->set_component_text(component, buffer);
}
void Nextion::set_wait_for_ack(bool wait_for_ack) { this->wait_for_ack_ = wait_for_ack; }

}  // namespace nextion
}  // namespace esphome
