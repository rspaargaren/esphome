#pragma once
#include "esphome/core/defines.h"
#include <map>
#include "nextion_base.h"
#ifdef USE_API
#include "esphome/components/api/api_server.h"
#endif
namespace esphome {
namespace nextion {
class NextionComponent;

class NextionComponent {
 public:
  // Component publishes its state
  virtual void update_component() = 0;
  // Component setup routine, triggered from the Nextion display
  virtual void nextion_setup() = 0;
  // Set the components variable name
  void set_variable_name(std::string variable_name, std::string variable_name_to_send = "") {
    variable_name_ = variable_name;
    if (variable_name_to_send.empty()) {
      variable_name_to_send_ = variable_name_;
    } else {
      variable_name_to_send_ = variable_name_to_send;
    }
  }

  void set_print_debug(bool print_debug) { this->print_debug_ = print_debug; }
  void set_hass_name(std::string hass_name) {
    this->hass_name_ = hass_name;
    get_entity_type(hass_name);
  }

  void get_entity_type(std::string hass_name) {
    std::string::size_type pos = hass_name.find('.');
    if (pos != std::string::npos) {
      this->hass_entity_type_ = hass_name.substr(0, hass_name.find('.'));
    } else {
      this->hass_entity_type_ = "switch";  // Maybe change this to error message and set hass_name_ to "none"
    }
  }

#ifdef USE_API
  /** Call a Home Assistant service from ESPHome.
   *
   * Usage:
   *
   * ```cpp
   * call_homeassistant_service("light.turn_on", {
   *   {"entity_id", "light.my_light"},
   *   {"brightness", "127"},
   * });
   * ```
   *
   * @param service_name The service to call.
   * @param data The data for the service call, mapping from string to string.
   */
  void call_homeassistant_service(const std::string &service_name, const std::map<std::string, std::string> &data) {
    api::HomeassistantServiceResponse resp;
    resp.service = service_name;
    for (auto &it : data) {
      api::HomeassistantServiceMap kv;
      kv.key = it.first;
      kv.value = it.second;
      resp.data.push_back(kv);
    }
    api::global_api_server->send_homeassistant_service_call(resp);
  }
  /** Subscribe to the state of an entity from Home Assistant.
   *
   * Usage:
   *
   * ```cpp
   * void setup() override {
   *   subscribe_homeassistant_state(&CustomNativeAPI::on_state_changed, "sensor.weather_forecast");
   * }
   *
   * void on_state_changed(std::string state) {
   *   // State of sensor.weather_forecast is `state`
   * }
   * ```
   *
   * @tparam T The class type creating the service, automatically deduced from the function pointer.
   * @param callback The member function to call when the entity state changes.
   * @param entity_id The entity_id to track.
   */
  template<typename T>
  void subscribe_homeassistant_state(void (T::*callback)(std::string), const std::string &entity_id) {
    auto f = std::bind(callback, (T *) this, std::placeholders::_1);
    api::global_api_server->subscribe_home_assistant_state(entity_id, f);
  }
#endif

 protected:
  NextionBase *nextion_;

  std::string variable_name_;
  std::string variable_name_to_send_;
  std::string hass_name_;
  std::string hass_entity_type_;
  bool print_debug_ = true;
};
}  // namespace nextion
}  // namespace esphome
