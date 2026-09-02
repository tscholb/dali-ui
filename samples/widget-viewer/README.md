# DALI UI Widget Viewer Sample

This sample exercises `Dali::Ui::WidgetViewManager` and `Dali::Ui::WidgetView`.
The provider button (or `P` key) cycles through DALi Toolkit, DALI UI, and NUI
widget providers.

The providers are intentionally not part of this repository. To run the
sample, install:

- `libwidget_viewer_dali-engine`
- `com.samsung.dali.widget` for the Toolkit provider
- `com.samsung.dali.ui.widget` for the optional DALI UI provider
- `Tizen.NUI.WidgetTest` for the optional NUI provider

If an engine or selected provider is unavailable, the sample reports the
failure on screen and remains usable for switching providers or exiting.

Use `1` or Return to resize the red widget, `2` to toggle automatic add/remove
of the blue widget, `3`/`4` to focus a widget for key forwarding, and `F` to
inject a fault into providers that support the sample MessagePort protocol.
Touch input and ContentInfo updates are reported on screen and in the log.
