#ifndef DALI_UI_INTERNAL_WIDGET_VIEW_MANAGER_IMPL_H
#define DALI_UI_INTERNAL_WIDGET_VIEW_MANAGER_IMPL_H

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

#include <dali-ui-foundation/public-api/views/widget/widget-view-manager.h>
#include <dali-ui-foundation/public-api/views/widget/widget-view.h>

#include <dali/devel-api/adaptor-framework/widget-engine/widget-engine.h>
#include <dali/public-api/object/base-object.h>
#include <map>

namespace Dali
{
namespace Ui
{
namespace Internal
{
class WidgetViewManager;
using WidgetViewManagerPtr = IntrusivePtr<WidgetViewManager>;

class WidgetViewManager : public BaseObject, public ConnectionTracker
{
public:
  static WidgetViewManagerPtr New(Application application, const std::string& name);
  Dali::Ui::WidgetView        AddWidget(const std::string& widgetId, const std::string& contentInfo, int width, int height, float updatePeriod);
  bool                        RemoveWidget(Dali::Ui::WidgetView widgetView);

private:
  explicit WidgetViewManager(Dali::WidgetEngine engine);
  ~WidgetViewManager() override;
  void OnTerminatedWidget(Dali::Ui::WidgetView widgetView);

  WidgetViewManager(const WidgetViewManager&)            = delete;
  WidgetViewManager& operator=(const WidgetViewManager&) = delete;

private:
  Dali::WidgetEngine                          mEngine;
  std::map<std::string, Dali::Ui::WidgetView> mWidgetViews;
};
} // namespace Internal

inline Internal::WidgetViewManager& GetImplementation(WidgetViewManager& object)
{
  DALI_ASSERT_ALWAYS(object);
  return static_cast<Internal::WidgetViewManager&>(object.GetBaseObject());
}

inline const Internal::WidgetViewManager& GetImplementation(const WidgetViewManager& object)
{
  DALI_ASSERT_ALWAYS(object);
  return static_cast<const Internal::WidgetViewManager&>(object.GetBaseObject());
}
} // namespace Ui
} // namespace Dali

#endif // DALI_UI_INTERNAL_WIDGET_VIEW_MANAGER_IMPL_H
