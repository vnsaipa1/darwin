
#pragma once

#include "physics.h"
#include "sandbox_factory.h"
#include "camera_widget.h"

#include <core/properties.h>
#include <core_ui/box2d_sandbox_window.h>
#include <core_ui/properties_widget.h>

#include <unordered_map>
#include <string>
#include <memory>
using namespace std;

class SandboxWindow : public core_ui::Box2dSandboxWindow {
  struct Variables {
    // simulation state
    core_ui::PropertyItem* state = nullptr;
    core_ui::PropertyItem* step = nullptr;
    core_ui::PropertyItem* timestamp = nullptr;
    core_ui::PropertyItem* objects_count = nullptr;
  };

 public:
  explicit SandboxWindow(SandboxFactory* factory);
  
  void newScene() override;
  void singleStep() override;
  void updateUI() override;

 private:
  void setupVariables();

 private:
  CameraWidget* camera_widget_ = nullptr;
  
  SandboxFactory* factory_ = nullptr;
  SandboxScenePackage scene_package_;
  int step_ = -1;

  Variables variables_;
  unordered_map<string, core_ui::PropertyItem*> scene_variables_map_;
  unordered_map<string, core_ui::PropertyItem*> scene_config_map_;
  
  unique_ptr<core::PropertySet> config_;
};

