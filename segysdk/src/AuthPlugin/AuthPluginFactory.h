/*
Copyright 2022 Schlumberger

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

#include "../AuthPlugin/AuthPluginInterface.h"

#include <cstddef>
#include <map>
#include <memory>
#include <mutex>
#include <string>

namespace segysdk::auth
{

/**
 * @class AuthPluginFactory
 *
 * @brief The Factory of Authentication Plugin Interface Instances
 *
 * Provides AuthPluginInterface instance for the third party authentication plugin.
 *
 */
struct SEGYSDK_DLL_PUBLIC AuthPluginFactory final
{
    /**
     * @brief Gets instance initialized factory object (singleton)
     *
     * @return reference to AuthPluginFactory class instance
     **/
    static AuthPluginFactory& instance();

    /**
     * @brief Creates and initializes authentication object instance for the first time then reuses from the internal cache.
     *
     * @param [IN] SharedLibraryPathName specifies file path name of the third party authentication plugin shared library binary file
     * @param [IN] RefreshBeforeTokenExpirationTime specifies minimal interval till the token expiration (in seconds) the token refresh procedure has to be triggered
     *
     * @return STL shared pointer owning the authentication object instance on success, an empty shared pointer otherwise
     *
     **/
    std::shared_ptr<AuthPluginInterface>
    CreatePluginInteface(std::string SharedLibraryPathName, size_t RefreshBeforeTokenExpirationTime);

private:
    using AuthPluginKey = std::pair<std::string, size_t>;
    using AuthPluginPtr = std::shared_ptr<AuthPluginInterface>;
    using AuthPluginMap = std::map<AuthPluginKey, AuthPluginPtr>;

private:
    AuthPluginFactory();

    AuthPluginFactory(AuthPluginFactory const&) = delete;
    AuthPluginFactory& operator=(AuthPluginFactory const&) = delete;
    AuthPluginFactory(AuthPluginFactory&&) = delete;
    AuthPluginFactory& operator=(AuthPluginFactory&&) = delete;

    bool hasCachedInterface(AuthPluginKey const& key) const;
    AuthPluginPtr getCachedInterface(AuthPluginKey const& key) const;
    void addCachedInterface(AuthPluginKey key, AuthPluginPtr authPlugin);

private:
    std::mutex factoryMutex_;
    AuthPluginMap authPluginMap_;
};

} // namespace segysdk::auth
