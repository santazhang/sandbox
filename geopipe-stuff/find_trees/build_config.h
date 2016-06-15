#pragma once

// #define USE_LIBSVM_INSTEAD_OF_CAFFE


#ifdef USE_LIBSVM_INSTEAD_OF_CAFFE

#include <iostream>

#else

#ifndef CPU_ONLY
#define CPU_ONLY
#endif  // CPU_ONLY

#endif  // #ifdef USE_LIBSVM_INSTEAD_OF_CAFFE
