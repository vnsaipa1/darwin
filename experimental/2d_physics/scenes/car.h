
#pragma once

#include "sandbox_factory.h"

#include <core/sim/scene.h>
#include <core/sim/accelerometer.h>
#include <core/sim/camera.h>
#include <core/sim/compass.h>
#include <core/sim/touch_sensor.h>
#include <core/properties.h>

#include <QKeyEvent>
#include <QPixmap>

#include <memory>
#include <unordered_map>
using namespace std;

namespace car_scene {

using sim::Accelerometer;
using sim::Camera;
using sim::Compass;
using sim::TouchSensor;

struct Config : public core::PropertySet {
  PROPERTY(car_length, float, 3.0f, "Car length");
  PROPERTY(move_force, float, 10.0f, "The force used to move the car");
  PROPERTY(rotate_torque, float, 1.0f, "The torque used to rotate the car");
};

struct SceneVariables : public core::PropertySet {
  PROPERTY(car_x, float, 0, "Drone x coordinate");
  PROPERTY(car_y, float, 0, "Drone y coordinate");
  PROPERTY(car_vx, float, 0, "Drone velocity (x component)");
  PROPERTY(car_vy, float, 0, "Drone velocity (y component)");
  PROPERTY(car_dir, float, 0, "Heading angle");
};

class Scene : public sim::Scene {
  static constexpr float kWidth = 40;
  static constexpr float kHeight = 20;

 public:
  explicit Scene(const core::PropertySet* config);

  const SceneVariables* variables() const override { return &variables_; }

  const Config* config() const override { return &config_; }

  const Camera* camera() const override { return camera_.get(); }
  const TouchSensor* touchSensor() const override { return touch_sensor_.get(); }
  const Accelerometer* accelerometer() const override { return accelerometer_.get(); }
  const Compass* compass() const override { return compass_.get(); }

  void postStep(float dt) override;

  void accelerate(float force);
  void rotateDrone(float torque);

  void addBalloon(float x, float y, float radius);
  void addBox(float x, float y, float sx, float sy);

 private:
  b2Body* createCar(const b2Vec2& pos, float length);
  b2Body* createDrone(const b2Vec2& pos, float radius);
  void createLight(b2Body* body, const b2Vec2& pos, const b2Color& color);
  void updateVariables();

 private:
  b2Body* car_ = nullptr;
  unique_ptr<Camera> camera_;
  unique_ptr<TouchSensor> touch_sensor_;
  unique_ptr<Accelerometer> accelerometer_;
  unique_ptr<Compass> compass_;
  SceneVariables variables_;
  Config config_;
};

class SceneUi : public physics_ui::Box2dSceneUi {
 public:
  explicit SceneUi(Scene* scene) : scene_(scene) {}

  bool keyPressed(int key) const {
    const auto it = key_state_.find(key);
    return it != key_state_.end() ? it->second : false;
  }

  void render(QPainter& painter, const QRectF&) override;

  void step() override;

  void mousePressEvent(const QPointF& pos, QMouseEvent* event) override;

  void keyPressEvent(QKeyEvent* event) override { key_state_[event->key()] = true; }

  void keyReleaseEvent(QKeyEvent* event) override { key_state_[event->key()] = false; }

  void focusOutEvent() override { key_state_.clear(); }

 private:
  void renderCamera(QPainter& painter, const sim::Camera* camera) const;

 private:
  Scene* scene_ = nullptr;
  unordered_map<int, bool> key_state_;
};

class Factory : public SandboxFactory {
  SandboxScenePackage createScenePackage(const core::PropertySet* config) override {
    auto scene = make_unique<Scene>(config);
    auto scene_ui = make_unique<SceneUi>(scene.get());
    return { std::move(scene), std::move(scene_ui) };
  }

  unique_ptr<core::PropertySet> defaultConfig() const override {
    return make_unique<Config>();
  }
};

}  // namespace car_scene
