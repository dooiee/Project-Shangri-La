# Project Shangri-La ⛩️

<img align="right" width="200" src="https://github.com/dooiee/Project-Shangri-La/blob/main/other-stuff/images/Koi_Vinyl_Rescaled.png?raw=true" alt="Project Shangri-La koi artwork">

**An IoT-powered koi pond restoration and automation project.**

Project Shangri-La began as an effort to transform a neglected concrete pond into a healthy, low-maintenance home for koi. It grew into a hands-on exercise in embedded systems, sensor integration, wireless communication, cloud infrastructure, and native iOS development. I designed and built the physical restoration and the hardware/software system that lets me monitor the pond and control equipment remotely.

This was one of my earliest end-to-end engineering projects. Some of its original decisions reflect what I knew at the time; the project is a record of learning by building, breaking, debugging, and improving a real system.

## Features

- **Water-quality monitoring:** temperature, pH, turbidity, total dissolved solids, and water depth, with live readings and historical trends.
- **Remote automation and control:** automatic rainwater top-offs using water-level thresholds, manual solenoid control, and RF control of underwater lighting.
- **Native iOS dashboard:** SwiftUI views for sensor data, historical charts, equipment controls, system status, and settings.
- **Camera monitoring:** live RTSP video in the app, with camera configuration and captured-photo browsing.
- **Motion alerts:** ONVIF camera event monitoring, snapshot capture, Firebase Storage uploads, and image-enabled push notifications.
- **Configurable notifications:** per-parameter thresholds and notification intervals, plus connectivity and server-status alerts.
- **BLE sensor integration:** Raspberry Pi-based sensor peripheral experiments and a Python/Node.js bridge for collecting readings and publishing them to Firebase.

## Architecture

```text
Sensors / microcontrollers ──┐
                             ├──> BLE / local services ──> Firebase Realtime Database
Raspberry Pi BLE peripheral ──┘                                    │
                                                                   ▼
                                                        SwiftUI iOS application
                                                                   │
                                     Remote commands / settings ───┘

ONVIF IP camera ──> Python camera service ──> RTSP snapshots / Firebase Storage
                                               │
                                               └──> Firebase Cloud Messaging
```

The system evolved over time. The repository includes original microcontroller integrations, Node.js BLE services, and newer Python services; these are **different implementations and experiments**, not necessarily services intended to run simultaneously.

### Main components

| Component | Responsibility |
| --- | --- |
| Embedded controllers | Collect sensor readings and operate connected hardware. |
| BLE peripheral and gateway | Expose sensor characteristics, connect to peripherals, aggregate readings, and forward data. |
| Firebase Realtime Database | Share current readings, historical logs, settings, and remote-control state. |
| Python camera service | Handle ONVIF motion events, RTSP image capture, photo storage, and notifications. |
| iOS app (Swift / SwiftUI) | Display live and historical data, stream camera video, and control the system. |
| Firebase Cloud Messaging | Deliver threshold, connection, and camera-event notifications. |

## Technology

**Hardware:** Arduino MKR WiFi 1010, Arduino Nano 33 IoT, ESP32, Raspberry Pi, water-quality and depth sensors, relays, solenoid valve, 433 MHz RF modules, and IP camera.

**Software:** C/C++, Python, JavaScript/Node.js, Swift/SwiftUI, Firebase Realtime Database, Firebase Storage, Firebase Cloud Messaging, BLE (Bleak / Noble / Bleno experiments), Flask, RTSP, ONVIF, and VLCKit-based video playback.

## Demos

<video src="https://user-images.githubusercontent.com/71622811/188991797-72c4ba37-2b89-409f-95e6-2039481f20bc.mp4" controls width="600"></video>

<video src="https://user-images.githubusercontent.com/71622811/189450137-4b5813f1-78b0-4a68-a956-98fa43d12203.mp4" controls width="600"></video>

<video src="https://user-images.githubusercontent.com/71622811/189450438-dd4cd04f-8a0b-4f7f-9acf-b6fe17a12255.mp4" controls width="600"></video>

## iOS Application

<img width="200" src="https://user-images.githubusercontent.com/71622811/189449200-38ace229-8d8e-48aa-a186-e808378520e1.jpeg?raw=true" alt="App screenshot 1"> <img width="200" src="https://user-images.githubusercontent.com/71622811/189451111-f41b833f-cb4b-457c-82ea-f9e1a42e2083.jpeg?raw=true" alt="App screenshot 2"> <img width="200" src="https://user-images.githubusercontent.com/71622811/189451517-78dd7e51-2ab2-45d4-823e-5aed73d0063c.jpeg?raw=true" alt="App screenshot 3"> <img width="200" src="https://user-images.githubusercontent.com/71622811/189451938-5e37e84f-62b7-4f49-8dd6-3c0d6a29ba01.jpeg?raw=true" alt="App screenshot 4">

The app includes dedicated screens for water parameters, historical data, camera streaming, RF lighting controls, solenoid operation, system status, and notification preferences. Screenshots above show an earlier version of the interface.

## Pond Restoration

![Pond restoration progress](https://github.com/dooiee/Project-Shangri-La/blob/main/other-stuff/Progress_Gifs/PanoramicStartToFinishProgress.gif)

<img width="350" src="https://user-images.githubusercontent.com/71622811/189450978-d58cb060-40e7-4df5-82b9-8dfa9a4d45da.jpeg?raw=true" alt="Pond restoration photo 1"> <img width="350" src="https://user-images.githubusercontent.com/71622811/189451202-2cd09bf6-2cb2-45cf-a671-2420ec180cc4.jpeg?raw=true" alt="Pond restoration photo 2">

<img width="300" src="https://user-images.githubusercontent.com/71622811/189453065-c1d580b3-102f-4a6d-b2d3-4b2a9d2108db.jpeg?raw=true" alt="Pond restoration photo 3"> <img width="300" src="https://user-images.githubusercontent.com/71622811/189451401-fcc9abcf-1d32-4a45-908f-519bcdb88be5.jpeg?raw=true" alt="Pond restoration photo 4">

<img width="500" src="https://user-images.githubusercontent.com/71622811/189453324-874c0459-3354-41c3-ab3b-9383e46dccb0.jpeg?raw=true" alt="Restored pond">

## What I Learned

The physical build was as demanding as the software. Water ingress damaged electronics; unreliable boards and cloud integrations forced redesigns; incompatible liner materials required rebuilding a section of the waterfall. The hardest part was rarely writing the first version—it was understanding failures across hardware, power, connectivity, and software, then making the entire system reliable enough to leave running unattended.

I replaced an early Arduino IoT Cloud integration with Firebase, improved controller responsiveness, and iterated on hardware protection and sensor selection. Those decisions taught me to isolate failures, account for real-world constraints, and favor maintainable, observable systems over solutions that only work in ideal conditions.

Looking back with more engineering experience, there are many things I would architect differently. That is part of why I keep this project public: it shows the progression from an idea to a working system, including the imperfect steps in between.

## Potential Next Steps

- Over-the-air firmware updates and improved device diagnostics.
- Additional automation, including chemical dosing and feeding.
- Further cleanup and consolidation of experimental services.
- More robust deployment, configuration, and recovery workflows.

---

*Personal project — built and iterated on over time. Some files are prototypes or historical implementations rather than a single deployable release.*
