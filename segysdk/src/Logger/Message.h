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

#include "../segysdk_details/DLL_EXPORT.h"

#include <string>

namespace segysdk::logger
{

enum class MessageType {
    Ordinary = 0,
    GSM
};

class SEGYSDK_DLL_PUBLIC Message
{
public:
    Message(std::string message);

protected:
    Message(MessageType type, std::string message);

public:
    Message(Message const& other) = default;
    Message& operator=(Message const& other) = default;
    Message(Message&& other) = default;
    Message& operator=(Message&& other) = default;

    virtual ~Message();

    virtual const std::string& message() const;

    virtual void updateMessage(std::string);

    virtual MessageType type() const;

private:
    MessageType type_;
    std::string message_;
};

} // namespace segysdk::logger
