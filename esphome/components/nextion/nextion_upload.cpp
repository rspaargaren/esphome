
#include "nextion.h"
#include "esphome/core/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nextion {

static const char *TAG = "nextion_upload";

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
}

}  // namespace nextion
}  // namespace esphome