# TensorFlow Lite Micro Issues

See [new tf-lite micro integration issues](https://github.com/tensorflow/tflite-micro/issues/483)


# Ways to resolve it

1. Pass the C/CXX flag (`-fno-rtti -fno-exceptions`)
2. Manually comment out various "private" code inside tensorflow-lite-micro files:
- `tensorflow/lite/micro/tflite_bridge/micro_error_reporter.cpp:30`:
```CXX
namespace tflite {
ErrorReporter* GetMicroErrorReporter() {
  // if (error_reporter_ == nullptr) {
  //   error_reporter_ = new (micro_error_reporter_buffer) MicroErrorReporter();
  // }
  return error_reporter_;
}
```
- `tensorflow/lite/micro/micro_allocator.cpp:84`:
```CXX
      // memory_planner = new (memory_planner_buffer) LinearMemoryPlanner();
```
- `tensorflow/lite/micro/micro_allocator.cpp:90`:
```CXX
      // memory_planner = new (memory_planner_buffer) GreedyMemoryPlanner();
```
- `tensorflow/lite/micro/micro_allocator.cpp:451`:
```CXX
  if (memory_planner_type == MemoryPlannerType::kGreedy) {
    memory_planner_buffer =
        persistent_buffer_allocator->AllocatePersistentBuffer(
            sizeof(GreedyMemoryPlanner), alignof(GreedyMemoryPlanner));
    // memory_planner = new (memory_planner_buffer) GreedyMemoryPlanner();
  } else if (memory_planner_type == MemoryPlannerType::kLinear) {
    memory_planner_buffer =
        persistent_buffer_allocator->AllocatePersistentBuffer(
            sizeof(LinearMemoryPlanner), alignof(LinearMemoryPlanner));
    // memory_planner = new (memory_planner_buffer) LinearMemoryPlanner();
  }
```
- `tensorflow/lite/micro/recording_micro_allocator.cpp:55`:
```CXX
RecordingMicroAllocator* RecordingMicroAllocator::Create(uint8_t* tensor_arena,
                                                         size_t arena_size) {
  // RecordingSingleArenaBufferAllocator* simple_memory_allocator =
  //     RecordingSingleArenaBufferAllocator::Create(tensor_arena, arena_size);
  // TFLITE_DCHECK(simple_memory_allocator != nullptr);

  // uint8_t* memory_planner_buffer =
  //     simple_memory_allocator->AllocatePersistentBuffer(
  //         sizeof(GreedyMemoryPlanner), alignof(GreedyMemoryPlanner));
  // GreedyMemoryPlanner* memory_planner =
  //     new (memory_planner_buffer) GreedyMemoryPlanner();

  // uint8_t* allocator_buffer = simple_memory_allocator->AllocatePersistentBuffer(
  //     sizeof(RecordingMicroAllocator), alignof(RecordingMicroAllocator));
  // RecordingMicroAllocator* allocator = new (allocator_buffer)
  //     RecordingMicroAllocator(simple_memory_allocator, memory_planner);
  // return allocator;
  return nullptr;
}
```

## Concerns
- MicroAllocator might be used in TensorFlowLite Micro during normal operations so this may not be a true fix
