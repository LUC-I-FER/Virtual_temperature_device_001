# stage 01 -> Device Registration Layer

Goal: Major/minor allocation Multiple minors Auto /dev creation Proper error handling

# stage 02 -> Per-Device State Isolation

Goal: Create a pre-device structure allocate an array of device instance, Map minor number -> device structure, Store device pointer in file->private_data, remove global shared data

# stage 03 -> Timer Integration

Goal: A passive data holder to An active, time-driven device

# stage 04 -> Blocking Read
# stage 05 -> IOCTL Interface
# stage 06 -> Robust Cleanup