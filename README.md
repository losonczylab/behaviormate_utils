# behaviormate_utils
Arduino Code and Debugging Tools for behaviorMate


---

## For Developers

Arduino software for both the behavior controller (GPIO circuit) and position
controller is supplied as source code which must be built into runnable arduino
.ino scripts using GNU make [https://www.gnu.org/software/make/]


---
### Behavior Controller (GPIO)

*Message objects should be nested JSON objects under their route with the structure:*
```
  {<route>: {"action": <action>, <option>: <value>}}
```
*For example to configure a context that plays a tone (pin 51) and turns on an LED (pin 47)
at the same time send the following messages:*
```
{"valves": {"pin": 51, "action": "create", "frequency": 5000}}
{"valves": {"pin": 47, "action": "create"}}
{"contexts": {"id": "tone_led", "action": "create", "valves": [51, 47], "durations": [500, 500]}}
```
*to start this example context send:*
```
{"contexts": {"id": "tone_led", "action": "start"}}
```

Message Routes:
- "communicator"
- "valves"
- "contexts"
- "sensors"

Communicator
- Actions:
  - "test"
  - "debug_mode"
  - "reset"
  - "info"

Valve Manager
- Address:
  - "pin" : int, required
- Actions:
  - "create"
    - "frequency" : int, optional, also autmatically sets defualt "type" to "tone"
    - "type" : str ("tone",), optional  
       *uses arduino signal generator to oscillate output at frequency provided, will play a tone if speaker is plugged into the supplied pin* 
    - "inverted" : bool, optional
      *invert the output such that the signal is normally HIGH and switches to LOW when the valve is opened*
  - "open"
  - "close"

Context Manager
 - Address:
  -- "id" : str, required
 - Actions:
  -- "create"
   --- "type" : str ("operant", "pulsed"), optional
       *Will default to none if not understood*
   --- if type is "operant":
    ---- "operant_rate" : int, optional, default=1
    ---- "initial_open" : int (0, 1), optional, default=0
    ---- "sensor" : int, optional, default=0
    ---- "enable_first" : bool, optional, default=false
    ---- "max_count" : int, optional
   --- if type is "pulsed":
    ---- "interval" : int, required
   --- "valves" : int[], required
   --- "durations" : int[], optional, defaults to stay on until context is stopped
  -- "start"
  -- "stop"
  -- "activate"

Sensor Manager
 - Address
  -- "pin" : int, required
 - Actions:
  -- "create"
   --- "type": str, ("lickport", "rfid_reader", "piezoport", "irport", "cap1188"), optional
    --- if type "lickport":
     *For MPR121 capacitance touch sensor circuit*
     ---- "lickstart_threshold" : int, optional, default=19
     ---- "lickstop_threshold" : int, optional, default=8
     ---- "sensor" : int, optional, default=0
    --- if type "rfid_reader":
     ---- *depreciated*
    --- if type "piezoport":
    ---- "lick_threshold" : float, required
    ---- "lick_duration" : int, required
    ---- "min_interval" : int, optional default=200
    --- if type "irport":
    ---- "lick_threshold" : float, optional, default=0.75
    --- if type "cap1188":
    *For CAP1188 capacitance touch sensor circuit*
    ---- "reset_pin": int, required
    ---- "sensitivity": int, optional (default=7)
    ---- "sensor": int, optional
    ---- "mapping" : JsonObject, optional
    ---- {"pin" int, required : "sensor" int, required}
    --- "report_pin": int, optional, defaults to any
  -- "clear"
