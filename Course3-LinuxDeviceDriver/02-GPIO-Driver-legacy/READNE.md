# 2 - GPIO Driver (legacy)

## Step 1: Find Documentation for Raspberry Pi 4 (BCM2711)

To control GPIO27, we need the BCM2711’s GPIO register details (e.g., addresses, bit settings). Here’s how to locate and verify the documentation:

### 1.1. Identify the SoC
Raspberry Pi 4: Uses the Broadcom BCM2711 SoC (quad-core Cortex-A72, 64-bit).

### 1.2. Locate the BCM2711 ARM Peripherals Document
Official Source: The Raspberry Pi Foundation provides the “BCM2711 ARM Peripherals” document, which details GPIO and other peripherals.

Steps to Find:
- Visit the Raspberry Pi documentation site: .
- Navigate to **Hardware > Raspberry Pi > Processors** or search for “BCM2711”.
- Look for the [BCM2711 ARM Peripherals](https://datasheets.raspberrypi.com/bcm2711/bcm2711-peripherals.pdf)) PDF.
- Download the latest version (e.g., Revision 2, September 2020, to avoid errors like the GPIO base address typo in Revision 1).

Verify Document:
- Ensure it’s titled “BCM2711 ARM Peripherals” and covers the BCM2711 SoC.
- Check the revision (Revision 2 or later) to avoid known errors (e.g., GPIO base address listed as 0x7E215000 instead of 0x7E200000).

## Step 2: Read the BCM2711 ARM Peripherals Document
The BCM2711 ARM Peripherals document is technical, so here’s how to navigate it to find GPIO information for controlling GPIO27 (PIN13):

### 2.1. Understand the Document Structure
Chapters:
- Chapter 1 (Introduction): Lists peripheral base addresses.
- Chapter 5 (GPIO): Details GPIO registers, pin functions, and bit settings.
- Other Chapters: Cover interrupts, timers, etc., but are less relevant here.

Key Sections for GPIO:
- Section 1.2 (Address Map): Confirms the GPIO base address.
- Section 5.2 (GPIO Register Map): Lists registers (e.g., GPFSEL2, GPSET0, GPCLR0).
- Section 5.3 (Alternate Functions): Describes pin modes (e.g., output, alternate functions).

### 2.2. Extract GPIO Information
Follow these steps to find details for GPIO27:

#### 1. Find the Peripheral Base Address:
- Go to Chapter 1, Section 1.2 (Address Map).
- Look for the GPIO base address.
- Expected: 0x7E200000 (bus address), which maps to 0xFE200000 (physical address for ioremap).
- Note: Revision 1 incorrectly listed 0x7E215000; use Revision 2 or later.
- Physical Address: Use 0xFE200000 in the kernel module, as ioremap requires physical addresses.

#### 2. Locate the GPIO Chapter:
- Navigate to Chapter 5 (GPIO) (around pages 80–100).
- Key subsections:
    - GPIO Pins: BCM2711 has 58 GPIOs (0–57), including GPIO27.
    - Registers: GPFSEL (function select), GPSET (set), GPCLR (clear).
    - Alternate Functions: GPIO modes (input, output, alternate).

#### 3. Identify Relevant Registers
- GPFSEL2 (Function Select 2):
    - Purpose: Configures GPIO20–29’s mode.
    - Address: Base (`0xFE200000`) + offset `0x08` = `0xFE200008`.
    - Bits for GPIO27 (FSEL27): Bits 23–21.
        - `000`: Input.
        - `001`: Output (for LED).
        - `010–111`: Alternate functions (e.g., SPI, PWM).
    - Action: Set bits 23–21 to `001`.
- GPSET0 (Set 0):
    - Purpose: Sets GPIO0–31 high.
    - Address: Base + offset 0x1C = `0xFE20001C`.
    - Bit 27: Write 1 to set GPIO27 high (LED on).
- GPCLR0 (Clear 0):
Purpose: Clears GPIO0–31 low.
    - Address: Base + offset 0x28 = `0xFE200028`.
    - Bit 27: Write 1 to clear GPIO27 low (LED off).

#### 4. Verify Pinout:
- Check Section 5.3 (Alternate Functions) or a Pi 4 pinout diagram.
- Source: Run pinout on the Pi or visit raspberrypi.com/documentation.
- Result: PIN13 is GPIO27

#### 5. Understand Register Access:
- Memory-Mapped I/O: Access registers via physical addresses.
- Kernel Module: Use `ioremap` to map `0xFE200000`, `read`/`write` with `readl`/`writel`.
- Bit Manipulation:
    - GPFSEL2: Clear bits 23–21, set to `001`.
    - GPSET0/GPCLR0: Write `1 << 27`.

## Step 3: Understand the Raspberry Pi 4’s GPIO Setup (Updated)
Using Revision 4, confirm GPIO27’s setup:

### 1. SoC and Reference Manual:
- Pi 4: BCM2711, documented in “BCM2711 ARM Peripherals” Revision 4, Chapter 5.
- Base Address: `0xFE200000` (Revision 4, Section 1.2).

### 2. Pinout:
- PIN13: GPIO27 (verified via Revision 4, Section 5.3, or pinout diagrams).
- LED Connection: Anode to PIN13, cathode to ground (e.g., PIN14) with 330Ω resistor.

### 3. GPIO Registers:
- GPFSEL2: 0xFE200008, bits 23–21 = 001 (output).
- GPSET0: 0xFE20001C, bit 27 = 1 (LED on).
- GPCLR0: 0xFE200028, bit 27 = 1 (LED off).
- Total GPIOs: 58 (0–57), GPIO27 valid.

### 4. Address Mapping:
Pi 4: Physical `0xFE200000`, used directly with `ioremap`.

## Step 4: Design the Kernel Module
- Maps `0xFE200000` with `ioremap`.
- Configures GPIO27 as output (GPFSEL2).
- Toggles LED with GPSET0/GPCLR0.
- Creates `/dev/m_led` for “on”/“off” writes.

## Step 5: Write the Kernel Module Code

- Setup (`insmod`):
    - `led_init` creates `/dev/m_led` (character device).
    - Maps GPIO registers (`0xFE200000`) with ioremap.
    - Sets GPIO27 as output (`GPFSEL2`).
    - Turns the external LED on (`GPSET0`).
- User Interaction:
    - Write “on” to `/dev/m_led` (e.g., `echo "on" > /dev/m_led`) to turn the LED on.
    - Write “off” to turn it off.
    - `m_write` reads the input, checks for “on”/“off”, and calls `gpio_set` or `gpio_clear`.
- Cleanup (`rmmod`):
    - `led_exit` turns the LED off (`GPCLR0`), frees resources, and removes `/dev/m_led`.
- Hardware Connection:
    - The external LED is connected to PIN13 (GPIO27) with a 330Ω resistor to ground (e.g., PIN14). When GPIO27 is high (3.3V), current flows, lighting the LED. When low (0V), the LED is off.

```c
#define GPIO_BASE       0xFE200000  // Physical base address (Chapter 1, Section 1.2)
#define GPFSEL2         0x08        // Offset for GPFSEL2 (Chapter 5, Section 5.2)
#define GPSET0          0x1C        // Offset for GPSET0 (Chapter 5, Section 5.2)
#define GPCLR0          0x28        // Offset for GPCLR0 (Chapter 5, Section 5.2)
#define GPIO_27         27          // GPIO27 (PIN13, Section 5.3, requires external LED)
#define FSEL27_SHIFT    21          // Bits 23-21 for FSEL27 in GPFSEL2 (Section 5.2)
#define OUTPUT          0x1         // 001 for output mode
```
These define memory addresses and settings for controlling GPIO27, based on the “BCM2711 ARM Peripherals” Revision 4 document. The Raspberry Pi 4’s BCM2711 chip uses specific memory locations (registers) to control GPIOs. These numbers tell the code where to “talk” to the hardware.
- `GPIO_BASE` = `0xFE200000`: The starting physical address for GPIO registers (from Chapter 1, Section 1.2). Think of it as the street address of the GPIO control room.
- `GPFSEL2` = `0x08`: Offset for the register that sets GPIO27’s mode (address `0xFE200008`). It’s like a specific desk in the control room.
- `GPSET0` = `0x1C`: Offset for turning GPIO27 on (`0xFE20001C`).
- `GPCLR0` = `0x28`: Offset for turning GPIO27 off (`0xFE200028`).
- `GPIO_27` = `27`: The GPIO number (PIN13 on the 40-pin header).
- `FSEL27_SHIFT` = `21`: Bits 23–21 in GPFSEL2 control GPIO27’s mode (from Chapter 5, Section 5.2).
- `OUTPUT` = `0x1`: Binary 0`01 sets GPIO27 as an output (to drive the LED).

```c
#define BUFFER_SIZE     10
```
Defines the size of a memory buffer to store user input (e.g., “on” or “off”). When you write to `/dev/m_led` (e.g., `echo "on" > /dev/m_led`), the code needs a small space to hold that text. 10 bytes is enough for “on” (2 bytes + null terminator) or “off” (3 bytes + null).

```c
struct m_led_dev {
    dev_t dev_num;
    struct class *m_class;
    struct cdev m_cdev;
    void __iomem *gpio_base; // Virtual address of GPIO registers
    char *buffer;            // Buffer for user input ("on"/"off")
};

static struct m_led_dev mdev;
```
Defines a structure (`m_led_dev`) to hold all the module’s data, like a container for tools. `mdev` is a global instance of this container. The module needs to keep track of the device file (`/dev/m_led`), GPIO memory, and user input. The structure organizes this data.
- `dev_t dev_num`: Stores the major/minor number for /dev/m_led (like an ID for the device file).
- `struct class *m_class`: Represents the device class (m_led), which helps create /dev/m_led.
- `struct cdev m_cdev`: Defines the character device, linking /dev/m_led to the code’s functions.
- `void __iomem *gpio_base`: Stores the virtual address of GPIO registers after mapping with ioremap.
- `char *buffer`: A pointer to the memory buffer for storing “on”/“off”.
- `static struct m_led_dev mdev`: Creates one instance of the structure, shared across the module.

```c
static void gpio_set_output(void __iomem *gpfsel2) {
    u32 val = readl(gpfsel2);
    val &= ~(7 << FSEL27_SHIFT);      // Clear FSEL27 bits
    val |= (OUTPUT << FSEL27_SHIFT);  // Set FSEL27 to output (001)
    writel(val, gpfsel2);
}
```
- `void __iomem *gpfsel2`: The virtual address of the GPFSEL2 register (0xFE200008 mapped by ioremap).
- `u32 val = readl(gpfsel2)`: Reads the current 32-bit value of GPFSEL2 (like checking the current switch settings).
- `val &= ~(7 << FSEL27_SHIFT)`: Clears bits 23–21 (FSEL27) by using a bitwise AND with ~ (not) and 7 (binary 111 shifted left by 21). This resets the mode without affecting other GPIOs.
- `val |= (OUTPUT << FSEL27_SHIFT)`: Sets bits 23–21 to 001 (output) using bitwise OR. OUTPUT = 0x1 is shifted left by 21.
- `writel(val, gpfsel2)`: Writes the updated value back to GPFSEL2.

GPFSEL2 at `0xFE200008` (Revision 4, Chapter 5, Section 5.2) controls GPIO20–29’s modes.

```c
static void gpio_set(void __iomem *gpset0) {
    writel(1 << GPIO_27, gpset0);     // Set GPIO27 high (LED on)
}
```
A high voltage on PIN13 lights the external LED (anode on PIN13, cathode to ground).
- `void __iomem *gpset0`: Virtual address of GPSET0 (0xFE20001C).
- `1 << GPIO_27`: Creates a 32-bit value with only bit 27 set (binary 1 << 27 = 0x08000000). GPIO_27 = 27.
- `writel(1 << GPIO_27, gpset0)`: Writes this value to GPSET0, setting GPIO27 high.

```c
static void gpio_clear(void __iomem *gpclr0) {
    writel(1 << GPIO_27, gpclr0);     // Clear GPIO27 low (LED off)
}
```
- `void __iomem *gpclr0`: Virtual address of GPCLR0 (0xFE200028).
- `writel(1 << GPIO_27, gpclr0)`: Sets bit 27 in GPCLR0 to clear GPIO27.

```c
static ssize_t m_write(struct file *filp, const char __user *user_buf, size_t size, loff_t *offset) {
    size_t to_write = min(size, (size_t)BUFFER_SIZE - 1);
    
    // Clear buffer
    memset(mdev.buffer, 0, BUFFER_SIZE);
    
    // Copy user data
    if (copy_from_user(mdev.buffer, user_buf, to_write)) {
        pr_err("Failed to copy data from user\n");
        return -EFAULT;
    }
    
    // Null-terminate buffer
    mdev.buffer[to_write] = '\0';
    
    pr_info("Received: %s\n", mdev.buffer);
    
    // Toggle LED based on input
    if (strncmp(mdev.buffer, "on", 2) == 0) {
        gpio_set(mdev.gpio_base + GPSET0);
        pr_info("External LED turned ON\n");
    } else if (strncmp(mdev.buffer, "off", 3) == 0) {
        gpio_clear(mdev.gpio_base + GPCLR0);
        pr_info("External LED turned OFF\n");
    } else {
        pr_info("Invalid command (use 'on' or 'off')\n");
    }
    
    return to_write;
}
```
Handles writes to /dev/m_led (e.g., echo "on" > /dev/m_led). Reads the user’s command (“on” or “off”) and toggles the LED accordingly.
- `size_t to_write = min(size, (size_t)BUFFER_SIZE - 1)`: Limits input to 9 bytes (`BUFFER_SIZE` = 10, minus 1 for null terminator) to avoid overflow.
- `memset(mdev.buffer, 0, BUFFER_SIZE)`: Clears the buffer (like erasing the notepad).
- `copy_from_user(mdev.buffer, user_buf, to_write)`: Copies the user’s input (e.g., “on”) from user space to the kernel’s buffer. Returns non-zero if it fails.
- `mdev.buffer[to_write] = '\0'`: Adds a null terminator to make the input a valid C string.
- `pr_info("Received: %s\n", mdev.buffer)`: Logs the input (e.g., “Received: on”).
- `strncmp(mdev.buffer, "on", 2)`: Checks if the input starts with “on”. If true, calls gpio_set to turn the LED on.
- `strncmp(mdev.buffer, "off", 3)`: Checks for “off”. If true, calls gpio_clear.
- `else`: Logs “Invalid command” for other inputs (e.g., “hello”).
- `return to_write`: Returns the number of bytes processed (success).

```c
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = m_open,
    .release = m_release,
    .write = m_write,
};
```
- `.owner = THIS_MODULE`: Links the device to this module (prevents unloading while in use).
- `.open = m_open`: Calls m_open when opening /dev/m_led.
- `.release = m_release`: Calls m_release when closing.
- `.write = m_write`: Calls m_write when writing (e.g., echo "on" > /dev/m_led).

```c
static int __init led_init(void) {
    // Allocate character device region
    if (alloc_chrdev_region(&mdev.dev_num, 0, 1, "m_led") < 0) {
        pr_err("Failed to allocate chrdev region\n");
        return -1;
    }
    pr_info("Major = %d Minor = %d\n", MAJOR(mdev.dev_num), MINOR(mdev.dev_num));

    // Create class
    if ((mdev.m_class = class_create("m_led")) == NULL) {
        pr_err("Cannot create class\n");
        goto rm_device_numb;
    }

    // Create device
    if (device_create(mdev.m_class, NULL, mdev.dev_num, NULL, "m_led") == NULL) {
        pr_err("Cannot create device\n");
        goto rm_class;
    }

    // Initialize cdev
    cdev_init(&mdev.m_cdev, &fops);
    if (cdev_add(&mdev.m_cdev, mdev.dev_num, 1) < 0) {
        pr_err("Cannot add device to system\n");
        goto rm_device;
    }

    // Allocate buffer for user input
    mdev.buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!mdev.buffer) {
        pr_err("Failed to allocate buffer\n");
        goto rm_cdev;
    }

    // Map GPIO registers
    mdev.gpio_base = ioremap(GPIO_BASE, 0x100);
    if (!mdev.gpio_base) {
        pr_err("Failed to map GPIO registers\n");
        goto rm_buffer;
    }

    // Configure GPIO27 as output
    gpio_set_output(mdev.gpio_base + GPFSEL2);

    // Turn external LED on
    gpio_set(mdev.gpio_base + GPSET0);
    pr_info("LED driver loaded, external LED ON\n");

    return 0;

rm_buffer:
    kfree(mdev.buffer);
rm_cdev:
    cdev_del(&mdev.m_cdev);
rm_device:
    device_destroy(mdev.m_class, mdev.dev_num);
rm_class:
    class_destroy(mdev.m_class);
rm_device_numb:
    unregister_chrdev_region(mdev.dev_num, 1);
    return -1;
}
```
- Character Device Setup:
    - `alloc_chrdev_region(&mdev.dev_num, 0, 1, "m_led")`: Requests a unique ID (major/minor number) for `/dev/m_led`. Fails if ID is unavailable.
    - `pr_info("Major = %d Minor = %d\n", ...)`: Logs the ID (e.g., “Major = 245 Minor = 0”).
    - `class_create("m_led")`: Creates a device class, like a category for `/dev/m_led`.
    - `device_create(...)`: Creates `/dev/m_led`, making it accessible to users.
    - `cdev_init(&mdev.m_cdev, &fops)`: Links the device to the file operations (fops).
    - `cdev_add(...)`: Registers the device with the kernel.
- Buffer Allocation:
    - `kmalloc(BUFFER_SIZE, GFP_KERNEL)`: Allocates 10 bytes for storing “on”/“off”. GFP_KERNEL means “normal kernel memory”.
GPIO Setup:
    - `ioremap(GPIO_BASE, 0x100)`: Maps 256 bytes of physical memory starting at `0xFE200000` (GPIO base) into a virtual address the kernel can safely access. Returns a pointer (`mdev.gpio_base`).
    - `gpio_set_output(mdev.gpio_base + GPFSEL2)`: Configures GPIO27 as output.
    - `gpio_set(mdev.gpio_base + GPSET0)`: Turns the LED on (fulfills insmod requirement).
- Error Handling:
    - If any step fails (e.g., ioremap returns NULL), the code jumps to labels (rm_buffer, rm_cdev, etc.) to clean up (free memory, remove device).
    - `return 0`: Success (module loaded).
    - `return -1`: Failure (module fails to load).

```c
static void __exit led_exit(void) {
    // Turn external LED off
    gpio_clear(mdev.gpio_base + GPCLR0);

    // Unmap GPIO registers
    iounmap(mdev.gpio_base);

    // Free buffer
    kfree(mdev.buffer);

    // Clean up character device
    cdev_del(&mdev.m_cdev);
    device_destroy(mdev.m_class, mdev.dev_num);
    class_destroy(mdev.m_class);
    unregister_chrdev_region(mdev.dev_num, 1);

    pr_info("LED driver unloaded, external LED OFF\n");
}
```
- `gpio_clear(...)`: Turns the LED off (fulfills rmmod requirement).
- `iounmap(mdev.gpio_base)`: Releases the mapped GPIO memory.
- `kfree(mdev.buffer)`: Frees the buffer.
- `cdev_del, device_destroy, class_destroy, unregister_chrdev_region`: Remove the character device and its ID.
- `pr_info`: Logs “LED driver unloaded, external LED OFF”.

## Step 6: Create the Makefile

```Makefile
# GPIO LED control Pi4

EXTRA_CFLAGS = -Wall
obj-m = led.o

KDIR = /lib/modules/uname -r/build

all:
	make -C $(KDIR) M=pwd modules

clean:
	make -C $(KDIR) M=pwd clean
```

## Step 7: Cross-Compile for Raspberry Pi 4 (Yocto)

