Scrolling text:

''yaml
display:
  - platform: max7219digit
    cs_pin: D8
    num_chips: 4
    lambda: |-
      it.moveString("123456",1);
''yaml
