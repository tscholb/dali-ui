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
#include <dali-ui-foundation/internal/views/view/view-data-impl.h>
#include <dali-ui-foundation/internal/visuals/image/image-visual.h>
#include <dali-ui-foundation/internal/visuals/visual-base-impl.h>
#include <dali-ui-foundation/public-api/image-loader/image-url-utils.h>
#include <dali-ui-foundation/public-api/visuals/visual-properties.h>
#include <dali/devel-api/actors/actor-devel.h>
#include <dali/devel-api/object/type-registry-helper.h>
#include <dali/devel-api/object/type-registry.h>
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-bridge.h>
#include <dali/integration-api/adaptor-framework/accessibility/accessibility-integ.h>
#include <dali/integration-api/debug.h>
#include <dali/integration-api/rendering/decorated-visual-renderer.h>
#include <dali/integration-api/string-utils.h>
#include <dali/public-api/animation/constraint.h>
#include <dali/public-api/animation/constraints.h>
#include <dali/public-api/object/property-array.h>
#include <dali/public-api/rendering/texture-set.h>
#include <dali/public-api/rendering/texture.h>
#include <algorithm>
#include <cstring>
#include <iterator>
#include <utility>
#include <vector>

using Dali::Integration::ToDaliString;
using Dali::Integration::ToPropertyValue;
using Dali::Integration::ToStdString;

namespace Dali
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

std::vector<Dali::WidgetEngine> gDefaultEngines;

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
DALI_PROPERTY_REGISTRATION(Dali::Ui, WidgetView, "preview", MAP, PREVIEW)
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

void ConfigureFillView(Dali::Ui::View view, const Vector2& size)
{
  view.SetProperty(Actor::Property::PARENT_ORIGIN, ParentOrigin::TOP_LEFT);
  view.SetProperty(Actor::Property::PIVOT, Pivot::TOP_LEFT);
  view.SetProperty(Actor::Property::SENSITIVE, false);
  // WidgetView can be directly added to a DALI window instead of a DALI UI layout.
  // Give overlay children an explicit size so an immediately loaded preview does
  // not use its intrinsic size from the WidgetView's centre.
  DevelActor::SetResizePolicy(view, ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  view.SetProperty(Actor::Property::SIZE, size);
  view.SetRequestedWidth(size.width);
  view.SetRequestedHeight(size.height);
}

void ConfigureImageView(Dali::Ui::ImageView image, const Vector2& size)
{
  ConfigureFillView(image, size);
  image.SetProperty(Dali::Ui::ImageView::Property::LOAD_POLICY, static_cast<int>(Dali::Ui::Image::LoadPolicy::IMMEDIATE));
  image.SetProperty(Actor::Property::VISIBLE, false);
}

void ConfigureLabel(Dali::Ui::Label label, const Vector2& size)
{
  ConfigureFillView(label, size);
  label.SetProperty(Dali::Ui::Label::Property::HORIZONTAL_ALIGNMENT, "CENTER");
  label.SetProperty(Dali::Ui::Label::Property::VERTICAL_ALIGNMENT, "CENTER");
  label.SetProperty(Dali::Ui::Label::Property::TEXT_COLOR, Dali::Color::WHITE);
  label.SetProperty(Dali::Ui::Label::Property::FONT_SIZE, DEFAULT_FONT_PIXEL_SIZE);
}
} // namespace

void WidgetView::SetDefaultEngine(Dali::WidgetEngine engine)
{
  if(engine)
  {
    gDefaultEngines.push_back(std::move(engine));
  }
}

void WidgetView::ClearDefaultEngine(const Dali::WidgetEngine& engine)
{
  auto iter = std::find_if(gDefaultEngines.rbegin(), gDefaultEngines.rend(), [&engine](const Dali::WidgetEngine& candidate)
  {
    return engine && candidate.GetObjectPtr() == engine.GetObjectPtr();
  });
  if(iter != gDefaultEngines.rend())
  {
    gDefaultEngines.erase(std::next(iter).base());
  }
}

Dali::Ui::WidgetView WidgetView::New(const std::string& widgetId, const std::string& contentInfo, int width, int height, float updatePeriod)
{
  if(gDefaultEngines.empty())
  {
    DALI_LOG_ERROR("WidgetView::New: WidgetViewManager must be created first.\n");
    return Dali::Ui::WidgetView();
  }
  return New(gDefaultEngines.back().CreateWidget(widgetId, contentInfo, width, height, updatePeriod), width, height);
}

Dali::Ui::WidgetView WidgetView::New(Dali::WidgetEngineInstance instance, int width, int height)
{
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
: Dali::Ui::Integration::SizeNegotiatedViewImpl(),
  mInstance(std::move(instance)),
  mWidgetWidth(width),
  mWidgetHeight(height)
{
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
  Dali::Ui::Integration::SizeNegotiatedViewImpl::OnInitialize();
  InitializeLayout();
  InitializeEvents();
  mInstance.SetEventCallback([this](Dali::WidgetEngineInstancePlugin::EventType event)
  { HandleEngineEvent(event); });
  mInstance.SetFrameUpdatedCallback([this](Dali::NativeImagePtr image)
  { UpdateWidgetImage(std::move(image)); });
}

void WidgetView::InitializeLayout()
{
  auto          self = Self();
  const Vector2 widgetSize(mWidgetWidth, mWidgetHeight);
  DevelActor::SetResizePolicy(self, ResizePolicy::FIXED, Dimension::ALL_DIMENSIONS);
  self.SetProperty(Actor::Property::SIZE, Vector2(mWidgetWidth, mWidgetHeight));
  SetRequestedWidth(static_cast<float>(mWidgetWidth));
  SetRequestedHeight(static_cast<float>(mWidgetHeight));

  mPreviewActor = Dali::Ui::View::New();
  ConfigureFillView(mPreviewActor, widgetSize);
  self.Add(mPreviewActor);

  mPreviewImage = Dali::Ui::ImageView::New();
  ConfigureImageView(mPreviewImage, widgetSize);
  Constraint previewCornerConstraint = Constraint::New<Vector4>(mPreviewImage, Dali::Ui::View::Property::CORNER_RADIUS, EqualToConstraint());
  previewCornerConstraint.AddSource(Source(self, Dali::Ui::View::Property::CORNER_RADIUS));
  previewCornerConstraint.Apply();
  mPreviewActor.Add(mPreviewImage);

  mStateTextActor = Dali::Ui::View::New();
  ConfigureFillView(mStateTextActor, widgetSize);
  mPreviewActor.Add(mStateTextActor);

  mLoadingText = Dali::Ui::Label::New(ToDaliString(std::string("Loading...")));
  ConfigureLabel(mLoadingText, widgetSize);
  mStateTextActor.Add(mLoadingText);

  mRetryText = Dali::Ui::Label::New(ToDaliString(std::string("Unable to load. Tap to retry.")));
  ConfigureLabel(mRetryText, widgetSize);
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
  self.SetFocusable(true);
  self.SetFocusOnTouchEnabled(true);
  auto view = Dali::Ui::View::DownCast(self);
  view.SetAccessibilityRole(Dali::Ui::Accessibility::Role::CONTAINER);
  view.SetAccessibilityHighlightable(false);
}

void WidgetView::SetPreviewImage()
{
  if(mPreviewImagePath.empty())
  {
    mPreviewImagePath = mInstance.GetPreviewImagePath();
    if(mPreviewImagePath.empty())
    {
      mPreviewImagePath = DALI_IMAGE_DIR "broken.png";
    }
  }
  mPreviewImage.SetResourceUrl(ToDaliString(mPreviewImagePath));
  mPreviewImage.SetProperty(Actor::Property::VISIBLE, mPreviewEnabled);
}

void WidgetView::UpdateWidgetImage(Dali::NativeImagePtr image)
{
  if(!image)
  {
    return;
  }

  mImageSource = std::move(image);

  auto& viewData   = ViewDataImpl::Get(*this);
  auto  visual     = viewData.GetVisual(WIDGET_IMAGE_VISUAL_INDEX);
  auto  renderer   = visual ? visual.GetRenderer() : Dali::VisualRenderer();
  auto  textureSet = renderer ? renderer.GetTextures() : Dali::TextureSet();

  // A registered visual can briefly have no renderer/TextureSet while it is
  // being staged. Rebuild it from the current native image instead of dropping
  // the frame: dropping it means no graphics resource is created and the
  // provider never receives the buffer release callback.
  if(visual && !textureSet)
  {
    viewData.UnregisterVisual(WIDGET_IMAGE_VISUAL_INDEX);
    visual.Reset();
  }

  if(visual)
  {
    // Keep the visual and renderer stable while widget frames are updated. Recreating
    // the visual for every provider buffer can discard each new texture before the
    // render thread catches up, leaving older renderers visible. This is also how the
    // Toolkit WidgetView updates subsequent native-image buffers.
    Dali::Texture texture = Dali::Texture::New(*mImageSource);
    textureSet.SetTexture(0u, texture);
    if(auto* imageVisual = dynamic_cast<Dali::Ui::Internal::ImageVisual*>(&Dali::Ui::GetImplementation(visual)))
    {
      imageVisual->ReleaseExternalTextureOwnership();
    }
  }
  else
  {
    // The visual keeps the external texture alive after registration. Keeping
    // ImageUrl as a WidgetView member pins the provider's first buffer for the
    // whole widget lifetime, leaving only two buffers available for updates.
    auto widgetImageUrl = Dali::Ui::ImageUrlUtils::GenerateUrl(mImageSource, true);

    Property::Map properties;
    properties.Insert(Dali::Ui::Integration::Visual::Property::TYPE, Dali::Ui::Integration::InternalVisualType::IMAGE);
    properties.Insert(Dali::Ui::Integration::ImageVisual::Property::URL, widgetImageUrl.GetUrl());
    visual = Dali::Ui::Integration::VisualFactory::Get().CreateVisual(properties);
    if(visual)
    {
      viewData.RegisterVisual(WIDGET_IMAGE_VISUAL_INDEX, visual, Dali::Ui::Integration::DepthIndex::CONTENT);
      viewData.EnableCornerPropertiesOverridden(visual, true);
      ApplyEffect();
    }
    else
    {
      // Ensure the NativeImage still enters the graphics resource lifecycle.
      // Its destruction callback releases the provider buffer even though the
      // visual could not be constructed.
      mFallbackTexture = Dali::Texture::New(*mImageSource);
      DALI_LOG_ERROR("WidgetView::UpdateWidgetImage: failed to create widget image visual.\n");
      return;
    }
  }

  mFallbackTexture.Reset();
  viewData.EnableVisual(WIDGET_IMAGE_VISUAL_INDEX, true);
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
    case Dali::WidgetEngineInstancePlugin::EventType::DELETED:
      NotifyAccessibilityChildrenChanged();
      mWidgetDeletedSignal.Emit(handle);
      break;
    case Dali::WidgetEngineInstancePlugin::EventType::CREATION_ABORTED:
      ShowLoadingState(false);
      ShowRetryState(true);
      mWidgetCreationAbortedSignal.Emit(handle);
      break;
    case Dali::WidgetEngineInstancePlugin::EventType::CONTENT_UPDATED:
      mWidgetContentUpdatedSignal.Emit(handle);
      break;
    case Dali::WidgetEngineInstancePlugin::EventType::UPDATE_PERIOD_CHANGED:
      mWidgetUpdatePeriodChangedSignal.Emit(handle);
      break;
    case Dali::WidgetEngineInstancePlugin::EventType::FAULTED:
      ViewDataImpl::Get(*this).EnableVisual(WIDGET_IMAGE_VISUAL_INDEX, false);
      ShowRetryState(true);
      NotifyAccessibilityChildrenChanged();
      mWidgetFaultedSignal.Emit(handle);
      break;
    case Dali::WidgetEngineInstancePlugin::EventType::TERMINATED:
      mWidgetTerminatedSignal.Emit(handle);
      break;
  }
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
  ShowRetryState(false);
  ShowLoadingState(true);
  mInstance.ActivateFaultedWidget();
}

bool WidgetView::TerminateWidget()
{
  return RemoveWidget();
}

bool WidgetView::RemoveWidget()
{
  return mInstance.Remove();
}

const std::string& WidgetView::GetInstanceId() const
{
  return mInstance.GetInstanceId();
}

bool WidgetView::OnTouch(Dali::Actor actor, Dali::TouchEvent event)
{
  if(mRetryState && event.GetPointCount() > 0u && event.GetState(0) == Dali::PointState::UP)
  {
    ActivateFaultedWidget();
    return true;
  }
  return mInstance.FeedTouchEvent(event);
}

bool WidgetView::OnWheel(Dali::Actor actor, Dali::WheelEvent event)
{
  return mInstance.FeedWheelEvent(event);
}

bool WidgetView::OnKeyEvent(const Dali::KeyEvent& event)
{
  return mInstance.FeedKeyEvent(event);
}

void WidgetView::OnInheritedVisibilityChanged(Actor actor, bool visible)
{
  mEffectiveVisible = visible;
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
  const bool visible = mOnScene && mEffectiveVisible && !mPausedManually;
  mInstance.SetVisible(visible);
  const bool updated = visible ? mInstance.Resume() : mInstance.Pause();
  NotifyAccessibilityChildrenChanged();
  if(visible)
  {
    SynchronizeAccessibilityChildPosition();
  }
  return updated;
}

bool WidgetView::IsPaused() const
{
  return !mOnScene || !mEffectiveVisible || mPausedManually;
}

void WidgetView::NotifyAccessibilityChildrenChanged()
{
  if(Dali::Integration::Accessibility::IsUp())
  {
    auto accessible = Dali::DynamicPointerCast<Dali::Accessibility::ActorAccessible>(Dali::Accessibility::Accessible::GetOwningPtr(Self()));
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

Vector3 WidgetView::GetNaturalSize()
{
  return Vector3(static_cast<float>(mWidgetWidth), static_cast<float>(mWidgetHeight), static_cast<float>(std::min(mWidgetWidth, mWidgetHeight)));
}

void WidgetView::ResizeWidget(int width, int height)
{
  if(!mKeepWidgetSize && (mWidgetWidth != width || mWidgetHeight != height))
  {
    mWidgetWidth  = width;
    mWidgetHeight = height;
    ResizeOverlayActors(Vector2(width, height));
    mInstance.Resize(width, height);
  }
}

void WidgetView::ResizeOverlayActors(const Vector2& size)
{
  const auto resize = [&size](Dali::Ui::View view)
  {
    view.SetProperty(Actor::Property::SIZE, size);
    view.SetRequestedWidth(size.width);
    view.SetRequestedHeight(size.height);
  };

  resize(mPreviewActor);
  resize(mPreviewImage);
  resize(mStateTextActor);
  resize(mLoadingText);
  resize(mRetryText);
}

void WidgetView::OnRelayout(const Vector2& size, RelayoutContainer& container)
{
  Dali::Ui::Integration::SizeNegotiatedViewImpl::OnRelayout(size, container);
  ResizeWidget(static_cast<int>(size.x), static_cast<int>(size.y));
  SynchronizeAccessibilityChildPosition();
}

void WidgetView::OnSizeAnimation(Animation& animation, const Vector3& targetSize)
{
  ViewImpl::OnSizeAnimation(animation, targetSize);
  ResizeWidget(static_cast<int>(targetSize.x), static_cast<int>(targetSize.y));
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

void WidgetView::SetLoadingTextPropertyMap(Dali::Property::Map map)
{
  if(auto* value = map.Find(STATE_TEXT))
  {
    Dali::String text;
    if(value->Get(text))
    {
      mLoadingTextString = ToStdString(text);
      mLoadingText.SetProperty(Dali::Ui::Label::Property::TEXT, text);
    }
  }
  if(auto* value = map.Find(FONT_STYLE)) mLoadingText.SetProperty(Dali::Ui::Label::Property::FONT_WEIGHT, *value);
  if(auto* value = map.Find(TEXT_PIXEL_SIZE)) mLoadingText.SetProperty(Dali::Ui::Label::Property::FONT_SIZE, *value);
  if(auto* value = map.Find(TEXT_COLOR)) mLoadingText.SetProperty(Dali::Ui::Label::Property::TEXT_COLOR, *value);
}

void WidgetView::SetRetryTextPropertyMap(Dali::Property::Map map)
{
  if(auto* value = map.Find(STATE_TEXT))
  {
    Dali::String text;
    if(value->Get(text))
    {
      mRetryTextString = ToStdString(text);
      mRetryText.SetProperty(Dali::Ui::Label::Property::TEXT, text);
    }
  }
  if(auto* value = map.Find(FONT_STYLE)) mRetryText.SetProperty(Dali::Ui::Label::Property::FONT_WEIGHT, *value);
  if(auto* value = map.Find(TEXT_PIXEL_SIZE)) mRetryText.SetProperty(Dali::Ui::Label::Property::FONT_SIZE, *value);
  if(auto* value = map.Find(TEXT_COLOR)) mRetryText.SetProperty(Dali::Ui::Label::Property::TEXT_COLOR, *value);
  if(auto* value = map.Find(TEXT_VISIBLE)) value->Get(mRetryTextEnabled);
}

void WidgetView::SetEffectPropertyMap(Dali::Property::Map map)
{
  if(!map.Empty())
  {
    auto* shaderValue = map.Find(CUSTOM_SHADER);
    if(shaderValue && map.Count() == 1u)
    {
      if(auto* shaderMap = shaderValue->GetMap())
      {
        mEffectPropertyMap = *shaderMap;
      }
    }
  }
  ApplyEffect();
}

void WidgetView::ApplyEffect()
{
  if(mEffectPropertyMap.Empty())
  {
    return;
  }

  auto visual = ViewDataImpl::Get(*this).GetVisual(WIDGET_IMAGE_VISUAL_INDEX);
  if(visual)
  {
    // A custom effect shader replaces the standard ImageVisual shader.
    // CornerRadius is therefore not guaranteed while an effect is applied.
    Property::Map properties;
    properties.Insert(Dali::Ui::Integration::Visual::Property::SHADER, mEffectPropertyMap);
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
      value.Get(impl.mPreviewEnabled);
      impl.mPreviewImage.SetProperty(Actor::Property::VISIBLE, impl.mPreviewEnabled);
      break;
    case Dali::Ui::WidgetView::Property::LOADING_TEXT:
      if(!value.Get(impl.mLoadingTextEnabled))
      {
        Property::Map map;
        if(value.Get(map)) impl.SetLoadingTextPropertyMap(std::move(map));
      }
      break;
    case Dali::Ui::WidgetView::Property::RETRY_TEXT:
    {
      Property::Map map;
      if(value.Get(map)) impl.SetRetryTextPropertyMap(std::move(map));
      break;
    }
    case Dali::Ui::WidgetView::Property::EFFECT:
    {
      Property::Map map;
      if(value.Get(map)) impl.SetEffectPropertyMap(std::move(map));
      break;
    }
    case Dali::Ui::WidgetView::Property::PERMANENT_DELETE:
      if(value.Get(impl.mPermanentDelete)) impl.mInstance.SetPermanentDelete(impl.mPermanentDelete);
      break;
    case Dali::Ui::WidgetView::Property::KEEP_WIDGET_SIZE:
      value.Get(impl.mKeepWidgetSize);
      break;
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
      return ToPropertyValue(impl.mInstance.GetWidgetId());
    case Dali::Ui::WidgetView::Property::INSTANCE_ID:
      return ToPropertyValue(impl.mInstance.GetInstanceId());
    case Dali::Ui::WidgetView::Property::CONTENT_INFO:
      return ToPropertyValue(impl.mInstance.GetContentInfo());
    case Dali::Ui::WidgetView::Property::TITLE:
      return ToPropertyValue(impl.mInstance.GetTitle());
    case Dali::Ui::WidgetView::Property::UPDATE_PERIOD:
      return Property::Value(impl.mInstance.GetUpdatePeriod());
    case Dali::Ui::WidgetView::Property::PREVIEW:
      return Property::Value(impl.mPreviewEnabled);
    case Dali::Ui::WidgetView::Property::LOADING_TEXT:
      return Property::Value(impl.mLoadingTextEnabled);
    case Dali::Ui::WidgetView::Property::WIDGET_STATE_FAULTED:
      return Property::Value(impl.mInstance.IsFaulted());
    case Dali::Ui::WidgetView::Property::PERMANENT_DELETE:
      return Property::Value(impl.mPermanentDelete);
    case Dali::Ui::WidgetView::Property::KEEP_WIDGET_SIZE:
      return Property::Value(impl.mKeepWidgetSize);
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
Dali::Ui::WidgetView::WidgetViewSignalType& WidgetView::WidgetTerminatedSignal()
{
  return mWidgetTerminatedSignal;
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

void WidgetView::WidgetViewAccessible::SynchronizeChildPosition()
{
  if(!Dali::Integration::Accessibility::IsUp() || !mRemoteChild.GetAddress())
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

  if(!Dali::Integration::Accessibility::IsUp())
  {
    return;
  }

  auto  self     = Dali::Ui::WidgetView::DownCast(Self());
  auto& selfImpl = Dali::Ui::Internal::GetImplementation(self);
  if(selfImpl.IsPaused())
  {
    return;
  }

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
  if(mRemoteChild.GetAddress())
  {
    attributes.insert_or_assign("child_bus", mRemoteChild.GetAddress().GetBus());
  }
  return attributes;
}

} // namespace Internal
} // namespace Ui
} // namespace Dali
