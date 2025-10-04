#pragma once
#include "Pater_MenuController.h"
#include "../../PluginProcessor.h"


namespace MarsDSP
{
    MenuBarController::MenuBarController(EQProcessor& p, MarsDSP::MenuBarView& v) : processor(p), view(v)
    {
        // view.undoButton.onClick    = [&]() { processor.getUndoManager().undo(); };  // TODO: PluginProcessor doesn't have getUndoManager
        // view.redoButton.onClick    = [&]() { processor.getUndoManager().redo(); };
        view.bypassButton.onClick  = [&]() { toggleBypass(); };
        view.settingButton.onClick = [&]() { toggleSettings(); };
        view.infoButton.onClick    = [&]() { toggleInfo(); };
    }
}

