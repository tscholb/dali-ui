/*
 * Samsung API
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.1 (the "License");
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

#include <dali-ui-foundation/internal/views/widget/widget-view-impl.h>
#include <dali-ui-foundation/internal/views/widget/widget-view-manager-impl.h>

#include <dali/integration-api/debug.h>

namespace Dali
{
namespace Ui
{
namespace Internal
{
WidgetViewManagerPtr WidgetViewManager::New(Application application, const std::string& name)
{
  Dali::WidgetEngine engine = Dali::WidgetEngine::New(name);
  if(!engine)
  {
    DALI_LOG_ERROR("WidgetViewManager::New: widget engine is unavailable.\n");
    return nullptr;
  }
  WidgetView::SetDefaultEngine(engine);
  return new WidgetViewManager(std::move(engine));
}

WidgetViewManager::WidgetViewManager(Dali::WidgetEngine engine)
: mEngine(std::move(engine))
{
}

WidgetViewManager::~WidgetViewManager()
{
  mWidgetViews.clear();
  // Do not clear a newer manager's default engine when managers overlap.
  WidgetView::ClearDefaultEngine(mEngine);
}

Dali::Ui::WidgetView WidgetViewManager::AddWidget(const std::string& widgetId, const std::string& contentInfo, int width, int height, float updatePeriod)
{
  Dali::WidgetEngineInstance instance = mEngine.CreateWidget(widgetId, contentInfo, width, height, updatePeriod);
  Dali::Ui::WidgetView       view     = WidgetView::New(std::move(instance), width, height);
  if(view)
  {
    WidgetView& implementation = GetImplementation(view);
    implementation.WidgetTerminatedSignal().Connect(this, &WidgetViewManager::OnTerminatedWidget);
    mWidgetViews.emplace(implementation.GetInstanceId(), view);
  }
  return view;
}

bool WidgetViewManager::RemoveWidget(Dali::Ui::WidgetView widgetView)
{
  if(!widgetView)
  {
    return false;
  }
  std::string instanceId = GetImplementation(widgetView).GetInstanceId();
  bool        result     = GetImplementation(widgetView).RemoveWidget();
  mWidgetViews.erase(instanceId);
  return result;
}

void WidgetViewManager::OnTerminatedWidget(Dali::Ui::WidgetView widgetView)
{
  if(widgetView)
  {
    mWidgetViews.erase(GetImplementation(widgetView).GetInstanceId());
  }
}
} // namespace Internal
} // namespace Ui
} // namespace Dali
