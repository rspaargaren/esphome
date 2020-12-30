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
  for (auto *sensortype : this->sensortype_) {
    sensortype->restore_from_nextion();
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
void Nextion::set_component_text(const char *component, const char *text) {
  this->send_command_printf("%s.txt=\"%s\"", component, text);
}
void Nextion::set_component_value(const char *component, int value) {
  this->send_command_printf("%s.val=%d", component, value);
}
void Nextion::display_picture(int picture_id, int x_start, int y_start) {
  this->send_command_printf("pic %d %d %d", x_start, y_start, picture_id);
}
void Nextion::set_component_background_color(const char *component, const char *color) {
  this->send_command_printf("%s.bco=\"%s\"", component, color);
}
void Nextion::set_component_background_color(const char *component, Color color) {
  this->send_command_printf("%s.bco=%d", component, color.to_565());
}
void Nextion::set_component_pressed_background_color(const char *component, const char *color) {
  this->send_command_printf("%s.bco2=\"%s\"", component, color);
}
void Nextion::set_component_pressed_background_color(const char *component, Color color) {
  this->send_command_printf("%s.bco2=%d", component, color.to_565());
}
void Nextion::set_component_font_color(const char *component, const char *color) {
  this->send_command_printf("%s.pco=\"%s\"", component, color);
}
void Nextion::set_component_font_color(const char *component, Color color) {
  this->send_command_printf("%s.pco=%d", component, color.to_565());
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
          touch->process(page_id, component_id, touch_event == 0 ? false : true);
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
      case 0x86:    // device automatically enters into sleep mode
      case 0x87:    // device automatically wakes up
      case 0x88:    // system successful start up
      case 0x89:    // start SD card upgrade
      case 0x90: {  // Response number made to read state from Nextion
        if (data_length != 3) {
          invalid_data_length = true;
          break;
        }
        uint8_t page_id = data[0];
        uint8_t component_id = data[1];
        uint8_t touch_event = data[2];  // 0 -> release, 1 -> press
        ESP_LOGD(TAG, "Got special state 0x90 page=%u component=%u type=%s", page_id, component_id,
                 touch_event ? "PRESS" : "RELEASE");
        for (auto *touch : this->switchtype_) {
          touch->process(page_id, component_id, touch_event);
        }
        break;
      }
      case 0x91: {
        if (data_length != 3) {
          invalid_data_length = true;
          break;
        }
        uint8_t page_id = data[0];
        uint8_t component_id = data[1];
        uint8_t touch_event = data[2];  // 0 -> release, 1 -> press
        ESP_LOGD(TAG, "Got special state 0x91 page=%u component=%u type=%u", page_id, component_id, touch_event);
        for (auto *sensor : this->sensortype_) {
          sensor->process_sensor(page_id, component_id, touch_event);
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
      ESP_LOGW(TAG, "Invalid data length from nextion!");
    }
  }

  return false;
}
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
bool Nextion::upload_from_stream_(Stream &my_file, int content_length) {
#if defined ESP8266
  yield();
#endif

  // create buffer for read
  uint8_t buff[2048] = {0};
  // read all data from server
  while (content_length > 0) {
    // get available data size
    size_t size = my_file.available();
    if (size) {
      int c = my_file.readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
      if (this->debug_print_)
        ESP_LOGD(TAG, "upload_from_stream_ sending %d bytes : total %d", c, this->total_);
      // Write the buffered bytes to the nextion. If this fails, return false.
      if (!this->upload_from_buffer_(buff, c)) {
        return false;
      }

      if (content_length > 0) {
        content_length -= c;
      }
    }
    delay(1);
  }

  return true;
}

//  0x70 0x61 0x62 0x31 0x32 0x33 0xFF 0xFF 0xFF
//  Returned when using get command for a string.
//  Each byte is converted to char.
//  data: ab123
bool Nextion::gets(const char *component_id, char *string_buffer) {
  char command[64];
  sprintf(command, "get %s.txt", component_id);
  String response = "";
  this->send_command_no_ack(command);
  this->recv_ret_string_(response);

  if (response[0] == 0x70) {
    response.remove(0, 1);
    strcpy(string_buffer, response.c_str());
    ESP_LOGD(TAG, "Received gets response \"%s\" for component id %s", string_buffer, component_id);
    return true;
  } else {
    ESP_LOGD(TAG, "Received unknown gets response \"%s\" for component id %s", response.c_str(), component_id);
  }
  return false;
}

//  0x71 0x01 0x02 0x03 0x04 0xFF 0xFF 0xFF
//  Returned when get command to return a number
//  4 byte 32-bit value in little endian order.
//  (0x01+0x02*256+0x03*65536+0x04*16777216)
//  data: 67305985
uint32_t Nextion::getn(const char *component_id) {
  char command[64];
  String response = "";
  uint32_t return_value = 0;

  sprintf(command, "get %s.val", component_id);
  this->send_command_no_ack(command);

  this->recv_ret_string_(response);
  if (response[0] == 0x71) {
    response.remove(0, 1);
    for (int i = 0; i < response.length(); ++i) {
      uint32_t factor = pow(2, (i * 8));
      uint32_t to_add = response[i] * factor;
      return_value += to_add;
    }
    ESP_LOGD(TAG, "Received gets response (%d) for component id %s", return_value, component_id);
  } else {
    ESP_LOGD(TAG, "Received unknown gets response  \"%s\" for component id %s", response.c_str(), component_id);
  }
  return return_value;
}

bool Nextion::upload_from_buffer_(const uint8_t *file_buf, size_t buf_size) {
#if defined ESP8266
  yield();
#endif

  uint8_t c;
  uint8_t timeout = 0;
  String string = String("");

  for (uint16_t i = 0; i < buf_size; i++) {
    // Users must split the .tft file contents into 4096 byte sized packets with
    // the final partial packet size equal to the last remaining bytes (<4096
    // bytes).
    if (this->sent_packets_ == 4096) {
      // wait for the Nextion to return its 0x05 byte confirming reception and
      // readiness to receive the next packets
      this->recv_ret_string_(string, 500, true);
      if (string.indexOf(0x05) != -1) {
        // reset sent packets counter
        this->sent_packets_ = 0;

        // reset receive String
        string = "";
      } else {
        if (timeout >= 8) {
          ESP_LOGD(TAG, "serial connection lost");
          return false;
        }

        timeout++;
      }

      // delay current byte
      i--;

    } else {
      // read buffer
      c = file_buf[i];

      // write byte to nextion over serial
      this->write_byte(c);

      // update sent packets counter
      this->sent_packets_++;
      this->total_++;
      delayMicroseconds(100);
    }
  }

  return true;
}

bool Nextion::upload_by_chunks_(int content_length, int chunk_size) {
  if (this->debug_print_)
    ESP_LOGD(TAG, "upload_by_chunks_: contentLength %d , chunk_size: %d", content_length, chunk_size);

  for (int range_start = 0; range_start < content_length; range_start += chunk_size) {
    int range_end = range_start + chunk_size - 1;
    if (range_end > content_length)
      range_end = content_length;

    HTTPClient http;
    if (!http.begin(this->tft_url_.c_str())) {
      ESP_LOGD(TAG, "upload_by_chunks_: connection failed");
      return false;
    }
    char range_header[64];
    sprintf(range_header, "bytes=%d-%d", range_start, range_end);

    http.addHeader("Range", range_header);

    if (this->debug_print_)
      ESP_LOGD(TAG, "upload_by_chunks_ Requesting range: %s", range_header);

    // http.setReuse(true);
    int tries = 1;
    int code = http.GET();
    while (code != 200 && code != 206 && tries <= 5) {
      ESP_LOGD(TAG, "upload_by_chunks_ retrying (%d/5)", tries);
      for (int i = 0; i < 12; ++i)  // Need a decent delay and since we will be rebooting this shouldnt be an issue.
        delay(40);

      code = http.GET();
      ++tries;
    }
    if (code == 200 || code == 206) {
      // Upload the received byte Stream to the nextion
      bool result = this->upload_from_stream_(*http.getStreamPtr(), range_end - range_start);
      if (result) {
        if (this->debug_print_)
          ESP_LOGD(TAG, "Succesfully sent chunk to Nextion");
      } else {
        ESP_LOGD(TAG, "upload_by_chunks_: Error updating Nextion");
        http.end();
        return false;
      }
    } else {
      http.end();
      return false;
    }
    http.end();  // End this HTTP call because we read all the data
  }

  if (content_length % 4096 != 0) {  // If not in 4096 chunks wait for the last bits to confirm
    String string = String("");
    uint8_t timeout = 0;
    this->recv_ret_string_(string, 500, true);
    if (string.indexOf(0x05) != -1) {
      // reset sent packets counter
      this->sent_packets_ = 0;

      // reset receive String
      string = "";
    } else {
      if (timeout >= 8) {
        ESP_LOGD(TAG, "serial connection lost");
        return false;
      }

      timeout++;
    }
  }

  return true;
}

uint16_t Nextion::recv_ret_string_(String &response, uint32_t timeout, bool recv_flag) {
#if defined ESP8266
  yield();
#endif

  uint16_t ret = 0;
  uint8_t c = 0;
  uint8_t nr_of_FF_bytes = 0;
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
        nr_of_FF_bytes++;
      else {
        nr_of_FF_bytes = 0;
        ff_flag = false;
      }

      if (nr_of_FF_bytes >= 3)
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

void Nextion::soft_reset() { this->send_command_no_ack("rest"); }

void Nextion::upload_tft() {
  int old_baud = this->parent_->get_baud_rate();
  if (this->has_updated_) {
    ESP_LOGD(TAG, "Already Updated");
    return;
  }

  if (this->is_updating_) {
    ESP_LOGD(TAG, "Currently updating");
    return;
  }

  if (!network_is_connected()) {
    ESP_LOGD(TAG, "network is not connected");
    return;
  }

  ESP_LOGD(TAG, "Updating tft from : %s", this->tft_url_.c_str());

  this->is_updating_ = true;
  this->total_ = 0;

  HTTPClient http;
  if (!http.begin(this->tft_url_.c_str())) {
    this->is_updating_ = false;
    ESP_LOGD(TAG, "connection failed");
    return;
  } else {
    ESP_LOGD(TAG, "Connected");
  }
  http.addHeader("Range", "bytes=0-255");
  const char *header_names[] = {"Content-Range"};
  http.collectHeaders(header_names, 1);
  ESP_LOGD(TAG, "Requesting URL: %s", this->tft_url_.c_str());

  http.setReuse(true);
  // try up to 5 times. DNS sometimes needs a second try or so
  int tries = 0;
  int code = http.GET();
  while (code != 200 && code != 206 && tries < 5) {
    for (int i = 0; i < 12; ++i)  // Need a decent delay and since we will be rebooting this shouldnt be an issue.
      delay(40);
    code = http.GET();
    ++tries;
  }
  // Update the nextion display
  if (code == 200 || code == 206) {
    String content_range_string = http.header("Content-Range");
    content_range_string.remove(0, 12);
    int content_length = content_range_string.toInt();
    http.end();  // End this HTTP call because we read all the data
    delay(2);

    ESP_LOGD(TAG, "Updating Nextion...");

    String response = String("");
    bool result;
    char command[128];
    sprintf(command, "whmi-wri %d,%d,0", content_length, this->parent_->get_baud_rate());
    this->send_command_no_ack(command);
    // Flush
    this->flush();

    this->recv_ret_string_(response, 800, true);  // normal response time is 400ms

    // The Nextion display will, if it's ready to accept data, send a 0x05 byte.
    if (response.indexOf(0x05) != -1) {
      ESP_LOGD(TAG, "preparation for tft update done");
    } else {
      ESP_LOGD(TAG, "preparation for tft update failed");
      this->sent_packets_ = 0;
      this->is_updating_ = false;
      return;
    }
    ESP_LOGD(TAG, "Start upload. File size is: %d bytes", content_length);
    // Upload the received byte Stream to the nextion
    result = this->upload_by_chunks_(content_length);

    if (result) {
      ESP_LOGD(TAG, "Succesfully updated Nextion! Sleep for 1600ms");
    } else {
      ESP_LOGD(TAG, "Error updating Nextion:");
      this->sent_packets_ = 0;
      this->is_updating_ = false;
      return;
    }

    // end: wait(delay) for the nextion to finish the update process, send
    // nextion reset command and end the serial connection to the nextion
    // wait for the nextion to finish internal processes
    delay(45);
    delay(45);

    // soft reset the nextion
    this->soft_reset();

    ESP_LOGD(TAG, "Nextion has been updated");
    this->has_updated_ = true;
    ESP.restart();
  } else {
    ESP_LOGD(TAG, "Nextion has NOT been updated, Bad HTTP status %d", code);
  }
  this->sent_packets_ = 0;
  this->is_updating_ = false;

}  // namespace nextion

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
