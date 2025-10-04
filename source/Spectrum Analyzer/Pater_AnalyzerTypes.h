#pragma once

#include "Analyzer/Pater_FFT.h"
#include "Display/Pater_FFTDisplay.h"
#include "Display/Pater_BandDisplay.h"
#include "Display/Pater_MSDisplay.h"
#include "Display/Pater_MenuDisplay.h"

// Forward declare PluginProcessor for EQProcessor typedef
class PluginProcessor;

namespace MarsDSP
{
    // Type aliases for naming consistency across the spectrum analyzer system
    
    // Use FFTAnalyser as the spectrum analyzer implementation
    template <typename Type>
    using SpectrumAnalyser = FFTAnalyser<Type>;
    
    // Alias for main analyzer view (FFT display)
    using AnalyserView = AnalyzerDisplay;
    
    // Alias for band control view
    using BandView = BandDisplay;
    
    // Alias for modulation source view
    using ModulationSourceView = ModulationSourceDisplay;
    
    // Alias for menu bar view
    using MenuBarView = MenuDisplay;
    
    // Alias for main plugin processor (expected by controllers)
    using EQProcessor = ::PluginProcessor;
}
