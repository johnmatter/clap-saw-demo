#pragma once

#ifdef HAS_GUI
#include "MLAppView.h"
#include "MLPlatformView.h"
#include "MLDrawContext.h"

// Forward declare the main plugin class
class ClapSawDemo;

// Minimal GUI implementation for tutorial purposes
class ClapSawDemoGUI : public ml::AppView {
public:
  ClapSawDemoGUI(ClapSawDemo* plugin);
  ~ClapSawDemoGUI() override;

  // ml::AppView pure virtual methods implementation
  void initializeResources(NativeDrawContext* nvg) override;
  void clearResources() override;
  void layoutView(ml::DrawContext dc) override;
  void onGUIEvent(const GUIEvent& event) override;
  void onResize(ml::Vec2 newSize) override;
  
  // Override render to draw our background
  void render(NativeDrawContext* nvg) override;

  // Custom methods for CLAP integration
  void setPlatformWindow(void* platformWindow);
  void showGUI();
  void hideGUI();

private:
  ClapSawDemo* plugin;
  void* platformWindow = nullptr;
  std::unique_ptr<ml::PlatformView> platformView;
};

#endif // HAS_GUI