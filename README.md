# PVO Novi Sad - Graphic Application

## Overview - 2D

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

## Overview - 3D

This graphical application simulates an Anti-Air Defense Station in a 3D environment. Below are the key features and instructions:

### Features

- **Student Information:** Displayed as a transparent texture in one corner of the screen, showing index, name, and surname.
- **Perspective Projection:** Initial setup with arbitrary parameters, ensuring the entire scene is visible.
- **Depth Testing:** Activated for accurate rendering.
- **Back-face Culling:** Enabled for efficient rendering.
- **Phong Lighting Model:** Implemented for realistic lighting effects.
- **Terrain:** Flat terrain with a mountainous region where the station is situated, featuring the same map texture as the previous version.
- **Specular Map:** Terrain includes a specular map, with water being reflective.
- **Camera:** Positioned in the drone launch station, facing the city.
- **Drone Control:** Drones can ascend and descend using designated keys.
- **Drone Destruction:** Drones are destroyed upon hitting the ground.
- **Drone Model:** Loaded as a 3D model for a more realistic representation.
- **Drone Lighting:** Emitting green point light with limited range.
- **Target Generation:** Targets are randomly generated at varying heights, loaded as arbitrary 3D models.
- **Target Lighting:** Point lights with limited range and intensity on targets, extinguishing upon destruction.
- **Low-flying Targets:** In addition to regular targets, there are low-flying targets with different colors, moving at a slower speed.
- **Night Scene:** Set in a nighttime environment with subtle directional light.
- **Cloud Layer:** Semi-transparent clouds above the terrain.
- **City Spotlight:** At least one strong spotlight in the city center, changing direction over time.

### Map Indicators

- The height of drones and targets affects the size of their indicators on the map. Higher entities have larger indicators.

## Instructions

- **Drone Control:** Use designated keys to ascend and descend.
- **Drone Destruction:** Press X to activate self-destruction or if the drone goes outside the map.
- **Map Toggle:** Press 1 to hide the map, and 2 to bring it back.
- **Student Information:** Displayed in one corner of the screen.

Enjoy the 3D Anti-Air Defense Simulation!


## Tools Used

- **Development Environment:** Visual Studio
- **Libraries:** GLEW, GLFW

## Screenshots - 2D
![window of app](https://github.com/MilosMilutinovic01/ComputerGraphic/assets/116631195/f033a773-8a96-4d3b-9674-c0212dc3320b)
)
![window of app](https://github.com/MilosMilutinovic01/ComputerGraphic/assets/116631195/9d38788c-63db-487e-b52e-697ddbce12db)
)

## Screenshot - 3D
![3D project](https://github.com/MilosMilutinovic01/ComputerGraphic/assets/116631195/ca48f82b-26eb-4f31-a0c9-8c61614715b9)
