# Home Assistant Dashboard for Project Aura

This folder contains a ready-to-use YAML configuration to visualize your Aura data.
It uses only standard Home Assistant cards. No HACS or external dependencies required.
The example view includes dedicated `CO2` and `AQI` gauge cards at the top, plus a
standard Home Assistant ventilation section with explicit `Auto` / `Manual` / `Stop`
mode switches, a manual speed slider in percent, a timer selector, and a timer
remaining indicator for the optional DAC-based exhaust output.

![Dashboard Preview](../assets/preview.png)

## How to Add
You can add this as a new view (tab) to your existing dashboard.

1. Open your Home Assistant Dashboard.
2. Click the Pencil icon (Edit Dashboard) in the top right.
3. Click the 3 dots menu (top right) -> Raw configuration editor.
4. Scroll to the place where you want to insert the code (or replace existing code if starting fresh).
5. Paste the contents of `dashboard.yaml`.
6. Click Save.

## MQTT Setup

Aura publishes data to Home Assistant through MQTT. For a local Home Assistant
installation, the usual setup is the Mosquitto Broker add-on.

1. In Home Assistant, install and start the `Mosquitto Broker` add-on.
2. Create a dedicated Home Assistant user for Aura, for example `aura_mqtt`.
   This is done in `Settings -> People -> Users`, not inside the Mosquitto
   add-on page.
3. On Aura, open `Settings -> MQTT` on the device screen. This unlocks the MQTT
   web setup page.
4. Open `http://<aura-ip>/mqtt` in a browser.
5. Set `Broker Address` to the internal IP address or hostname of your Home
   Assistant server. Do not include `http://`.
6. For local Mosquitto without TLS, use port `1883` and keep `Use TLS / SSL`
   disabled. Do not use the Home Assistant web UI port `8123`.
7. Enter the username and password of the dedicated Home Assistant user.
8. Keep `Enable Home Assistant Discovery` enabled.
9. Use a unique `MQTT base topic` for each Aura device.
10. Click `Save`.

After Aura connects and publishes discovery data, the entities should appear in
Home Assistant under the MQTT integration. The dashboard YAML in this folder
uses the default `project_aura` base topic.

## MQTT TLS Brokers

For cloud brokers or TLS-enabled brokers such as HiveMQ Cloud, enable
`Use TLS / SSL` on Aura's `/mqtt` page.

TLS setup requirements:
- Use the broker hostname, not an IP address. Certificate verification matches
  the broker certificate against that hostname.
- Use the broker TLS port, usually `8883`.
- Paste the broker CA certificate into `CA Certificate (PEM)`.
- Make sure Aura has valid time from NTP. TLS certificate validation can wait
  until system time is available.
- Client certificates and private keys are not used.

Local Home Assistant Mosquitto installations usually do not need TLS unless you
configured TLS on the broker yourself.

## MQTT Troubleshooting

If Home Assistant shows entities as unavailable:
- Confirm that Aura shows MQTT as connected.
- Confirm that the broker address is the Home Assistant host/IP, not a URL.
- Confirm that the port is `1883` for local non-TLS Mosquitto or `8883` for TLS.
- Confirm that Home Assistant MQTT discovery is enabled in Aura.
- Confirm that the discovery prefix is the Home Assistant default:
  `homeassistant`.
- If you have more than one Aura, make sure each one has a different
  `MQTT base topic`.

If Mosquitto logs show `Not authorized`, check the username and password first.
The credentials should belong to a Home Assistant user, unless your broker is
explicitly configured for anonymous access.

## Entity Configuration
Entity IDs are derived from your `MQTT base topic`.

If your base topic is the default `project_aura`, the example YAML should match directly:
- `sensor.project_aura_temperature`
- `switch.project_aura_fan_auto`
- `select.project_aura_fan_timer`

If you use a custom base topic, replace the `project_aura_` prefix in the YAML with the
slugged base topic. For example:
- base topic `project_aura_kitchen` -> `sensor.project_aura_kitchen_temperature`
- base topic `project_aura/bedroom` -> `sensor.project_aura_bedroom_temperature`

If you see "Entity not found" warnings:
1. Go to Settings -> Devices & Services -> Entities.
2. Search for your MQTT base topic or `Aura`.
3. Open the dashboard code and use Find & Replace to swap the prefix.

## Multiple Aura Devices
Two Aura devices must use different `MQTT base topic` values.

This is important for two reasons:
1. State and command topics are built from the base topic.
2. Home Assistant entity IDs are also derived from the base topic.

Recommended examples:
- `project_aura_kitchen`
- `project_aura_bedroom`
- `project_aura_office`

## Events Sensor

Aura now mirrors the same event stream shown in the built-in web dashboard `Events` tab to Home Assistant over MQTT.

Discovery creates one additional entity derived from your MQTT base topic, for example:
- `sensor.project_aura_room1_events`

Behavior:
- Sensor state is the human-readable event message.
- Attributes include `ts_ms`, `level`, `severity`, and `type`.
- The entity uses `force_update`, so repeated identical messages are still recorded as new state changes when they are not deduplicated by firmware.
- This entity is intended for Home Assistant `Activity` / `logbook` views.
- Long-term event history in Home Assistant is controlled by `recorder`, not by the firmware queue size.
- The final `entity_id` depends on your configured MQTT base topic, so names in `dashboard.yaml` may need to be adjusted to match the entities discovered in your own Home Assistant instance.
