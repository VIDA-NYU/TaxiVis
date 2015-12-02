#include "RenderingLayer.hpp"
#include <QtGui/QPainter>

RenderingLayer::RenderingLayer(bool _enabled)
    : enabled(_enabled)
{
}

void RenderingLayer::setEnabled(bool b)
{
  if (this->enabled!=b)
    this->enabled = b;
}

void RenderingLayer::useGLPainting(QPainter *painter)
{
  if (!this->glPainting) {
    painter->beginNativePainting();
    this->glPainting = true;
  }
}

void RenderingLayer::useQtPainting(QPainter *painter)
{
  if (this->glPainting) {
    painter->endNativePainting();
    this->glPainting = false;
  }
}

bool RenderingLayer::paint(QPainter *painter, bool currentGLPainting)
{
  if (this->enabled) {
    this->glPainting = currentGLPainting;
    this->render(painter);
    return this->glPainting;
  }
  return currentGLPainting;
}
