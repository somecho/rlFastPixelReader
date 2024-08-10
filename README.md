# rlFastPixelReader

A class to obtain pixel data from [Raylib's](https://github.com/raysan5/raylib)
`RenderTexture2D`. It uses PBOs (`GL_PIXEL_PACK_BUFFER`) to utilize OpenGL's
asynchronous calls, making it very fast. 

## Using in projects

There is only one header file. Just drop it into your project. It depends on
`<glad/glad.h>` to make raw OpenGL calls. So make sure you have this
[header](https://github.com/raysan5/raylib/blob/master/src/external/glad.h)
file in your project and include it appropriately.

## Example usage

```cpp
#include <raylib.h>
#include <cmath>
#include "rlFastPixelReader.hpp"

int main() {
  InitWindow(1280, 720, "rlFastPixelReader");
  rlFastPixelReader reader(1280, 720);
  RenderTexture2D target = LoadRenderTexture(1280, 720);
  SetTargetFPS(60);
  while (!WindowShouldClose()) {
    float r = sin(GetTime()) * 0.5 + 0.5;
    BeginTextureMode(target);
    ClearBackground(WHITE);
    DrawCircle(640, 360, r * 300, BLACK);
    EndTextureMode();

    reader.readPixels(target);

    // The size of this buffer is WIDTH * HEIGHT * 4 (4 GL_RGBA channels)
    unsigned char* data = reader.getData();
    // do something with `data`

    BeginDrawing();
    DrawTextureRec(target.texture, (Rectangle){0, 0, 1280, 720}, {0, 0}, WHITE);
    DrawFPS(20, 20);
    EndDrawing();
  }
}
```

---

MIT License, Copyright © 2024 Somē Cho