# TensorFlow Lite Micro Issues

See [new tf-lite micro integration issues](https://github.com/tensorflow/tflite-micro/issues/483)


# Ways to resolve it

1. Pass the C/CXX flag (`-fno-rtti -fno-exceptions`)
2. Manually turn the macro `TF_LITE_REMOVE_VIRTUAL_DELETE` into public from private inside tensorflow-lite-micro files:
- `tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h:32`:
```CXX
 private:
 public:
  TF_LITE_REMOVE_VIRTUAL_DELETE
```
- `tensorflow/lite/micro/memory_planner/greedy_memory_planner.h:165`:
```CXX
public:
  TF_LITE_REMOVE_VIRTUAL_DELETE
```
- `tensorflow/lite/micro/memory_planner/linear_memory_planner.h:48`:
```CXX
public:
  TF_LITE_REMOVE_VIRTUAL_DELETE
```

## Concerns
- MicroAllocator might be used in TensorFlowLite Micro during normal operations so this may not be a true fix
