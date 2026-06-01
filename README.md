# DIY sACN Node

## Introduction

I was wondering if it was possible to create a cheaper DIY alternative to commercial network DMX nodes, so I decided to build one myself.

Unfortunately, due to my very limited C++ knowledge, the code is currently written entirely by AI. I plan to learn C++ properly and rewrite the software in the future, but at the moment it is not my highest priority.

## The Project

I designed a case in Fusion 360 (files can be found in `/sacn node 3d files`), which I still use to some extent.

Since creating the original design, a lot of the core functionality has changed. For example, the current version only has three XLR ports instead of the originally planned four.

## Parts List

### Core Components

| Part | Link |
|--------|--------|
| WT32-ETH01 | aliexpress.com/item/1005009497813354.html |
| MAX485 | aliexpress.com/item/1005008908714833.html |
| 0.91" OLED Display | aliexpress.com/item/1005006153887805.html |
| XLR Connectors | aliexpress.com/item/1005004216943482.html |

### Additional Components

| Part | Link |
|--------|--------|
| USB-C Extension | aliexpress.com/item/1005007619862484.html |
| Mini Breadboard | aliexpress.com/item/1005005304653006.html |
| Dupont Cables | aliexpress.com/item/1005007046465880.html |

Other required parts:

- Screws for the lid (I used 2.5x16 but any simmilar will do)
- Screws for mounting the WT32-ETH01 (I used 2.5x10)
- Cable (I used DMX cable from Thomann: https://www.thomann.de/de/cordial_csp_1.htm)

## Required Tools

### Electronics

- Soldering iron
- Solder
- Screwdriver

### Programming

- Computer
- USB-to-TTL adapter

Alternatively, another ESP32 or an Arduino with USB can be used as a programmer.

For a detailed explanation, see:
https://new.esphome.io/guides/devboard_as_flasher

### Manufacturing

- 3D printer
- Filament
