#ifndef INFERENCE_MODEL_HP_H
#define INFERENCE_MODEL_HP_H
#ifdef USE_TENSORFLOW

#ifdef USE_ORBIWISE
#define NUM_CLASSES                 21  // Orbiwise class_id is somehow bigger than 19 actual classes

#ifdef USE_NPU_MODEL

#ifdef USE_MODELS_DSP_PREPROCESSING
#include <models_dsp_preprocessing/model_orbw_19_Q_HP_vela.h>
#else
#include <models_librosa/model_orbw_19_Q_HP_vela.h>
#endif // USE_MODELS_DSP_PREPROCESSING

#else // NOT USE_NPU_MODEL

#ifdef USE_MODELS_DSP_PREPROCESSING
#include <models_dsp_preprocessing/model_orbw_19_Q.h>
#else
#include <models_librosa/model_orbw_19_Q.h>
#endif // USE_MODELS_DSP_PREPROCESSING

#endif // USE_NPU_MODEL

#else // NOT USE_ORBIWISE
#define NUM_CLASSES                 10

#ifdef USE_NPU_MODEL

#ifdef USE_MODELS_DSP_PREPROCESSING
#include <models_dsp_preprocessing/model_us_Q_HP_vela.h>
#else
#include <models_librosa/model_us_Q_HP_vela.h>
#endif // USE_MODELS_DSP_PREPROCESSING

#else // NOT USE_NPU_MODEL

#ifdef USE_MODELS_DSP_PREPROCESSING
#include <models_dsp_preprocessing/model_us_Q.h>
#else
#include <models_librosa/model_us_Q.h>
#endif // USE_MODELS_DSP_PREPROCESSING

#endif // USE_NPU_MODEL

#endif // USE_ORBIWISE

#endif // USE_TENSORFLOW
#endif // INFERENCE_MODEL_HP_H