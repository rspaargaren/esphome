#include "esphome/components/dsmr/esp_dsmr.h"
#include "esphome/core/log.h"
#include "dsmr_data.h"

namespace esphome {
namespace dsmr {

static const char *TAG = "dsmr";

void Dsmr::setup() {
  pinMode(D5, OUTPUT);    // Set D5 as output pin
  digitalWrite(D5, LOW);  // Set low, don't request message from P1 port
}

float Dsmr::get_setup_priority() const { return setup_priority::PROCESSOR; }

void Dsmr::update() {
  digitalWrite(D5, HIGH);  // TO BE ADJUSTED IN CONFIGURATION
}

void Dsmr::loop() {
  while (this->available()) {
    if (this->telegramlen_ >= Max_Telegraph_Length) {  // Buffer overflow
      this->header_found_ = false;
      this->footer_found_ = false;
      ESP_LOGE(TAG, "Error: Message larger than buffer!");
    }
    this->bytes_read++;
    this->tel_char_ = this->read();
    if (this->tel_char_ == 47) {  // header: forward slash
      // ESP_LOGD("DmsrCustom","Header found");
      this->header_found_ = true;
      this->telegramlen_ = 0;
      ESP_LOGD(TAG, "Header found in message");
    }
    if (this->header_found_) {
      this->telegram[this->telegramlen_] = this->tel_char_;
      this->telegramlen_++;
      if (this->tel_char_ == 33) {  // footer: exclamation mark
        ESP_LOGD(TAG "Footer found in message");
        this->footer_found_ = true;
      } else {
        if (this->footer_found_ && this->tel_char_ == 10) {  // last \n after footer
          // Parse message
          this->MyData data;
          // ESP_LOGD("DmsrCustom","Trying to parse");
          ParseResult<void> res =
              P1Parser::parse(&data, telegram, telegramlen,
                              false);  // Parse telegram accoring to data definition. Ignore unknown values.
          if (res.err) {
            // Parsing error, show it
            // Serial.println(res.fullError(telegram, telegram + telegramlen));
          } else {
            // publish_sensors(data);
            return true;  // break out function
          }
        }
      }
    }
  }
}

void Dsmr::dump_config() {}

}  // namespace dsmr
}  // namespace esphome