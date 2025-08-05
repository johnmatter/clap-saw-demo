#pragma once

#include "CLAPExport.h"  // Includes all necessary MLVG headers and CLAPAppView
#include "nanovg.h"
#include <string>

// Forward declaration
class ClapSawDemo;

// Minimal GUI class - only implement what's specific to your plugin
class ClapSawDemoGUI : public ml::CLAPAppView<ClapSawDemo> {
public:
  // Constructor
  ClapSawDemoGUI(ClapSawDemo* processor);
  ~ClapSawDemoGUI() override = default;

  // Create your specific widgets
  void makeWidgets() override;

  // Layout widgets with consistent positioning
  void layoutView(ml::DrawContext dc) override;

  // Set up your visual style
  void initializeResources(NativeDrawContext* nvg) override;

private:
  // Helper function to load fonts from disk
  void loadFontFromFile(NativeDrawContext* nvg, const std::string& fontName, const std::string& filePath);
};
