# behaviormate_utils
Arduino Code and Debugging Tools for behaviorMate


---

## For Developers

Arduino software for both the behavior controller (GPIO circuit) and position
controller is supplied as source code which must be built into runnable arduino
.ino scripts using GNU make [https://www.gnu.org/software/make/]


---
Behavior Controller (GPIO) Message Routes:
 - "communicator"
 - "valves"
 - "contexts"
 - "sensors"

Communicator
 - Actions:
  -- "test"
  -- "debug_mode"
  -- "reset"
  -- "info"

Valve Manager
 - Address:
  -- "pin" : int, required
 - Actions:
  -- "create"
   --- "frequency" : int, optional
    *Sets to "type": "tone"*
   --- "type" : str ("tone"), optional
   --- "inverted" : bool, optional
 -- "open"
 -- "close"

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
   --- "durations" : int[], required
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
