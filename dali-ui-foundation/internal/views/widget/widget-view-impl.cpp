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

#include <dali-ui-foundation/integration-api/view-depth-index-ranges.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-base.h>
#include <dali-ui-foundation/integration-api/visual-factory/visual-factory.h>
#include <dali-ui-foundation/integration-api/visuals/image-visual-properties-integ.h>
#include <dali-ui-foundation/integration-api/visuals/visual-properties-integ.h>
#include <dali-ui-foundation/internal/text/property-string-parser.h>
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/configuration/ui-localization-manager.h>
#include <dali-ui-foundation/public-api/image-loader/image-url-utils.h>
#include <dali-ui-foundation/public-api/image/image-enumerations.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/adaptor-framework/widget-engine/widget-engine.h>
#include <dali/devel-api/common/singleton-service.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-bridge.h>
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-integ.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/rendering/decorated-visual-renderer.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/adaptor-framework/window.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/object/property-conditions.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <utility>
#include <vector>

#ifndef DALI_WIDGET_VIEWER_LOCALE_DIR
#define DALI_WIDGET_VIEWER_LOCALE_DIR "/usr/share/widget_viewer_dali/locale"
#endif

using Dali::Integration::ToDaliString;
using Dali::Integration::ToStdString;

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
namespace
{
constexpr int DEFAULT_FONT_PIXEL_SIZE = 30;

const char* const STATE_TEXT("stateText");
const char* const FONT_STYLE("fontStyle");
const char* const TEXT_PIXEL_SIZE("textPixelSize");
const char* const TEXT_COLOR("textColor");
const char* const TEXT_VISIBLE("textVisible");
const char* const CUSTOM_SHADER("shader");

constexpr Property::Index WIDGET_IMAGE_VISUAL_INDEX = Dali::Ui::WidgetView::Property::WIDGET_ID;

Dali::WidgetEngine GetWidgetEngine(const std::string& appId)
{
  // Keep the empty engine until application shutdown: widget-service loses
  // provider-death monitoring after fini/init in the same process.
  // This ownership policy can be removed when service reinitialization is fixed.
  auto service = SingletonService::Get();
  if(!service)
  {
    return {};
  }
  static std::string applicationId;

  auto engine = Dali::WidgetEngine::DownCast(service.GetSingleton(typeid(Dali::WidgetEngine)));
  if(engine)
  {
    if(applicationId != appId)
    {
      DALI_LOG_ERROR("WidgetView::New: application ID differs from the live engine.\n");
      return {};
    }
    return engine;
  }

  engine = Dali::WidgetEngine::New(appId);
  if(engine)
  {
    applicationId = appId;
    service.Register(typeid(Dali::WidgetEngine), engine);
    // Publish the engine before refreshing bindings, which can create views.
    UiLocalizationManager::Get().RegisterDomain("widget_viewer_dali", DALI_WIDGET_VIEWER_LOCALE_DIR);
  }
  return engine;
}

BaseHandle Create()
{
  return Dali::BaseHandle();
}

DALI_TYPE_REGISTRATION_BEGIN(Dali::Ui::WidgetView, Dali::Ui::View, Create);
DALI_PROPERTY_REGISTRATION(Dali::Ui, WidgetView, "widgetId", STRING, WIDGET_ID)
DALI_PROPERTY_REGISTRATION(Dali::Ui, WidgetView, "instanceId", STRING, INSTANCE_ID)
DALI_PROPERTY_REGISTRATION(Dali::Ui, WidgetView, "contentInfo", STRING, CONTENT_INFO)
DALI_PROPERTY_REGISTRATION(Dali::Ui, WidgetView, "title", STRING, TITLE)
DALI_PROPERTY_REGISTRATION(Dali::Ui, WidgetView, "updatePeriod", FLOAT, UPDATE_PERIOD)
DALI_PROPERTY_REGISTRATION(Dali::Ui, WidgetView, "preview", BOOLEAN, PREVIEW)
DALI_PROPERTY_REGISTRATION(Dali::Ui, WidgetView, "loadingText", MAP, LOADING_TEXT)
DALI_PROPERTY_REGISTRATION(Dali::Ui, WidgetView, "widgetStateFaulted", BOOLEAN, WIDGET_STATE_FAULTED)
DALI_PROPERTY_REGISTRATION(Dali::Ui, WidgetView, "permanentDelete", BOOLEAN, PERMANENT_DELETE)
DALI_PROPERTY_REGISTRATION(Dali::Ui, WidgetView, "retryText", MAP, RETRY_TEXT)
DALI_PROPERTY_REGISTRATION(Dali::Ui, WidgetView, "effect", MAP, EFFECT)
DALI_PROPERTY_REGISTRATION(Dali::Ui, WidgetView, "keepWidgetSize", BOOLEAN, KEEP_WIDGET_SIZE)
DALI_SIGNAL_REGISTRATION(Dali::Ui, WidgetView, "widgetAdded", SIGNAL_WIDGET_ADDED)
DALI_SIGNAL_REGISTRATION(Dali::Ui, WidgetView, "widgetDeleted", SIGNAL_WIDGET_DELETED)
DALI_SIGNAL_REGISTRATION(Dali::Ui, WidgetView, "widgetCreationAborted", SIGNAL_WIDGET_CREATION_ABORTED)
DALI_SIGNAL_REGISTRATION(Dali::Ui, WidgetView, "widgetContentUpdated", SIGNAL_WIDGET_CONTENT_UPDATED)
DALI_SIGNAL_REGISTRATION(Dali::Ui, WidgetView, "widgetUpdatePeriodChanged", SIGNAL_WIDGET_UPDATE_PERIOD_CHANGED)
DALI_SIGNAL_REGISTRATION(Dali::Ui, WidgetView, "widgetFaulted", SIGNAL_WIDGET_FAULTED)
DALI_ACTION_REGISTRATION(Dali::Ui, WidgetView, "pauseWidget", ACTION_WIDGETVIEW_PAUSE_WIDGET)
DALI_ACTION_REGISTRATION(Dali::Ui, WidgetView, "resumeWidget", ACTION_WIDGETVIEW_RESUME_WIDGET)
DALI_ACTION_REGISTRATION(Dali::Ui, WidgetView, "cancelTouchEvent", ACTION_WIDGETVIEW_CANCEL_TOUCH_EVENT)
DALI_ACTION_REGISTRATION(Dali::Ui, WidgetView, "activateFaultedWidget", ACTION_WIDGETVIEW_ACTIVATE_FAULTED_WIDGET)
DALI_TYPE_REGISTRATION_END()

void ConfigureOverlayView(Dali::Ui::View view, const Vector2& size)
{
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  view.SetProperty(Actor::Property::SENSITIVE, false);
  // WidgetView can be directly added to a DALI window instead of a DALI UI layout.
  // Give overlay children an explicit size so an immediately loaded preview does
  // not use its intrinsic size from the WidgetView's centre. MATCH_PARENT fills
  // the arranged area without applying UiScale to those pixel dimensions again.
  DevelActor::SetResizePolicy(view, ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  view.SetProperty(Actor::Property::SIZE, size);
  view.SetRequestedWidth(MATCH_PARENT);
  view.SetRequestedHeight(MATCH_PARENT);
}

void ConfigurePreviewImage(Dali::Ui::ImageView image, const Vector2& size)
{
  ConfigureOverlayView(image, size);
  image.SetProperty(Dali::Ui::ImageView::Property::LOAD_POLICY, static_cast<int>(Dali::Ui::Image::LoadPolicy::IMMEDIATE));
  image.SetProperty(Actor::Property::VISIBLE, false);
}

void ConfigureStateLabel(Dali::Ui::Label label, const Vector2& size)
{
  ConfigureOverlayView(label, size);
  label.SetProperty(Dali::Ui::Label::Property::HORIZONTAL_ALIGNMENT, "CENTER");
  label.SetProperty(Dali::Ui::Label::Property::VERTICAL_ALIGNMENT, "CENTER");
  label.SetProperty(Dali::Ui::Label::Property::TEXT_COLOR, Dali::Color::WHITE);
  label.SetProperty(Dali::Ui::Label::Property::FONT_SIZE, DEFAULT_FONT_PIXEL_SIZE);
  label.SetProperty(Dali::Ui::Label::Property::FONT_WEIGHT, Dali::Ui::Text::FontWeight::BOLD);
}

void SetStateText(Dali::Ui::Label label, const Dali::String& text, const char* resourceId)
{
  if(text.Empty())
  {
    label.SetTranslatableText(resourceId, "widget_viewer_dali");
  }
  else
  {
    // Application text must survive subsequent locale/binding refreshes.
    label.ClearTranslatableText();
    label.SetProperty(Dali::Ui::Label::Property::TEXT, text);
  }
}

void SetStateFontStyle(Dali::Ui::Label label, const Property::Value& value)
{
  Dali::String style;
  if(!value.Get(style))
  {
    return;
  }

  Property::Map properties;
  Dali::Ui::Text::ParsePropertyString(ToStdString(style), properties);
  if(properties.Empty())
  {
    return;
  }

  // Reset omitted attributes so a previous width or slant does not survive
  // a weight-only update. Use the defaults exposed by Label's font properties.
  label.SetFontWeight(Dali::Ui::Text::FontWeight::NORMAL);
  label.SetFontWidth(Dali::Ui::Text::FontWidth::NORMAL);
  label.SetFontSlant(Dali::Ui::Text::FontSlant::NORMAL);
  if(auto* weight = properties.Find("weight")) label.SetProperty(Dali::Ui::Label::Property::FONT_WEIGHT, *weight);
  if(auto* width = properties.Find("width")) label.SetProperty(Dali::Ui::Label::Property::FONT_WIDTH, *width);
  if(auto* slant = properties.Find("slant")) label.SetProperty(Dali::Ui::Label::Property::FONT_SLANT, *slant);
}
} // namespace

Dali::Ui::WidgetView WidgetView::New(const std::string& appId, const std::string& widgetId, const std::string& contentInfo, int width, int height, float updatePeriod)
{
  auto engine = GetWidgetEngine(appId);
  if(!engine)
  {
    return {};
  }
  auto instance = engine.CreateWidget(widgetId, contentInfo, width, height, updatePeriod);
  if(!instance)
  {
    return Dali::Ui::WidgetView();
  }
  IntrusivePtr<WidgetView> implementation = new WidgetView(std::move(instance), width, height);
  Dali::Ui::WidgetView     handle(*implementation);
  implementation->Initialize();
  return handle;
}

WidgetView::WidgetView(Dali::WidgetEngineInstance instance, int width, int height)
: Dali::Ui::ViewImpl(),
  mInstance(std::move(instance)),
  mNaturalSize(width, height),
  mWidgetWidth(width),
  mWidgetHeight(height)
{
  // Accessibility embeds a remote tree whose screen position depends on ancestors.
  SetArrangePolicy(ArrangePolicy::ALWAYS);
  Dali::Ui::Integration::ViewAccessibility::SetAccessibleObjectCreator(*this, &WidgetView::CreateAccessibleObject);
}

WidgetView::~WidgetView()
{
  if(mInstance)
  {
    mInstance.ResetCallbacks();
  }
}

void WidgetView::OnInitialize()
{
  ViewImpl::OnInitialize();
  InitializeLayout();
  InitializeEvents();
  // New() may be followed by a long period off scene. Apply the paused state
  // before the provider's asynchronous creation notification arrives.
  UpdateEngineState();
  // Installing callbacks can immediately replay pending events and the first frame.
  // Let New() return so the application can connect its signals.
  mInitializeEngineCallbacksTrigger = std::make_unique<Dali::EventThreadCallback>(MakeCallback(this, &WidgetView::InitializeEngineCallbacks));
  mInitializeEngineCallbacksTrigger->Trigger();
}

void WidgetView::InitializeEngineCallbacks()
{
  // Replayed events can remove the widget and release the application's handles.
  Dali::Ui::WidgetView keepAlive(GetOwner());
  mInstance.SetEventCallback([this](Dali::WidgetEngineInstancePlugin::EventType event)
  {
    HandleEngineEvent(event);
  });
  mInstance.SetFrameUpdatedCallback([this](Dali::NativeImagePtr image)
  {
    UpdateWidgetImage(std::move(image));
  });
}

void WidgetView::InitializeLayout()
{
  auto          self = Self();
  const Vector2 widgetSize(mWidgetWidth, mWidgetHeight);
  self.SetProperty(Actor::Property::SIZE, Vector2(mWidgetWidth, mWidgetHeight));
  SetRequestedWidth(static_cast<float>(mWidgetWidth));
  SetRequestedHeight(static_cast<float>(mWidgetHeight));

  mPreviewActor = Dali::Ui::View::New();
  ConfigureOverlayView(mPreviewActor, widgetSize);
  self.Add(mPreviewActor);

  mPreviewImage = Dali::Ui::ImageView::New();
  ConfigurePreviewImage(mPreviewImage, widgetSize);
  Constraint previewCornerConstraint = Constraint::New<Vector4>(mPreviewImage, Dali::Ui::View::Property::CORNER_RADIUS, EqualToConstraint());
  previewCornerConstraint.AddSource(Source(self, Dali::Ui::View::Property::CORNER_RADIUS));
  previewCornerConstraint.Apply();
  mPreviewActor.Add(mPreviewImage);

  mStateTextActor = Dali::Ui::View::New();
  ConfigureOverlayView(mStateTextActor, widgetSize);
  mPreviewActor.Add(mStateTextActor);

  mLoadingText = Dali::Ui::Label::New();
  SetStateText(mLoadingText, Dali::String(), "IDS_ST_POP_LOADING_ING");
  ConfigureStateLabel(mLoadingText, widgetSize);
  mStateTextActor.Add(mLoadingText);

  mRetryText = Dali::Ui::Label::New();
  SetStateText(mRetryText, Dali::String(), "IDS_HS_BODY_UNABLE_TO_LOAD_DATA_TAP_TO_RETRY");
  ConfigureStateLabel(mRetryText, widgetSize);
  mRetryText.SetProperty(Actor::Property::VISIBLE, false);
  mStateTextActor.Add(mRetryText);
  SetPreviewImage();
}

void WidgetView::InitializeEvents()
{
  auto self = Self();
  self.TouchEventSignal().Connect(this, &WidgetView::OnTouch);
  self.WheelEventSignal().Connect(this, &WidgetView::OnWheel);
  self.EffectiveVisibilityChangedSignal().Connect(this, &WidgetView::OnInheritedVisibilityChanged);
  // Parent motion and per-axis size setters do not necessarily call our layout hooks.
  mPositionNotification    = self.AddPropertyNotification(Actor::Property::WORLD_POSITION, StepCondition(1.0f, 1.0f));
  mSizeNotification        = self.AddPropertyNotification(Actor::Property::SIZE, StepCondition(1.0f, 1.0f));
  mScaleNotification       = self.AddPropertyNotification(Actor::Property::WORLD_SCALE, StepCondition(0.1f, 1.0f));
  mOrientationNotification = self.AddPropertyNotification(Actor::Property::WORLD_ORIENTATION, StepCondition(0.01f));
  mPositionNotification.NotifySignal().Connect(this, &WidgetView::OnGeometryUpdated);
  mSizeNotification.NotifySignal().Connect(this, &WidgetView::OnGeometryUpdated);
  mScaleNotification.NotifySignal().Connect(this, &WidgetView::OnGeometryUpdated);
  mOrientationNotification.NotifySignal().Connect(this, &WidgetView::OnGeometryUpdated);
  self.SetFocusable(true);
  self.SetFocusOnTouchEnabled(true);
  auto view = Dali::Ui::View::DownCast(self);
  view.SetAccessibilityRole(Dali::Ui::Accessibility::Role::CONTAINER);
  view.SetAccessibilityHighlightable(false);
}

void WidgetView::SetPreviewImage()
{
  mPreviewImage.SetResourceUrl(ToDaliString(mInstance.GetPreviewImagePath()));
  mPreviewImage.SetProperty(Actor::Property::VISIBLE, mPreviewEnabled);
}

void WidgetView::UpdateWidgetImage(Dali::NativeImagePtr image)
{
  if(!image)
  {
    return;
  }

  // RegisterVisual can emit ResourceReadySignal and release the application's handles.
  Dali::Ui::WidgetView keepAlive(GetOwner());

  auto          widgetImageUrl = Dali::Ui::ImageUrlUtils::GenerateUrl(image, true);
  Property::Map properties;
  properties.Insert(Dali::Ui::Integration::Visual::Property::TYPE, Dali::Ui::Integration::InternalVisualType::IMAGE);
  properties.Insert(Dali::Ui::Integration::ImageVisual::Property::URL, widgetImageUrl.GetUrl());
  // Retain the last frame across scene detachment while the provider is paused.
  properties.Insert(Dali::Ui::Integration::ImageVisual::Property::RELEASE_POLICY, Dali::Ui::Image::ReleasePolicy::DESTROYED);
  auto visual = Dali::Ui::Integration::VisualFactory::Get().CreateVisual(properties);
  if(!visual)
  {
    DALI_LOG_ERROR("WidgetView::UpdateWidgetImage: failed to create widget image visual.\n");
    return;
  }

  auto& viewData = ViewDataImpl::Get(*this);
  viewData.UnregisterVisual(WIDGET_IMAGE_VISUAL_INDEX);
  viewData.RegisterVisual(WIDGET_IMAGE_VISUAL_INDEX, visual, Dali::Ui::Integration::DepthIndex::CONTENT);
  viewData.EnableCornerPropertiesOverridden(visual, true);
  ApplyEffect();
  ShowLoadingState(false);
  ShowRetryState(false);
}

void WidgetView::HandleEngineEvent(Dali::WidgetEngineInstancePlugin::EventType event)
{
  Dali::Ui::WidgetView handle(GetOwner());
  switch(event)
  {
    case Dali::WidgetEngineInstancePlugin::EventType::ADDED:
      if(!mAddedEmitted)
      {
        mAddedEmitted = true;
        mWidgetAddedSignal.Emit(handle);
      }
      NotifyAccessibilityChildrenChanged();
      break;
    case Dali::WidgetEngineInstancePlugin::EventType::CREATION_ABORTED:
      ShowLoadingState(false);
      ShowRetryState(true);
      NotifyAccessibilityChildrenChanged();
      mWidgetCreationAbortedSignal.Emit(handle);
      break;
    case Dali::WidgetEngineInstancePlugin::EventType::CONTENT_UPDATED:
      mWidgetContentUpdatedSignal.Emit(handle);
      break;
    case Dali::WidgetEngineInstancePlugin::EventType::UPDATE_PERIOD_CHANGED:
      mWidgetUpdatePeriodChangedSignal.Emit(handle);
      break;
    case Dali::WidgetEngineInstancePlugin::EventType::FAULTED:
    {
      const bool wasAdded = mAddedEmitted;
      mAddedEmitted       = false;
      // Release the last frame so the engine can close the old connector before
      // registering a replacement for this instance after provider death.
      ViewDataImpl::Get(*this).UnregisterVisual(WIDGET_IMAGE_VISUAL_INDEX);
      ShowLoadingState(false);
      ShowRetryState(true);
      NotifyAccessibilityChildrenChanged();
      if(wasAdded)
      {
        mWidgetDeletedSignal.Emit(handle);
      }
      mWidgetFaultedSignal.Emit(handle);
      break;
    }
  }
}

Dali::String WidgetView::GetWidgetId() const
{
  return ToDaliString(mInstance.GetWidgetId());
}

Dali::String WidgetView::GetInstanceId() const
{
  return ToDaliString(mInstance.GetInstanceId());
}

Dali::String WidgetView::GetContentInfo() const
{
  return ToDaliString(mInstance.GetContentInfo());
}

Dali::String WidgetView::GetTitle() const
{
  return ToDaliString(mInstance.GetTitle());
}

float WidgetView::GetUpdatePeriod() const
{
  return mInstance.GetUpdatePeriod();
}

bool WidgetView::IsWidgetFaulted() const
{
  return mInstance.IsFaulted();
}

void WidgetView::SetPreviewEnabled(bool enabled)
{
  mPreviewEnabled = enabled;
  mPreviewImage.SetProperty(Actor::Property::VISIBLE, enabled);
}

bool WidgetView::IsPreviewEnabled() const
{
  return mPreviewEnabled;
}

void WidgetView::SetLoadingTextVisible(bool visible)
{
  mLoadingTextEnabled = visible;
  mLoadingText.SetProperty(Actor::Property::VISIBLE, visible);
}

bool WidgetView::IsLoadingTextVisible() const
{
  return mLoadingText.GetProperty<bool>(Actor::Property::VISIBLE);
}

void WidgetView::SetRetryTextVisible(bool visible)
{
  mRetryTextEnabled = visible;
  mRetryText.SetProperty(Actor::Property::VISIBLE, visible);
}

bool WidgetView::IsRetryTextVisible() const
{
  return mRetryText.GetProperty<bool>(Actor::Property::VISIBLE);
}

void WidgetView::SetKeepWidgetSize(bool keepWidgetSize)
{
  mKeepWidgetSize = keepWidgetSize;
}

bool WidgetView::IsKeepWidgetSize() const
{
  return mKeepWidgetSize;
}

bool WidgetView::PauseWidget()
{
  mPausedManually = true;
  return UpdateEngineState();
}

bool WidgetView::ResumeWidget()
{
  mPausedManually = false;
  return UpdateEngineState();
}

bool WidgetView::CancelTouchEvent()
{
  return mInstance.CancelTouchEvent();
}

void WidgetView::ActivateFaultedWidget()
{
  if(!mInstance.IsFaulted())
  {
    return;
  }
  ShowRetryState(false);
  ShowLoadingState(true);
  mInstance.ActivateFaultedWidget();
}

bool WidgetView::OnTouch(Dali::Actor actor, Dali::TouchEvent event)
{
  if(event.GetPointCount() == 0u)
  {
    return false;
  }
  const auto deviceClass = event.GetDeviceClass(0);
  if(deviceClass != Dali::Device::Class::MOUSE && deviceClass != Dali::Device::Class::TOUCH)
  {
    return false;
  }
  if(mRetryState)
  {
    if(event.GetState(0) == Dali::PointState::UP)
    {
      ActivateFaultedWidget();
    }
    return false;
  }
  return mInstance.FeedTouchEvent(event);
}

bool WidgetView::OnWheel(Dali::Actor actor, Dali::WheelEvent event)
{
  mInstance.FeedWheelEvent(event);
  return false;
}

bool WidgetView::OnKeyEvent(const Dali::KeyEvent& event)
{
  return mInstance.FeedKeyEvent(event);
}

void WidgetView::OnInheritedVisibilityChanged(Actor, bool)
{
  UpdateEngineState();
}

void WidgetView::OnSceneConnection(int depth)
{
  ViewImpl::OnSceneConnection(depth);
  mOnScene = true;
  UpdateEngineState();
}

void WidgetView::OnSceneDisconnection()
{
  mOnScene = false;
  UpdateEngineState();
  ViewImpl::OnSceneDisconnection();
}

void WidgetView::OnPropertySet(Property::Index index, const Property::Value& propertyValue)
{
  ViewImpl::OnPropertySet(index, propertyValue);

  if(!mPreviewImage)
  {
    return;
  }

  switch(index)
  {
    case Dali::Ui::View::Property::CORNER_RADIUS:
    {
      Vector4 radius;
      if(propertyValue.Get(radius))
      {
        mPreviewImage.SetCornerRadius(radius);
      }
      break;
    }
    case Dali::Ui::View::Property::CORNER_RADIUS_POLICY:
    {
      int policy;
      if(propertyValue.Get(policy))
      {
        mPreviewImage.SetCornerRadiusPolicy(static_cast<Dali::Ui::CornerRadiusPolicy>(policy));
      }
      break;
    }
    case Dali::Ui::View::Property::CORNER_SQUARENESS:
    {
      Vector4 squareness;
      if(propertyValue.Get(squareness))
      {
        mPreviewImage.SetCornerSquareness(squareness);
      }
      break;
    }
  }
}

bool WidgetView::UpdateEngineState()
{
  const bool visible = !IsPaused();
  mInstance.SetVisible(visible);
  const bool updated = visible ? mInstance.Resume() : mInstance.Pause();
  if(mAccessibilityVisible != visible)
  {
    mAccessibilityVisible = visible;
    NotifyAccessibilityChildrenChanged();
  }
  if(visible)
  {
    SynchronizeAccessibilityChildPosition();
  }
  return updated;
}

bool WidgetView::IsPaused() const
{
  return !mOnScene || !Self().IsEffectivelyVisible() || mPausedManually || IsOutOfScreen();
}

bool WidgetView::IsOutOfScreen() const
{
  auto self   = Self();
  auto window = Dali::Window::Get(self);
  if(!window)
  {
    return true;
  }

  // Use rendered geometry so an animation only pauses after leaving the screen,
  // rather than when its off-screen target is set. Extents include parent scale.
  const auto bounds     = Dali::DevelActor::CalculateCurrentScreenExtents(self);
  const auto windowSize = window.GetPositionSize();
  return bounds.x > windowSize.width || bounds.y > windowSize.height ||
         bounds.x + bounds.width < 0.0f || bounds.y + bounds.height < 0.0f;
}

void WidgetView::NotifyAccessibilityChildrenChanged()
{
  if(Dali::Integration::Accessibility::IsUp())
  {
    auto accessible = Dali::DynamicPointerCast<WidgetViewAccessible>(Dali::Accessibility::Accessible::GetOwningPtr(Self()));
    if(accessible)
    {
      accessible->OnChildrenChanged();
    }
  }
}

void WidgetView::SynchronizeAccessibilityChildPosition()
{
  if(Dali::Integration::Accessibility::IsUp())
  {
    auto accessible = Dali::DynamicPointerCast<WidgetViewAccessible>(Dali::Accessibility::Accessible::GetOwningPtr(Self()));
    if(accessible)
    {
      accessible->SynchronizeChildPosition();
    }
  }
}

MeasuredSize WidgetView::OnMeasure(float widthConstraint, float heightConstraint)
{
  const float scale   = GetEffectiveScale();
  const auto  measure = [scale](float requested, float natural, float constraint)
  {
    if(requested >= 0.0f)
    {
      return requested * scale;
    }
    if(requested == MATCH_PARENT && std::isfinite(constraint))
    {
      return constraint;
    }
    return std::min(natural * scale, constraint);
  };
  return MeasuredSize(measure(GetRequestedWidth(), mNaturalSize.width, widthConstraint),
                      measure(GetRequestedHeight(), mNaturalSize.height, heightConstraint));
}

void WidgetView::ResizeWidget(const Vector2& size)
{
  if(mKeepWidgetSize)
  {
    return;
  }

  // Keep layout precision: rounding provider pixels back into this size would
  // progressively shrink WRAP_CONTENT under fractional UI scales.
  const Vector2 naturalSize = size / GetEffectiveScale();
  if(mNaturalSize != naturalSize)
  {
    mNaturalSize = naturalSize;
    ViewDataImpl::Get(*this).InvalidateMeasure();
  }

  const int width  = static_cast<int>(size.width);
  const int height = static_cast<int>(size.height);
  if(mWidgetWidth != width || mWidgetHeight != height)
  {
    mWidgetWidth  = width;
    mWidgetHeight = height;
    mInstance.Resize(width, height);
  }
}

void WidgetView::ResizeOverlayActors(const Vector2& size)
{
  const auto resize = [&size](Dali::Ui::View view)
  {
    view.SetProperty(Actor::Property::SIZE, size);
  };

  resize(mPreviewActor);
  resize(mPreviewImage);
  resize(mStateTextActor);
  resize(mLoadingText);
  resize(mRetryText);
}

LayoutRect WidgetView::OnArrange(const LayoutRect& bounds)
{
  ResizeWidget(Vector2(bounds.width, bounds.height));
  LayoutRect result = ViewImpl::OnArrange(bounds);
  SynchronizeAccessibilityChildPosition();
  return result;
}

void WidgetView::OnSizeSet(const Vector3& targetSize)
{
  ViewImpl::OnSizeSet(targetSize);
  // Initial actor sizing precedes construction of the overlay children.
  if(mRetryText)
  {
    ResizeOverlayActors(targetSize.GetVectorXY());
    ResizeWidget(targetSize.GetVectorXY());
    SynchronizeAccessibilityChildPosition();
  }
}

void WidgetView::OnGeometryUpdated(Dali::PropertyNotification notification)
{
  // Use the target size, as OnSizeAnimation does, rather than resizing the
  // provider for every intermediate animation frame.
  const Vector2 size = Self().GetProperty<Vector3>(Actor::Property::SIZE).GetVectorXY();
  ResizeOverlayActors(size);
  ResizeWidget(size);
  UpdateEngineState();
}

void WidgetView::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
  ViewImpl::OnSizeAnimation(animation, targetSize);
  ResizeOverlayActors(targetSize.GetVectorXY());
  ResizeWidget(targetSize.GetVectorXY());
}

void WidgetView::ShowLoadingState(bool show)
{
  mPreviewImage.SetProperty(Actor::Property::VISIBLE, show && mPreviewEnabled);
  mLoadingText.SetProperty(Actor::Property::VISIBLE, show && mLoadingTextEnabled);
}

void WidgetView::ShowRetryState(bool show)
{
  mRetryState = show;
  mRetryText.SetProperty(Actor::Property::VISIBLE, show && mRetryTextEnabled);
  if(show)
  {
    mPreviewImage.SetProperty(Actor::Property::VISIBLE, false);
  }
}

void WidgetView::SetLoadingTextProperties(const Dali::Property::Map& map)
{
  if(auto* value = map.Find(STATE_TEXT))
  {
    Dali::String text;
    if(value->Get(text))
    {
      SetStateText(mLoadingText, text, "IDS_ST_POP_LOADING_ING");
    }
  }
  if(auto* value = map.Find(FONT_STYLE)) SetStateFontStyle(mLoadingText, *value);
  if(auto* value = map.Find(TEXT_PIXEL_SIZE)) mLoadingText.SetProperty(Dali::Ui::Label::Property::FONT_SIZE, *value);
  if(auto* value = map.Find(TEXT_COLOR)) mLoadingText.SetProperty(Dali::Ui::Label::Property::TEXT_COLOR, *value);
}

void WidgetView::SetRetryTextProperties(const Dali::Property::Map& map)
{
  if(auto* value = map.Find(STATE_TEXT))
  {
    Dali::String text;
    if(value->Get(text))
    {
      SetStateText(mRetryText, text, "IDS_HS_BODY_UNABLE_TO_LOAD_DATA_TAP_TO_RETRY");
    }
  }
  if(auto* value = map.Find(FONT_STYLE)) SetStateFontStyle(mRetryText, *value);
  if(auto* value = map.Find(TEXT_PIXEL_SIZE)) mRetryText.SetProperty(Dali::Ui::Label::Property::FONT_SIZE, *value);
  if(auto* value = map.Find(TEXT_COLOR)) mRetryText.SetProperty(Dali::Ui::Label::Property::TEXT_COLOR, *value);
  bool visible;
  if(auto* value = map.Find(TEXT_VISIBLE); value && value->Get(visible))
  {
    SetRetryTextVisible(visible);
  }
}

void WidgetView::SetEffect(const Dali::Property::Map& map)
{
  if(map.Empty())
  {
    mEffectPropertyMap.Clear();
  }
  else if(auto* shaderValue = map.Find(CUSTOM_SHADER); shaderValue && map.Count() == 1u)
  {
    auto* shaderMap = shaderValue->GetMap();
    if(!shaderMap)
    {
      return;
    }
    mEffectPropertyMap = *shaderMap;
  }
  else
  {
    return;
  }
  ApplyEffect();
}

void WidgetView::ApplyEffect()
{
  auto visual = ViewDataImpl::Get(*this).GetVisual(WIDGET_IMAGE_VISUAL_INDEX);
  if(visual)
  {
    // A custom shader replaces the standard image shader. An empty array
    // clears custom shaders and restores native-image/corner handling.
    Property::Map properties;
    if(mEffectPropertyMap.Empty())
    {
      properties.Insert(Dali::Ui::Integration::Visual::Property::SHADER, Property::Array());
    }
    else
    {
      properties.Insert(Dali::Ui::Integration::Visual::Property::SHADER, mEffectPropertyMap);
    }
    visual.SetProperties(properties);
  }
}

void WidgetView::SetProperty(BaseObject* object, Property::Index index, const Property::Value& value)
{
  Dali::Ui::WidgetView handle = Dali::Ui::WidgetView::DownCast(Dali::BaseHandle(object));
  if(!handle) return;
  WidgetView& impl = GetImplementation(handle);
  switch(index)
  {
    case Dali::Ui::WidgetView::Property::PREVIEW:
    {
      bool enabled;
      if(value.Get(enabled)) impl.SetPreviewEnabled(enabled);
      break;
    }
    case Dali::Ui::WidgetView::Property::LOADING_TEXT:
    {
      bool visible;
      if(value.Get(visible))
      {
        impl.SetLoadingTextVisible(visible);
      }
      else
      {
        Property::Map map;
        if(value.Get(map)) impl.SetLoadingTextProperties(map);
      }
      break;
    }
    case Dali::Ui::WidgetView::Property::RETRY_TEXT:
    {
      Property::Map map;
      if(value.Get(map)) impl.SetRetryTextProperties(map);
      break;
    }
    case Dali::Ui::WidgetView::Property::EFFECT:
    {
      Property::Map map;
      if(value.Get(map)) impl.SetEffect(map);
      break;
    }
    case Dali::Ui::WidgetView::Property::PERMANENT_DELETE:
      value.Get(impl.mPermanentDelete);
      break;
    case Dali::Ui::WidgetView::Property::KEEP_WIDGET_SIZE:
    {
      bool keepWidgetSize;
      if(value.Get(keepWidgetSize)) impl.SetKeepWidgetSize(keepWidgetSize);
      break;
    }
  }
}

Property::Value WidgetView::GetProperty(BaseObject* object, Property::Index index)
{
  Dali::Ui::WidgetView handle = Dali::Ui::WidgetView::DownCast(Dali::BaseHandle(object));
  if(!handle) return Property::Value();
  WidgetView& impl = GetImplementation(handle);
  switch(index)
  {
    case Dali::Ui::WidgetView::Property::WIDGET_ID:
      return Property::Value(impl.GetWidgetId());
    case Dali::Ui::WidgetView::Property::INSTANCE_ID:
      return Property::Value(impl.GetInstanceId());
    case Dali::Ui::WidgetView::Property::CONTENT_INFO:
      return Property::Value(impl.GetContentInfo());
    case Dali::Ui::WidgetView::Property::TITLE:
      return Property::Value(impl.GetTitle());
    case Dali::Ui::WidgetView::Property::UPDATE_PERIOD:
      return Property::Value(impl.GetUpdatePeriod());
    case Dali::Ui::WidgetView::Property::PREVIEW:
      return Property::Value(impl.IsPreviewEnabled());
    case Dali::Ui::WidgetView::Property::LOADING_TEXT:
      return Property::Value(impl.IsLoadingTextVisible());
    case Dali::Ui::WidgetView::Property::WIDGET_STATE_FAULTED:
      return Property::Value(impl.IsWidgetFaulted());
    case Dali::Ui::WidgetView::Property::PERMANENT_DELETE:
      return Property::Value(impl.mPermanentDelete);
    case Dali::Ui::WidgetView::Property::KEEP_WIDGET_SIZE:
      return Property::Value(impl.IsKeepWidgetSize());
  }
  return Property::Value();
}

bool WidgetView::DoAction(BaseObject* object, const Dali::String& actionName, const Property::Map& attributes)
{
  Dali::Ui::WidgetView handle = Dali::Ui::WidgetView::DownCast(Dali::BaseHandle(object));
  if(!handle) return false;
  WidgetView& impl = GetImplementation(handle);
  if(std::strcmp(actionName.CStr(), ACTION_WIDGETVIEW_PAUSE_WIDGET) == 0) return impl.PauseWidget();
  if(std::strcmp(actionName.CStr(), ACTION_WIDGETVIEW_RESUME_WIDGET) == 0) return impl.ResumeWidget();
  if(std::strcmp(actionName.CStr(), ACTION_WIDGETVIEW_CANCEL_TOUCH_EVENT) == 0) return impl.CancelTouchEvent();
  if(std::strcmp(actionName.CStr(), ACTION_WIDGETVIEW_ACTIVATE_FAULTED_WIDGET) == 0)
  {
    impl.ActivateFaultedWidget();
    return true;
  }
  return false;
}

bool WidgetView::DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const Dali::String& signalName, FunctorDelegate* functor)
{
  Dali::Ui::WidgetView handle = Dali::Ui::WidgetView::DownCast(Dali::BaseHandle(object));
  if(!handle) return false;
  if(std::strcmp(signalName.CStr(), SIGNAL_WIDGET_ADDED) == 0)
    handle.WidgetAddedSignal().Connect(tracker, functor);
  else if(std::strcmp(signalName.CStr(), SIGNAL_WIDGET_DELETED) == 0)
    handle.WidgetDeletedSignal().Connect(tracker, functor);
  else if(std::strcmp(signalName.CStr(), SIGNAL_WIDGET_CREATION_ABORTED) == 0)
    handle.WidgetCreationAbortedSignal().Connect(tracker, functor);
  else if(std::strcmp(signalName.CStr(), SIGNAL_WIDGET_CONTENT_UPDATED) == 0)
    handle.WidgetContentUpdatedSignal().Connect(tracker, functor);
  else if(std::strcmp(signalName.CStr(), SIGNAL_WIDGET_UPDATE_PERIOD_CHANGED) == 0)
    handle.WidgetUpdatePeriodChangedSignal().Connect(tracker, functor);
  else if(std::strcmp(signalName.CStr(), SIGNAL_WIDGET_FAULTED) == 0)
    handle.WidgetFaultedSignal().Connect(tracker, functor);
  else
    return false;
  return true;
}

Dali::Ui::WidgetView::WidgetViewSignalType& WidgetView::WidgetAddedSignal()
{
  return mWidgetAddedSignal;
}
Dali::Ui::WidgetView::WidgetViewSignalType& WidgetView::WidgetDeletedSignal()
{
  return mWidgetDeletedSignal;
}
Dali::Ui::WidgetView::WidgetViewSignalType& WidgetView::WidgetCreationAbortedSignal()
{
  return mWidgetCreationAbortedSignal;
}
Dali::Ui::WidgetView::WidgetViewSignalType& WidgetView::WidgetContentUpdatedSignal()
{
  return mWidgetContentUpdatedSignal;
}
Dali::Ui::WidgetView::WidgetViewSignalType& WidgetView::WidgetUpdatePeriodChangedSignal()
{
  return mWidgetUpdatePeriodChangedSignal;
}
Dali::Ui::WidgetView::WidgetViewSignalType& WidgetView::WidgetFaultedSignal()
{
  return mWidgetFaultedSignal;
}
Dali::Ui::ViewAccessible* WidgetView::CreateAccessibleObject(Dali::Ui::View view)
{
  return new WidgetViewAccessible(view);
}

WidgetView::WidgetViewAccessible::WidgetViewAccessible(Dali::Actor self)
: ViewAccessible(self)
{
  mRemoteChild.SetParent(this);
  Dali::Integration::Accessibility::Bridge::EnabledSignal().Connect<WidgetViewAccessible>(this, &WidgetViewAccessible::OnChildrenChanged);
  Dali::Integration::Accessibility::Bridge::DisabledSignal().Connect<WidgetViewAccessible>(this, &WidgetViewAccessible::OnChildrenChanged);
}

void WidgetView::WidgetViewAccessible::OnChildrenChanged()
{
  mRemoteChild.SetAddress({});
  ViewAccessible::OnChildrenChanged();
}

bool WidgetView::WidgetViewAccessible::CanExposeRemoteChild() const
{
  if(!Dali::Integration::Accessibility::IsUp())
  {
    return false;
  }
  auto        self     = Dali::Ui::WidgetView::DownCast(Self());
  const auto& selfImpl = Dali::Ui::Internal::GetImplementation(self);
  return !selfImpl.IsPaused() && selfImpl.mInstance.GetProcessId() > 0;
}

void WidgetView::WidgetViewAccessible::SynchronizeChildPosition()
{
  if(!CanExposeRemoteChild())
  {
    mRemoteChild.SetAddress({});
    return;
  }
  if(!mRemoteChild.GetAddress())
  {
    return;
  }

  auto extents = GetExtents(Dali::Devel::Accessibility::CoordinateType::SCREEN);
  if(auto bridge = Dali::Integration::Accessibility::Bridge::GetCurrentBridge())
  {
    bridge->SetSocketOffset(&mRemoteChild, extents.x, extents.y);
  }
}

void WidgetView::WidgetViewAccessible::DoGetChildren(std::vector<Dali::Accessibility::Accessible*>& children)
{
  using Dali::Integration::Accessibility::Bridge;

  if(!CanExposeRemoteChild())
  {
    mRemoteChild.SetAddress({});
    return;
  }

  auto  self     = Dali::Ui::WidgetView::DownCast(Self());
  auto& selfImpl = Dali::Ui::Internal::GetImplementation(self);

  if(auto bridge = Bridge::GetCurrentBridge())
  {
    std::string busName = Bridge::MakeBusNameForWidget(selfImpl.mInstance.GetInstanceId(), selfImpl.mInstance.GetProcessId());
    auto        address = bridge->EmbedSocket(GetAddress(), {std::move(busName), "root"});
    mRemoteChild.SetAddress(address);
    if(address)
    {
      children.push_back(&mRemoteChild);
      SynchronizeChildPosition();
    }
  }
}

Dali::Devel::Accessibility::Attributes WidgetView::WidgetViewAccessible::GetAttributes() const
{
  auto attributes = Dali::Ui::ViewAccessible::GetAttributes();
  if(CanExposeRemoteChild() && mRemoteChild.GetAddress())
  {
    attributes.insert_or_assign("child_bus", mRemoteChild.GetAddress().GetBus());
  }
  return attributes;
}

} // namespace Internal
} // namespace Ui
} // namespace DALI_NAMESPACE
