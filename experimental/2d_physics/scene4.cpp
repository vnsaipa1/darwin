
#include "scene4.h"

#include <core/global_initializer.h>

namespace sandbox_scene_4 {

GLOBAL_INITIALIZER {
  scenesRegistry().add<Factory>("Scene4");
}

Scene::Scene() : phys::Scene(b2Vec2(0, -9.8f), phys::Rect(-100, -100, 200, 200)) {
  constexpr float kOuterBoxSize = 100.0f;
  constexpr float kInnerBoxSize = 99.0f;

  b2BodyDef body_def;
  b2EdgeShape shape;

  // outer box
  fence_ = world_.CreateBody(&body_def);

  shape.Set(b2Vec2(-kOuterBoxSize, -kOuterBoxSize),
            b2Vec2(kOuterBoxSize, -kOuterBoxSize));
  fence_->CreateFixture(&shape, 0.0f);

  shape.Set(b2Vec2(kOuterBoxSize, -kOuterBoxSize), b2Vec2(kOuterBoxSize, kOuterBoxSize));
  fence_->CreateFixture(&shape, 0.0f);

  shape.Set(b2Vec2(kOuterBoxSize, kOuterBoxSize), b2Vec2(-kOuterBoxSize, kOuterBoxSize));
  fence_->CreateFixture(&shape, 0.0f);

  shape.Set(b2Vec2(-kOuterBoxSize, kOuterBoxSize),
            b2Vec2(-kOuterBoxSize, -kOuterBoxSize));
  fence_->CreateFixture(&shape, 0.0f);

  // inner box
  auto inner_box = world_.CreateBody(&body_def);

  shape.Set(b2Vec2(kInnerBoxSize, -kInnerBoxSize), b2Vec2(kInnerBoxSize, kInnerBoxSize));
  inner_box->CreateFixture(&shape, 0.0f);

  shape.Set(b2Vec2(kInnerBoxSize, kInnerBoxSize), b2Vec2(-kInnerBoxSize, kInnerBoxSize));
  inner_box->CreateFixture(&shape, 0.0f);

  shape.Set(b2Vec2(-kInnerBoxSize, kInnerBoxSize),
            b2Vec2(-kInnerBoxSize, -kInnerBoxSize));
  inner_box->CreateFixture(&shape, 0.0f);

  // reaper (bottom of the inner box)
  reaper_ = world_.CreateBody(&body_def);
  shape.Set(b2Vec2(-kInnerBoxSize, -kInnerBoxSize),
            b2Vec2(kInnerBoxSize, -kInnerBoxSize));
  reaper_->CreateFixture(&shape, 0.0f);

  // agitator (top)
  auto top_agitator = phys::addCross(0, 0, 45, 2, &world_);

  b2RevoluteJointDef top_jd;
  top_jd.bodyA = inner_box;
  top_jd.bodyB = top_agitator;
  top_jd.localAnchorA.Set(0.0f, 50.0f);
  top_jd.localAnchorB.Set(0.0f, 0.0f);
  top_jd.referenceAngle = 0.0f;
  top_jd.motorSpeed = -0.5f * b2_pi;
  top_jd.maxMotorTorque = 1e8f;
  top_jd.enableMotor = true;
  world_.CreateJoint(&top_jd);

  // agitator (bottom)
  auto bottom_agitator = phys::addCross(0, 0, 45, 2, &world_);

  b2RevoluteJointDef bottom_jd;
  bottom_jd.bodyA = inner_box;
  bottom_jd.bodyB = bottom_agitator;
  bottom_jd.localAnchorA.Set(0.0f, -40.0f);
  bottom_jd.localAnchorB.Set(0.0f, 0.0f);
  bottom_jd.referenceAngle = 0.0f;
  bottom_jd.motorSpeed = 0.5f * b2_pi;
  bottom_jd.maxMotorTorque = 1e8f;
  bottom_jd.enableMotor = true;
  world_.CreateJoint(&bottom_jd);

  // script (shooting projectiles)
  for (int i = 0; i < 200; ++i) {
    script_.record(i / 20.0f, [&, i](float) {
      phys::addBullet(0, 0, 100 * (i % 2 ? 100 : -100), (i - 50) * 150, &world_);
    });
  }
}

void Scene::postStep() {
  for (auto body : reaped_bodies_) {
    world_.DestroyBody(body);
  }
  reaped_bodies_.clear();
}

void Scene::onContact(b2Contact* contact) {
  if (contact->IsTouching()) {
    const auto body_a = contact->GetFixtureA()->GetBody();
    const auto body_b = contact->GetFixtureB()->GetBody();
    CHECK(body_a != fence_);
    CHECK(body_b != fence_);

    if (body_a == reaper_)
      reaped_bodies_.insert(body_b);
    else if (body_b == reaper_)
      reaped_bodies_.insert(body_a);
  }
}

}  // namespace sandbox_scene_4
