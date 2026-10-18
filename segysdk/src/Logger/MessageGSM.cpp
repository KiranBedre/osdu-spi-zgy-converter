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

#include "../Logger/MessageGSM.h"

namespace segysdk::logger
{

MessageGSM::MessageGSM(std::string message)
  : Message(MessageType::GSM, std::move(message))
{
}

Json::Value
MessageGSM::getGsmMessage() const
{
    Json::Value root;

    root[StatusDetails::message] = message();

    return root;
}

ProgressMessageGSM::ProgressMessageGSM(std::string message)
  : MessageGSM(std::move(message))
{
}

ProgressMessageGSM::ProgressMessageGSM(std::string message, std::string taskName, double taskProgress)
  : MessageGSM(message), taskName_(taskName), taskProgress_(taskProgress)
{
}

std::string
ProgressMessageGSM::taskName() const
{
    return taskName_;
}

double
ProgressMessageGSM::taskProgress() const
{
    return taskProgress_;
}

Json::Value
ProgressMessageGSM::getGsmMessage() const
{
    Json::Value root;

    root[StatusDetails::message] = message();

    Json::Value properties;
    properties[StatusDetails::Properties::taskName] = taskName_;
    properties[StatusDetails::Properties::taskProgress] = taskProgress_;

    root[StatusDetails::additionalProperties] = properties;

    return root;
}

} // namespace segysdk::logger