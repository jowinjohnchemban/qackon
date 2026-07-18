# Qackon Beacon

![Test](https://github.com/jowinjohnchemban/qackon/actions/workflows/test.yml/badge.svg)
![Release](https://github.com/jowinjohnchemban/qackon/actions/workflows/release.yml/badge.svg)

Open-source safety beacon/tracker for automotive and personal use. GPS
location, BLE + WiFi setup, cellular SMS fallback, and a live web dashboard —
flash it once over USB, then configure and update it entirely from a phone
browser, no app required.

This repo now ships a working PlatformIO firmware project, not just a spec.
See **Hardware gaps** below before you build one for real use — a couple of
the original README's claims need actual sensor hardware this design doesn't
yet include.

## Quick start

1. Install [PlatformIO](https://platformio.org/) (VS Code extension or CLI).
2. Wire the hardware per **Wiring** below.
3. `pio run -t uploadfs` — flashes the web UI (`data/`) to LittleFS.
4. `pio run -t upload` — builds and flashes the firmware.
5. On first boot with no WiFi configured, the device opens a **Qackon-Setup**
   access point (password `qackon123`). Connect from a phone, a captive
   portal should pop up automatically (or visit `http://4.3.2.1`).
6. Enter your WiFi network + emergency contact, save, and it reboots joined
   to your network. From then on the dashboard is at the device's IP (check
   your router, or use `qackon-beacon.local` if your OS supports mDNS/Bonjour
   — not yet implemented in firmware, see roadmap).

## Architecture

Each subsystem is an isolated module with a `begin()`/`loop()` pair, wired
together in `src/main.cpp`. Nothing reaches across modules except through
the shared `cfg` struct (`src/config.h`) — same shape as WLED's `cfg.json`
pattern, which is what makes one web form / JSON API able to drive
everything.

| Module | File | Responsibility |
|---|---|---|
| Config | `config.h/.cpp` | Load/save all settings as `/cfg.json` on LittleFS |
| WiFi | `wifi_manager.h/.cpp` | STA connect, AP + captive-portal fallback |
| Web | `web_server.h/.cpp` | Serves UI, JSON status/config API |
| OTA | `ota.h/.cpp` | Web-based firmware upload |
| BLE | `ble_config.h/.cpp` | WiFi/contact setup without a WiFi connection at all |
| GPS | `gps.h/.cpp` | NEO-6M parsing via TinyGPS++ |
| IMU | `imu.h/.cpp` | MPU6050 impact/motion detection → auto-arm trigger |
| GSM | `gsm.h/.cpp` | SIM800L registration + emergency SMS |
| Beacon | `beacon.h/.cpp` | Triggers the external 457 kHz transmitter board |
| Cloud | `cloud.h/.cpp` | Periodic HTTPS POST of the current fix |

The web UI (`data/`) is plain HTML/CSS/JS with zero dependencies — it has to
work fully offline inside the setup AP, so no CDN fonts or frameworks.

## Wiring

| Peripheral | ESP32 pins | Notes |
|---|---|---|
| NEO-6M GPS | RX2=16, TX2=17 | 9600 baud |
| SIM800L | RX1=26, TX1=27 | **Needs its own 3.7–4.2V supply**, not the ESP32 3.3V rail — current spikes to ~2A on transmit |
| MPU6050 | SDA=21, SCL=22 | Default I2C pins |
| Beacon transmitter trigger | GPIO25 | Enable/trigger line to the external RF board |
| Status LED | GPIO2 | Onboard LED on most dev boards |

Adjust pin constants at the top of each module's `.cpp` if your wiring
differs.

## Hardware gaps (read before building)

The original spec claimed capabilities this design doesn't actually deliver
yet — worth being upfront about:

- **457 kHz RF generation**: the ESP32 does not synthesize this itself. The
  firmware only toggles a trigger/enable line — you need a real 457 kHz
  transmitter board (the "custom-designed antenna/module" the spec
  mentions) and its own regulatory clearance for your region. Avalanche
  beacon frequencies are regulated; check local law before transmitting.
- **Submersion detection**: an accelerometer alone can't detect being
  underwater. `imu.cpp` handles impact/motion triggering only. Real
  submersion sensing needs a conductivity probe or pressure sensor wired in
  separately — there's a `submersionAutoArm` config flag reserved for this,
  but no sensor driver yet.
- **Waterproof/impact-resistant casing** is a mechanical design task, not
  firmware — out of scope for this repo.

## CI/CD

Two GitHub Actions workflows live in `.github/workflows/`:

- **test.yml** — runs on every push/PR to `main`: compiles the firmware,
  builds the LittleFS image, runs `pio check` static analysis, and
  sanity-checks `data/script.js`. Catches broken builds before merge.
- **release.yml** — runs when you push a tag like `v1.0.0`: builds firmware,
  bootloader, partitions, and LittleFS binaries, stamps the version into the
  firmware, and publishes them all to a GitHub Release with checksums and
  flashing instructions attached.

To cut a release: `git tag v1.0.0 && git push origin v1.0.0`.

## Roadmap

- [ ] Submersion sensor driver (conductivity or pressure-based)
- [ ] GPRS fallback for cloud upload when WiFi is unavailable (GSM module
      already brings up a data-capable modem; upload path isn't wired to it)
- [ ] mDNS (`qackon-beacon.local`) so you don't need to hunt for the IP
- [ ] Geofencing enforcement (config fields exist, no trigger logic yet)
- [ ] Companion mobile app for BLE setup (the BLE service is ready; needs a
      client)
- [ ] Basic auth / token on the web API before shipping to strangers

## License

Apache License 2.0 — see `LICENSE`.
