
#include "nextion.h"
#include "esphome/core/util.h"
#include "esphome/core/log.h"

namespace esphome {
namespace nextion {

static const char *TAG = "nextion_upload";
// Followed guide
// https://nextion.tech/2017/12/08/nextion-hmi-upload-protocol-v1-1/

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

      // update sent packets and total counter
      this->sent_packets_++;
      this->total_++;
    }
  }

  return true;
}

#ifdef ARDUINO_ARCH_ESP8266
WiFiClient *Nextion::get_wifi_client_() {
  if (this->wifi_client_ == nullptr) {
    this->wifi_client_ = new WiFiClient();
  }
  return this->wifi_client_;
}
#endif

bool Nextion::upload_by_chunks_(int content_length, uint32_t chunk_size) {
  if (this->print_debug_)
    ESP_LOGD(TAG, "upload_by_chunks_: contentLength %d , chunk_size: %d", content_length, chunk_size);

  for (int range_start = 0; range_start < content_length; range_start += chunk_size) {
    int range_end = range_start + chunk_size - 1;
    if (range_end > content_length)
      range_end = content_length;

    HTTPClient http;

    bool begin_status = false;
#ifdef ARDUINO_ARCH_ESP32
    begin_status = http.begin(this->tft_url_.c_str());
#endif
#ifdef ARDUINO_ARCH_ESP8266
#ifndef CLANG_TIDY
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.setRedirectLimit(3);
    begin_status = http.begin(*this->get_wifi_client_(), this->tft_url_.c_str());
#endif
#endif

    if (!begin_status) {
      ESP_LOGD(TAG, "upload_by_chunks_: connection failed");
      return false;
    }

    char range_header[64];
    sprintf(range_header, "bytes=%d-%d", range_start, range_end);

    http.addHeader("Range", range_header);

    if (this->print_debug_)
      ESP_LOGD(TAG, "upload_by_chunks_ Requesting range: %s", range_header);

    int tries = 1;
    int code = http.GET();
    while (code != 200 && code != 206 && tries <= 5) {
      ESP_LOGD(TAG, "upload_by_chunks_ retrying (%d/5)", tries);
      for (int i = 0; i < 12; ++i)  // Needs a decent delay and since we will be rebooting this shouldnt be an issue.
        delay(40);

      code = http.GET();
      ++tries;
    }
    if (code == 200 || code == 206) {
      // Upload the received byte Stream to the nextion
      bool result = this->upload_from_stream_(*http.getStreamPtr(), range_end - range_start, chunk_size);
      if (result) {
        if (this->print_debug_)
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

  if (content_length % 4096 != 0) {  // Wait for the last bits to confirm. Normally the above loop does this
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

bool Nextion::upload_from_stream_(Stream &my_file, int content_length, uint32_t chunk_size) {
#if defined ESP8266
  yield();
#endif
  // Anything over 65K seems to cause uart issues
  int mysize = chunk_size > 65536 ? 65536 : chunk_size;

  if (this->transfer_buffer_ == nullptr) {
    if (this->print_debug_)
      ESP_LOGD(TAG, "upload_from_stream_ allocating %d buffer", mysize);
    this->transfer_buffer_ = new uint8_t[mysize];
    if (!this->transfer_buffer_) {  // Try a smaller size
      ESP_LOGD(TAG, "upload_from_stream_ could not allocate buffer size: %d trying 8192 instead", mysize);
      mysize = 8192;
      if (this->print_debug_)
        ESP_LOGD(TAG, "upload_from_stream_ allocating %d buffer", mysize);
      this->transfer_buffer_ = new uint8_t[mysize];
      return false;
    }
  }

  int dosend = 0;
  int start = millis();

  while (content_length > 0) {
    size_t size = my_file.available();
    if (size) {
      if (dosend + size >= mysize) {
        if (this->print_debug_)
          ESP_LOGD(TAG, "upload_from_stream_ write_array %d %d", dosend, size);
        if (!this->upload_from_buffer_(transfer_buffer_, dosend)) {
          return false;
        }
        dosend = 0;
      }
      int c = my_file.readBytes(&transfer_buffer_[dosend], ((size > mysize) ? mysize : size));
      dosend += c;
      if (content_length > 0) {
        content_length -= c;
      }
    }
  }
  if (dosend != 0) {
    if (!this->upload_from_buffer_(transfer_buffer_, dosend)) {
      return false;
    }
  }
  uint32_t end = millis() - start;
  uint32_t realms = (chunk_size * 1000) / end;
  ESP_LOGD(TAG, "upload_from_stream_ %d bytes in %d ms, %d  bytes/sec", this->total_, end, realms);
  return true;
}
void Nextion::upload_tft() {
  if (this->is_updating_) {
    ESP_LOGD(TAG, "Currently updating");
    return;
  }

  if (!network_is_connected()) {
    ESP_LOGD(TAG, "network is not connected");
    return;
  }

  // This controls the range got from the webserver and the transfer buffer
  // The http client "can" use up to this if we arent fast enough so its best
  // to leave room for both. We send 4096 bytes to the Nextion so get
  // x 4096 chunks
  int chunk = ((ESP.getFreeHeap()) * .4) / 4096;  // 40% for the chunk and maybe the http buffer.
  uint32_t chunk_size = chunk * 4096;

  ESP_LOGD(TAG, "Heap Size %d", ESP.getFreeHeap());

  ESP_LOGD(TAG, "Updating tft from : %s : using %d chunksize", this->tft_url_.c_str(), chunk_size);

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

  // OK or Partial Content
  if (code == 200 || code == 206) {
    String content_range_string = http.header("Content-Range");
    content_range_string.remove(0, 12);
    int content_length = content_range_string.toInt();
    http.end();  // End this HTTP call because we read all the data
    delay(2);

    ESP_LOGD(TAG, "Updating Nextion...");
    // The Nextion will ignore the update command if it is sleeping
    this->sleep(false);
    for (int delayloop = 0; delayloop < 20; ++delayloop)  // Wait for it to wake up
      delay(40);

    char command[128];
    // Tells the Nextion the content length of the tft file and baud rate it will be sent at
    // Once the Nextion accepts the command it will wait until the file is successfully uploaded
    // If it fails for any reason a power cycle of the display will be needed
    sprintf(command, "whmi-wri %d,%d,0", content_length, this->parent_->get_baud_rate());
    this->send_command_no_ack(command);
    // Flush serial
    this->flush();

    String response = String("");
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

    bool result = this->upload_by_chunks_(content_length, chunk_size);

    if (result) {
      ESP_LOGD(TAG, "Succesfully updated Nextion!");
    } else {
      ESP_LOGD(TAG, "Error updating Nextion:");
    }

    this->upload_end_();
  }
}

void Nextion::upload_end_() {
  this->soft_reset();
  ESP.restart();
}
}  // namespace nextion
}  // namespace esphome
