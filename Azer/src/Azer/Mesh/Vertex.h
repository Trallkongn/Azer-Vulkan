#pragma once

namespace Azer {

  struct Vertex2D
  {
    float pos[2];
    float color[3];
  };

  struct Vertex3D
  {
    float pos[3];
    float normal[3];
    float uv[2];
  };
}