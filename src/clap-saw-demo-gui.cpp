#include "clap-saw-demo-gui.h"

#ifdef HAS_GUI
#include "clap-saw-demo.h"
#include <iostream>

ClapSawDemoGUI::ClapSawDemoGUI(ClapSawDemo* p) 
  : ml::AppView("ClapSawDemo", 0), plugin(p) {
  std::cout << "ClapSawDemoGUI: Constructor called" << std::endl;
}

ClapSawDemoGUI::~ClapSawDemoGUI() {
  std::cout << "ClapSawDemoGUI: Destructor called" << std::endl;
  
  // Clean up resources before destroying platform view
  if (platformView) {
    clearResources();
    platformView.reset();
  }
}

void ClapSawDemoGUI::initializeResources(NativeDrawContext* nvg) {
  std::cout << "ClapSawDemoGUI: initializeResources called with nvg: " << nvg << std::endl;
  // This is where we would load fonts, images, etc.
  // For now, just log that we were called
}

void ClapSawDemoGUI::clearResources() {
  std::cout << "ClapSawDemoGUI: clearResources called" << std::endl;
  // Clean up any resources allocated in initializeResources
}

void ClapSawDemoGUI::layoutView(ml::DrawContext dc) {
  std::cout << "ClapSawDemoGUI: layoutView called" << std::endl;
  // This is where we would position widgets based on the current view size
  // For now, just log the call
}

void ClapSawDemoGUI::onGUIEvent(const GUIEvent& event) {
  std::cout << "ClapSawDemoGUI: onGUIEvent called" << std::endl;
  // Handle GUI events like mouse clicks, etc.
  // For now, just log that we received an event
}

void ClapSawDemoGUI::onResize(ml::Vec2 newSize) {
  std::cout << "ClapSawDemoGUI: onResize called: " << newSize.x() << "x" << newSize.y() << std::endl;
  // Handle view resizing
}

void ClapSawDemoGUI::setPlatformWindow(void* platformWindow) {
  this->platformWindow = platformWindow;
  std::cout << "ClapSawDemoGUI: Platform window set: " << platformWindow << std::endl;
  
  if (platformWindow) {
    // Create PlatformView to connect AppView to the platform window
    // This is the missing piece for proper MLVG rendering!
    platformView = std::make_unique<ml::PlatformView>("ClapSawDemo", platformWindow, this, nullptr, 0, 60);
    
    // Initialize resources with the platform view's drawing context
    initializeResources(platformView->getNativeDrawContext());
    
    // Attach the view to make it visible
    platformView->attachViewToParent();
    
    std::cout << "ClapSawDemoGUI: PlatformView created and attached" << std::endl;
  }
}

void ClapSawDemoGUI::showGUI() {
  std::cout << "ClapSawDemoGUI: showGUI called" << std::endl;
  // Make the GUI visible
  // This might involve calling methods on the AppView base class
}

void ClapSawDemoGUI::hideGUI() {
  std::cout << "ClapSawDemoGUI: hideGUI called" << std::endl;
  // Hide the GUI
}

void ClapSawDemoGUI::render(NativeDrawContext* nvg) {
  if (!nvg) return;
  
  // Get current view dimensions
  auto coords = getCoords();
  float width = coords.viewSizeInPixels.x();
  float height = coords.viewSizeInPixels.y();
  
  std::cout << "ClapSawDemoGUI: render called, size: " << width << "x" << height << std::endl;
  
  // Draw pastel teal background using NanoVG HSL
  // Hue=180 (cyan), Saturation=0.4 (muted), Lightness=0.8 (light)
  NVGcolor bgColor = nvgHSL(180.0f / 360.0f, 0.4f, 0.8f);
  
  nvgBeginPath(nvg);
  nvgRect(nvg, 0, 0, width, height);
  nvgFillColor(nvg, bgColor);
  nvgFill(nvg);
  
  // Draw some text to show it's working
  nvgFontSize(nvg, 24.0f);
  nvgFillColor(nvg, nvgRGBA(255, 255, 255, 255)); // White text
  nvgTextAlign(nvg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
  nvgText(nvg, width/2, height/2, "CLAP Saw Demo - MLVG Tutorial", nullptr);
  
  // Draw smaller text for the knobs (placeholder)
  nvgFontSize(nvg, 16.0f);
  nvgText(nvg, width/4, height/2 + 60, "Cutoff", nullptr);
  nvgText(nvg, 3*width/4, height/2 + 60, "Resonance", nullptr);
  
  // Call base class to handle any other rendering
  AppView::render(nvg);
}

#endif // HAS_GUI