# behaviormate_utils
Arduino Code and Debugging Tools for behaviorMate

pre-built version of Arduino Firmware: [https://www.losonczylab.org/behaviormate/]

---

## For Developers

Arduino software for both the behavior controller (GPIO circuit) and position
controller is supplied as source code which must be built into runnable arduino
.ino scripts using GNU make [https://www.gnu.org/software/make/]

makefiles are in the ```bin/behavior_controller/``` and ```bin/position_controller_v2/``` folders.
Built/runnable .ino files are found in the ```arduino_builds/``` folder.

The behaviorMate UI logs all communications with controllers specified in the settings.json file loaded,
however, function tests of the arduino can be performed by sending/recieveing via scripting e.g. with
the python ```socket``` library (see ```python/UdpComm.py```) or from the command line using GNU NetCat
[https://netcat.sourceforge.net/]

---
### Behavior Controller (GPIO)

*Message objects should be nested JSON objects under their route with the structure:*
```
  {<route>: {"action": <action>, <option>: <value>}}
```
*For example to configure a context that plays a tone (pin 51) and turns on an LED (pin 47)
at the same time send the following messages:*
```json
{"valves": {"pin": 51, "action": "create", "frequency": 5000}}
{"valves": {"pin": 47, "action": "create"}}
{"contexts": {"id": "tone_led", "action": "create", "valves": [51, 47], "durations": [500, 500]}}
```
*to start this example context send:*
```json
{"contexts": {"id": "tone_led", "action": "start"}}
```

**Message Routes:**
- "communicator" *(Communicator)*
- "valves" *(Valve Manager)*
- "contexts" *(Context Manager)*
- "sensors" *(Sensor Manager)*


**Communicator**
- Actions:
  - "test"  
     $${\color{red} \text{\it trigger the arduino to respond with a "test passed" message}}$$
  - "debug_mode"  
     $${\color{red} \text{\it toggle debug mode on the arduino. when enabled, debug mode results in more aggressive logging of the arduinio's state}}$$
  - "reset"  
     $${\color{red} \text{\it trigger the arduino to reset and clear memory (when running the behaviorMate UI a reset will result in all context, valve, and sensor}}$$  
     $${\color{red} \text{\it information to be resent to the arduino)}}$$
  - "info"  
     $${\color{red} \text{\it causes the arduino to respond with the current version number of the firmware it is running}}$$


**Valve Manager**
- Address:
  - "pin" : int, required
- Actions:
  - "create"
    - "frequency" : int, optional, also autmatically sets defualt "type" to "tone"  
       $${\color{red} \text{\it uses arduino signal generator to oscillate output at frequency provided, will play a tone if speaker is plugged into the supplied pin}}$$
    - "type" : str ("tone",), optional  
       $${\color{red} \text{\it uses arduino signal generator to oscillate output at frequency provided, will play a tone if speaker is plugged into the supplied pin}}$$ 
    - "inverted" : bool, optional  
       $${\color{red} \text{\it invert the output such that the signal is normally HIGH and switches to LOW when the valve is opened}}$$
  - "open"  
     $${\color{red} \text{\it set pin associated with this valve to HIGH}}$$
  - "close"  
     $${\color{red} \text{\it set pin associated with this valve to LOW}}$$


**Context Manager**
- Address:
  - "id" : str, required
- Actions:
  - "create"
    - "type" : str ("operant", "pulsed"), optional, default is none  
       $${\color{red} \text{\it sets a custom context type on the arduino, if not included a base context is created}}$$
    - for type "operant":  
      $${\color{red} \text{\it operant contexts are triggered by sensor input}}$$
      - "operant_rate" : int, optional, default=1  
         $${\color{red} \text{\it how many times does the sensor have to flip from LOW to HIGH in order to start the context}}$$
      - "initial_open" : int (0, 1), optional, default=0  
         $${\color{red} \text{\it if set to 1, the valves will open once without sensor input}}$$
      - "sensor" : int, optional, default=0  
         $${\color{red} \text{\it Specify which sensor pin to trigger this context}}$$
      - "enable_first" : bool, optional, default=false  
         $${\color{red} \text{\it If operant rate > 1 than only require 1 input for the first triggering of the context (and use operant rate for all}}$$  
         $${\color{red} \text{\it subsequent triggers)}}$$
      - "max_count" : int, optional  
         $${\color{red} \text{\it Sets a maximum number of times the context can be triggered for each time this context gets the "start" action}}$$
   - if type is "pulsed":  
     $${\color{red} \text{\it continue to re-start the context until the "stop" action is sent}}$$
     - "interval" : int, required  
        $${\color{red} \text{\it the interval (in ms) between triggering the context}}$$
  - "valves" : int[], required.  
      $${\color{red} \text{\it list of pins numbers/valves to associate with this context}}$$
  - "durations" : int[], optional, defaults to stay on until context is stopped  
     $${\color{red} \text{\it list of durations to activate valves for (in order as specified in the "valves" list)}}$$
  - "start"  
     $${\color{red} \text{\it start the context}}$$
  - "stop"  
     $${\color{red} \text{\it stop the context}}$$
  - "activate"  
     $${\color{red} \text{\it force context to be activated}}$$


**Sensor Manager**
- Address
  - "pin" : int, required
- Actions:
  - "create"
    - "type": str, ("lickport", "rfid_reader", "piezoport", "irport", "cap1188"), optional  
       $${\color{red} \text{\it specify a custom "type" of sensor, defaults to the base sensor which reports TTL pulses}}$$
    - if type "lickport":  
      $${\color{red} \text{\it For SparkFun MPR121 capacitance touch sensor circuit (discontinued)}}$$
      - "lickstart_threshold" : int, optional, default=19
      - "lickstop_threshold" : int, optional, default=8
      - "sensor" : int, optional, default=0  
         $${\color{red} \text{\it Only response to licks on this input channel on the MPR121 board}}$$
    - if type "rfid_reader":
      $${\color{red} \text{\it depreciated}}$$
    - if type "piezoport":
      $${\color{red} \text{\it }}$$
      - "lick_threshold" : float, required
      - "lick_duration" : int, required
      - "min_interval" : int, optional default=200
    - if type "irport":
      $${\color{red} \text{\it }}$$
      - "lick_threshold" : float, optional, default=0.75
    - if type "cap1188":  
      $${\color{red} \text{\it For Adafruit CAP1188 capacitance touch sensor circuit}}$$
      - "reset_pin": int, required  
      - "sensitivity": int, optional (default=7)  
      - "sensor": int, optional  
         $${\color{red} \text{\it Only detect on this input to the CAP1188, 0-indexed (offset by 1 from board label), detect on all if omitted}}$$
      - "mapping" : JsonObject, optional 
        - {"pin" int, required : "sensor" int, required}  
         $${\color{red} \text{\it map input channels on CAP1188 board to arbtrarty virtual "pin" numbers in Sensor Manager (for dual lickport setups)}}$$
    - "report_pin": int, optional, defaults to any  
       $${\color{red} \text{\it Set report pin to HIGH when sensor input is detected}}$$
  - "clear"  
    $${\color{red} \text{\it remove the sensor specified at the "pin" supplied in this message}}$$

---
### Position Controller

*Position Controller sends 2 types of messages, but does not receive messages.*
1. Position update message:  
   ```
   {"position": {"dy": <change in position>, "dt": <sampling time>}, "millis": <arduino time>}
   ```
   - position update messages are only sent if position has changed  
3. Lap reset message (only for treadmill configuration with lap reset circuit attached):  
   ```json
   {"lap_reset": {"type": "IR 2 pin"}
   ```
