# Compiler and IDE Issues: Symptoms, Root Causes, and Fixes

This document explains a set of recent compiler and IntelliSense/IDE errors encountered in this repository, their root causes, and how they were fixed. It also provides guidance to avoid similar regressions.

Last updated: 2025-10-02 23:15 (local)

---

## 1) Alias template used without template arguments (Pater_Fifo)

Symptoms (compiler errors):
- "use of alias template without template arguments"
- "unknown type name 'Pater_Fifo'" or similar symbol resolution issues

Root cause:
- The project provides a generic lock-free FIFO template in `source/Pater_Fifo.h` under namespace `Fifo` as `Fifo::Fifo<T, Size>`.
- Some code referenced `Pater_Fifo` as if it were a concrete type, or expected a `Pater_Fifo::Fifo<T>` alias to exist, which initially did not.
- When used without specifying template parameters (e.g., `Pater_Fifo` or `Pater_Fifo::Fifo` without `<T>`), the compiler reported alias template usage errors.

Fix implemented:
- `source/Pater_Fifo.h` now provides a project-wide compatibility alias:
  
  namespace Pater_Fifo {
      template<typename T, size_t Size = 30>
      using Fifo = ::Fifo::Fifo<T, Size>;
  }
  
- This allows both forms to work:
  - `Fifo::Fifo<juce::AudioBuffer<float>> myFifo;`
  - `Pater_Fifo::Fifo<juce::AudioBuffer<float>> myFifo;`

How to avoid:
- Always provide template arguments when declaring a FIFO, e.g. `Pater_Fifo::Fifo<MyType>`.
- Prefer consistent use of one namespace (`Pater_Fifo::Fifo<...>`) across the codebase to reduce confusion.

---

## 2) Incorrect JUCE DSP type usage: `juce::dsp::Reverb<float>`

Symptoms (compiler errors):
- "Cannot substitute template argument juce::dsp::Reverb<float> for type template parameter"
- Or simply "no type named 'type' in 'juce::dsp::Reverb'"

Root cause:
- `juce::dsp::Reverb` is not a class template. It should be used as `juce::dsp::Reverb` (without `<float>`), whereas components like `Phaser` and `Chorus` are templated (e.g., `juce::dsp::Phaser<float>`).

Fix implemented:
- Updated `Pater_DSPOption.h` to use `juce::dsp::Reverb` (non-templated) inside the `DSP_Choice` wrapper.
- Also corrected include names to use the actual header `Pater_DSPOption.h` wherever it was referenced incorrectly as `DSPOption.h`.

How to avoid:
- Check JUCE DSP class documentation for whether a component is templated. As a rule of thumb:
  - Templated: `Phaser<T>`, `Chorus<T>`, `DelayLine<T>`
  - Non-templated: `Reverb`

---

## 3) "Cannot resolve symbol" in IDE for project symbols

Examples:
- `Cannot resolve symbol 'phaser'`
- `Cannot resolve symbol 'Pater_Fifo'`

Root causes and fixes:
- `phaser` member: The smoothed parameter storage `float phaser { 0.0f };` was declared in `Pater_Params.h` to match its use in `Pater_Params.cpp`.
- `Pater_Fifo` symbol: See section (1); the namespace alias was added so `Pater_Fifo::Fifo<T>` resolves.

---

## 4) External warning from JUCE (macOS): unhandled enum value

Symptoms (warning):
- From JUCE file `juce_NSViewComponentPeer_mac.mm`, something like "enumeration value not handled in switch".

Context:
- This is in JUCE’s platform code (third-party). It does not originate from this project’s sources and is typically benign. We do not modify third‑party sources in this repo.

What to do:
- Safe to ignore for normal development. If desired, update JUCE to a newer commit where the warning might be addressed, or suppress external warnings in your IDE settings.

---

## Current status
- After the fixes listed above, the project builds in the Debug profile and the Tests target passes (2 assertions in 2 test cases).

## Tips for contributors
- Prefer including the correct project headers (e.g., `Pater_DSPOption.h`) with their exact filenames.
- When adding new DSP components to `DSPOption`, double-check whether classes are templated and match the `DSP_Choice` wrapper accordingly.
- When using the FIFO, always specify template parameters and prefer `Pater_Fifo::Fifo<...>` for consistency.


---

## 5) Undefined references to DSPOption constructor/destructor (linker errors)

Symptoms (linker errors):
- Undefined symbols for architecture x86_64/arm64: "DSPOption::DSPOption()", "DSPOption::~DSPOption()" referenced from various translation units

Root cause:
- The class `DSPOption` declared its constructor and destructor in the header but did not provide definitions in a compiled translation unit.

Fix implemented:
- Added trivial out-of-line definitions in `source/Pater_DSPOption.cpp`:
  
      DSPOption::DSPOption() = default;
      DSPOption::~DSPOption() = default;

How to avoid:
- When declaring special members (ctors/dtor) in headers, ensure you either define them inline in the header or add corresponding definitions in a .cpp file included in the build.

---

## 6) Wiring DSP order to actual processing chain (runtime/logic issue + potential null deref)

Symptoms:
- DSP chain order not applied, or potential runtime issues when trying to process with uninitialized processor pointers.

Root cause:
- The `PluginProcessor` needed to translate the current `DSPOption::DSP_Order` into an array of `juce::dsp::ProcessorBase*` to process in order, but this mapping was missing/incomplete.

Fix implemented:
- In `PluginProcessor::processBlock`, consumed any pending orders from `DSPOption::dspOrderFifo`, updated `currentDspOrder`, built an array of pointers by switching over enum cases, and processed via `juce::dsp::ProcessContextReplacing<float>`.
- Exposed DSPOption members minimally for access from `PluginProcessor`.

How to avoid:
- Keep the enum that encodes order and the storage of processors in one cohesive unit (e.g., `DSPOption`) and provide explicit mapping code with exhaustive `switch` over the enum. Assert on `END_OF_LIST` to catch logic errors in debug builds.

---

## Verification
- Built Debug profile target `Tests` and executed: all tests passed (2 assertions in 2 test cases).
- Manual compile of plugin targets also succeeds in the current workspace.

---

## 7) Runtime crash: Process finished with exit code 139 (SIGSEGV)

Symptoms:
- On launching/processing audio, the app terminated with exit code 139 (SIGSEGV).

Root cause:
- `PluginProcessor::prepareToPlay` did not prepare the JUCE DSP processors (`phaser`, `chorus`, `reverb`, `delay`) with a `juce::dsp::ProcessSpec`. Calling `process()` on unprepared components can dereference uninitialized internal buffers, causing a segmentation fault.

Fix implemented:
- In `PluginProcessor::prepareToPlay`, construct a `juce::dsp::ProcessSpec` from the host-provided `sampleRate`, `samplesPerBlock`, and current channel count.
- Call `prepare(spec)` on all DSP components.
- Initialize a sensible default DSP order and cache it in `currentDspOrder`.
- In `releaseResources`, call `reset()` on all DSP components.

How to avoid:
- Always prepare all DSP processors in `prepareToPlay` before any calls to `process()`.
- Reset/release resources in `releaseResources` to keep internal buffers consistent when playback stops.

---

## 8) CMake error: Cannot find source file ../source/Pater_Tanh_IIRFilter.h (Benchmarks target)

Symptoms (CMake configuration/build error):
- CMake Error at cmake/Benchmarks.cmake:6 (add_executable): Cannot find source file: ../source/Pater_Tanh_IIRFilter.h

Root cause:
- The two newly added filter headers were created under `source/filters/`, but `cmake/Benchmarks.cmake` still referenced them as if they lived directly under `source/`.

Fix implemented:
- Updated `cmake/Benchmarks.cmake` to use the correct paths:
  - `../source/filters/Pater_Tanh_IIRFilter.h`
  - `../source/filters/Pater_IIRFilter.h`

Why additional compile errors appeared afterward:
- The `Benchmarks` target links against `SharedCode`. In this project, `SharedCode` is implemented as an OBJECT library, so its source files (e.g., `source/PluginProcessor.cpp`, `source/Pater_Params.cpp`) are compiled in the context of dependent targets (like `Benchmarks`). This exposed latent compile issues:
  - `PluginProcessor.cpp` had a block trying to cache APVTS parameter pointers into undeclared member fields.
  - `Pater_Params.cpp` was out of sync with `Pater_Params.h`, using old identifiers (`phaserParamID`, `phaserParam`).

Follow-up fixes implemented:
- Removed the incorrect parameter-pointer initialization block from `PluginProcessor`’s constructor; parameters can be retrieved from APVTS on demand.
- Brought `Pater_Params.cpp` in line with `Pater_Params.h`:
  - Constructor now binds all declared Phaser parameter pointers using the `ParameterID`s defined in the header.
  - `Parameters::createParameterLayout()` now declares the corresponding Phaser parameters with sensible ranges and defaults.
  - `prepareToPlay/reset/smoothen` updated to avoid referencing undefined symbols.

Verification:
- Rebuilt `Benchmarks` and `Tests` in Debug; both link successfully and the `Tests` executable passes all tests (2 assertions in 2 test cases).

How to avoid:
- Keep CMake source lists aligned with file moves/additions; prefer using variables or functions to centralize common source lists.
- When `SharedCode` is an OBJECT library, expect consumer targets to compile its sources; this can surface errors in different build targets. Ensure all translation units are self-consistent and in sync with their headers.

