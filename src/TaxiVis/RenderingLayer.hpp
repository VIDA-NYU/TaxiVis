#ifndef RENDERING_LAYER_HPP
#define RENDERING_LAYER_HPP
#include <GL/glew.h>
#include <qgl.h>
#include <QtCore/QSharedPointer>
#include <QtGui/QImage>

class QPainter;
class QGLShaderProgram;
class QGLFramebufferObject;
class RenderingLayer;
typedef QSharedPointer<QGLShaderProgram>     PQGLShaderProgram;
typedef QSharedPointer<QGLFramebufferObject> PQGLFramebufferObject;

class RenderingLayer
{
public:
  RenderingLayer(bool enabled=true);
  virtual ~RenderingLayer() {}

  bool isEnabled();
  void toggleEnabled();
  
  bool isGLPainting();
  void useGLPainting(QPainter *painter);
  void useQtPainting(QPainter *painter);
  
  bool paint(QPainter *painter, bool currentGLPainting=false);

  virtual void setEnabled(bool b);
  virtual void initGL() {}

  virtual void render(QPainter *painter) = 0;

protected:  
  bool enabled;

private:
  bool glPainting;
};

struct GLBuffer
{
  GLBuffer(): buffer(0), size(0) {}

  void generate() {
    glGenBuffers(1, &this->buffer);
    this->size = 0;
  }

  void setData(GLenum target, GLsizeiptr dataSize, const GLvoid* data, GLenum usage)
  {
    glBindBuffer(target, this->buffer);
    if (this->size<dataSize) {
      glBufferData(target, dataSize, 0, usage);
      this->size = dataSize;
    }
    glBufferSubData(target, 0, dataSize, data);
  }

  GLuint buffer;
  GLsizeiptr size;
};

struct GLTexture
{
  void bind()
  {
    if (!this->size.isValid()) {
      glGenTextures(1, &this->id);
      glBindTexture(GL_TEXTURE_2D, this->id);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    }
    else
      glBindTexture(GL_TEXTURE_2D, this->id);
  }
  
  void ensureSize(const QSize &newSize)
  {
    if (!this->size.isValid() ||
        this->size.width()<newSize.width() ||
        this->size.height()<newSize.height()) {
      this->size = QSize(std::max(this->size.width(), newSize.width()),
                         std::max(this->size.height(), newSize.height()));
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                   this->size.width(), this->size.height(),
                   0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    }
  }

  void setImage(const QImage &img)
  {
    QImage texImg = QGLWidget::convertToGLFormat(img);
    this->bind();
    this->ensureSize(texImg.size());
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                    texImg.width(), texImg.height(),
                    GL_RGBA, GL_UNSIGNED_BYTE, texImg.bits());
    
  }

  GLuint id;
  QSize  size;
};

inline bool RenderingLayer::isEnabled()
{
  return this->enabled;
}

inline void RenderingLayer::toggleEnabled()
{
  this->setEnabled(!this->enabled);
}

inline bool RenderingLayer::isGLPainting()
{
  return this->glPainting;
}

#endif
