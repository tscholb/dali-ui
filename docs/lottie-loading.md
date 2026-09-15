# Lottie loading lifecycle

## Implemented scope

`LottieAnimationView` no longer uses `mVisualDirty`. Property setters retain their
current values and update an existing visual. Queries and playback commands do not
create a visual. URL replacement and `Reload()` discard the old visual and create
the replacement when the load policy allows it.

| Policy / state | First visual creation and load |
| --- | --- |
| ATTACHED, outside the scene | Deferred |
| ATTACHED, hidden view or hidden ancestor | Deferred |
| ATTACHED, on scene with visible view and ancestors | Allowed |
| IMMEDIATE | Allowed outside the scene and while hidden |

ATTACHED is the default. Visibility here is the actor/ancestor visibility state,
not size, opacity, clipping, or whether the content falls within the viewport.
Hide before Add is covered. Add followed by Hide does not cancel a load that has
already started. `SynchronousLoading` controls how an allowed load runs and does
not override the load policy.

The animated vector visual also enforces LoadPolicy when used directly. It retains
animation data until loading completes so commands cannot schedule rasterization
or allocate raster buffers before loading is allowed. Its existing resend flags
continue to batch runtime updates.

## Deferred state and layout

- Configuration is used in the initial property map. Only the latest requested
  frame and Play/Pause/Stop state are retained, together with owned dynamic callbacks.
- Commands apply after load metadata is available; frame clamping uses that metadata.
- URL changes and Reload clear resource-specific frame/playback requests and callbacks.
  Set these requests after setting the URL or calling Reload.
- Two positive desired dimensions are available without creating a visual. Otherwise
  natural size is zero before creation. Creation and load completion invalidate
  measurement; creation also applies the current arranged size.
- Readiness retains the existing View semantics: only registered visuals are checked.
  Before the main visual is created, IsResourceReady() can return true and a ready
  background can emit ResourceReadySignal even though the Lottie source is deferred.
  GetLoadingStatus() reports PREPARING while the main visual is absent. Placeholders
  follow the same initial creation gate.

## Memory issue coverage

Regression tests count renderer Load calls and raster size requests. Detached and
hidden-before-Add cases must issue none, including when playback APIs are called.
This prevents those paths from reaching Lottie decoding and embedded PNG decoding.
It does not establish the peak memory required by a displayed composition, cancel
an existing load, or diagnose a separate decoder/cache leak. Actual RSS should be
checked with the original PNG-containing file on the target runtime.

## TODO: ImageView and AnimatedImageView update scheduling

Their current dirty behavior is retained. Evaluate this separately:

- Reuse the ViewData processor lifecycle, or register a one-shot processor to apply
  a complete configuration once after consecutive setters.
- Treat processor registration as pending-work state instead of adding another
  independent dirty flag; define deduplication, wake-up, detachment, and destruction.
- Preserve resource replacement, desired-size handling, batch/cache configuration,
  and pre-Add natural-size behavior.
- Verify which changes the visual already merges and which cause immediate resource
  work. Core message batching alone does not merge visual creation or decoder work.
