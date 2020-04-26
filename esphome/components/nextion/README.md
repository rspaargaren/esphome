
Library still in development but ready for first testing.

In the YAML config file the following entries can be added:

binary_sensor:  <-- No changes made
  - platform: nextion
    page_id: 0
    component_id: 3
    name: "Nextion Component Binary"

switch:
  - platform: nextion
    page_id: 0        <-- Page number of item in Nextion
    component_id: 3   <-- Item number in Nextion
    name: "Nextion Component Switch"
    button_id: "bt0"  <-- ID Name of item in Nextion

sensor:
  - platform: nextion
    page_id: 0        <-- Page number of item in Nextion
    component_id: 5   <-- Item number in Nextion
    name: "Nextion sensor"

