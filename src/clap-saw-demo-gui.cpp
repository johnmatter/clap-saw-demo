#include "clap-saw-demo-gui.h"
#include "clap-saw-demo.h"

ClapSawDemoGUI::ClapSawDemoGUI(ClapSawDemo* processor) 
  : ml::AppView("ClapSawDemo", 1), processor(processor) {
  
  // Debug: Log construction
  if (processor) {
    processor->logToHost(0, ("ClapSawDemoGUI constructor: this=" + 
                            std::to_string(reinterpret_cast<uintptr_t>(this))).c_str());
  }
  
  // Set up grid system for responsive layout
  setGridSizeDefault(60);
  setGridSizeLimits(30, 120);
  setFixedAspectRatio({10, 6});  // 10x6 grid for our layout
}

void ClapSawDemoGUI::makeWidgets() {
  if (processor) {
    processor->logToHost(0, "Creating widgets...");
  }
  
  // Add cutoff frequency knob - use parameter name "f0"
  _view->_widgets.add_unique<DialBasic>("f0", ml::WithValues{
    {"bounds", {2, 2, 2, 2}},
    {"range", {10.0f, 10000.0f}},  // Match parameter range
    {"default", 1000.0f},
    {"log", true},
    {"visible", true},
    {"draw_number", false},  // Avoid font issues for tutorial
    {"param", "f0"}         // CRITICAL: Tell widget which parameter to control
  });

  // Add resonance knob - use parameter name "Q"
  _view->_widgets.add_unique<DialBasic>("Q", ml::WithValues{
    {"bounds", {6, 2, 2, 2}},
    {"range", {0.01f, 10.0f}},  // Match parameter range
    {"default", 1.4f},          // Match parameter default
    {"visible", true},
    {"draw_number", false},  // Avoid font issues for tutorial
    {"param", "Q"}          // CRITICAL: Tell widget which parameter to control
  });
  
  if (processor) {
    processor->logToHost(0, ("Created " + std::to_string(_view->_widgets.size()) + " widgets").c_str());
  }
}

void ClapSawDemoGUI::connectParameters() {
  // Connect widgets to processor parameters
  if (processor) {
    // Convert Tree to ParameterDescriptionList for _setupWidgets
    ml::ParameterDescriptionList pdl;
    for (const auto& paramDesc : processor->getParameterTree().descriptions) {
      pdl.push_back(std::make_unique<ml::ParameterDescription>(*paramDesc));
      
      // Debug: Log parameter setup
      std::string paramName = std::string(paramDesc->getTextProperty("name").getText());
      processor->logToHost(0, ("Setting up parameter: " + paramName).c_str());
    }
    
    // CRITICAL: Set all widgets to visible first (Aaltoverb pattern)
    ml::forEach<ml::Widget>(_view->_widgets, [&](ml::Widget& w) {
      w.setProperty("visible", true);
      processor->logToHost(0, "Set widget to visible");
    });
    
    processor->logToHost(0, ("Calling _setupWidgets with " + std::to_string(pdl.size()) + " parameters").c_str());
    _setupWidgets(pdl);
    processor->logToHost(0, "Widget parameter setup completed");
  }
}

void ClapSawDemoGUI::render(NativeDrawContext* nvg) {
  // Debug: Log render calls to see if rendering is happening
  static int renderCount = 0;
  if (processor && (renderCount % 60 == 0)) { // Log every 60th frame (once per second at 60fps)
    processor->logToHost(0, ("Render call #" + std::to_string(renderCount)).c_str());
  }
  renderCount++;
  
  // Skip all custom drawing - let MLVG handle everything
  AppView::render(nvg);
}

void ClapSawDemoGUI::initializeResources(NativeDrawContext* nvg) {
  if (!nvg) return;

  // Set up drawing properties - this is likely why widgets aren't visible!
  // Colors from TestAppView example
  _drawingProperties.setProperty("mark", ml::colorToMatrix({ 0.01, 0.01, 0.01, 1.0 }));           // Dark for knob outlines
  _drawingProperties.setProperty("mark_bright", ml::colorToMatrix({ 0.9, 0.9, 0.9, 1.0 }));       // Bright for highlights  
  _drawingProperties.setProperty("background", ml::colorToMatrix({ 0.8, 0.8, 0.8, 1.0 }));        // Light gray background
  _drawingProperties.setProperty("common_stroke_width", 1 / 32.f);
  
  // Helpful for debugging widget visibility
  _drawingProperties.setProperty("draw_widget_bounds", true);
}

// bool ClapSawDemoGUI::willHandleEvent(GUIEvent g) {
//   // Debug: Log all events reaching willHandleEvent
//   if (processor) {
//     std::string msg = "willHandleEvent: type=" + std::string(g.type.getTextFragment().getText()) + 
//                      " pos=(" + std::to_string(g.position.x()) + "," + 
//                      std::to_string(g.position.y()) + ")";
//     processor->logToHost(0, msg.c_str());
//   }
//   
//   // Call base class implementation
//   bool result = AppView::willHandleEvent(g);
//   if (processor) {
//     processor->logToHost(0, ("willHandleEvent returning: " + std::to_string(result)).c_str());
//   }
//   return result;
// }

// bool ClapSawDemoGUI::pushEvent(GUIEvent g) {
//   // Debug: Log all events reaching pushEvent
//   if (processor) {
//     std::string msg = "pushEvent: type=" + std::string(g.type.getTextFragment().getText()) + 
//                      " pos=(" + std::to_string(g.position.x()) + "," + 
//                      std::to_string(g.position.y()) + ")";
//     processor->logToHost(0, msg.c_str());
//   }
//   bool result = AppView::pushEvent(g);
//   if (processor) {
//     processor->logToHost(0, ("pushEvent returning: " + std::to_string(result)).c_str());
//   }
//   return result;
// }

void ClapSawDemoGUI::onGUIEvent(const GUIEvent& event) {
  // Debug: Log grid-converted events to see what widgets receive
  // if (processor) {
  //   std::string msg = "onGUIEvent (grid): type=" + std::string(event.type.getTextFragment().getText()) + 
  //                    " pos=(" + std::to_string(event.position.x()) + "," + 
  //                    std::to_string(event.position.y()) + ")";
  //   processor->logToHost(0, msg.c_str());
  // }
}

void ClapSawDemoGUI::onMessage(Message msg) {
  // Handle parameter messages from widgets - send them to our processor
  if (processor && msg.address) {
    ml::Path addr = msg.address;
    
    // The path from DialBasic is "editor/set_param/param_name"
    if (addr.getSize() > 2 && second(addr) == "set_param") {
      // This is a parameter change from a widget
      ml::Path paramName = tail(tail(addr));
      float normalizedValue = msg.value.getFloatValue();
      
      if (processor) {
        std::string paramNameStr = std::string(pathToText(paramName).getText());
        std::string logMsg = "Parameter change from GUI: " + paramNameStr + 
                            " = " + std::to_string(normalizedValue);
        processor->logToHost(0, logMsg.c_str());
        
        // Update the processor parameter
        processor->setParamFromNormalizedValue(paramNameStr.c_str(), normalizedValue);
      }
    }
  }
  
  // Call base class to handle other message types
  AppView::onMessage(msg);
}

void ClapSawDemoGUI::animate(NativeDrawContext* nvg) {
  // CRITICAL: CLAP-specific workaround for event handling.
  // In plugin contexts, the AppView's _ioTimer may not fire reliably because
  // the host controls the event loop. We manually call _handleGUIEvents() here
  // to ensure GUI events are processed on every frame.
  // Note: The base class animate() does NOT call _handleGUIEvents() anymore,
  // so this is the only place it gets called in CLAP plugins.
  _handleGUIEvents();
  
  // Call base class for widget animations and message handling
  AppView::animate(nvg);
}

void ClapSawDemoGUI::clearResources() {
  // Clean up any resources here
  // For this tutorial, we don't have any custom resources
}

void ClapSawDemoGUI::layoutView(DrawContext dc) {
  // CRITICAL: Resize all widgets so they can be rendered!
  ml::forEach<ml::Widget>(_view->_widgets, [&](ml::Widget& w) {
    w.resize(dc);
  });
}

void ClapSawDemoGUI::onResize(Vec2 newSize) {
  if (processor) {
    processor->logToHost(0, ("onResize: newSize=(" + std::to_string(newSize.x()) + "," + std::to_string(newSize.y()) + ")").c_str());
    processor->logToHost(0, ("_GUICoordinates.gridSizeInPixels = " + std::to_string(_GUICoordinates.gridSizeInPixels)).c_str());
  }
}
