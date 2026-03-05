Here is the **complete `README.md` file**. You can **copy this directly into a file named `README.md` in your project root**.

```markdown
# Virtual Multi-Device Temperature Character Driver (Linux Kernel)

A professional Linux Kernel Character Driver that simulates multiple temperature sensors.  
This project demonstrates the full lifecycle of a production-style Linux device driver including device registration, per-device isolation, timers, blocking I/O, IOCTL control interface, and safe resource cleanup.

The driver creates multiple virtual temperature devices that periodically update their values using kernel timers and expose them through `/dev` nodes.

---

# Project Architecture

The driver follows a **multi-stage system design approach** similar to how production drivers are developed.

```

Userspace
│
│ read / ioctl
▼
Character Device Driver
│
├── Device Registration Layer
├── Per-Device State Management
├── Kernel Timer Engine
├── Blocking Read Mechanism
├── IOCTL Command Interface
└── Resource Cleanup Layer

```

Each device behaves like an independent temperature sensor.

Example devices created:

```

/dev/driver_002_0
/dev/driver_002_1

```

---

# Features

- Multi-device character driver  
- Automatic `/dev` node creation using `device_create()`  
- Per-device isolated data structures  
- Periodic sensor update using Linux Kernel Timers  
- Blocking read using wait queues  
- IOCTL interface for userspace control  
- Robust error handling and safe cleanup  

---

# Development Stages

The driver is implemented using a **six stage architecture** to simulate real kernel driver development.

---

# Stage 01 — Device Registration Layer

## Goal

Establish the foundation of the character driver.

### Responsibilities

- Allocate major and minor numbers  
- Support multiple device instances  
- Automatically create `/dev` entries  
- Implement proper kernel error handling  

### Key Kernel APIs Used

```

alloc_chrdev_region()
cdev_init()
cdev_add()
class_create()
device_create()

```

### Result

Multiple character devices appear automatically:

```

/dev/driver_002_0
/dev/driver_002_1

````

---

# Stage 02 — Per-Device State Isolation

## Goal

Move from a single global device model to **multiple isolated device instances**.

### Responsibilities

- Create a per-device structure  
- Allocate an array of device objects  
- Map minor numbers to device instances  
- Store device pointer inside `file->private_data`  
- Remove shared global state  

### Example Device Structure

```c
struct driver_002_dev {
    int device_id;
    int temperature;

    struct cdev cdev;
    struct device *device;

    struct timer_list timer;
};
````

### Result

Each device behaves independently.

```
driver_002_0 -> temperature sensor #0
driver_002_1 -> temperature sensor #1
```

---

# Stage 03 — Timer Integration

## Goal

Convert the driver from a **passive data holder** to an **active time-driven device**.

### Responsibilities

* Add a kernel timer per device
* Periodically update temperature values
* Simulate real sensor updates

### Key APIs

```
timer_setup()
mod_timer()
del_timer_sync()
```

### Behavior

Every few seconds the driver updates the temperature value automatically.

Example kernel log:

```
driver_002_0: Temp updated to 27
driver_002_1: Temp updated to 28
```

---

# Stage 04 — Blocking Read

## Goal

Convert the driver from **polling-based** to **event-driven**.

### Responsibilities

* Allow user programs to wait for new sensor data
* Implement blocking read operations
* Use kernel wait queues to wake sleeping processes

### Key Concepts

```
wait_queue_head_t
wait_event_interruptible()
wake_up_interruptible()
```

### Result

Userspace applications no longer need to continuously poll the device.

Example:

```
cat /dev/driver_002_0
```

The process sleeps until new data becomes available.

---

# Stage 05 — IOCTL Interface

## Goal

Allow **controlled commands from userspace to the driver**.

### Responsibilities

* Define IOCTL commands
* Implement `unlocked_ioctl()` handler
* Allow runtime driver configuration

### Example Commands

```
RESET_TEMPERATURE
SET_UPDATE_INTERVAL
GET_DEVICE_INFO
```

### Key APIs

```
unlocked_ioctl()
copy_to_user()
copy_from_user()
```

### Example Usage

```c
ioctl(fd, RESET_TEMPERATURE);
```

This enables **two-way communication between user space and kernel space**.

---

# Stage 06 — Robust Cleanup

## Goal

Ensure the driver **never leaks resources and handles failures safely**.

### Responsibilities

* Stop all timers
* Destroy device nodes
* Remove class objects
* Delete character devices
* Release allocated device numbers

### Cleanup APIs

```
del_timer_sync()
device_destroy()
class_destroy()
cdev_del()
unregister_chrdev_region()
```

### Result

Driver unloads safely using:

```
sudo rmmod driver_002
```

---

# Build Instructions

## 1. Compile Driver

```
make
```

This generates:

```
driver_002.ko
```

---

## 2. Insert Driver

```
sudo insmod driver_002.ko
```

Check kernel logs:

```
dmesg
```

---

## 3. Verify Device Nodes

```
ls /dev/driver_002*
```

Example output:

```
/dev/driver_002_0
/dev/driver_002_1
```

---

## 4. Test Driver

```
cat /dev/driver_002_0
```

Example output:

```
Temp: 27
```

---

## 5. Remove Driver

```
sudo rmmod driver_002
```

---

# Skills Demonstrated

* Linux Kernel Development
* Character Device Driver Design
* Kernel Timers
* Blocking I/O and Wait Queues
* IOCTL Interface Design
* Kernel Resource Management
* Multi-device Driver Architecture

---

# Learning Outcomes

This project demonstrates how to build a **production-style Linux device driver** using best practices followed in embedded Linux systems.

The driver simulates a **multi-sensor temperature device** and showcases important kernel mechanisms used in real hardware drivers.

---

# Author

**Adarsh Meshram**

Embedded Systems & Embedded Linux Engineering

```

---

✅ After saving it:

```

project_009/
│
├── driver_002.c
├── Makefile
└── README.md

```

Then push to GitHub and your project will look **very professional for Embedded Linux roles**.

---

If you want, I can also show you **one small extra feature (Stage 07) that makes this driver look like a real industry hardware driver** and **impress kernel engineers in interviews**. 🚀
```
