#ifndef DALI_UI_INTERNAL_WIDGET_VIEW_IMPL_H
#define DALI_UI_INTERNAL_WIDGET_VIEW_IMPL_H

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

#include <dali-ui-foundation/public-api/views/widget/widget-view.h>

#include <memory>

#include <dali-ui-foundation/integration-api/view-accessibility.h>
#include <dali-ui-foundation/integration-api/view-accessible.h>
#include <dali-ui-foundation/public-api/image-loader/image-url.h>
#include <dali-ui-foundation/public-api/views/image/image-view.h>
#include <dali-ui-foundation/public-api/views/text-controls/label.h>
#include <dali-ui-foundation/public-api/views/view-impl.h>
#include <dali/devel-api/adaptor-framework/event-thread-callback.h>
#include <dali/devel-api/adaptor-framework/proxy-accessible.h>
#include <dali/devel-api/adaptor-framework/widget-engine/widget-engine-instance.h>
#include <dali/public-api/object/property-map.h>
#include <dali/public-api/object/property-notification.h>

namespace DALI_NAMESPACE
{
namespace Ui
{
namespace Internal
{
class WidgetView : public Dali::Ui::ViewImpl
{
public:
  static Dali::Ui::WidgetView New(const std::string& appId, const std::string& widgetId, const std::string& contentInfo, int width, int height, float updatePeriod);

  Dali::String GetWidgetId() const;
  Dali::String GetInstanceId() const;
  Dali::String GetContentInfo() const;
  Dali::String GetTitle() const;
  float        GetUpdatePeriod() const;
  bool         IsWidgetFaulted() const;
  void         SetPreviewEnabled(bool enabled);
  bool         IsPreviewEnabled() const;
  void         SetLoadingTextVisible(bool visible);
  bool         IsLoadingTextVisible() const;
  void         SetRetryTextVisible(bool visible);
  bool         IsRetryTextVisible() const;
  void         SetKeepWidgetSize(bool keepWidgetSize);
  bool         IsKeepWidgetSize() const;

  void SetLoadingTextProperties(const Dali::Property::Map& map);
  void SetRetryTextProperties(const Dali::Property::Map& map);
  void SetEffect(const Dali::Property::Map& map);

  bool PauseWidget();
  bool ResumeWidget();
  bool CancelTouchEvent();
  void ActivateFaultedWidget();

  static void            SetProperty(BaseObject* object, Property::Index index, const Property::Value& value);
  static Property::Value GetProperty(BaseObject* object, Property::Index propertyIndex);
  static bool            DoAction(BaseObject* object, const Dali::String& actionName, const Property::Map& attributes);
  static bool            DoConnectSignal(BaseObject* object, ConnectionTrackerInterface* tracker, const Dali::String& signalName, FunctorDelegate* functor);

  Dali::Ui::WidgetView::WidgetViewSignalType& WidgetAddedSignal();
  Dali::Ui::WidgetView::WidgetViewSignalType& WidgetDeletedSignal();
  Dali::Ui::WidgetView::WidgetViewSignalType& WidgetCreationAbortedSignal();
  Dali::Ui::WidgetView::WidgetViewSignalType& WidgetContentUpdatedSignal();
  Dali::Ui::WidgetView::WidgetViewSignalType& WidgetUpdatePeriodChangedSignal();
  Dali::Ui::WidgetView::WidgetViewSignalType& WidgetFaultedSignal();

private:
  explicit WidgetView(Dali::WidgetEngineInstance instance, int width, int height);
  ~WidgetView() override;

  void                             OnInitialize() override;
  void                             OnSceneConnection(int depth) override;
  void                             OnSceneDisconnection() override;
  void                             OnPropertySet(Property::Index index, const Property::Value& propertyValue) override;
  bool                             OnKeyEvent(const Dali::KeyEvent& event) override;
  MeasuredSize                     OnMeasure(float widthConstraint, float heightConstraint) override;
  LayoutRect                       OnArrange(const LayoutRect& bounds) override;
  void                             OnSizeSet(const Vector3& targetSize) override;
  void                             OnSizeAnimation(Animation& animation, const Vector3& targetSize) override;
  static Dali::Ui::ViewAccessible* CreateAccessibleObject(Dali::Ui::View view);

  void InitializeLayout();
  void InitializeEvents();
  void InitializeEngineCallbacks();
  void SetPreviewImage();
  void ShowLoadingState(bool show);
  void ShowRetryState(bool show);
  void UpdateWidgetImage(Dali::NativeImagePtr image);
  void HandleEngineEvent(Dali::WidgetEngineInstancePlugin::EventType event);
  void ResizeWidget(const Vector2& size);
  void ResizeOverlayActors(const Vector2& size);
  void ApplyEffect();
  bool OnTouch(Dali::Actor actor, Dali::TouchEvent event);
  bool OnWheel(Dali::Actor actor, Dali::WheelEvent event);
  void OnInheritedVisibilityChanged(Actor actor, bool visible);
  void OnGeometryUpdated(Dali::PropertyNotification notification);
  bool UpdateEngineState();
  bool IsPaused() const;
  bool IsOutOfScreen() const;
  void NotifyAccessibilityChildrenChanged();
  void SynchronizeAccessibilityChildPosition();

  WidgetView(const WidgetView&)            = delete;
  WidgetView& operator=(const WidgetView&) = delete;

protected:
  class WidgetViewAccessible : public Dali::Ui::ViewAccessible
  {
  public:
    explicit WidgetViewAccessible(Dali::Actor self);

    void OnChildrenChanged();
    void SynchronizeChildPosition();

  protected:
    void                                   DoGetChildren(std::vector<Dali::Accessibility::Accessible*>& children) override;
    Dali::Devel::Accessibility::Attributes GetAttributes() const override;

  private:
    bool CanExposeRemoteChild() const;

    Dali::Accessibility::ProxyAccessible mRemoteChild;
  };

private:
  Dali::WidgetEngineInstance mInstance;

  Dali::Property::Map mEffectPropertyMap;
  Dali::Ui::ImageView mPreviewImage;
  Dali::Ui::View      mPreviewActor;
  Dali::Ui::View      mStateTextActor;
  Dali::Ui::Label     mLoadingText;
  Dali::Ui::Label     mRetryText;

  Dali::PropertyNotification mPositionNotification;
  Dali::PropertyNotification mSizeNotification;
  Dali::PropertyNotification mScaleNotification;
  Dali::PropertyNotification mOrientationNotification;

  std::unique_ptr<Dali::EventThreadCallback> mInitializeEngineCallbacksTrigger;

  Dali::Ui::WidgetView::WidgetViewSignalType mWidgetAddedSignal;
  Dali::Ui::WidgetView::WidgetViewSignalType mWidgetDeletedSignal;
  Dali::Ui::WidgetView::WidgetViewSignalType mWidgetCreationAbortedSignal;
  Dali::Ui::WidgetView::WidgetViewSignalType mWidgetContentUpdatedSignal;
  Dali::Ui::WidgetView::WidgetViewSignalType mWidgetUpdatePeriodChangedSignal;
  Dali::Ui::WidgetView::WidgetViewSignalType mWidgetFaultedSignal;

  Vector2 mNaturalSize; ///< Current logical widget size, before UI scaling and provider pixel rounding.
  int     mWidgetWidth;
  int     mWidgetHeight;

  bool mPreviewEnabled{true};
  bool mLoadingTextEnabled{true};
  bool mRetryTextEnabled{true};
  bool mRetryState{false};
  bool mKeepWidgetSize{false};
  bool mPermanentDelete{false};
  bool mPausedManually{false};
  bool mAddedEmitted{false};
  bool mOnScene{false};
  bool mAccessibilityVisible{false};
};

inline Internal::WidgetView& GetImplementation(Dali::Ui::WidgetView& widgetView)
{
  DALI_ASSERT_ALWAYS(widgetView);
  return static_cast<Internal::WidgetView&>(widgetView.GetImplementation());
}

inline const Internal::WidgetView& GetImplementation(const Dali::Ui::WidgetView& widgetView)
{
  DALI_ASSERT_ALWAYS(widgetView);
  return static_cast<const Internal::WidgetView&>(widgetView.GetImplementation());
}
} // namespace Internal
} // namespace Ui
} // namespace DALI_NAMESPACE

#endif // DALI_UI_INTERNAL_WIDGET_VIEW_IMPL_H
