Scrolling text:

```yaml
display:
  - platform: max7219digit
    cs_pin: D8
    num_chips: 4
    lambda: |-
      it.moveString("123456",1);
```
moveString("123456",1) <-- 1 left to right

moveString("123456",0) <-- 0 right to left

The speed cannot be controlled now as this is depending on the loop pauze. I will make this adjustable in future

```yaml
it.moveStringf("Hi %s",1,"LOL!");
```
The 1 or 0 gives the direction
