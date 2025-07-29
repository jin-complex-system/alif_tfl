#ifndef INFERENCE_MODEL_H
#define INFERENCE_MODEL_H
#ifdef USE_TENSORFLOW

#include <inference_definitions.h>
#include <RTE_Components.h>                 /* For CPU related defintiions */
#include CMSIS_device_header

#if CPU_ID == 2 // HP core
#include "inference_model_HP.h"
#endif // CPU_NAME == 2 // HP core

#if CPU_ID == 3 // HE core
#include "inference_model_HE.h"
#endif // CPU_ID == 3 // HE core

#endif // USE_TENSORFLOW
#endif // INFERENCE_MODEL_H