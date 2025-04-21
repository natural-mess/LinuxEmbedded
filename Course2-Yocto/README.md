# Yocto

## What is Yocto?
The Yocto Project is an open source collaboration project that helps developers create custom Linux-based systems for embedded products and other targeted environments, regardless of the hardware architecture. The project provides a flexible set of tools and a space where embedded developers worldwide can share technologies, software stacks, configurations and best practices which can be used to create tailored Linux and RTOS images for embedded devices.

Yocto is like a custom kitchen where you can build your own Linux operating system (OS) for devices like the Raspberry Pi 4. Instead of using a pre-made OS like Raspberry Pi OS, Yocto lets you pick exactly what goes into your Linux—only the ingredients you need for your project, like GPIO tools for controlling pins. It’s perfect for embedded devices (small computers like the Raspberry Pi) because it makes your OS lightweight and tailored.

Yocto is a set of tools and recipes that helps you create a custom Linux operating system (OS) for embedded devices, like the Raspberry Pi. Think of it as a kitchen where you pick ingredients (software components) and cook them together to make a tailored Linux system that fits your device perfectly.

### Why use Yocto?
- It lets you build a lightweight Linux OS with only the software you need (e.g., for GPIO control on Raspberry Pi).
- It’s flexible and widely used in embedded systems (robots, IoT devices, cars, etc.).
- It supports many hardware platforms, including Raspberry Pi 4.

Yocto doesn’t give you a ready-made OS. Instead, it provides tools to build your own OS from scratch, customized for your project.

---

## Key Concepts in Yocto
### 1. Poky:
- Poky is like a starter recipe book. It’s an example of how to build a small Linux OS.
- It includes the tools and instructions to get you started, but you can change it to fit your needs.
- Think of Poky as a sample cake recipe—you can follow it exactly or add your own flavors (like GPIO tools).

### 2. Recipes (.bb files):
- Recipes are like cooking instructions for one piece of software (e.g., a GPIO library, Python, or a web browser).
- Each recipe says: “Download this software, build it, and add it to the OS.”
- Example: A recipe for libgpiod tells BitBake how to add a GPIO control tool to your OS.

### 3. Layers:
- Layers are like folders that organize recipes and settings.
- Each layer has a specific job, like supporting the Raspberry Pi or adding Python tools.
- Example: The meta-raspberrypi layer has recipes and settings for the Raspberry Pi 4.

### 4. BitBake:
- BitBake is the chef who follows the recipes to cook your Linux OS.
- It reads instructions, gathers ingredients (software), and builds everything step-by-step.
- You tell BitBake, “Make me a Linux OS,” and it does the hard work.

### 5. Image:
- The image is the final dish—the complete Linux OS you built.
- It’s a file you put on an SD card to run on your Raspberry Pi 4.

### 6. Configuration Files:
- These are like notes that tell BitBake what you’re building (e.g., “This is for a Raspberry Pi 4”).
- They let you customize things, like saying, “Add GPIO tools and skip the web browser.”

### 7. OpenEmbedded-Core (oe-core):
- This is a core ingredient list—a collection of essential recipes that most Linux systems need (e.g., basic Linux kernel, standard tools).
- It’s shared by Yocto and other projects to keep things consistent.

### 8. Metadata:
Metadata is a fancy word for all the instructions (recipes, layers, config files) that tell Yocto how to build your OS.

---

## How These Concepts Work Together in a Project
Imagine you’re building a custom Linux OS for your Raspberry Pi 4 to control GPIO pins. Here’s how the Yocto pieces fit together, step-by-step:

### 1. Start with Poky:
- You download Poky, which gives you the basic tools and an example setup (like a starter kitchen with a recipe book).
- Poky includes BitBake and oe-core, so you’re ready to start cooking.

### 2. Add Layers:
- You grab the meta-raspberrypi layer, which is like a folder of Raspberry Pi recipes (e.g., for the Pi 4’s hardware).
- You might also add a layer for extra tools, like meta-openembedded, which has Python or GPIO libraries.
- Layers organize the instructions so BitBake knows what’s available.

### 3. Write or Use Recipes:
- You pick recipes for the software you need. For GPIO, you might choose:
    - A recipe for `libgpiod` (to control GPIO from the command line).
    - A recipe for `python3-rpi-gpio` (to control GPIO with Python).
- These recipes are stored in layers, and they tell BitBake how to download and build the software.

### 4. Set Configuration Files:
- You edit a config file (like local.conf) to say, “I’m building for a Raspberry Pi 4, and I want libgpiod in my OS.”
- This is like telling the chef, “Make a cake for a specific oven and add chocolate chips.”

### 5. BitBake Does the Work:
- You run BitBake, and it reads the layers, recipes, and config files.
- BitBake:
    - Downloads the software (e.g., libgpiod source code).
    - Compiles it for the Raspberry Pi 4.
    - Puts everything together into a final image.
- It’s like the chef following the recipe, baking the cake, and decorating it.

### 6. Get the Image:
- BitBake creates an image file (your custom Linux OS).
- You flash this image to an SD card and boot it on your Raspberry Pi 4.
- Now your Pi has a Linux OS with GPIO tools ready to use!

### 7. Use Metadata:
- All the recipes, layers, and config files are “metadata.” They’re the instructions BitBake uses to build your OS.
- If you want to change something (e.g., add Wi-Fi support), you update the metadata, and BitBake rebuilds the image.

### 8. OpenEmbedded-Core Helps:
- oe-core provides standard recipes (like the Linux kernel) that meta-raspberrypi builds on.
- It ensures your OS has the basics, while layers like meta-raspberrypi add Pi-specific stuff.

---

## What is devtool in Yocto?
devtool is a command-line tool in Yocto that makes it easier to develop and test software (like your C program for controlling an LED) for your custom Linux OS. Think of devtool as a helper that simplifies adding your own code to a Yocto project, building it, and including it in the OS image for your Raspberry Pi 4.

### What does devtool do?
- Creates recipes: It automatically generates a Yocto recipe (instructions for building your code) so your program can be included in the Linux OS.
- Sets up a workspace: It creates a folder where you can write and edit your code, and it connects your code to the Yocto build system.
- Builds and tests: It helps you compile your code for the Raspberry Pi (cross-compiling) and deploy it to the target device.
- Updates easily: If you change your code, devtool updates the recipe and rebuilds it without hassle.

### Why use devtool?
- Without devtool, you’d have to manually write Yocto recipes and configure the build system, which is tricky for beginners.
- With devtool, Yocto handles the complicated parts, letting you focus on writing your C program for the LED.

Devtool is like a cooking assistant who sets up the kitchen, writes the recipe, and helps you cook your dish (your C program) so it fits perfectly into your Yocto-built Linux OS.
