#ifndef GLDISPLAY_H
#define GLDISPLAY_H

#include <QMatrix4x4>
#include <QMouseEvent>
#include <QMutex>
#include <QMutexLocker>
#include <QOpenGLBuffer>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLWidget>
#include <iostream>

class GLDisplay : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    using QOpenGLWidget::QOpenGLWidget;
    GLDisplay(QWidget *parent);
    ~GLDisplay();

    void resetZoom();
    void setTexture(unsigned short *buffer, QMutex *mutex);

    // public getter/setter to get image widht and heiht
    int getTexHeigth() const;
    int getTexWidth() const;
    void setTexWidth(int newTexWidth);
    void setTexHeigth(int newTexHeigth);

protected:
    void wheelEvent(QWheelEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int width, int height) override;

    void makeObject();
    void initShaders();
    void initTexture();

signals:
    void clicked();

private:
    int texWidth = 1200;
    int texHeigth = 1000;

    QPointF screenToNDC(QPointF *p);
    QPointF screenToNDCObject(QPointF *p);
    QPoint NDCToScreen(QPointF *p);
    QPoint NDCObjToScreenOpenGL(QPointF *p);
    QPoint NDCToScreenOpengGL(QPointF *p);

    QOpenGLTexture *textures = nullptr;
    QOpenGLShaderProgram *program = nullptr;
    QOpenGLBuffer vbo;

    QMatrix4x4 projection;
    QMatrix4x4 scale;
    QMatrix4x4 translation;

    QColor clearCol = QColor(100, 100, 100);
    float zoom = 1.0;
    QPointF lastPos;
    QPointF lastPosObj;
    QPointF croix;
    int oldWidth;
    int oldHeight;

    unsigned short *private_buffer;
};

#endif // GLDISPLAY_H
