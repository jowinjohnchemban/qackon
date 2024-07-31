# Qackon Beacon

## Overview

**Qackon Beacon** is a cutting-edge safety device designed for automobile and personal security. Ideal for use in extreme conditions such as underwater and underground, Qackon Beacon offers real-time GPS location tracking, emergency signaling, and Bluetooth configuration. With advanced features like telephone network connectivity, Wi-Fi internet access, and SMS notifications, Qackon Beacon ensures reliability and safety in diverse environments.

## Features

- **Beacon Transmission:** Emits a powerful signal at 457 kHz for long-range detection and reliable communication.
- **GPS Tracking:** High-sensitivity GPS module for precise location tracking and monitoring.
- **Bluetooth Configuration:** Easily manage settings and personal details via Bluetooth Low Energy (BLE).
- **Telephone Network Connectivity:** Provides internet access and SMS functionality for remote tracking and management.
- **Wi-Fi Connectivity:** Connects to available Wi-Fi networks for seamless internet access.
- **Automatic Activation:** Uses integrated sensors to detect movement or submersion, automatically activating the beacon when needed.
- **Data Upload:** Updates location on a cloud server at preset intervals if no network is available.
- **Durable Design:** Features a waterproof and impact-resistant casing to withstand harsh environments.
- **Power Management:** Equipped with a long-lasting battery and efficient power management for extended operation.

## Components

1. **Microcontroller Unit (MCU):** ESP32-WROOM-32
   - Dual-core processor with integrated Wi-Fi and Bluetooth
   - Low power consumption for extended battery life

2. **Beacon Transmitter Module:** Custom-designed for 457 kHz
   - High signal strength for reliable long-range detection

3. **Power Supply:**
   - Battery: Panasonic NCR18650B (3.7V 3400mAh)
   - Voltage Regulator: Texas Instruments TPS7A4700 for stable power supply

4. **GPS Module:** Ublox NEO-6M
   - High sensitivity for accurate location tracking

5. **Bluetooth Module:** Integrated in ESP32
   - Bluetooth Low Energy (BLE) for efficient configuration

6. **Telephone Network Module:** GSM/GPRS/LTE Module (e.g., SIM800L)
   - Supports SMS and internet access for remote management

7. **Wi-Fi Module:** Integrated in ESP32
   - Connects to available Wi-Fi networks for internet connectivity

8. **Activation Sensors:** MPU6050
   - 3-axis accelerometer and gyroscope for precise movement detection

9. **Antenna:** Custom-designed for 457 kHz
   - Optimized for effective signal transmission

## Installation

### Hardware Assembly

1. **Assemble Components:**
   - Connect the MCU, GPS module, Bluetooth module, telephone network module, and activation sensors to the beacon transmitter module following the circuit diagram.
   - Ensure all connections are secure and components are correctly mounted within the durable casing.

2. **Power Supply:**
   - Install the battery and connect it to the power management circuit.
   - Verify that the voltage regulator maintains a stable power supply.

3. **Antenna:**
   - Attach the custom-designed antenna to the beacon transmitter module.

4. **Telephone Network Module:**
   - Integrate the GSM/GPRS/LTE module for SMS and internet access.
   - Ensure proper connection and signal reception.

### Software Setup

1. **Microcontroller Programming:**
   - Program the ESP32 with firmware to manage beacon transmission, GPS data processing, Bluetooth communication, telephone network functionality, Wi-Fi connectivity, and sensor integration.
   - Upload the firmware using an IDE or compatible programming environment.

2. **Mobile App Installation:**
   - Download and install the Qackon mobile app from the app store.
   - Ensure compatibility with your smartphone and Bluetooth Low Energy (BLE).

3. **Cloud/Server Setup:**
   - Configure the cloud/server to receive location updates from Qackon.
   - Set up data update intervals for when no network is available.

## Usage

### Activating Qackon

1. **Automatic Activation:**
   - The beacon will automatically activate if it detects significant movement or submersion.

2. **Manual Activation:**
   - Use the mobile app to manually activate or deactivate the beacon.

### Configuring Qackon

1. **Open the Mobile App:**
   - Enable Bluetooth on your smartphone and connect to Qackon.

2. **Configuration Options:**
   - Set personal details (name, contact information).
   - Configure emergency contacts and sensor sensitivity.
   - Adjust beacon transmission frequency and power-saving modes.
   - Configure Wi-Fi settings and telephone network parameters.

3. **GPS Tracking:**
   - View the current location and history of Qackon on the app's map interface.
   - Set geofencing boundaries and enable location sharing with emergency contacts.

### Data Upload and SMS

1. **Internet Access:**
   - Qackon automatically connects to available Wi-Fi networks or uses the telephone network (with an active SIM card) for internet access.

2. **SMS Notifications:**
   - Sends SMS alerts with the current location to predefined contacts in emergencies or if internet connectivity is unavailable.

3. **Cloud/Server Updates:**
   - Stores location data and updates the cloud/server once network connection is restored.

## Maintenance

- **Battery Replacement:** Replace the battery when power levels are low or the battery no longer holds a charge.
- **Firmware Updates:** Check the mobile app for firmware updates to ensure optimal performance.
- **Inspection:** Regularly inspect the device for damage and ensure all components are functioning properly.

## Safety and Compliance

- **Compliance:** Ensure Qackon Beacon complies with relevant regulatory standards for transmission frequencies, electronic devices, and telephone network usage.
- **Safety:** Follow all safety instructions to prevent damage or injury.

## License

This project is licensed under the **Apache License 2.0**. See the [LICENSE](LICENSE) file for details.
