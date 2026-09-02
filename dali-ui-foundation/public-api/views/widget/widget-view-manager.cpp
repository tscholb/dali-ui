/*
 * Samsung API
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.1 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// CLASS HEADER
#include <dali-ui-foundation/public-api/views/widget/widget-view-manager.h>

// EXTERNAL INCLUDES

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/views/widget/widget-view-manager-impl.h>

namespace Dali
{

namespace Ui
{

WidgetViewManager WidgetViewManager::New(Application application, const std::string& appId)
{
  Internal::WidgetViewManagerPtr internal = Internal::WidgetViewManager::New(application, appId);

  return WidgetViewManager(internal.Get());
}

WidgetViewManager WidgetViewManager::DownCast(BaseHandle handle)
{
  return WidgetViewManager(dynamic_cast<Internal::WidgetViewManager*>(handle.GetObjectPtr()));
}

WidgetViewManager::WidgetViewManager()
{
}

WidgetViewManager::WidgetViewManager(const WidgetViewManager& handle)
: BaseHandle(handle)
{
}

WidgetViewManager& WidgetViewManager::operator=(const WidgetViewManager& handle)
{
  BaseHandle::operator=(handle);
  return *this;
}

WidgetViewManager::~WidgetViewManager()
{
}

WidgetViewManager::WidgetViewManager(Internal::WidgetViewManager* implementation)
: BaseHandle(implementation)
{
}

WidgetView WidgetViewManager::AddWidget(const std::string& widgetId, const std::string& contentInfo, int width, int height, float updatePeriod)
{
  return GetImplementation(*this).AddWidget(widgetId, contentInfo, width, height, updatePeriod);
}

bool WidgetViewManager::RemoveWidget(WidgetView widgetView)
{
  return GetImplementation(*this).RemoveWidget(widgetView);
}

} // namespace Ui

} // namespace Dali
