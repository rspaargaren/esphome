Further update:

```yaml
display:
  - platform: max7219digit
    cs_pin: D8
    num_chips: 4
    offset: 2
    intensity: 15
    lambda: |-
      it.strftime(0, 0, id(digit_font), "%H:%M%a", id(hass_time).now());
      it.scroll_left(1);
      it.invert_on_off();
```
it.invert_on_off(); -> Inverts the output of the display Background and Dots the next time update is run. To be put at the end of the lambda input.
it.invert_on_off(true); -> Output is inverted
it.invert_on_off(false);-> Output is normal

The invert can also be set in between the lambda call to set the dot color to write.
```yaml
it.invert_on_off();
it.line(X,Y,X,Y);
it.invert_on_off();
```
Wipes a line of text in the display. Can be used to blink!
