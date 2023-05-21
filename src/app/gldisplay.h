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

class GLDisplay : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    using QOpenGLWidget::QOpenGLWidget;
    ~GLDisplay();

    void resetZoom();
    void setTexture(unsigned short *bufferT, QMutex *mutex);

    int texWidth;
    int texHeigth;

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
    QPointF screenToNDC(QPointF *p);
    QPoint NDCToScreen(QPointF *p);
    QPoint NDCToScreenOpengGL(QPointF *p);

    QOpenGLTexture *textures = nullptr;
    QOpenGLShaderProgram *program = nullptr;
    QOpenGLBuffer vbo;

    QMatrix4x4 projection;
    QMatrix4x4 scale;
    QMatrix4x4 translation;

    float zoom = 1.0;
    QPointF lastPos;
    QPointF croix;
    int oldWidth;
    int oldHeight;

    unsigned short *buffer;
};

#endif // GLDISPLAY_H
