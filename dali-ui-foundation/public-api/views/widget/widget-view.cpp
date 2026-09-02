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
#include <dali-ui-foundation/public-api/views/widget/widget-view.h>

// INTERNAL INCLUDES
#include <dali-ui-foundation/internal/views/widget/widget-view-impl.h>

namespace DALI_NAMESPACE
{

namespace Ui
{

WidgetView WidgetView::New(const std::string& appId, const std::string& widgetId, const std::string& contentInfo, int width, int height, float updatePeriod)
{
  return Internal::WidgetView::New(appId, widgetId, contentInfo, width, height, updatePeriod);
}

WidgetView WidgetView::DownCast(BaseHandle handle)
{
  return Ui::View::DownCast<WidgetView, Internal::WidgetView>(handle);
}

WidgetView::WidgetView() = default;

WidgetView::WidgetView(const WidgetView& handle) = default;

WidgetView& WidgetView::operator=(const WidgetView& handle) = default;

WidgetView::~WidgetView() = default;

Dali::String WidgetView::GetWidgetId() const
{
  return Dali::Ui::Internal::GetImplementation(*this).GetWidgetId();
}

Dali::String WidgetView::GetInstanceId() const
{
  return Dali::Ui::Internal::GetImplementation(*this).GetInstanceId();
}

Dali::String WidgetView::GetContentInfo() const
{
  return Dali::Ui::Internal::GetImplementation(*this).GetContentInfo();
}

Dali::String WidgetView::GetTitle() const
{
  return Dali::Ui::Internal::GetImplementation(*this).GetTitle();
}

float WidgetView::GetUpdatePeriod() const
{
  return Dali::Ui::Internal::GetImplementation(*this).GetUpdatePeriod();
}

bool WidgetView::IsWidgetFaulted() const
{
  return Dali::Ui::Internal::GetImplementation(*this).IsWidgetFaulted();
}

void WidgetView::SetPreviewEnabled(bool enabled)
{
  Dali::Ui::Internal::GetImplementation(*this).SetPreviewEnabled(enabled);
}

bool WidgetView::IsPreviewEnabled() const
{
  return Dali::Ui::Internal::GetImplementation(*this).IsPreviewEnabled();
}

void WidgetView::SetLoadingTextVisible(bool visible)
{
  Dali::Ui::Internal::GetImplementation(*this).SetLoadingTextVisible(visible);
}

bool WidgetView::IsLoadingTextVisible() const
{
  return Dali::Ui::Internal::GetImplementation(*this).IsLoadingTextVisible();
}

void WidgetView::SetRetryTextVisible(bool visible)
{
  Dali::Ui::Internal::GetImplementation(*this).SetRetryTextVisible(visible);
}

bool WidgetView::IsRetryTextVisible() const
{
  return Dali::Ui::Internal::GetImplementation(*this).IsRetryTextVisible();
}

void WidgetView::SetKeepWidgetSize(bool keepWidgetSize)
{
  Dali::Ui::Internal::GetImplementation(*this).SetKeepWidgetSize(keepWidgetSize);
}

bool WidgetView::IsKeepWidgetSize() const
{
  return Dali::Ui::Internal::GetImplementation(*this).IsKeepWidgetSize();
}

void WidgetView::SetLoadingTextProperties(const Dali::Property::Map& properties)
{
  Dali::Ui::Internal::GetImplementation(*this).SetLoadingTextProperties(properties);
}

void WidgetView::SetRetryTextProperties(const Dali::Property::Map& properties)
{
  Dali::Ui::Internal::GetImplementation(*this).SetRetryTextProperties(properties);
}

void WidgetView::SetEffect(const Dali::Property::Map& effect)
{
  Dali::Ui::Internal::GetImplementation(*this).SetEffect(effect);
}

bool WidgetView::PauseWidget()
{
  return Dali::Ui::Internal::GetImplementation(*this).PauseWidget();
}

bool WidgetView::ResumeWidget()
{
  return Dali::Ui::Internal::GetImplementation(*this).ResumeWidget();
}

bool WidgetView::CancelTouchEvent()
{
  return Dali::Ui::Internal::GetImplementation(*this).CancelTouchEvent();
}

void WidgetView::ActivateFaultedWidget()
{
  return Dali::Ui::Internal::GetImplementation(*this).ActivateFaultedWidget();
}

WidgetView::WidgetViewSignalType& WidgetView::WidgetAddedSignal()
{
  return Dali::Ui::Internal::GetImplementation(*this).WidgetAddedSignal();
}

WidgetView::WidgetViewSignalType& WidgetView::WidgetDeletedSignal()
{
  return Dali::Ui::Internal::GetImplementation(*this).WidgetDeletedSignal();
}

WidgetView::WidgetViewSignalType& WidgetView::WidgetCreationAbortedSignal()
{
  return Dali::Ui::Internal::GetImplementation(*this).WidgetCreationAbortedSignal();
}

WidgetView::WidgetViewSignalType& WidgetView::WidgetContentUpdatedSignal()
{
  return Dali::Ui::Internal::GetImplementation(*this).WidgetContentUpdatedSignal();
}

WidgetView::WidgetViewSignalType& WidgetView::WidgetUpdatePeriodChangedSignal()
{
  return Dali::Ui::Internal::GetImplementation(*this).WidgetUpdatePeriodChangedSignal();
}

WidgetView::WidgetViewSignalType& WidgetView::WidgetFaultedSignal()
{
  return Dali::Ui::Internal::GetImplementation(*this).WidgetFaultedSignal();
}

WidgetView::WidgetView(Internal::WidgetView& implementation)
: View(implementation)
{
}

WidgetView::WidgetView(Dali::Internal::CustomActor* internal)
: View(internal)
{
  VerifyCustomActorPointer<Internal::WidgetView>(internal);
}

} // namespace Ui

} // namespace DALI_NAMESPACE
