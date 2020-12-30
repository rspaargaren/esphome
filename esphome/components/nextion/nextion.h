#pragma once

#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#include "esphome/components/uart/uart.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/color.h"
#include <HTTPClient.h>

#ifdef USE_TIME
#include "esphome/components/time/real_time_clock.h"
#endif

namespace esphome {
namespace nextion {

class NextionTouchComponent;
class NextionSwitch;
class NextionSensor;
class Nextion;

using nextion_writer_t = std::function<void(Nextion &)>;

class Nextion : public PollingComponent, public uart::UARTDevice {
 public:
  /**
   * Set the text of a component to a static string.
   * @param component The component name.
   * @param text The static text to set.
   *
   * Example:
   * ```cpp
   * it.set_component_text("textview", "Hello World!");
   * ```
   *
   * This will set the `txt` property of the component `textview` to `Hello World`.
   */
  void set_component_text(const char *component, const char *text);
  /**
   * Set the text of a component to a formatted string
   * @param component The component name.
   * @param format The printf-style format string.
   * @param ... The arguments to the format.
   *
   * Example:
   * ```cpp
   * it.set_component_text_printf("textview", "The uptime is: %.0f", id(uptime_sensor).state);
   * ```
   *
   * This will change the text on the component named `textview` to `The uptime is:` Then the value of `uptime_sensor`.
   * with zero decimals of accuracy (whole number).
   * For example when `uptime_sensor` = 506, then, `The uptime is: 506` will be displayed.
   */
  void set_component_text_printf(const char *component, const char *format, ...) __attribute__((format(printf, 3, 4)));
  /**
   * Set the integer value of a component
   * @param component The component name.
   * @param value The value to set.
   *
   * Example:
   * ```cpp
   * it.set_component_value("gauge", 50);
   * ```
   *
   * This will change the property `value` of the component `gauge` to 50.
   */
  void set_component_value(const char *component, int value);
  /**
   * Set the picture of an image component.
   * @param component The component name.
   * @param value The picture name.
   *
   * Example:
   * ```cpp
   * it.set_component_picture("pic", "4");
   * ```
   *
   * This will change the image of the component `pic` to the image with ID `4`.
   */
  void set_component_picture(const char *component, const char *picture) {
    this->send_command_printf("%s.val=%s", component, picture);
  }
  /**
   * Set the background color of a component.
   * @param component The component name.
   * @param color The color (as a string).
   *
   * Example:
   * ```cpp
   * it.set_component_background_color("button", "RED");
   * ```
   *
   * This will change the background color of the component `button` to blue.
   * Use this [color picker](https://nodtem66.github.io/nextion-hmi-color-convert/index.html) to convert color codes to
   * Nextion HMI colors.
   */
  void set_component_background_color(const char *component, const char *color);
  /**
   * Set the background color of a component.
   * @param component The component name.
   * @param color The color (as Color).
   *
   * Example:
   * ```cpp
   * it.set_component_background_color("button", color);
   * ```
   *
   * This will change the background color of the component `button` to blue.
   * Use this [color picker](https://nodtem66.github.io/nextion-hmi-color-convert/index.html) to convert color codes to
   * Nextion HMI colors.
   */
  void set_component_background_color(const char *component, Color color);
  /**
   * Set the pressed background color of a component.
   * @param component The component name.
   * @param color The color (as a string).
   *
   * Example:
   * ```cpp
   * it.set_component_pressed_background_color("button", "RED");
   * ```
   *
   * This will change the pressed background color of the component `button` to blue. This is the background color that
   * is shown when the component is pressed. Use this [color
   * picker](https://nodtem66.github.io/nextion-hmi-color-convert/index.html) to convert color codes to Nextion HMI
   * colors.
   */
  void set_component_pressed_background_color(const char *component, const char *color);
  /**
   * Set the pressed background color of a component.
   * @param component The component name.
   * @param color The color (as Color).
   *
   * Example:
   * ```cpp
   * it.set_component_pressed_background_color("button", color);
   * ```
   *
   * This will change the pressed background color of the component `button` to blue. This is the background color that
   * is shown when the component is pressed. Use this [color
   * picker](https://nodtem66.github.io/nextion-hmi-color-convert/index.html) to convert color codes to Nextion HMI
   * colors.
   */
  void set_component_pressed_background_color(const char *component, Color color);
  /**
   * Set the font color of a component.
   * @param component The component name.
   * @param color The color (as a string).
   *
   * Example:
   * ```cpp
   * it.set_component_font_color("textview", "RED");
   * ```
   *
   * This will change the font color of the component `textview` to a blue color.
   * Use this [color picker](https://nodtem66.github.io/nextion-hmi-color-convert/index.html) to convert color codes to
   * Nextion HMI colors.
   */
  void set_component_font_color(const char *component, const char *color);
  /**
   * Set the font color of a component.
   * @param component The component name.
   * @param color The color (as Color).
   *
   * Example:
   * ```cpp
   * it.set_component_font_color("textview", color);
   * ```
   *
   * This will change the font color of the component `textview` to a blue color.
   * Use this [color picker](https://nodtem66.github.io/nextion-hmi-color-convert/index.html) to convert color codes to
   * Nextion HMI colors.
   */
  void set_component_font_color(const char *component, Color color);
  /**
   * Set the pressed font color of a component.
   * @param component The component name.
   * @param color The color (as a string).
   *
   * Example:
   * ```cpp
   * it.set_component_pressed_font_color("button", "RED");
   * ```
   *
   * This will change the pressed font color of the component `button` to a blue color.
   * Use this [color picker](https://nodtem66.github.io/nextion-hmi-color-convert/index.html) to convert color codes to
   * Nextion HMI colors.
   */
  void set_component_pressed_font_color(const char *component, const char *color);
  /**
   * Set the pressed font color of a component.
   * @param component The component name.
   * @param color The color (as Color).
   *
   * Example:
   * ```cpp
   * it.set_component_pressed_font_color("button", color);
   * ```
   *
   * This will change the pressed font color of the component `button` to a blue color.
   * Use this [color picker](https://nodtem66.github.io/nextion-hmi-color-convert/index.html) to convert color codes to
   * Nextion HMI colors.
   */
  void set_component_pressed_font_color(const char *component, Color color);
  /**
   * Set the coordinates of a component on screen.
   * @param component The component name.
   * @param x The x coordinate.
   * @param y The y coordinate.
   *
   * Example:
   * ```cpp
   * it.set_component_coordinates("pic", 55, 100);
   * ```
   *
   * This will move the position of the component `pic` to the x coordinate `55` and y coordinate `100`.
   */
  void set_component_coordinates(const char *component, int x, int y);
  /**
   * Set the font id for a component.
   * @param component The component name.
   * @param font_id The ID of the font (number).
   *
   * Example:
   * ```cpp
   * it.set_component_font("textview", "3");
   * ```
   *
   * Changes the font of the component named `textveiw`. Font IDs are set in the Nextion Editor.
   */
  void set_component_font(const char *component, uint8_t font_id);
#ifdef USE_TIME
  /**
   * Send the current time to the nextion display.
   * @param time The time instance to send (get this with id(my_time).now() ).
   */
  void set_nextion_rtc_time(time::ESPTime time);
#endif

  /**
   * Show the page with a given name.
   * @param page The name of the page.
   *
   * Example:
   * ```cpp
   * it.goto_page("main");
   * ```
   *
   * Switches to the page named `main`. Pages are named in the Nextion Editor.
   */
  void goto_page(const char *page);
  /**
   * Hide a component.
   * @param component The component name.
   *
   * Example:
   * ```cpp
   * hide_component("button");
   * ```
   *
   * Hides the component named `button`.
   */
  void hide_component(const char *component);
  /**
   * Show a component.
   * @param component The component name.
   *
   * Example:
   * ```cpp
   * show_component("button");
   * ```
   *
   * Shows the component named `button`.
   */
  void show_component(const char *component);
  /**
   * Enable touch for a component.
   * @param component The component name.
   *
   * Example:
   * ```cpp
   * enable_component_touch("button");
   * ```
   *
   * Enables touch for component named `button`.
   */
  void enable_component_touch(const char *component);
  /**
   * Disable touch for a component.
   * @param component The component name.
   *
   * Example:
   * ```cpp
   * disable_component_touch("button");
   * ```
   *
   * Disables touch for component named `button`.
   */
  void disable_component_touch(const char *component);
  /**
   * Add waveform data to a waveform component
   * @param component_id The integer component id.
   * @param channel_number The channel number to write to.
   * @param value The value to write.
   */
  void add_waveform_data(int component_id, uint8_t channel_number, uint8_t value);
  /**
   * Display a picture at coordinates.
   * @param picture_id The picture id.
   * @param x1 The x coordinate.
   * @param y1 The y coordniate.
   *
   * Example:
   * ```cpp
   * display_picture(2, 15, 25);
   * ```
   *
   * Displays the picture who has the id `2` at the x coordinates `15` and y coordinates `25`.
   */
  void display_picture(int picture_id, int x_start, int y_start);
  /**
   * Fill a rectangle with a color.
   * @param x1 The starting x coordinate.
   * @param y1 The starting y coordinate.
   * @param width The width to draw.
   * @param height The height to draw.
   * @param color The color to draw with (as a string).
   *
   * Example:
   * ```cpp
   * fill_area(50, 50, 100, 100, "RED");
   * ```
   *
   * Fills an area that starts at x coordiante `50` and y coordinate `50` with a height of `100` and width of `100` with
   * the color of blue. Use this [color picker](https://nodtem66.github.io/nextion-hmi-color-convert/index.html) to
   * convert color codes to Nextion HMI colors
   */
  void fill_area(int x1, int y1, int width, int height, const char *color);
  /**
   * Fill a rectangle with a color.
   * @param x1 The starting x coordinate.
   * @param y1 The starting y coordinate.
   * @param width The width to draw.
   * @param height The height to draw.
   * @param color The color to draw with (as Color).
   *
   * Example:
   * ```cpp
   * fill_area(50, 50, 100, 100, color);
   * ```
   *
   * Fills an area that starts at x coordiante `50` and y coordinate `50` with a height of `100` and width of `100` with
   * the color of blue. Use this [color picker](https://nodtem66.github.io/nextion-hmi-color-convert/index.html) to
   * convert color codes to Nextion HMI colors
   */
  void fill_area(int x1, int y1, int width, int height, Color color);
  /**
   * Draw a line on the screen.
   * @param x1 The starting x coordinate.
   * @param y1 The starting y coordinate.
   * @param x2 The ending x coordinate.
   * @param y2 The ending y coordinate.
   * @param color The color to draw with (as a string).
   *
   * Example:
   * ```cpp
   * it.line(50, 50, 75, 75, "17013");
   * ```
   *
   * Makes a line that starts at x coordinate `50` and y coordinate `50` and ends at x coordinate `75` and y coordinate
   * `75` with the color of blue. Use this [color
   * picker](https://nodtem66.github.io/nextion-hmi-color-convert/index.html) to convert color codes to Nextion HMI
   * colors.
   */
  void line(int x1, int y1, int x2, int y2, const char *color);
  /**
   * Draw a line on the screen.
   * @param x1 The starting x coordinate.
   * @param y1 The starting y coordinate.
   * @param x2 The ending x coordinate.
   * @param y2 The ending y coordinate.
   * @param color The color to draw with (as Color).
   *
   * Example:
   * ```cpp
   * it.line(50, 50, 75, 75, "17013");
   * ```
   *
   * Makes a line that starts at x coordinate `50` and y coordinate `50` and ends at x coordinate `75` and y coordinate
   * `75` with the color of blue. Use this [color
   * picker](https://nodtem66.github.io/nextion-hmi-color-convert/index.html) to convert color codes to Nextion HMI
   * colors.
   */
  void line(int x1, int y1, int x2, int y2, Color color);
  /**
   * Draw a rectangle outline.
   * @param x1 The starting x coordinate.
   * @param y1 The starting y coordinate.
   * @param width The width of the rectangle.
   * @param height The height of the rectangle.
   * @param color The color to draw with (as a string).
   *
   * Example:
   * ```cpp
   * it.rectangle(25, 35, 40, 50, "17013");
   * ```
   *
   * Makes a outline of a rectangle that starts at x coordinate `25` and y coordinate `35` and has a width of `40` and a
   * length of `50` with color of blue. Use this [color
   * picker](https://nodtem66.github.io/nextion-hmi-color-convert/index.html) to convert color codes to Nextion HMI
   * colors.
   */
  void rectangle(int x1, int y1, int width, int height, const char *color);
  /**
   * Draw a rectangle outline.
   * @param x1 The starting x coordinate.
   * @param y1 The starting y coordinate.
   * @param width The width of the rectangle.
   * @param height The height of the rectangle.
   * @param color The color to draw with (as Color).
   *
   * Example:
   * ```cpp
   * it.rectangle(25, 35, 40, 50, "17013");
   * ```
   *
   * Makes a outline of a rectangle that starts at x coordinate `25` and y coordinate `35` and has a width of `40` and a
   * length of `50` with color of blue. Use this [color
   * picker](https://nodtem66.github.io/nextion-hmi-color-convert/index.html) to convert color codes to Nextion HMI
   * colors.
   */
  void rectangle(int x1, int y1, int width, int height, Color color);
  /**
   * Draw a circle outline
   * @param center_x The center x coordinate.
   * @param center_y The center y coordinate.
   * @param radius The circle radius.
   * @param color The color to draw with (as a string).
   */
  void circle(int center_x, int center_y, int radius, const char *color);
  /**
   * Draw a circle outline
   * @param center_x The center x coordinate.
   * @param center_y The center y coordinate.
   * @param radius The circle radius.
   * @param color The color to draw with (as Color).
   */
  void circle(int center_x, int center_y, int radius, Color color);
  /**
   * Draw a filled circled.
   * @param center_x The center x coordinate.
   * @param center_y The center y coordinate.
   * @param radius The circle radius.
   * @param color The color to draw with (as a string).
   *
   * Example:
   * ```cpp
   * it.filled_cricle(25, 25, 10, "17013");
   * ```
   *
   * Makes a filled circle at the x cordinates `25` and y coordinate `25` with a radius of `10` with a color of blue.
   * Use this [color picker](https://nodtem66.github.io/nextion-hmi-color-convert/index.html) to convert color codes to
   * Nextion HMI colors.
   */
  void filled_circle(int center_x, int center_y, int radius, const char *color);
  /**
   * Draw a filled circled.
   * @param center_x The center x coordinate.
   * @param center_y The center y coordinate.
   * @param radius The circle radius.
   * @param color The color to draw with (as Color).
   *
   * Example:
   * ```cpp
   * it.filled_cricle(25, 25, 10, color);
   * ```
   *
   * Makes a filled circle at the x cordinates `25` and y coordinate `25` with a radius of `10` with a color of blue.
   * Use this [color picker](https://nodtem66.github.io/nextion-hmi-color-convert/index.html) to convert color codes to
   * Nextion HMI colors.
   */
  void filled_circle(int center_x, int center_y, int radius, Color color);

  /** Set the brightness of the backlight.
   *
   * @param brightness The brightness, from 0 to 100.
   *
   * Example:
   * ```cpp
   * it.set_backlight_brightness(30);
   * ```
   *
   * Changes the brightness of the display to 30%.
   */
  void set_backlight_brightness(uint8_t brightness);
  /**
   * Set the touch sleep timeout of the display.
   * @param timeout Timeout in seconds.
   *
   * Example:
   * ```cpp
   * it.set_touch_sleep_timeout(30);
   * ```
   *
   * After 30 seconds the display will go to sleep. Note: the display will only wakeup by a restart or by setting up
   * `thup`.
   */
  void set_touch_sleep_timeout(uint16_t timeout);
  /**
   * Sets which page Nextion loads when exiting sleep mode. Note this can be set even when Nextion is in sleep mode.
   * @param page_id The page id, from 0 to the lage page in Nextion. Set 255 (not set to any existing page) to
   * wakes up to current page.
   *
   * Example:
   * ```cpp
   * it.set_wake_up_page(2);
   * ```
   *
   * The display will wake up to page 2.
   */
  void set_wake_up_page(uint8_t page_id = 255);
  /**
   * Sets if Nextion should auto-wake from sleep when touch press occurs.
   * @param auto_wake True or false. When auto_wake is true and Nextion is in sleep mode,
   * the first touch will only trigger the auto wake mode and not trigger a Touch Event.
   *
   * Example:
   * ```cpp
   * it.set_auto_wake_on_touch(true);
   * ```
   *
   * The display will wake up by touch.
   */
  void set_auto_wake_on_touch(bool auto_wake);
  /**
   * Sets Nextion mode between sleep and awake
   * @param True or false. Sleep=true to enter sleep mode or sleep=false to exit sleep mode.
   */
  void sleep(bool sleep);

  // ========== INTERNAL METHODS ==========
  // (In most use cases you won't need these)
  void register_touch_component(NextionTouchComponent *obj) { this->touch_.push_back(obj); }
  void register_switch_component(NextionSwitch *obj) { this->switchtype_.push_back(obj); }
  void register_sensor_component(NextionSensor *obj) { this->sensortype_.push_back(obj); }
  void setup() override;
  void set_brightness(float brightness) { this->brightness_ = brightness; }
  float get_setup_priority() const override;
  void update() override;
  void loop() override;
  void set_writer(const nextion_writer_t &writer);

  /**
   * Manually send a raw command to the display and don't wait for an acknowledgement packet.
   * @param command The command to write, for example "vis b0,0".
   */
  void send_command_no_ack(const char *command);
  /**
   * Manually send a raw formatted command to the display.
   * @param format The printf-style command format, like "vis %s,0"
   * @param ... The format arguments
   * @return Whether the send was successful.
   */
  bool send_command_printf(const char *format, ...) __attribute__((format(printf, 2, 3)));

  void set_wait_for_ack(bool wait_for_ack);

  /**
   * Set the tft file URL. https seems problamtic with arduino..
   */
  void set_tft_url(const std::string &tft_url) { this->tft_url_ = tft_url; }

  /**
   * Upload the tft file and softreset the Nextion
   */
  void upload_tft();
  void dump_config() override;

  /**
   * Softreset the Nextion
   */
  void soft_reset();

  /**
   * will request the text of component id
   * from the nextion
   * @param const char *component_id Component id to get the text from
   * @param char *string_buffer Buffer to put the text in
   * @return true if success, false for failure.
   */
  bool gets(const char *component_id, char *string_buffer);
  uint32_t getn(const char *component_id);

 protected:
  bool ack_();
  bool read_until_ack_();
  bool is_updating_ = false;
  /**
   * will request chunk_size chunks from the web server
   * and send each to the nextion
   * @param int contentLength Total size of the file
   * @return true if success, false for failure.
   */
  bool upload_by_chunks_(int content_length, int chunk_size = 4096);
  /**
   * start update tft file to nextion.
   *
   * @param Stream &myFile
   * @param int contentLength Total size of the file
   * @return true if success, false for failure.
   */
  bool upload_from_stream_(Stream &my_file, int content_length);

  /**
   * start update tft file to nextion.
   *
   * @param const uint8_t *file_buf
   * @param size_t buf_size
   * @return true if success, false for failure.
   */
  bool upload_from_buffer_(const uint8_t *file_buf, size_t buf_size);
  /*
   * Receive string data.
   *
   * @param response - save string data.
   * @param timeout - set timeout time.
   * @param recv_flag - if recv_flag is true,will braak when receive 0x05.
   *
   * @return the length of string buffer.
   *
   */
  uint16_t recv_ret_string_(String &response, uint32_t timeout = 500, bool recv_flag = false);

  std::vector<NextionTouchComponent *> touch_;
  std::vector<NextionSwitch *> switchtype_;
  std::vector<NextionSensor *> sensortype_;
  optional<nextion_writer_t> writer_;
  bool wait_for_ack_{true};
  float brightness_{1.0};
  std::string tft_url_;
  int total_ = 0;
  int sent_packets_ = 0;
  bool has_updated_ = false;
  bool debug_print_ = false;
  char device_model_[64];
  char firmware_version_[64];
  char serial_number_[64];
  char flash_size_[64];
};

class NextionTouchComponent : public binary_sensor::BinarySensorInitiallyOff {
 public:
  void set_page_id(uint8_t page_id) { page_id_ = page_id; }
  void set_component_id(uint8_t component_id) { component_id_ = component_id; }
  void process(uint8_t page_id, uint8_t component_id, bool on);

 protected:
  uint8_t page_id_;
  uint8_t component_id_;
};
class NextionSwitch : public switch_::Switch, public Component, public uart::UARTDevice {
 public:
  void set_page_id(uint8_t page_id) { page_id_ = page_id; }
  void set_component_id(uint8_t component_id) { component_id_ = component_id; }
  void set_device_id(std::string device_id) { device_id_ = device_id; }
  void process(uint8_t page_id, uint8_t component_id, bool on);
  void send_command_no_ack(const char *command);
  bool send_command_printf(const char *format, ...);

 protected:
  uint8_t page_id_;
  uint8_t component_id_;
  std::string device_id_;
  void write_state(bool state) override;
};

class NextionSensor : public sensor::Sensor, public uart::UARTDevice {
 public:
  void set_page_id(uint8_t page_id) { page_id_ = page_id; }
  void set_component_id(uint8_t component_id) { component_id_ = component_id; }
  void process(uint8_t page_id, uint8_t component_id, float state);

 protected:
  uint8_t page_id_;
  uint8_t component_id_;
};

}  // namespace nextion
}  // namespace esphome
