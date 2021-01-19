#include "esphome.h"
#include "dsmr.h"

// * Parsed Data
using MyData = ParsedData<
    /* String */ identification,
    /* String */ p1_version,
    /* String */ timestamp,
    /* String */ equipment_id,
    /* FixedValue */ energy_delivered_tariff1,
    /* FixedValue */ energy_delivered_tariff2,
    /* FixedValue */ energy_returned_tariff1,
    /* FixedValue */ energy_returned_tariff2,
    /* String */ electricity_tariff,
    /* FixedValue */ power_delivered,
    /* FixedValue */ power_returned,
    /* FixedValue */ electricity_threshold,
    /* uint8_t */ electricity_switch_position,
    /* uint32_t */ electricity_failures,
    /* uint32_t */ electricity_long_failures,
    /* String */ electricity_failure_log,
    /* uint32_t */ electricity_sags_l1,
    /* uint32_t */ electricity_sags_l2,
    /* uint32_t */ electricity_sags_l3,
    /* uint32_t */ electricity_swells_l1,
    /* uint32_t */ electricity_swells_l2,
    /* uint32_t */ electricity_swells_l3,
    /* String */ message_short,
    /* String */ message_long,
    /* FixedValue */ voltage_l1,
    /* FixedValue */ voltage_l2,
    /* FixedValue */ voltage_l3,
    /* FixedValue */ current_l1,
    /* FixedValue */ current_l2,
    /* FixedValue */ current_l3,
    /* FixedValue */ power_delivered_l1,
    /* FixedValue */ power_delivered_l2,
    /* FixedValue */ power_delivered_l3,
    /* FixedValue */ power_returned_l1,
    /* FixedValue */ power_returned_l2,
    /* FixedValue */ power_returned_l3,
    /* uint16_t */ gas_device_type,
    /* String */ gas_equipment_id,
    /* uint8_t */ gas_valve_position,
    /* TimestampedFixedValue */ gas_delivered,
    /* uint16_t */ thermal_device_type,
    /* String */ thermal_equipment_id,
    /* uint8_t */ thermal_valve_position,
    /* TimestampedFixedValue */ thermal_delivered,
    /* uint16_t */ water_device_type,
    /* String */ water_equipment_id,
    /* uint8_t */ water_valve_position,
    /* TimestampedFixedValue */ water_delivered,
    /* uint16_t */ slave_device_type,
    /* String */ slave_equipment_id,
    /* uint8_t */ slave_valve_position,
    /* TimestampedFixedValue */ slave_delivered>;

uint8_t req_pin = 5;
unsigned long last;
bool readmessage = false;

P1Reader reader(&Serial, req_pin);

struct Printer {
  template<typename Item> void apply(Item &i) {
    if (i.present()) {
      ESP_LOGD("DmsrCustom", "%s", Item::name);
      Serial.print(Item::name);
      Serial.print(F(": "));
      Serial.print(i.val());
      // ESP_LOGD("DmsrCustom","%s",String(i.val()));
      Serial.print(Item::unit());
      ESP_LOGD("DmsrCustom", "%s", Item::unit());
      Serial.println();
      delay(50);
    }
  }
};

class DsmrP1CustomSensor : public PollingComponent, public UARTDevice {
 public:
  DsmrP1CustomSensor(UARTComponent *parent) : UARTDevice(parent) {}

  //  Sensor *ts_identification = new Sensor(); Not available yet as I do not know how to send a combination of
  //  textsensors and sensors Sensor *ts_p1_version = new Sensor(); Sensor *ts_timestamp = new Sensor(); Sensor
  //  *ts_equipment_id = new Sensor();
  Sensor *s_energy_delivered_tariff1 = new Sensor();
  Sensor *s_energy_delivered_tariff2 = new Sensor();
  Sensor *s_energy_returned_tariff1 = new Sensor();
  Sensor *s_energy_returned_tariff2 = new Sensor();
  //  Sensor *ts_electricity_tariff = new Sensor():
  Sensor *s_power_delivered = new Sensor();
  Sensor *s_power_returned = new Sensor();
  //  Sensor *instant_power_current_sensor = new Sensor();
  //  Sensor *instant_power_usage_sensor = new Sensor();
  Sensor *s_voltage_l1 = new Sensor();
  Sensor *s_current_l1 = new Sensor();
  Sensor *s_gas_delivered = new Sensor();
  Sensor *actual_tarif_sensor = new Sensor();
  Sensor *s_electricity_failures = new Sensor();
  Sensor *s_electricity_long_failures = new Sensor();
  Sensor *s_electricity_sags_l1 = new Sensor();
  Sensor *s_electricity_swells_l1 = new Sensor();

  void PublishSensors(MyData data) {
    // if(data.identification_present) ts_identification->publish_state(data.identification);
    // if(data.p1_version_present) ts_p1_version->publish_state(data.p1_version);
    // if(data.timestamp_present) ts_timestamp->publish_state(data.timestamp);
    // if(data.equipment_id_present) ts_equipment_id->publish_state(data.equipment_id);
    if (data.energy_delivered_tariff1_present)
      s_energy_delivered_tariff1->publish_state(data.energy_delivered_tariff1);
    if (data.energy_delivered_tariff2_present)
      s_energy_delivered_tariff2->publish_state(data.energy_delivered_tariff2);
    if (data.energy_returned_tariff1_present)
      s_energy_returned_tariff1->publish_state(data.energy_returned_tariff1);
    if (data.energy_returned_tariff2_present)
      s_energy_returned_tariff2->publish_state(data.energy_returned_tariff2);
    // if(data.electricity_tariff_present) ts_electricity_tariff->publish_state(data.electricity_tariff);
    if (data.power_delivered_present)
      s_power_delivered->publish_state(data.power_delivered);
    if (data.power_returned_present)
      s_power_returned->publish_state(data.power_returned);
    delay(100);  // Delay is added so home assistant is not overflooded with sensor data and disconnects.
    if (data.voltage_l1_present)
      s_voltage_l1->publish_state(data.voltage_l1);
    if (data.current_l1_present)
      s_current_l1->publish_state(data.current_l1);
    if (data.gas_delivered_present)
      s_gas_delivered->publish_state(data.gas_delivered);
    delay(100);  // Delay is added so home assistant is not overflooded with sensor data and disconnects.
    if (data.electricity_failures_present)
      s_electricity_failures->publish_state(data.electricity_failures);
    if (data.electricity_long_failures_present)
      s_electricity_long_failures->publish_state(data.electricity_long_failures);
    if (data.electricity_sags_l1_present)
      s_electricity_sags_l1->publish_state(data.electricity_sags_l1);
    if (data.electricity_swells_l1_present)
      s_electricity_swells_l1->publish_state(data.electricity_swells_l1);
  };

  void setup() override {
    MyData data;
    pinMode(D5, OUTPUT);
    digitalWrite(D5, LOW);
    reader.enable(true);
    last = millis();
    readmessage = false;
  }

  void update() override {
    // Allow the reader to check the serial buffer regularly
    reader.loop();
    digitalWrite(D5, HIGH);
    // Every minute, fire off a one-off reading
    unsigned long now = millis();
    if (now - last > 15000) {
      reader.enable(true);
      last = now;
      readmessage = false;
    }
    if (available()) {
      MyData data;
      String err;
      // ESP_LOGD("DmsrCustom","READING....");
      if (reader.parse(&data, &err)) {
        if (!readmessage) {
          ESP_LOGD("DmsrCustom", "READING READY");
          PublishSensors(data);
          reader.enable(false);
          readmessage = true;
          // Parse succesful, print result
          // data.applyEach(Printer());
        };
      } else {
        // Parser error, print error
        Serial.println(err);
      }
    }
  }
};