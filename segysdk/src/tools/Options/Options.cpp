/*
Copyright 2025 Schlumberger

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

#include "tools/Options/Options.h"

#include <boost/program_options.hpp>

#include <sstream>

namespace tool
{

namespace po = boost::program_options;

class ProgramOptionsStore
{
public:
    ProgramOptionsStore(int argc, char* argv[], const segysdk::utilities::StringPairs& customOptions);
    std::string getOptionValue(const std::string& name) const;
    std::string getOptionsDescription() const;

private:
    po::options_description description_;
    po::variables_map options_;
};

ProgramOptions::~ProgramOptions() = default;

ProgramOptions::ProgramOptions(int argc, char* argv[], const segysdk::utilities::StringPairs& customOptions)
  : store_{std::make_unique<ProgramOptionsStore>(argc, argv, customOptions)}
{
}

static auto
makeOptionValue()
{
    return po::value<std::string>()->default_value("");
}

static auto
getDescription(const segysdk::utilities::StringPairs& customOptions)
{
    auto result = po::options_description{"supported options"};
    result.add_options()                                                                 //
        (keywords::configFile, makeOptionValue(), "// SEG-Y configuration file")         //
        (keywords::seismicTraceDataId, makeOptionValue(), "// or Seismic Trace Data ID") //
        (keywords::workProductId, makeOptionValue(), "// or Work Product ID")            //
        (keywords::sdServiceUrl, makeOptionValue(), "// Seismic drive service URL")      //
        (keywords::verbosity, makeOptionValue(), "// 0: Info, default: Debug")           //
        (keywords::correlationId, makeOptionValue());                                    //
    for (const auto& [name, description] : customOptions) {
        result.add_options()(name.c_str(), makeOptionValue(), description.c_str());
    }
    return result;
}

static auto
getProgramOptions(const int argc, char* argv[], const po::options_description& description)
{
    auto result = po::variables_map{};
    po::store(po::command_line_parser(argc, argv).options(description).allow_unregistered().run(), result);
    po::notify(result);
    return result;
}

ProgramOptionsStore::ProgramOptionsStore(const int argc, char* argv[], const segysdk::utilities::StringPairs& customOptions)
  : description_{getDescription(customOptions)},
    options_{getProgramOptions(argc, argv, description_)}
{
}

tool::Params
ProgramOptions::getBasicParams() const
{
    return tool::Params{}
        .withCorrelationId(getValue(keywords::correlationId))
        .withSdUrl(getValue(keywords::sdServiceUrl))
        .withSeismicTraceDataId(getValue(keywords::seismicTraceDataId))
        .withWorkProductId(getValue(keywords::workProductId))
        .withConfigFile(getValue(keywords::configFile))
        .withVerbosity(getValue(keywords::verbosity));
}

std::string
ProgramOptions::getValue(const std::string& optionName) const
{
    return store_->getOptionValue(optionName);
}

std::string
ProgramOptionsStore::getOptionValue(const std::string& name) const
{
    return options_[name].as<std::string>();
}

std::string
ProgramOptions::getDescription() const
{
    return store_->getOptionsDescription();
}

std::string
ProgramOptionsStore::getOptionsDescription() const
{
    std::stringstream result;
    result << description_;
    return result.str();
}

} // namespace tool
