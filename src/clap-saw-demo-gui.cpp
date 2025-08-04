#include "clap-saw-demo-gui.h"
#include "clap-saw-demo.h"

ClapSawDemoGUI::ClapSawDemoGUI(ClapSawDemo* processor)
  : CLAPAppView("ClapSawDemo", processor) {

  // // override grid setup
  setGridSizeDefault(60);
  setGridSizeLimits(30, 120);
  setFixedAspectRatio({10, 10});

}

void ClapSawDemoGUI::makeWidgets() {
  // Add cutoff frequency knob
  _view->_widgets.add_unique<DialBasic>("f0", ml::WithValues{
    {"bounds", {2, 2, 2, 2}},
    // {"range", {10.0f, 10000.0f}},
    {"log", true},
    {"visible", true},
    {"draw_number", true},
    {"param", "f0"}  // Links to parameter (gets default from parameter tree)
  });

  // Add resonance knob
  _view->_widgets.add_unique<DialBasic>("Q", ml::WithValues{
    {"bounds", {6, 2, 2, 2}},
    // {"range", {0.5f, 20.0f}},
    {"visible", true},
    {"draw_number", true},
    {"param", "Q"}  // Links to parameter (gets default from parameter tree)
  });
}

void ClapSawDemoGUI::initializeResources(NativeDrawContext* nvg) {
  if (!nvg) return;

  // Set up visual style for this plugin
  _drawingProperties.setProperty("mark", ml::colorToMatrix({ 0.01, 0.01, 0.01, 1.0 }));
  _drawingProperties.setProperty("mark_bright", ml::colorToMatrix({ 0.9, 0.9, 0.9, 1.0 }));
  _drawingProperties.setProperty("background", ml::colorToMatrix({ 0.8, 0.8, 0.8, 1.0 }));
  _drawingProperties.setProperty("common_stroke_width", 1 / 32.f);

  // Helpful for debugging layout
  // _drawingProperties.setProperty("draw_widget_bounds", true);
}
