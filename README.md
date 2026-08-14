# GDPR-Compliant Laser Selfie Lab

A compact browser-only portrait editor with local HDR-style enhancement, automatic or manual eye placement, seven laser renderers, a browser-camera selfie flow, an eye-local aiming rig, and JPG or PNG export.

The name is deliberately tongue-in-cheek. Local-only processing is privacy-friendly, but the title is not a legal certification and does not replace an actual GDPR assessment.

## Run it

Double-click `index.html`.

The standalone HTML embeds its CSS, JavaScript, WebAssembly filter, face detector, and eye detector. Ordinary file editing does not require a server or an internet connection.

Camera permission remains controlled by the browser. When a browser or device policy blocks camera access from a `file:///` page, serve the folder locally:

```bash
python3 -m http.server 8080
```

Then open `http://localhost:8080`.


## Mobile behaviour

The desktop control deck is unchanged. On touch-first phone layouts up to 900 CSS pixels wide, the app uses a mobile-specific presentation:

- The complete image is always fitted inside the preview stage. A `ResizeObserver`, viewport listeners, and explicit canvas sizing prevent tall portrait images from retaining a stale desktop-sized canvas and being clipped after the control panel appears.
- The preview receives most of the available portrait-screen height. The HDR or laser control area scrolls internally, while the document itself remains fixed to the viewport.
- Safe-area insets and the stable small viewport height are respected on notched phones and browsers with dynamic toolbars.
- In landscape orientation, the preview and controls move side by side.
- The redundant WebRTC **Selfie** button is hidden on coarse-pointer phones. The **Photo** button opens the platform image chooser, which can offer the camera or photo library without forcing either choice.
- If the live browser-camera workflow is invoked on a supported layout, it becomes a full-screen preview on mobile. The video element is muted and inline, camera playback is actively started, and capture remains disabled until a real video frame is available.

The app does not set the file input's `capture` attribute. This is intentional: forcing `capture="user"` can bypass the ordinary library choice on some devices, whereas `accept="image/*"` allows the operating system to present the appropriate camera and photo options.

## Eye-local aim model

The default aim is the original geometry used by the first version of the app:

- the left-eye beam points outward along the negative eye-line axis
- the right-eye beam points outward along the positive eye-line axis
- the visible angle is exactly 180 degrees
- both beams default to continuing outside the image frame

The two selected or detected eye centres define the reference frame. The horizontal axis follows the subject's eye line, including a tilted head. The vertical axis is perpendicular to that line and points toward subject-up. All aiming controls are evaluated in that local frame rather than against the browser window.

The controls have deliberately separated visual jobs:

- **Pitch** adds the same eye-local vertical component to both beams, so both move up or both move down.
- **Roll** adds equal and opposite vertical components, so one beam moves up while the other moves down.
- **Yaw** models a virtual head turn. In a single 2D photograph, true depth cannot be recovered, so yaw is represented through near/far foreshortening, beam reach, width, glow, and a controlled side bias. One beam comes toward the viewer while the other recedes.
- **Spread** is signed. `+100` is the original 180-degree outward aim, `0` is a parallel projection, and negative values produce crossed beams.

Pitch, yaw, roll, and spread use nonlinear response curves. Values near the neutral point produce very small angular changes for precision, while the ends of the sliders retain wide ranges. The spread curve is especially fine around zero, where a small change can move an intersection point a large distance.

The **Original aim** button resets only these aiming values to the original outward geometry. The general **Reset** button continues to reset the complete effect configuration.

The on-image aiming overlay is preview-only. It shows frame exits or visible endpoints, the actual visible angle, and a crossing marker where applicable. It is not included in exported files.

This is a constrained 2.5D photo-compositing rig, not a claim that a single portrait contains enough information for physical 3D ray reconstruction. The mechanisation is designed around predictable, observable effects in the finished image.

## Beam ending

- **Exit frame (recommended)** guarantees that both beam endpoints lie beyond the picture boundary.
- **Visible endpoints** keeps both endpoints inside the image and adds an endpoint energy flare.
- **Outside reach / Endpoint position** adjusts the continuation distance in the selected mode.

## Laser variants

1. **Classic HDR beam**: layered white-hot core, cinematic bloom, and volumetric haze.
2. **Plasma lance**: tapered plasma form, energy knots, and molten core.
3. **Arc lightning**: jagged electrical paths with side branches.
4. **Inferno fire**: flame tongues, heat bloom, and embers.
5. **Cyber neon pulse**: segmented pulses, scan bars, and digital eye rings.
6. **Cosmic prism**: chromatic rays and prismatic sparkles.
7. **Focused cinematic ray**: narrow high-intensity core with restrained flare.

Every style supports adjustable colour, brightness, width, glow, aiming, spread, and reach. Preview rendering is repeated at the source image's full resolution during export.

## Eye placement

Eye detection starts automatically after a file or selfie is opened. The app uses, in order:

1. Browser-native `FaceDetector` landmarks when available.
2. An embedded LBP frontal-face cascade.
3. An embedded Haar eye cascade.
4. Face-guided and global local fallbacks.

No detector model is downloaded. **Manual select** remains available for side profiles, sunglasses, closed eyes, heavy blur, or unusual poses.

## Selfie camera

The camera workflow uses standard browser interfaces:

- `navigator.mediaDevices.getUserMedia()` requests video only, never audio.
- `facingMode: "user"` prefers the front camera.
- `enumerateDevices()` enables camera switching where supported.
- The preview uses `autoplay`, `muted`, `playsinline`, and the WebKit inline-playback attribute.
- Playback is explicitly started and the capture button is enabled only after metadata and a current video frame are available.
- Front-camera preview and capture use matching mirroring.
- Camera tracks stop after capture, cancellation, media removal, replacement, page exit, or navigation.

## Remove-image privacy cleanup

The Remove or Close control:

- stops camera tracks
- cancels pending detection and export work
- closes the decoded `ImageBitmap` where supported
- overwrites retained pixel arrays and the used WebAssembly buffer region
- clears and shrinks canvases
- revokes temporary object URLs
- clears file inputs and application references

Browser JavaScript cannot certify physical RAM zeroisation or directly erase inaccessible browser, decoder, GPU, or operating-system buffers. The app clears all image-derived data it can directly control.

## Export

JPG and PNG are presented as two equal buttons side by side. There is no format drop-down. Each button shows the format as its main label and a concise quality note underneath.

### JPG

The **Export JPG** button creates a high-quality JPEG at quality `0.95`. Transparent source areas are composited over white because JPEG does not support alpha. The button identifies JPG as the smaller-file option.

### PNG

The **Export PNG** button creates a lossless PNG with alpha preserved. This is a standard Canvas PNG containing the HDR-style appearance as SDR pixels. It is not an HDR10, PQ, HLG, 10-bit, or mastering-metadata export.

## Cross-browser UI

The empty-state file control is an actual `<button>` that opens the hidden file input. It does not rely on a styled `<label>`, avoiding Safari's differing label-baseline behaviour. The button also uses explicit appearance reset, flex centring, line-height normalisation, and a WebKit alignment fallback.

The interface uses standards-based Canvas, WebAssembly, File, Blob, MediaDevices, and download APIs. It is intended for current Safari, Chrome, Edge, Firefox, and their modern mobile equivalents.

## Verification performed for this build

Automated Chromium checks covered:

- viewport fit with no document scrolling at 1440x900, 1280x720, 1280x600, 1024x768, 900x650, 768x1024, 390x844, and 360x568
- no clipped aim controls at those dimensions
- exact restoration of the original eye-line geometry
- pitch moving both beams in the same subject-local vertical direction
- roll moving the beams in opposite subject-local vertical directions
- yaw producing the expected near/far scale asymmetry
- positive spread remaining outward, zero spread remaining parallel, and negative spread remaining crossed
- 13,310 combinations of pitch, yaw, roll, spread, and endpoint mode without non-finite geometry
- every outside-mode endpoint lying outside the frame
- every inside-mode endpoint remaining inside the frame
- no JavaScript runtime errors during these checks
- full-image containment at 402x874, 390x664, 412x915, 360x568, 874x402, 1440x900, and 1280x600
- no document scrolling at those sizes, with internal control scrolling where required
- the mobile camera action hidden while the native Photo chooser remains available
- a simulated live `MediaStream` producing a visible 640x480 desktop preview and a full-screen 720x1280 mobile preview before capture was enabled

A physical Safari/WebKit or Android device was not available in the build environment. The mobile checks used Chromium device emulation and a simulated live `MediaStream`, so final acceptance should still include one real iPhone Safari and one real Android browser.

## Files

- `index.html`: ready-to-run self-contained application
- `app.js`: JavaScript extracted from the standalone file for inspection
- `styles.css`: CSS extracted from the standalone file for inspection
- `enhance.c`: source for the HDR-style WebAssembly pixel loop
- `enhance.wasm`: compiled WebAssembly module
- `THIRD_PARTY_NOTICES.md`: detector attribution and licence notice


## Browser notes

- File-picker buttons use a single dynamically updated label, avoiding Safari/WebKit overlap between desktop and mobile button text.
