/**
 * Copyright (c) 2016-present, Facebook, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "caffe2/core/context_hip.h"
#include "caffe2/operators/elementwise_op.h"
#include "hip/hip_runtime.h"

namespace caffe2 {

template <typename T>
__global__ void NegativeKernel(const int N, const T* x, T* y)
{
    HIP_1D_KERNEL_LOOP(i, N) { y[i] = -x[i]; }
}

struct NegativeCUDAFunctor
{
    template <typename T>
    inline void operator()(const int n, const T* x, T* y, HIPContext* device_context)
    {
        hipLaunchKernelGGL((NegativeKernel<T>),
                           dim3(CAFFE_GET_BLOCKS(n)),
                           dim3(CAFFE_HIP_NUM_THREADS),
                           0,
                           device_context->hip_stream(),
                           n,
                           x,
                           y);
        return;
    }
};

REGISTER_HIP_OPERATOR(
    Negative,
    UnaryElementwiseOp<TensorTypes<float, double, int, long>, HIPContext, NegativeCUDAFunctor>);
} // namespace caffe2
