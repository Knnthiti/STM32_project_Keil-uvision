# STM32G474 Peripheral Examples — Keil µVision

A collection of **STM32G474RE** peripheral examples written in C using **STM32 Low-Layer (LL) Drivers**, ready to open, modify, and run on hardware with Keil µVision.

The examples cover PWM generation, frequency measurement with Input Capture, UART transfers using DMA, ADC oversampling, and Ethernet connectivity through a W5500 module. Each folder is a separate project that can be built and flashed independently.

## Examples

| Example | Topics | Keil Project | Target |
| --- | --- | --- | --- |
| Timer | TIM2 PWM, TIM3 Input Capture, and the PC13 button | [Timer.uvprojx](STM32G474_Timer/MDKproject/Timer.uvprojx) | `STM32G474RET` |
| UART | USART1/USART3 with DMA and Multiprocessor Address Mark mode | [UART.uvprojx](STM32G474_UART/MDKproject/UART.uvprojx) | `STM32G474` |
| ADC Oversampling | ADC2 Regular + DMA, ADC5 Injected, timer triggers, and Analog Watchdog | [ADC_oversampling.uvprojx](STM32_G474_oversampling/MDKproject/ADC_oversampling.uvprojx) | `STM32G474` |
| SPI / Ethernet | SPI2 communication with a W5500 TCP client and a Python TCP server | [SPI.uvprojx](STM32_G474_SPI/MDKproject/SPI.uvprojx) | `STM32G474RET` |

## Requirements

- An **STM32G474RE** development board. The projects target `STM32G474RETx`.
- **Keil MDK / µVision 5**. The project files specify **Arm Compiler 6.24 (ArmClang)**.
- Device Family Pack **Keil.STM32G4xx_DFP 2.2.0**. The [`.pack` file](Keil.STM32G4xx_DFP.2.2.0.pack) is included in this repository.
- An ST-LINK programmer/debugger and cables for SWD programming and debugging.
- Jumper wires and equipment appropriate to each example, such as an analog signal source, oscilloscope, or logic analyzer.
- For the Ethernet example: a **W5500** module, an Ethernet cable, and a computer with **Python 3** installed.

CMSIS and STM32G4xx drivers are included in each project's `libraries/` directory.

### System Clock

All examples configure the clock in `BPS/Src/Clock_system.c` as follows:

| Setting | Value |
| --- | --- |
| HSE | 24 MHz, bypass disabled |
| PLL | M = 6, N = 85, R = 2 |
| SYSCLK / HCLK | 170 MHz |
| APB1 / PCLK1 | 85 MHz |
| APB2 / PCLK2 | 42.5 MHz |

Check your board's clock source and frequency before running an example. If they differ, update `Clock_system.c` and any constants that depend on the clock frequency. The firmware waits until HSE is ready before continuing.

## Getting Started

1. Clone the repository, or download and extract the ZIP archive.

   ```bash
   git clone https://github.com/Knnthiti/STM32_project_Keil-uvision.git
   cd STM32_project_Keil-uvision
   ```

2. Install Keil MDK and the device pack from `Keil.STM32G4xx_DFP.2.2.0.pack`.
3. Open the `.uvprojx` file for your chosen example from the table above.
4. In **Options for Target**, verify that the device is `STM32G474RETx` and select an installed Arm Compiler. The existing projects use version 6.24.
5. Connect the hardware for the example, then select ST-LINK and SWD in the debug settings.
6. Select **Build Target**, then **Download** to flash the firmware to the board.
7. Start a debug session, run the program, and inspect the results in the Watch window or with the relevant measurement equipment.

Keep the directory structure intact: the projects use relative paths for source files and include directories.

## 1. Timer — PWM and Input Capture

Source: [main.c](STM32G474_Timer/App/Src/main.c) · [Timer.c](STM32G474_Timer/BPS/Src/Timer.c)

| Function | Peripheral | STM32 Pin |
| --- | --- | --- |
| PWM output | TIM2_CH1 | PA0 |
| Input Capture | TIM3_CH1 | PA6 |
| Button for changing PWM settings | GPIO input | PC13 |

**Try it:** Connect `PA0 → PA6` to feed the PWM output into Input Capture. Monitor `current_time`, `time_diff`, and `frequency` in the Watch window. You can also measure the PA0 signal with an oscilloscope.

TIM2 uses center-aligned mode with a prescaler value of `2` and a compare value of `2048`. The program sets the auto-reload value to `8192` when PC13 is high and `4095` when it is low, changing both the frequency and duty cycle.

**Current limitation:** The `time_diff` calculation does not compensate for TIM3 counter wraparound. Verify the reported frequency against measurement equipment.

## 2. UART — DMA and Multiprocessor Mode

Source: [main.c](STM32G474_UART/App/Src/main.c) · [USART.c](STM32G474_UART/BPS/Src/USART.c)

| Port | TX | RX | Usage |
| --- | --- | --- | --- |
| USART1 | PC4 | PC5 | DMA / master in Multiprocessor mode |
| USART3 | PB10 | PB11 | DMA / slave address `0x01` |
| UART4 | PC10 | PC11 | Slave address `0x02` in Multiprocessor mode |

### DMA Mode — Enabled by Default

`main.c` enables `#define USART_DMA`. USART1 and USART3 use **115200 baud, 8 data bits, odd parity, and 1 stop bit (8O1)**. The peripheral data width is set to 9 bits to include the parity bit.

**Try it:** Connect `PC4 (USART1 TX) → PB11 (USART3 RX)` and `PB10 (USART3 TX) → PC5 (USART1 RX)`. Monitor `rx_buffer_U1` and `rx_buffer_U3` in the Watch window.

The program alternates between sending `"56"` and `"67"` through DMA, with a one-second delay between transmissions. The transfer length uses `sizeof()`, so it includes the trailing `\0` byte. Each receive operation is configured for 20 bytes. The code does not yet handle message boundaries or wait for a complete reception before starting the next cycle.

### Multiprocessor Mode

To try Address Mark mode, disable `USART_DMA` and enable `USART_mutil` in `main.c`. Enable only one mode at a time so that there is a single `main()` function.

```c
//#define USART_DMA
#define USART_mutil
```

This mode uses **115200 baud, 9 data bits, no parity, and 1 stop bit**. USART1 sends an address packet to wake USART3 or UART4 from Mute Mode, using the addresses defined in [USART.h](STM32G474_UART/BPS/Inc/USART.h). Wire the master's TX to both slave RX pins, and connect the slave TX return paths to the master's RX.

## 3. ADC — Oversampling, DMA, and Timer Triggers

Source: [main.c](STM32_G474_oversampling/App/Src/main.c) · [ADC.c](STM32_G474_oversampling/BPS/Src/ADC.c) · [DMA.c](STM32_G474_oversampling/BPS/Src/DMA.c)

| Component | Configuration |
| --- | --- |
| ADC2 Regular | 12-bit, differential, sequence using channels 1 and 3 |
| ADC2 analog GPIO pins | PA0, PA1, PA6, PA7 |
| ADC2 oversampling | 4× ratio, 2-bit right shift |
| ADC2 trigger | TIM3_CH4, with continuous conversion enabled |
| ADC2 DMA | DMA1 Channel 1, circular mode, two-value buffer |
| Analog Watchdog 1 | ADC2 Channel 1, thresholds 0–2048 |
| ADC5 Injected | 12-bit, differential, channel 1 |
| ADC5 analog GPIO pins | PA8, PA9 |
| ADC5 oversampling | 8× ratio, 3-bit right shift |
| ADC5 trigger | TIM2_TRGO |
| Timer PWM outputs | PB1 = TIM3_CH4, PB3 = TIM2_CH2 |

**Try it:** Connect analog signals suitable for differential measurements according to the MCU pinout and electrical specifications. Monitor `ADC2_Buffer[0]`, `ADC2_Buffer[1]`, `V0`, and `V1` in the Watch window.

The code calculates `V0` and `V1` using `ADC × 3.3 / 4095`. Check the conversion formula for differential mode and the actual reference voltage before using these values as voltage measurements.

**ADC5 status:** Injected conversion and interrupts are configured, but `ADC5_IRQHandler()` currently only clears the interrupt flag. It does not read the conversion result into `ADC5_Buffer`. The main program also does not update `V2` or `V3`.

## 4. SPI / Ethernet — W5500 TCP Client

Source: [main.c](STM32_G474_SPI/App/Src/main.c) · [SPI.c](STM32_G474_SPI/BPS/Src/SPI.c) · [test_TCP.py](STM32_G474_SPI/test_TCP.py)

### W5500 Wiring

| W5500 Signal | STM32 Pin |
| --- | --- |
| SCK | PB13 / SPI2_SCK |
| MISO | PB14 / SPI2_MISO |
| MOSI | PB15 / SPI2_MOSI |
| CS / SCS | PB1 |
| RST | PB2 |
| GND | Common ground with the board |

Power the W5500 module according to its specifications. SPI2 is configured as master, full duplex, 8-bit, MSB first, Mode 0, with a `/8` clock prescaler. Chip select is controlled through GPIO.

### Network Settings

| Setting | Default Value | Where to Change It |
| --- | --- | --- |
| W5500 IP | `192.168.3.2` | `W5500_Init()` in `SPI.c` |
| Subnet mask | `255.255.255.0` | `W5500_Init()` in `SPI.c` |
| Gateway | `192.168.3.16` | `W5500_Init()` in `SPI.c` |
| MAC address | `00:08:DC:01:02:03` | `W5500_Init()` in `SPI.c` |
| TCP source port | `50000` | `W5500_TCP_Client()` in `main.c` |
| PC / server IP | `192.168.3.10` | `W5500_TCP_Client()` in `main.c` |
| Server port | `5000` | `main.c` and `test_TCP.py` |

### Running the TCP Example

1. Connect the W5500 to the board and to the same LAN as your computer.
2. Set the computer's IP address to `192.168.3.10/24`, or update the destination IP in `main.c` to match your computer. Adjust the W5500 IP and subnet settings for your network, using an address that is not already assigned to another device.
3. Open a terminal at the repository root and run the Python TCP server. It uses only the standard library's `socket` module.

   ```bash
   python STM32_G474_SPI/test_TCP.py
   ```

4. Allow incoming connections on TCP port `5000` for the network used for the experiment.
5. Build, flash, and run the SPI project on the board.
6. Once connected, the server displays the received data. The firmware sends `Hello STM32` approximately once per second.

The Python server listens on `0.0.0.0:5000` to accept connections through the computer's network interfaces. The firmware initiates the TCP connection.

If the connection fails, inspect `w5500_version` in the debugger. The code expects `0x04`; any other value causes it to remain in an error loop before configuring the network.

## Project Structure

```text
STM32_project_Keil-uvision/
├── STM32G474_Timer/
├── STM32G474_UART/
├── STM32_G474_oversampling/
├── STM32_G474_SPI/
│   └── test_TCP.py
├── datasheet/
├── Keil.STM32G4xx_DFP.2.2.0.pack
└── README.md
```

Each example uses the following directories:

| Directory | Purpose |
| --- | --- |
| `App/Inc`, `App/Src` | Application headers and `main.c` |
| `BPS/Inc`, `BPS/Src` | System clock and peripheral configuration |
| `libraries/` | CMSIS, startup and system files, and STM32G4xx drivers |
| `MDKproject/` | Keil project files and build/debug settings |

## Documentation

- [STM32G474 Datasheet](datasheet/stm32g474_datasheet.pdf)
- [RM0440 — STM32G4 Reference Manual](datasheet/RM0440_ReferenceManual_STM32G4.pdf)
- [UM2570 — STM32G4 Low-Layer Drivers](datasheet/um2570-lowlayer-driver.pdf)
- [UM2505 — STM32G4 Nucleo-64 Boards](datasheet/um2505-stm32g4-nucleo64-boards.pdf)
- [Thai USART/UART document based on RM0440, pages 1689–1774](datasheet/output/pdf/RM0440_USART_UART_pages_1689_1774_TH.pdf)

## License

This repository does not currently include a `LICENSE` file for the example code. CMSIS and ST libraries include their own licensing terms in their respective files.
