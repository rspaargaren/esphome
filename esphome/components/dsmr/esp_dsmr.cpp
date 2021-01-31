#include "esphome/components/dsmr/esp_dsmr.h"
#include "esphome/core/log.h"
#include "Arduino.h"

namespace esphome {
namespace espdsmr {

static const char *TAG = "dsmr";

struct printer {
  template<typename Item> void apply(Item &i) {
    if (i.present()) {
      for (auto *sensortype : this->sensortype_) {
        sensortype->update_component();
      }
      // Serial.print(Item::name);
      // Serial.print(F(": "));
      // Serial.print(i.val());
      // Serial.print(Item::unit());
      // Serial.println();
    }
  }
};

void EspDsmr::setup() {
  pinMode(5, OUTPUT);    // Set D5 as output pin
  digitalWrite(5, LOW);  // Set low, don't request message from P1 port
}

float EspDsmr::get_setup_priority() const { return setup_priority::PROCESSOR; }

void EspDsmr::update() {
  digitalWrite(5, HIGH);  // TO BE ADJUSTED IN CONFIGURATION
}

void EspDsmr::loop() {
  while (this->available()) {
    if (this->telegramlen_ >= Max_Telegraph_Length) {  // Buffer overflow
      this->header_found_ = false;
      this->footer_found_ = false;
      ESP_LOGE(TAG, "Error: Message larger than buffer!");
    }
    this->bytes_read_++;
    this->tel_char_ = this->read();
    if (this->tel_char_ == 47) {  // header: forward slash
      this->header_found_ = true;
      this->telegramlen_ = 0;
      ESP_LOGD(TAG, "Header found in message");
    }
    if (this->header_found_) {
      this->telegram_[this->telegramlen_] = this->tel_char_;
      this->telegramlen_++;
      if (this->tel_char_ == 33) {  // footer: exclamation mark
        ESP_LOGD(TAG, "Footer found in message");
        this->footer_found_ = true;
      } else {
        if (this->footer_found_ && this->tel_char_ == 10) {  // last \n after footer
          // Parse message
          MyData data;
          // ESP_LOGD("DmsrCustom","Trying to parse");
          ParseResult<void> res = P1Parser::parse(&data, telegram_, telegramlen_, false);
          // Parse telegram accoring to data definition. Ignore unknown values.
          if (res.err) {
            // Parsing error, show it
            // Serial.println(res.fullError(telegram, telegram + telegramlen));
          } else {
            data.applyEach(printer());
            // publish_sensors(data);
            // return true;  // break out function
          }
        }
      }
    }
  }
}

void EspDsmr::dump_config() {}

}  // namespace espdsmr
}  // namespace esphome