When exec() is called:
- The current process image is completely replaced.
- A new program is loaded into memory (new code, new data, new heap, new stack).
- The PID remains the same, but the old program's memory layout is gone.
