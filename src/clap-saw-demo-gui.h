#pragma once

#include "MLAppView.h"
#include "MLPlatformView.h"
#include "MLDrawContext.h"
#include "MLDialBasic.h"
#include "MLTextLabelBasic.h"
#include "MLWidget.h"
#include "nanovg.h"

// Forward declaration
class ClapSawDemo;

// Simplified GUI class - plugin developers only need to implement makeWidgets()
class ClapSawDemoGUI : public ml::AppView {
public:
  // Constructor - processor reference is optional
  ClapSawDemoGUI(ClapSawDemo* processor = nullptr);
  ~ClapSawDemoGUI() override = default;

  // REQUIRED: Create widgets - this is the main method plugin developers implement
  void makeWidgets();
  
  // Public method to connect widgets to parameters (called by CLAPExport)
  void connectParameters();

  // OPTIONAL: Custom rendering (background, etc.)
  void render(NativeDrawContext* nvg) override;

  // OPTIONAL: Resource loading
  void initializeResources(NativeDrawContext* nvg) override;

  // OPTIONAL: Event handling
  void onGUIEvent(const GUIEvent& event) override;
  
  // Override to debug event filtering
  // bool willHandleEvent(GUIEvent g);
  // bool pushEvent(GUIEvent g) override;
  
  // Override to handle parameter messages from widgets
  void onMessage(Message msg) override;
  
  // Override animate to ensure event processing in CLAP context
  void animate(NativeDrawContext* nvg) override;

  // Required pure virtual methods from AppView
  void clearResources() override;
  void layoutView(DrawContext dc) override;
  void onResize(Vec2 newSize) override;

private:
  ClapSawDemo* processor;
};
