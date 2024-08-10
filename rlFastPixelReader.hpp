#pragma once
#include <glad/glad.h>
#include <raylib.h>

#include <cstring>

/// A class to obtain pixel data from Raylib's `RenderTexture2D` quickly
class rlFastPixelReader {
 public:
  /// @param _width Width of `RenderTexture2D` to read from
  /// @param _height Height of `RenderTexture2D` to read from
  inline rlFastPixelReader(int _width, int _height)
      : width(_width), height(_height) {
    numBytes = width * height * numChannels;
    setupPbos();
  }

  /// Writes the data from `RenderTexture2D` to a PBO using
  /// `GL_PIXEL_PACK_BUFFER` and `glReadPixels`.
  inline void readPixels(RenderTexture2D src) {
    advanceIndices();
    glReadBuffer(GL_FRONT);

    glBindFramebuffer(GL_FRAMEBUFFER, src.id);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[index]);

    glReadPixels(0, 0, width, height, glType, GL_UNSIGNED_BYTE, nullptr);

    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  /// Maps the data from the PBO using `glMapBuffer` and return its adress in
  /// memory. Unmaps and unbinds the buffer afterwards, in this order.
  inline unsigned char* getData() {
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[nextIndex]);
    unsigned char* data =
        (unsigned char*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    return data;
  }

 private:
  /// Initializes the PBO array and fill each with empty data.
  inline void setupPbos() {
    glGenBuffers(numBuffers, pbos);
    for (GLuint pbo : pbos) {
      glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
      glBufferData(GL_PIXEL_PACK_BUFFER, numBytes, nullptr, GL_STREAM_READ);
    }
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
  }

  /// Advances `index` and `nextIndex` by `1`
  inline void advanceIndices() {
    index = (index + 1) % numBuffers;
    nextIndex = (index + 1) % numBuffers;
  }

  /// For proper synchronization of data, we need 3 buffers
  const static int numBuffers = 3;

  /// Raylib's RenderTexture2D uses GL_RGBA, so we need 4 channels
  const static std::size_t numChannels = 4;

  int index = 0;
  int nextIndex = 0;
  int width, height;
  std::size_t numBytes;
  GLint glType = GL_RGBA;
  GLuint pbos[numBuffers];
};