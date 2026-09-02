#ifndef __DALI_UI_WIDGET_VIEW_WIDGET_VIEW_H__
#define __DALI_UI_WIDGET_VIEW_WIDGET_VIEW_H__

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

// EXTERNAL INCLUDES
#include <dali-ui-foundation/public-api/views/view.h>
#include <dali/public-api/common/dali-string.h>
#include <dali/public-api/object/property-map.h>
#include <string>

namespace DALI_NAMESPACE
{

namespace Ui
{

namespace Internal DALI_INTERNAL
{
class WidgetView;
}

/**
 * @addtogroup dali_widget_view
 * @{
 */

/**
 * @brief WidgetView is a class for displaying the widget image and controlling the widget.
 * Input events that WidgetView gets are delivered to the widget.
 * Create widget views using New(). The application and actor hierarchy own the
 * view; releasing their last reference also removes the widget instance.
 * A view disconnected from the scene is paused and can be attached again.
 *
 * Signals
 * <table>
 * <tr>
 *  <th> Signal Name </th>
 *  <th> Method </th>
 * </tr>
 * <tr>
 *  <td> widgetAdded </td>
 *  <td> WidgetAddedSignal() </td>
 * </tr>
 * <tr>
 *  <td> widgetDeleted </td>
 *  <td> WidgetDeletedSignal() </td>
 * </tr>
 * <tr>
 *  <td> widgetCreationAborted </td>
 *  <td> WidgetCreationAbortedSignal() </td>
 * </tr>
 * <tr>
 *  <td> widgetContentUpdated </td>
 *  <td> WidgetContentUpdatedSignal() </td>
 * </tr>
 * <tr>
 *  <td> widgetUpdatePeriodChanged </td>
 *  <td> WidgetUpdatePeriodChangedSignal() </td>
 * </tr>
 * <tr>
 *  <td> widgetFaulted </td>
 *  <td> WidgetFaultedSignal() </td>
 * </tr>
 * </table>
 *
 * @since_tizen 3.0
 */
class DALI_UI_API WidgetView : public View
{
public:
  /**
   * @brief The start and end property ranges for this control.
   * @since_tizen 3.0
   */
  enum PropertyRange
  {
    PROPERTY_START_INDEX = View::PROPERTY_END_INDEX + 1, ///< @since_tizen 3.0
    PROPERTY_END_INDEX   = PROPERTY_START_INDEX + 1000,  ///< Reserve property indices @since_tizen 3.0
  };

  /**
   * @brief Properties for generic GetProperty() and SetProperty() access.
   *
   * Applications can use the corresponding typed methods below. These are
   * event-side properties; they are not registered as animatable properties.
   *
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @since_tizen 3.0
   */
  struct Property
  {
    /**
     * @brief An enumeration of properties belonging to the WidgetView class.
     * @privlevel public
     * @privilege %http://tizen.org/privilege/widget.viewer
     * @since_tizen 3.0
     */
    enum
    {
      // Event side properties

      /**
       * @brief name "widgetId", Get the id of the widget, type string, read-only
       * @since_tizen 3.0
       * @see GetWidgetId()
       */
      WIDGET_ID = PROPERTY_START_INDEX,

      /**
       * @brief name "instanceId", type string, read-only
       * @since_tizen 3.0
       * @see GetInstanceId()
       */
      INSTANCE_ID,

      /**
       * @brief name "contentInfo", type string, read-only
       * @since_tizen 3.0
       * @see GetContentInfo()
       */
      CONTENT_INFO,

      /**
       * @brief name "title", type string, read-only
       * @since_tizen 3.0
       * @see GetTitle()
       */
      TITLE,

      /**
       * @brief name "updatePeriod", type float, read-only
       * @since_tizen 3.0
       * @see GetUpdatePeriod()
       */
      UPDATE_PERIOD,

      /**
       * @brief name "preview", type boolean, controls preview visibility.
       * @since_tizen 3.0
       * @see SetPreviewEnabled(), IsPreviewEnabled()
       */
      PREVIEW,

      /**
       * @brief name "loadingText", type boolean if it is a show/hide flag, map otherwise
       * Setting a boolean changes visibility immediately. Reading returns the
       * loading label's current visibility. A map accepts "stateText",
       * "fontStyle" (a JSON string with weight/width/slant), "textPixelSize",
       * and "textColor".
       * @since_tizen 3.0
       * @see SetLoadingTextProperties(), SetLoadingTextVisible(), IsLoadingTextVisible()
       */
      LOADING_TEXT,

      /**
       * @brief name "widgetStateFaulted", type boolean, read-only
       * @since_tizen 3.0
       * @see IsWidgetFaulted()
       */
      WIDGET_STATE_FAULTED,

      /**
       * @brief name "permanentDelete", type boolean
       * @remarks We recommend not to use this enumeration directly because it was deprecated before.
       * @since_tizen 3.0
       */
      PERMANENT_DELETE,

      /**
       * @brief name "retryText", type Property::MAP
       * Accepts the same text settings as LOADING_TEXT and a "textVisible"
       * boolean that changes visibility immediately. This property is write-only.
       * @since_tizen 4.0
       * @see SetRetryTextProperties(), SetRetryTextVisible(), IsRetryTextVisible()
       */
      RETRY_TEXT,

      /**
       * @brief name "effect", type Property::MAP
       * Accepts a single "shader" map. An empty map clears the effect and
       * restores the standard native-image shader and corner properties.
       * @since_tizen 4.0
       * @see SetEffect()
       */
      EFFECT,

      /**
       * @brief name "keepWidgetSize", type boolean
       * @since_tizen 8.0
       * @see SetKeepWidgetSize(), IsKeepWidgetSize()
       */
      KEEP_WIDGET_SIZE

    };
  };

  /**
   * @brief Creates a view displaying a platform widget.
   *
   * Call this on the event thread after DALi initialization. Widget views share
   * an engine and must use the same viewer application ID for the application
   * lifetime. The engine and service connection are kept until application
   * shutdown, including while no views exist. Destroying a view releases its
   * widget instance and image resources after deferred cleanup.
   * Initial engine notifications are delivered on a subsequent event-loop turn.
   * Connect the view's signals immediately after this function returns to receive
   * them. Destroying the view before that turn discards initial notifications.
   * The widget is paused until the view is connected to the scene and visible.
   *
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @param[in] appId Viewer application ID used for platform IPC.
   * @param[in] widgetId The widget ID.
   * @param[in] contentInfo Contents passed to the widget instance.
   * @param[in] width The widget width in pixels.
   * @param[in] height The widget height in pixels.
   * @param[in] updatePeriod The period of updating widget contents.
   * @return A WidgetView handle, or an empty handle if engine initialization or
   * instance creation fails, or appId differs from the ID used to initialize the
   * shared engine. Instance creation can also fail while a previous widget's
   * deferred resources are being released.
   * @note A valid handle does not mean provider creation has completed. If the
   * instance exists but provider launch or creation fails, the view remains valid
   * and reports the failure through WidgetCreationAbortedSignal(). Connect this
   * signal immediately after New() returns, as described above.
   * @note WRAP_CONTENT initially uses width and height as the logical natural
   * size. Subsequent resizes update that size after removing the effective UI
   * scale, including resizes imposed by parent constraints. Keeping the widget
   * size with SetKeepWidgetSize(true) also preserves its logical natural size.
   */
  static WidgetView New(const std::string& appId, const std::string& widgetId, const std::string& contentInfo, int width, int height, float updatePeriod);

  /**
   * @brief Downcast a handle to WidgetView handle.
   *
   * If the BaseHandle points is a WidgetView the downcast returns a valid handle.
   * If not the returned handle is left empty.
   *
   * @since_tizen 3.0
   * @param[in] handle Handle to an object
   * @return handle to a WidgetView or an empty handle
   */
  static WidgetView DownCast(BaseHandle handle);

  /**
   * @brief Creates an empty handle.
   * @since_tizen 3.0
   */
  WidgetView();

  /**
   * @brief Copy constructor.
   *
   * @since_tizen 3.0
   * @param[in] handle The handle to copy from.
   */
  WidgetView(const WidgetView& handle);

  /**
   * @brief Assignment operator.
   *
   * @since_tizen 3.0
   * @param[in] handle The handle to copy from.
   * @return A reference to this.
   */
  WidgetView& operator=(const WidgetView& handle);

  /**
   * @brief Destructor
   *
   * This is non-virtual since derived Handle types must not contain data or virtual methods.
   * @since_tizen 3.0
   */
  ~WidgetView();

  /**
   * @brief Gets the ID of the widget being displayed.
   *
   * @return The widget ID.
   */
  Dali::String GetWidgetId() const;

  /**
   * @brief Gets the ID of this widget instance.
   *
   * @return The instance ID.
   */
  Dali::String GetInstanceId() const;

  /**
   * @brief Gets the latest available content information from the widget.
   *
   * @return The content information, or the previously stored value if querying fails.
   */
  Dali::String GetContentInfo() const;

  /**
   * @brief Gets the widget title.
   *
   * @return The widget title, or an empty string if unavailable.
   */
  Dali::String GetTitle() const;

  /**
   * @brief Gets the latest known update period for this widget.
   *
   * The value is initialized from New() and refreshed before
   * WidgetUpdatePeriodChangedSignal() is emitted.
   *
   * @return The latest successfully queried update period.
   */
  float GetUpdatePeriod() const;

  /**
   * @brief Queries whether the widget provider is in a faulted state.
   *
   * @return True if the instance reports a faulted state.
   */
  bool IsWidgetFaulted() const;

  /**
   * @brief Sets preview visibility and whether to show it during loading.
   *
   * @param[in] enabled True to show the preview.
   */
  void SetPreviewEnabled(bool enabled);

  /**
   * @brief Gets whether the preview is enabled.
   *
   * @return True if the preview is enabled. This is the setting, not effective visibility.
   */
  bool IsPreviewEnabled() const;

  /**
   * @brief Sets loading text visibility immediately and for subsequent loading states.
   *
   * @param[in] visible True to show the loading text.
   */
  void SetLoadingTextVisible(bool visible);

  /**
   * @brief Gets the loading label's current visibility property.
   *
   * @return True if the label is visible locally; ancestor visibility is not included.
   */
  bool IsLoadingTextVisible() const;

  /**
   * @brief Sets retry text visibility immediately and for subsequent retry states.
   *
   * @param[in] visible True to show the retry text.
   */
  void SetRetryTextVisible(bool visible);

  /**
   * @brief Gets the retry label's current visibility property.
   *
   * @return True if the label is visible locally; ancestor visibility is not included.
   */
  bool IsRetryTextVisible() const;

  /**
   * @brief Sets whether layout changes preserve the provider's current size.
   *
   * @param[in] keepWidgetSize True to keep the provider size.
   * @note Setting false does not resize immediately; a subsequent layout or size
   * change applies the new size.
   */
  void SetKeepWidgetSize(bool keepWidgetSize);

  /**
   * @brief Gets whether the provider size is kept across layout changes.
   *
   * @return True if the provider size is kept.
   */
  bool IsKeepWidgetSize() const;

  /**
   * @brief Updates the loading text settings supplied in the map.
   *
   * Supported keys are "stateText" (string), "fontStyle" (a JSON string with
   * weight/width/slant), "textPixelSize" (float), and "textColor" (Vector4).
   * Omitted settings are unchanged. An empty stateText restores the translated
   * default. Use SetLoadingTextVisible() to change visibility.
   *
   * @param[in] properties The loading text settings.
   */
  void SetLoadingTextProperties(const Dali::Property::Map& properties);

  /**
   * @brief Updates the retry text settings supplied in the map.
   *
   * Accepts the same settings as SetLoadingTextProperties(), plus "textVisible"
   * (boolean), which has the same effect as SetRetryTextVisible().
   * An empty stateText restores the translated retry message.
   *
   * @param[in] properties The retry text settings.
   */
  void SetRetryTextProperties(const Dali::Property::Map& properties);

  /**
   * @brief Sets the shader effect applied to current and subsequent widget frames.
   *
   * Accepts a map containing only a "shader" map. An empty map clears the
   * effect and restores the standard native-image shader and corner handling.
   * Other map layouts are ignored.
   *
   * @param[in] effect The effect settings.
   */
  void SetEffect(const Dali::Property::Map& effect);

  /**
   * @brief Pauses a given widget.
   *
   * A manual pause is retained when visibility or screen position changes.
   *
   * @since_tizen 3.0
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @return true on success, false otherwise.
   */
  bool PauseWidget();

  /**
   * @brief Resume a given widget.
   *
   * Clears the manual pause request. The widget remains paused while hidden,
   * disconnected from the scene, or entirely outside the window. It resumes
   * automatically when these conditions no longer apply.
   *
   * @since_tizen 3.0
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @return true on success, false otherwise.
   */
  bool ResumeWidget();

  /**
   * @brief Cancels touch event procedure.
   * If you call this function after feed the touch down event, the widget will get ON_HOLD events.
   * If a widget gets ON_HOLD event, it will not do anything even if you feed touch up event.
   *
   * @since_tizen 3.0
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @return true on success, false otherwise.
   */
  bool CancelTouchEvent();

  /**
   * @brief Activate a widget in faulted state.
   * Requests a retry for an existing widget whose provider failed.
   * Calling this on a non-faulted or removed widget has no effect.
   * This also resets the per-instance automatic recovery limit.
   *
   * @since_tizen 3.0
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   */
  void ActivateFaultedWidget();

public:                                                  //Signals
  typedef Signal<void(WidgetView)> WidgetViewSignalType; ///< WidgetView signal type @since_tizen 3.0

  /**
   * @brief This signal is emitted when the first widget frame becomes available.
   *
   * It is emitted again when frames resume after a widget fault.
   *
   * @since_tizen 3.0
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @return The signal to connect to.
   */
  WidgetViewSignalType& WidgetAddedSignal();

  /**
   * @brief This signal is emitted when a previously added widget loses its displayed content due to a fault.
   *
   * It is emitted before WidgetFaultedSignal(). The widget can recover and
   * emit WidgetAddedSignal() again.
   * Scene disconnection only pauses the widget and does not emit this signal.
   * Destruction of the view disconnects callbacks and does not emit this signal.
   *
   * @since_tizen 3.0
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @return The signal to connect to.
   */
  WidgetViewSignalType& WidgetDeletedSignal();

  /**
   * @brief This signal is emitted when the widget creation is aborted.
   *
   * @since_tizen 3.0
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @return The signal to connect to.
   */
  WidgetViewSignalType& WidgetCreationAbortedSignal();

  /**
   * @brief This signal is emitted when the widget content is updated.
   *
   * @since_tizen 3.0
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @return The signal to connect to.
   */
  WidgetViewSignalType& WidgetContentUpdatedSignal();

  /**
   * @brief This signal is emitted when the widget update period is changed.
   *
   * @since_tizen 3.0
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @return The signal to connect to.
   */
  WidgetViewSignalType& WidgetUpdatePeriodChangedSignal();

  /**
   * @brief This signal is emitted when the widget process is not running.
   *
   * If a frame was previously added, WidgetDeletedSignal() is emitted first.
   *
   * @since_tizen 3.0
   * @privlevel public
   * @privilege %http://tizen.org/privilege/widget.viewer
   * @return The signal to connect to.
   */
  WidgetViewSignalType& WidgetFaultedSignal();

public: // Not intended for application developers
  /**
   * @internal
   * @brief Creates a handle using the Ui::Internal implementation.
   *
   * @since_tizen 3.0
   * @param[in] implementation The WidgetView implementation.
   */
  DALI_INTERNAL WidgetView(Internal::WidgetView& implementation);

  /**
   * @internal
   * @brief Allows the creation of this control from an Internal::CustomActor pointer.
   *
   * @since_tizen 3.0
   * @param[in] internal A pointer to the internal CustomActor.
   */
  DALI_INTERNAL WidgetView(Dali::Internal::CustomActor* internal);
};

/**
 * @}
 */
} // namespace Ui

} // namespace DALI_NAMESPACE

#endif // __DALI_UI_WIDGET_VIEW_WIDGET_VIEW_H__
