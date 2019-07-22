/*******************************************************************************
* Copyright 2019 Intel Corporation
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
*******************************************************************************/

#ifndef SYCL_UTILS_HPP
#define SYCL_UTILS_HPP

#include "common/c_types_map.hpp"

#include <CL/sycl.hpp>
#include <vector>

namespace mkldnn {
namespace impl {
namespace sycl {

using buffer_u8_t = cl::sycl::buffer<uint8_t, 1>;

inline std::vector<cl::sycl::device> get_sycl_devices(
        engine_kind_t engine_kind) {
    std::vector<cl::sycl::device> devices;
    auto all_platforms = cl::sycl::platform::get_platforms();
    for (auto &plat : all_platforms) {
        auto dev_type = (engine_kind == engine_kind::cpu)
                ? cl::sycl::info::device_type::cpu
                : cl::sycl::info::device_type::gpu;
        auto devs = plat.get_devices(dev_type);
        if (devs.empty())
            continue;
        devices.insert(devices.end(), devs.begin(), devs.end());
    }
    return devices;
}

inline void copy_from_buffer(buffer_u8_t &src_buf, void *dst_ptr, size_t size) {
    constexpr auto read_mode = cl::sycl::access::mode::read;

    uint8_t *dst_u8_ptr = static_cast<uint8_t *>(dst_ptr);
    auto sycl_host_acc = src_buf.get_access<read_mode>();
    for (size_t i = 0; i < size; i++) {
        dst_u8_ptr[i] = sycl_host_acc[i];
    }
}

inline void copy_to_buffer(void *src_ptr, buffer_u8_t &dst_buf, size_t size) {
    constexpr auto write_mode = cl::sycl::access::mode::write;

    uint8_t *src_u8_ptr = static_cast<uint8_t *>(src_ptr);
    auto sycl_host_acc = dst_buf.get_access<write_mode>();
    for (size_t i = 0; i < size; i++) {
        sycl_host_acc[i] = src_u8_ptr[i];
    }
}

} // namespace sycl
} // namespace impl
} // namespace mkldnn

#endif