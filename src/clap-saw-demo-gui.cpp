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
  setFixedAspectRatio({10, 7}); // 10x4 grid for symmetrical layout

}

void ClapSawDemoGUI::makeWidgets() {

  //  _   _ _   _
  // | |_(_) |_| | ___
  // | __| | __| |/ _ \
  // | |_| | |_| |  __/
  //  \__|_|\__|_|\___|

  _view->_backgroundWidgets.add_unique<TextLabelBasic>("title", ml::WithValues{
    {"bounds", {2, 0.2, 6, 0.6}},
    {"text", "madronalib/mlvg demo"},
    {"font", "d_din"},
    {"text_size", 0.7f},
    {"h_align", "center"},
    {"v_align", "middle"},
    {"text_color", ml::colorToMatrix({ 0.01, 0.01, 0.01, 1.0 })}
  });

  //  _
  // | | ___  _ __   __ _ ___ ___
  // | |/ _ \| '_ \ / _` / __/ __|
  // | | (_) | |_) | (_| \__ \__ \
  // |_|\___/| .__/ \__,_|___/___/
  //         |_|

  // Cutoff
  _view->_widgets.add_unique<DialBasic>("f0", ml::WithValues{
    {"bounds", {0.0, 1.0, 5.0, 2.5}},
    {"log", true},
    {"visible", true},
    {"draw_number", true},
    {"param", "f0"}
  });

  _view->_backgroundWidgets.add_unique<TextLabelBasic>("f0_label", ml::WithValues{
    {"text", "Cutoff"},
    {"font", "d_din"},
    {"text_size", 0.7f},
    {"h_align", "center"},
    {"v_align", "middle"},
    {"text_color", ml::colorToMatrix({ 0.01, 0.01, 0.01, 1.0 })}
  });

  // Resonance
  _view->_widgets.add_unique<DialBasic>("Q", ml::WithValues{
    {"bounds", {5, 1.0, 5.0, 2.5}},
    {"visible", true},
    {"draw_number", true},
    {"param", "Q"}
  });

  _view->_backgroundWidgets.add_unique<TextLabelBasic>("Q_label", ml::WithValues{
    {"text", "Resonance"},
    {"font", "d_din"},
    {"text_size", 0.7f},
    {"h_align", "center"},
    {"v_align", "middle"},
    {"text_color", ml::colorToMatrix({ 0.01, 0.01, 0.01, 1.0 })}
  });

  //            _
  //   __ _  __| |___ _ __
  //  / _` |/ _` / __| '__|
  // | (_| | (_| \__ \ |
  //  \__,_|\__,_|___/_|

  // Attack
  _view->_widgets.add_unique<DialBasic>("attack", ml::WithValues{
    {"bounds", {0.0, 4, 2.5, 2}},
    {"visible", true},
    {"draw_number", true},
    {"param", "attack"}
  });

  _view->_backgroundWidgets.add_unique<TextLabelBasic>("attack_label", ml::WithValues{
    {"text", "Attack"},
    {"font", "d_din"},
    {"text_size", 0.7f},
    {"h_align", "center"},
    {"v_align", "middle"},
    {"text_color", ml::colorToMatrix({ 0.01, 0.01, 0.01, 1.0 })}
  });

  // Decay
  _view->_widgets.add_unique<DialBasic>("decay", ml::WithValues{
    {"bounds", {2.5, 4, 2.5, 2}},
    {"visible", true},
    {"draw_number", true},
    {"param", "decay"}
  });

  _view->_backgroundWidgets.add_unique<TextLabelBasic>("decay_label", ml::WithValues{
    {"text", "Decay"},
    {"font", "d_din"},
    {"text_size", 0.7f},
    {"h_align", "center"},
    {"v_align", "middle"},
    {"text_color", ml::colorToMatrix({ 0.01, 0.01, 0.01, 1.0 })}
  });

  // Sustain
  _view->_widgets.add_unique<DialBasic>("sustain", ml::WithValues{
    {"bounds", {5.0, 4, 2.5, 2}},
    {"visible", true},
    {"draw_number", true},
    {"param", "sustain"}
  });

  _view->_backgroundWidgets.add_unique<TextLabelBasic>("sustain_label", ml::WithValues{
    {"text", "Sustain"},
    {"font", "d_din"},
    {"text_size", 0.7f},
    {"h_align", "center"},
    {"v_align", "middle"},
    {"text_color", ml::colorToMatrix({ 0.01, 0.01, 0.01, 1.0 })}
  });

  // Release
  _view->_widgets.add_unique<DialBasic>("release", ml::WithValues{
    {"bounds", {7.5, 4, 2.5, 2}},
    {"visible", true},
    {"draw_number", true},
    {"param", "release"}
  });

  _view->_backgroundWidgets.add_unique<TextLabelBasic>("release_label", ml::WithValues{
    {"text", "Release"},
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
    ml::Rect labelRect(0, 0, 3, 0.4); // TODO: make this smarter, using label's bounds?
    _view->_backgroundWidgets[labelName]->setRectProperty(
      "bounds",
      ml::alignCenterToPoint(labelRect, dialRect.bottomCenter() - ml::Vec2(0, 0.1))
    );
  };

  // Position lopass dials
  positionLabelUnderDial("f0", "f0_label");
  positionLabelUnderDial("Q", "Q_label");

  // Position ADSR labels
  positionLabelUnderDial("attack", "attack_label");
  positionLabelUnderDial("decay", "decay_label");
  positionLabelUnderDial("sustain", "sustain_label");
  positionLabelUnderDial("release", "release_label");
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
