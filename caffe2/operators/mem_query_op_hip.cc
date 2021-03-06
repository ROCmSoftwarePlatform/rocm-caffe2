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
#include "caffe2/core/operator.h"

namespace caffe2 {
namespace {

class GetGPUMemoryUsageOp final : public Operator<HIPContext>
{
    public:
    GetGPUMemoryUsageOp(const OperatorDef& operator_def, Workspace* ws)
        : Operator<HIPContext>(operator_def, ws)
    {
    }
    ~GetGPUMemoryUsageOp() {}

    bool RunOnDevice() override
    {
        CHECK_EQ(InputSize(), 0);
        CHECK_EQ(OutputSize(), 1);
        std::vector<long> total_by_gpu = HIPContext::TotalMemoryByGpu();
        std::vector<long> max_by_gpu   = HIPContext::MaxMemoryByGpu();
        CHECK_EQ(total_by_gpu.size(), max_by_gpu.size());

        auto* stats = Output(0);
        stats->Resize(2, total_by_gpu.size());
        context_.Copy<long, CPUContext, HIPContext>(
            total_by_gpu.size(), total_by_gpu.data(), stats->mutable_data<long>());
        context_.Copy<long, CPUContext, HIPContext>(max_by_gpu.size(),
                                                    max_by_gpu.data(),
                                                    stats->mutable_data<long>() +
                                                        total_by_gpu.size());
        return true;
    }
};

OPERATOR_SCHEMA(GetGPUMemoryUsage)
    .NumInputs(0)
    .NumOutputs(1)
    .SetDoc(R"DOC(Fetches GPU memory stats from HIPContext. Result is stored
      in output blob with shape (2, num_gpus). First row contains the total
      current memory usage, and the second row the maximum usage during
      this execution.

      NOTE: --caffe2_gpu_memory_tracking flag must be enabled to use this op.
    )DOC");

REGISTER_HIP_OPERATOR(GetGPUMemoryUsage, GetGPUMemoryUsageOp);
}

} // namespace caffe2
