// Copyright 2018 The Darwin Neuroevolution Framework Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <string>
using namespace std;

namespace pong {

class Game;

class Player {
 public:
  enum class Action { None, MoveUp, MoveDown };
  enum class Side { Left, Right };

 public:
  Player() = default;
  virtual ~Player() = default;

  virtual void newGame(const Game* game, Side side) {
    game_ = game;
    side_ = side;
  }

  virtual Action action() = 0;
  virtual string name() const = 0;

 protected:
  const Game* game_ = nullptr;
  Side side_ = Side::Left;
};

}  // namespace pong
