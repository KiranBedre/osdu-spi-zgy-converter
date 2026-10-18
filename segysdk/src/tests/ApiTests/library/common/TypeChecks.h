/*
Copyright 2023 Schlumberger

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include <memory>
#include <vector>

namespace segysdk::api::tests
{
template<class T>
struct is_unique_ptr : std::false_type
{};

template<class T, class D>
struct is_unique_ptr<std::unique_ptr<T, D>> : std::true_type
{};

template<class T>
struct is_shared_ptr : std::false_type
{};

template<class T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type
{};

template<typename>
struct is_std_vector : std::false_type
{};

template<typename T, typename A>
struct is_std_vector<std::vector<T, A>> : std::true_type
{
};
}