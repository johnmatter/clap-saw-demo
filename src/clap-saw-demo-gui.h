#pragma once

#include "CLAPExport.h"  // Includes all necessary MLVG headers and CLAPAppView
#include "nanovg.h"

// Forward declaration
class ClapSawDemo;

// Minimal GUI class - only implement what's specific to your plugin
class ClapSawDemoGUI : public ml::CLAPAppView<ClapSawDemo> {
public:
  // Constructor
  ClapSawDemoGUI(ClapSawDemo* processor);
  ~ClapSawDemoGUI() override = default;

  // REQUIRED: Create your specific widgets
  void makeWidgets() override;

  // REQUIRED: Set up your visual style
  void initializeResources(NativeDrawContext* nvg) override;
};
