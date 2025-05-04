# GPIO Subsystem (Integer)

## 1. GPIO subsystem
- The GPIO subsystem is a Linux kernel framework for managing GPIO pins on SoCs like the BCM2711 (Raspberry Pi 4).
- It provides APIs for kernel modules and user-space applications to configure and control GPIOs (e.g., set as input/output, read/write values).
- It abstracts hardware details, making code portable across different SoCs (unlike direct register access, which is BCM2711-specific).

### Integer-Based Interface:
- In kernel 5.4.72, the integer-based GPIO API (also called the legacy GPIO API) uses integer GPIO numbers to identify pins.
- Example: GPIO27 (PIN13 on Pi 4) is referenced by its GPIO number (27).
- Functions like `gpio_request()`,` gpio_direction_output()`, and `gpio_set_value()` control the pin.
- Note: The integer-based interface is considered “legacy” in newer kernels (e.g., 5.10+), replaced by the descriptor-based API (gpiod_*). However, for kernel 5.4.72-v8, the integer-based API is standard and widely used.

### Why use it ?
- Abstraction: No need to manually manipulate registers (e.g., 0xFE200000 for BCM2711 GPIO).
- Simplicity: Cleaner code than bit-level operations.
- Portability: Works across SoCs with GPIO controllers.
- Safety: The kernel handles pin multiplexing and conflicts.

```c
gpio_request(27, "led"); // Request GPIO27
gpio_direction_output(27, 0); // Set as output, initial low
gpio_set_value(27, 1); // Set high (LED on)
```

## 2. Concept
- Each GPIO pin has a unique number (e.g., 27 for GPIO27/PIN13).
- On Raspberry Pi 4, GPIO numbers match the BCM numbering scheme (e.g., GPIO27 is PIN13, per Revision 4, Chapter 1, Section 1.2).
- Numbers are defined in the kernel’s device tree or board-specific files.
- The BCM2711 has a GPIO controller managing all GPIO pins.
- The kernel maps GPIO numbers to the controller via a device tree (e.g., bcm2711-rpi-4-b.dts in meta-raspberrypi).
- GPIOs can be controlled from user-space via `/sys/class/gpio/`:

```bash
echo 27 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio27/direction
echo 1 > /sys/class/gpio/gpio27/value
```

- Kernel modules use the same subsystem but with in-kernel APIs for better performance and control.

### Integer-Based API Functions (Kernel Space):
- `gpio_request(unsigned gpio, const char *label)`: Reserves a GPIO pin.
- `gpio_direction_output(unsigned gpio, int value)`: Sets GPIO as output with initial value.
- `gpio_set_value(unsigned gpio, int value)`: Sets GPIO high (1) or low (0).
- `gpio_free(unsigned gpio)`: Releases the GPIO.
- Include: `<linux/gpio.h>`.

### Limitations:
- Less flexible than the descriptor-based API (e.g., no bulk operations).
- Still suitable for simple tasks like LED control in kernel 5.4.72.
