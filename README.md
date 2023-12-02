# PVO Novi Sad - Graphic Application

## Overview

Develop a graphical application to manage the Anti-Air Defense Station of Novi Sad. The application should include the following elements:

- **Map Display:** A green screen displaying a map of Novi Sad, large enough to encompass Fruška Gora.
- **LED Indicator:** Lit when an aircraft is currently in the air.
- **7-Segment Indicator:** Represents the number of unmanned aerial vehicles that can be launched. Launching a vehicle decreases the indicator by one. Once the indicator reaches 0, launching vehicles is not possible.
- **Enemy Helicopters:** Up to 5 helicopters appear randomly at the screen edges, represented by red dots. They move in a straight path towards the center of Novi Sad, pulsating at a faster frequency as they get closer.
- **Anti-Air Defense Installation:** A bright green circle on Fruška Gora represents the Anti-Air Defense installation.
- **Interceptor Drone:** Launched from the Anti-Air Defense station using the SPACE key. It is represented in blue and moves three times faster than helicopters. If the drone hits a helicopter, both are removed from the screen.
- **Launch Limit:** Only one vehicle can be launched at a time. The SPACE bar does not launch a new vehicle while one is active.
- **Map Toggle:** Pressing the 1 key hides the map, and pressing the 2 key brings it back to the screen.
- **Self-Destruction:** Pressing the X key activates the self-destruction of the current vehicle. The vehicle is also considered destroyed if it goes outside the map.
- Include your name, surname, and index in any corner of the screen.

## Tools Used

- **Development Environment:** Visual Studio
- **Libraries:** GLEW, GLFW

## Screenshots
![window of app](https://cdn.discordapp.com/attachments/1151815941051650079/1180566208081313922/image2.png?ex=657de2f5&is=656b6df5&hm=22a6ed239319f9cf1e2f8b96f4447b3d5fe10b195a0ee15c3d98d912ddada35a&)
![window of app](https://cdn.discordapp.com/attachments/1151815941051650079/1180566208500748399/image1.png?ex=657de2f5&is=656b6df5&hm=e29a9990ee3a080bcd3ef6bec081b2779ac2c8ef8040b11e84323c56fef6fee6&)
