# Patriarch Dev Log — Problems, Solutions, and Implementation Details

Date: 2025-10-03 15:00 (local)

This document summarizes the problems addressed during the current session and the concrete solutions and implementation details applied to the Patriarch JUCE plugin project. It’s intended as a personal, detailed engineering log.

---

## Executive Summary

Across this session, we:
- Fixed header include issues and miscellaneous build errors by reorganizing files and normalizing JUCE usage in custom analysers.
- Integrated a real-time spectrum analyzer into the plugin, feeding it from the audio processing path.
- Positioned the spectrum analyzer above the phaser parameters.
- Enlarged the spectrum analyzer view and then reworked the overall layout.
- Increased the plugin window size by 3× and split the vertical space so both the analyzer and phaser parameters are visible and scale appropriately.
- Verified everything builds via the Standalone target.

---

## Problems and Solutions

### 1) Header file inconsistencies and miscellaneous build errors

- Problems:
  - Some DSP headers lived in folders that didn’t match include paths; this caused include path conflicts.
  - Two custom analyzer headers relied on bare JUCE symbols (e.g., `Thread`, `AbstractFifo`, `Path`) without namespacing and lacked explicit module includes, leading to ambiguous or missing symbol errors across platforms.
  - Unfinished Spectrum Analyzer controller sources were being picked up by the build and caused compile errors.

- Solutions:
  - Reorganized DSP header file locations to reflect their module/usage and avoid fragile include paths.
  - Normalized the analyzer classes to explicitly use the `juce::` namespace and added the correct JUCE module includes in those headers.
  - Excluded unfinished Spectrum Analyzer controller sources (and some legacy phaser sources) from the build to prevent unresolved references or mismatched interfaces.

- Implementation details:
  - Files moved (per VCS status):
    - `source/filters/Pater_IIRFilter.h` → `source/DSP/IIR Filters/Pater_IIRFilter.h`
    - `source/filters/Pater_Tanh_IIRFilter.h` → `source/DSP/IIR Filters/Pater_Tanh_IIRFilter.h`
    - `source/dsp/Delay/DelayLine.h` → `source/DSP/Delay Line/Delay/DelayLine.h`
  - Analyzer headers updated and added:
    - `source/Spectrum Analyzer/Analyzer/Pater_FFT.h`
      - Now includes: `juce_core`, `juce_audio_basics`, `juce_dsp`, `juce_graphics`.
      - Uses `juce::Thread`, `juce::AbstractFifo`, `juce::WaitableEvent`, `juce::CriticalSection`, `juce::AudioBuffer`, `juce::Path`, `juce::Rectangle`, and `juce::dsp::FFT/WindowingFunction` explicitly.
      - Starts the worker thread with `startThread()` (no priority arg).
    - `source/Spectrum Analyzer/Analyzer/Pater_MSFFT.h`
      - Same JUCE namespacing and includes normalization; analogous thread and FIFO handling.
  - CMake changes (curated):
    - Registered the analyzer headers with `SharedCode` target.
    - Removed unfinished controller sources from the globbed SourceFiles list:
      - `source/Spectrum Analyzer/Controllers/Pater_AnalyzerController.cpp`
      - `source/Spectrum Analyzer/Controllers/Pater_BandController.cpp`
      - `source/Spectrum Analyzer/Controllers/Pater_MenuController.cpp`
      - `source/Spectrum Analyzer/Controllers/Pater_ModulationSourceController.cpp`
    - Filtered out all Spectrum Analyzer subfolders except `Analyzer` (and other non-Controller parts) from being compiled via:
      - `list(FILTER SourceFiles EXCLUDE REGEX ".*/Spectrum Analyzer/(Controllers|Display|Processor|Params)/.*")`
    - Also removed older/legacy phaser sources from the build list to avoid mismatches:
      - `source/DSP/Phasers/Pater_MonoPhaser.cpp/.h`
      - `source/DSP/Phasers/Pater_StereoPhaser.cpp/.h`

---

### 2) Implement and show the Spectrum Analyzer above the phaser parameters

- Problems:
  - No real-time spectrum view existed in the editor.
  - The editor had to fetch FFT data from the processor safely and draw at a reasonable frame rate.

- Solutions:
  - Added a minimal `MarsDSP::FFTAnalyser<float>` helper that consumes audio in a background thread and exposes a method to generate a `juce::Path` for drawing.
  - Embedded a simple `SpectrumAnalyserComponent` in the editor that repaints at ~30 fps and queries the processor for new FFT data.
  - Integrated analyser creation and feeding inside the processor.

- Implementation details:
  - Processor public interface (PluginProcessor.h/.cpp):
    - Include and instance:
      - `#include "Spectrum Analyzer/Analyzer/Pater_FFT.h"`
      - `std::unique_ptr<MarsDSP::FFTAnalyser<float>> spectrumAnalyser;`
    - Prepare to play:
      - `spectrumAnalyser = std::make_unique<MarsDSP::FFTAnalyser<float>>();`
      - `spectrumAnalyser->setupAnalyser(48000, (float) sampleRate);`
    - Process block:
      - After DSP chain, feed analyser:
        - `spectrumAnalyser->addAudioData(buffer, 0, (int) totalNumOutputChannels);`
    - Editor-facing helpers:
      - `bool checkForNewAnalyserData() const;`
      - `void createAnalyserPlot(juce::Path& p, juce::Rectangle<float> bounds, float minFreq) const;`
  - Editor integration (PluginEditor.h/.cpp):
    - New member `std::unique_ptr<SpectrumAnalyserComponent> spectrumView;`
    - `SpectrumAnalyserComponent` is a local class inside PluginEditor.cpp; it:
      - Runs a `juce::Timer` at 30 Hz.
      - Calls `processor.checkForNewAnalyserData()` and `processor.createAnalyserPlot(...)`.
      - Draws a simple background panel, light grid, and the `juce::Path` of the spectrum.
    - The editor adds and makes the `spectrumView` visible and places it above the parameter group in `resized()`.

---

### 3) Enlarge the spectrum analyzer and then rework layout to always show phaser params

- Problems:
  - Initially, the analyzer area was too small and not scaling with the window.
  - Making the analyzer full-size could hide the phaser controls if the window was small.

- Solutions:
  - First: expanded the analyzer to fill available space below the header (full-width and tall), ensuring it scales with the window.
  - Then: reworked the layout to split the space below the header between analyzer and phaser, ensuring phaser controls remain visible.

- Implementation details (PluginEditor.cpp::resized):
  - Header and margins retained; compute the available vertical space below header.
  - Split the available space: ~65% analyzer, remaining for phaser.
  - Enforce minimum heights:
    - Analyzer: min 100 px.
    - Phaser section: min 180 px.
  - Place analyzer first; then position `phaserGroup` below with standard knob layout.

---

### 4) Increase plugin window size by 3× and ensure scaling

- Problems:
  - Default editor size was small and constrained; scaling needed to be friendlier at large sizes.

- Solutions:
  - Increased default editor size by ~3× and raised resize limits to accommodate very large windows.
  - The dynamic layout logic (above) ensures both analyzer and phaser components scale cleanly.

- Implementation details (PluginEditor.cpp):
  - In the constructor:
    - `setResizeLimits(300, 200, 4000, 3000);`
    - `setSize(2070, 690);`  // previously ~690×230

---

## File-by-File Implementation Notes (Key Changes)

- PluginProcessor.h
  - Added: `#include "Spectrum Analyzer/Analyzer/Pater_FFT.h"`
  - Added members and public analyzer helper methods to expose FFT to editor.

- PluginProcessor.cpp
  - `prepareToPlay`: create and setup the analyser.
  - `processBlock`: feed analyser with current block after DSP.
  - Added `checkForNewAnalyserData()` and `createAnalyserPlot(...)` definitions.

- PluginEditor.h
  - Forward-declared `SpectrumAnalyserComponent`.
  - Added `std::unique_ptr<SpectrumAnalyserComponent> spectrumView;`.

- PluginEditor.cpp
  - Added `SpectrumAnalyserComponent` (inner class) with a `juce::Timer` and drawing logic.
  - Created `spectrumView` in the editor ctor; added to the component hierarchy.
  - Reworked `resized()` to implement the 65% analyzer / 35% phaser split with min sizes.
  - Increased default window size and resize limits.

- Spectrum Analyzer/Analyzer/Pater_FFT.h
  - Fully namespaced to `juce::` and added explicit module includes.
  - Background thread-based FFT with windowing and averaging; exposes `createPath()`.

- Spectrum Analyzer/Analyzer/Pater_MSFFT.h
  - Similar normalization to use `juce::` classes explicitly; ready for modulation source visuals.

- CMakeLists.txt
  - Added analyzer headers to `SharedCode`.
  - Removed unfinished/legacy sources from build to prevent errors.
  - Filtered out problematic Spectrum Analyzer subfolders from the globbed source list.

- Moved DSP headers
  - IIR and Delay Line headers moved into semantically named directories under `source/DSP/...` to make include paths stable and meaningful.

- Parameters (Pater_Params.cpp)
  - Parameter layout present and used by APVTS; includes phaser controls and tempo sync choice.
  - No breaking changes were required for the analyzer work; referenced here for completeness regarding the UI knobs now laid out under the analyzer.

---

## Build & Verification

- Target built: `Pamplejuce_Standalone` (Debug profile).
- Command (CLion did this under the hood):
  - `cmake --build <build-dir> --target Pamplejuce_Standalone`
- Result: Build completed successfully; some static libraries had no symbols, but this did not affect success.
- Runtime expectation:
  - The Standalone app displays a header, a large spectrum analyzer, and below it, the phaser parameter group and knobs. The analyzer updates at ~30 fps and tracks the processed audio output.

---

## How to View

- Open the Standalone app target after building. The spectrum analyzer occupies roughly the top two-thirds of the window; phaser parameters fill the bottom portion.
- Resize the window—both analyzer and parameter areas scale. The phaser retains a minimum height to preserve usability.

---

## Known Limitations / Future Enhancements

- The analyzer currently uses a fixed Kaiser window and a single FFT size (2^12). Exposing an FFT size option and smoothing/hold could enhance UX.
- The analyzer’s grid is minimal; frequency labels and dB lines would make it more informative.
- Some legacy phaser classes are excluded; reconciling the old phaser implementation with the current DSP chain might be desirable later.
- Consider a toggle to collapse/expand the phaser section for even more analyzer space when desired.

---

## Appendix: Quick Diff Summary (high level)

- Added/normalized:
  - `source/Spectrum Analyzer/Analyzer/Pater_FFT.h`
  - `source/Spectrum Analyzer/Analyzer/Pater_MSFFT.h`
  - JUCE namespacing and includes inside analyzers
- Editor UI:
  - New `SpectrumAnalyserComponent` (local to `PluginEditor.cpp`)
  - Larger window and split layout (65% analyzer, min heights)
- Processor:
  - `spectrumAnalyser` member, setup in `prepareToPlay`, fed in `processBlock`
  - Helper methods for editor
- CMake:
  - Add analyzer headers to `SharedCode`
  - Exclude unfinished controllers and some legacy phaser files
- File moves:
  - IIR filters and DelayLine headers reorganized under `source/DSP/...`

---

If you want this document linked in the README, let me know and I’ll add a short reference section.