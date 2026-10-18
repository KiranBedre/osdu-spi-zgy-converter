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

#include "../AuthPlugin/AuthPluginFactory.h"

#include "../AuthPlugin/AuthPluginBasicImpl.h"
#include "../AuthPlugin/AuthPluginException.h"
#include "../AuthPlugin/AuthPluginLibWrapperImpl.h"
#include "../LibraryLoader/DynamicLibraryLoader.h"
#include "../Utilities/Environment.h"

namespace segysdk::auth
{

AuthPluginFactory&
AuthPluginFactory::instance()
{
    static AuthPluginFactory s_instance;
    return s_instance;
}

std::shared_ptr<AuthPluginInterface>
AuthPluginFactory::CreatePluginInteface(std::string SharedLibraryPathName, size_t RefreshBeforeTokenExpirationTime)
{
    std::lock_guard<std::mutex> lock(factoryMutex_);

    auto const key = AuthPluginKey{SharedLibraryPathName, RefreshBeforeTokenExpirationTime};

    if (hasCachedInterface(key)) {
        return getCachedInterface(key);
    }

    std::shared_ptr<AuthPluginInterface> result{nullptr};

    if (SharedLibraryPathName.length()) {
        auto libraryLoader
            = std::make_unique<segysdk::library_loader::DynamicLibraryLoader>(SharedLibraryPathName);

        if (not libraryLoader) {
            throw segysdk::auth::AuthPluginException("AuthPluginFactory: shared library loading has failed");
        }

        result = std::make_shared<segysdk::auth::AuthPluginLibWrapperImpl>(std::move(libraryLoader),
                                                                           RefreshBeforeTokenExpirationTime);
    }
    else {
        auto const givenToken = segysdk::utilities::Environment::instance().sdToken();

        result = std::make_shared<segysdk::auth::AuthPluginBasicImpl>(givenToken);
    }

    addCachedInterface(key, result);

    return result;
}

AuthPluginFactory::AuthPluginFactory()
{
    /* EMPTY */
}

bool
AuthPluginFactory::hasCachedInterface(AuthPluginKey const& key) const
{
    return authPluginMap_.end() not_eq authPluginMap_.find(key);
}

AuthPluginFactory::AuthPluginPtr
AuthPluginFactory::getCachedInterface(AuthPluginKey const& key) const
{
    return authPluginMap_.at(key);
}

void
AuthPluginFactory::addCachedInterface(AuthPluginKey key, AuthPluginPtr authPlugin)
{
    if (not hasCachedInterface(key)) {
        authPluginMap_[key] = authPlugin;
    }
}

} // namespace segysdk::auth
