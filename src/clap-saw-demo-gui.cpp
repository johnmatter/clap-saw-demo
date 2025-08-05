#include "clap-saw-demo-gui.h"
#include "clap-saw-demo.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

ClapSawDemoGUI::ClapSawDemoGUI(ClapSawDemo* processor)
  : CLAPAppView("ClapSawDemo", processor) {

  // // override grid setup
  setGridSizeDefault(60);
  setGridSizeLimits(30, 120);
  setFixedAspectRatio({10, 6}); // 10x4 grid for symmetrical layout

}

void ClapSawDemoGUI::makeWidgets() {
  // Add plugin title (centered at top)
  _view->_backgroundWidgets.add_unique<TextLabelBasic>("title", ml::WithValues{
    {"bounds", {2, 0.2, 6, 0.6}},
    {"text", "madronalib/mlvg demo"},
    {"font", "d_din"},
    {"text_size", 0.7f},
    {"h_align", "center"},
    {"v_align", "middle"},
    {"text_color", ml::colorToMatrix({ 0.01, 0.01, 0.01, 1.0 })}
  });

  // Add cutoff frequency knob (centered on left side)
  _view->_widgets.add_unique<DialBasic>("f0", ml::WithValues{
    {"bounds", {0.0, 1.0, 5.0, 2.5}},
    {"log", true},
    {"visible", true},
    {"draw_number", true},
    {"param", "f0"}
  });

  // Add cutoff label (positioned by layoutView)
  _view->_backgroundWidgets.add_unique<TextLabelBasic>("f0_label", ml::WithValues{
    // {"bounds", {1.5, 1.5, 2.5, 0.4}}, // Temporary bounds, will be positioned by layoutView
    {"text", "Cutoff"},
    {"font", "d_din"},
    {"text_size", 0.7f},
    {"h_align", "center"},
    {"v_align", "middle"},
    {"text_color", ml::colorToMatrix({ 0.01, 0.01, 0.01, 1.0 })}
  });

  // Add resonance knob (centered on right side)
  _view->_widgets.add_unique<DialBasic>("Q", ml::WithValues{
    {"bounds", {5, 1.0, 5.0, 2.5}},
    {"visible", true},
    {"draw_number", true},
    {"param", "Q"}
  });

  // Add resonance label (positioned by layoutView)
  _view->_backgroundWidgets.add_unique<TextLabelBasic>("Q_label", ml::WithValues{
    // {"bounds", {6, 1.5, 2.5, 0.4}}, // Temporary bounds, will be positioned by layoutView
    {"text", "Resonance"},
    {"font", "d_din"},
    {"text_size", 0.7f},
    {"h_align", "center"},
    {"v_align", "middle"},
    {"text_color", ml::colorToMatrix({ 0.01, 0.01, 0.01, 1.0 })}
  });
}

void ClapSawDemoGUI::layoutView(ml::DrawContext dc) {

  // Helper function to position labels under dials consistently
  auto positionLabelUnderDial = [&](ml::Path dialName, ml::Path labelName) {
    // Safety check: ensure both widgets exist before accessing them
    if (!_view->_widgets[dialName] || !_view->_backgroundWidgets[labelName]) {
      return;
    }

    ml::Rect dialRect = _view->_widgets[dialName]->getRectProperty("bounds");
    ml::Rect labelRect(0, 0, 3, 0.4);
    _view->_backgroundWidgets[labelName]->setRectProperty(
      "bounds",
      ml::alignCenterToPoint(labelRect, dialRect.bottomCenter() - ml::Vec2(0, 0.1))
    );
  };

  // Position labels under their corresponding dials
  positionLabelUnderDial("f0", "f0_label");
  positionLabelUnderDial("Q", "Q_label");
}

void ClapSawDemoGUI::loadFontFromFile(NativeDrawContext* nvg, const std::string& fontName, const std::string& filePath) {
  FILE* fontFile = fopen(filePath.c_str(), "rb");
  if (fontFile) {
    // Read font file into memory
    fseek(fontFile, 0, SEEK_END);
    long fontSize = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);

    unsigned char* fontData = (unsigned char*)malloc(fontSize);
    if (fontData) {
      size_t bytesRead = fread(fontData, 1, fontSize, fontFile);
      if (bytesRead == fontSize) {
        // Create FontResource and store in resources map
        _resources.fonts[ml::Path(fontName.c_str())] = std::make_unique<ml::FontResource>(
          nvg, fontName.c_str(), fontData, fontSize, 1  // freeData=1 so FontResource owns the data
        );
      } else {
        free(fontData);
      }
    }
    fclose(fontFile);
  } else {
    // :(
  }
}

void ClapSawDemoGUI::initializeResources(NativeDrawContext* nvg) {
  if (!nvg) return;

  // Set up visual style for this plugin
  _drawingProperties.setProperty("mark", ml::colorToMatrix({ 0.01, 0.01, 0.01, 1.0 }));
  _drawingProperties.setProperty("mark_bright", ml::colorToMatrix({ 0.9, 0.9, 0.9, 1.0 }));
  _drawingProperties.setProperty("background", ml::colorToMatrix({ 0.6, 0.7, 0.8, 1.0 }));
  _drawingProperties.setProperty("common_stroke_width", 1 / 32.f);

  // Load default MLVG fonts (essential for text to work properly)
  // These fonts must be loaded into _resources.fonts before widgets are created
  // so that getFontResource() can find them when widgets render
  loadFontFromFile(nvg, "d_din", "libs/mlvg/examples/app/resources/D-DIN.otf");
  loadFontFromFile(nvg, "d_din_italic", "libs/mlvg/examples/app/resources/D-DIN-Italic.otf");

  // Helpful for debugging layout
  _drawingProperties.setProperty("draw_widget_bounds", true);

}
