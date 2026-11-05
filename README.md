# PWM on STM32F103C8T6 (Blue Pill)

Learning project: generating PWM signals with the hardware timers of the STM32F103C8T6, implemented twice so the two approaches can be compared:

| Folder | Approach | What it contains |
|---|---|---|
| [`bare_metal/`](bare_metal/) | Direct register access, no libraries | DC motor speed + direction control through an H-bridge. **Start here: the README inside explains every register used.** |
| [`HAL/`](HAL/) | STM32CubeIDE + HAL | CubeMX project (`PWM.ioc`) generating the HAL/CMSIS code for the same board |

## Repository layout

```
PWM/
├── bare_metal/     main.c, link.ld, Makefile  (motor control, TIM2_CH1)
├── HAL/            STM32CubeIDE project (PWM.ioc, Core/, Drivers/)
└── main.c, link.ld, Makefile   dual-timer experiment (see below)
```

### Root-level experiment (to be moved)

The loose `main.c`, `link.ld` and `Makefile` at the repo root are an earlier bare-metal experiment that runs two timers at once:

- **TIM2 channel 2 on PA1**: duty-cycle sweep.
- **TIM3 channel 2 on PA7**: 20 ms period (50 Hz) with pulse widths of 1000 to 2000 µs, i.e. standard hobby-servo timing.

## Hardware

- Board: STM32F103C8T6 "Blue Pill" (Cortex-M3)
- Programmer: ST-Link (flashed with `st-flash`)
- Motor driver: H-bridge (see `bare_metal/README.md` for the wiring)

## Toolchain

- `arm-none-eabi-gcc` (bare metal)
- `st-flash` from the stlink tools (flashing)
- STM32CubeIDE (HAL project)

## Build and flash (bare metal)

```
cd bare_metal
make            # builds firmware.elf and firmware.bin
make stmflash   # flashes firmware.bin to 0x08000000
make clean
```
