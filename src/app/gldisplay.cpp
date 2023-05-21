#include "gldisplay.h"
#include <iostream>
#include <random>

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

/*!
 * \brief GLDisplay::~GLDisplay
 * 
 * Destructor of gldisplay. Make current OpenGL context 
 * then destroy vbo shader programm and texture
 */
GLDisplay::~GLDisplay()
{
    makeCurrent();
    vbo.destroy();
    delete textures;
    delete program;
    doneCurrent();
}
/*!
 * \brief GLDisplay::resetZoom
 * 
 * Function to resize the window for full display
 * For that you can adject the zoom level ...
 * 
 * TO DO : Fix it !!
 */
void GLDisplay::resetZoom()
{
    float zoom_ = 0;

    //Calculate zoom level needed
    if (width() > textures->width()) {
        // if window width bigger than texture width set zoom level for height !
        zoom_ = float(textures->height()) / float(height());
    } else {
        // if window width smaller than texture width set zoom level for width !
        zoom_ = float(textures->width()) / float(width());
    }

    scale.setToIdentity();
    scale.scale(1 / zoom_);
    translation.setToIdentity();
    update();
}

/*!
 * \brief GLDisplay::setTexture
 * \param unsigned short *buffer pointer to the data from the camera
 */
void GLDisplay::setTexture(unsigned short *bufferT, QMutex *mutex)
{
    QMutexLocker locker(mutex);

    textures->setData(QOpenGLTexture::Red_Integer, QOpenGLTexture::UInt16, bufferT);

    std::clog << "GlDispay : Set Texture : test Value : " << bufferT[400] << std::endl;
}

/*!
 * \brief GLDisplay::wheelEvent
 * \param e : QWheelEvent pointer
 * 
 * Apply a zoom throught the scale matrix when mouse is scroll
 * 
 * TO DO: Make the zoom center on the cursor position and not affect the
 * position of the "croix"
 */
void GLDisplay::wheelEvent(QWheelEvent *e)
{
    QPoint numSteps = e->angleDelta() / 8 / 15;
    float old_zoom = zoom;
    zoom = zoom + numSteps.y() * 0.05;

    if (zoom >= 2) {
        zoom = 2.0;
    }
    if (zoom <= 0.5) {
        zoom = 0.5;
    }

    scale.setToIdentity();
    scale.scale(zoom);

    if (old_zoom != zoom) {
        QPointF mousePos = e->position();
        QPointF mouseNDC = screenToNDC(&mousePos);

        float xnorm = (mousePos.x() - width() / 2) / (width() / 2);
        float ynorm = -(mousePos.y() - height() / 2) / (height() / 2);

        QVector4D p4;
        p4.setX(xnorm);
        p4.setY(ynorm);
        p4.setZ(0.0);
        p4.setW(1.0);

        QVector4D shift = projection.inverted() * scale.inverted() * p4;

        QPointF mouseNDCScale = shift.toPointF();

        float dx = mouseNDCScale.x() - mouseNDC.x();
        float dy = mouseNDCScale.y() - mouseNDC.y();

        croix = croix + QPointF(dx, dy);
    }

    e->accept();
    update();
}

/*!
 * \brief GLDisplay::mousePressEvent
 * \param e : QMouseEvent pointer
 * 
 * When mouse pressed get the mouse position in NDC coordinate.
 * If left button clicked then position the "croix" at the place
 */
void GLDisplay::mousePressEvent(QMouseEvent *e)
{
    QPointF mousePos = e->position();
    lastPos = screenToNDC(&mousePos);

    if (e->buttons() & Qt::LeftButton)
        croix = lastPos;
    update();
}

/*!
 * \brief GLDisplay::mouseMoveEvent
 * \param e QMouseEvent pointer
 * 
 * When mouse move keep track of the position in NDC coordiante.
 * If left button clicked adjuste the "croix position"
 * If right button cliked move the canvas using the translation matrix.
 * 
 * TO DO : adjust translation limits parameter to forbid left and top side
 * to move away from the window edge
 */
void GLDisplay::mouseMoveEvent(QMouseEvent *e)
{
    QPointF mousePos = e->position();
    QPointF currentPos = screenToNDC(&mousePos);

    float dx = currentPos.x() - lastPos.x();
    float dy = currentPos.y() - lastPos.y();

    if (e->buttons() & Qt::LeftButton) {
        // TO DO : Draw lines
        croix = currentPos;
    } else if (e->buttons() & Qt::RightButton) {
        if (std::abs(translation.toTransform().dx() + dx) > 1.0
            || std::abs(translation.toTransform().dy() + dy) > 1.0) {
            translation.translate(0, 0);
        } else {
            translation.translate(dx, dy);
            croix += QPointF(dx, dy);
        }
    }
    lastPos = currentPos;
    update();
}

/*!
 * \brief GLDisplay::mouseReleaseEvent
 * \param e QMouveEvent pointer
 * 
 * Emit the signal clicked when mouse release
 */
void GLDisplay::mouseReleaseEvent(QMouseEvent *e)
{
    emit clicked();
}

/*!
 * \brief GLDisplay::initializeGL
 * 
 * Initialize OpenGL context and some private properties
 * 
 * White clear color
 * transformationmatrix to unity
 * "Croix" to center
 * OldWidth and height to current width and height
 */
void GLDisplay::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(1, 1, 1, 0);

    scale.setToIdentity();
    translation.setToIdentity();

    QPointF pf = QPointF(width() / 2, height() / 2);
    croix = screenToNDC(&pf);

    oldWidth = width();
    oldHeight = height();

    makeObject();
    initShaders();
    initTexture();

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/*!
 * \brief GLDisplay::paintGL
 * 
 * paintGL function called to paint the canvas
 * Send uniform and set-up atribute to work with the 
 * correct shaders ! then draw a quad with the texture
 * being the image from the camera
 */
void GLDisplay::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    textures->bind();

    // Calculate model view transformation

    //scale.scale(0.2);

    //translation.translate(1.0, 0.0, 0.0);

    // Set modelview-projection matrix
    program->setUniformValue("mvp_matrix", projection * translation * scale);

    program->setUniformValue("in_line", NDCToScreenOpengGL(&croix));

    program->setUniformValue("myTexture", 0);

    // Draw
    program->enableAttributeArray(PROGRAM_VERTEX_ATTRIBUTE);
    program->enableAttributeArray(PROGRAM_TEXCOORD_ATTRIBUTE);
    program->setAttributeBuffer(PROGRAM_VERTEX_ATTRIBUTE, GL_FLOAT, 0, 3, 5 * sizeof(GLfloat));
    program->setAttributeBuffer(PROGRAM_TEXCOORD_ATTRIBUTE,
                                GL_FLOAT,
                                3 * sizeof(GLfloat),
                                2,
                                5 * sizeof(GLfloat));
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

/*!
 * \brief GLDisplay::resizeGL
 * \param int width
 * \param int height
 * function called a each resize. Use to keep the corect 
 * aspect ratio of the texture unsing the projection matrix
 * bound to the NDC spacec with the horital dimension allowing
 * to change for keeping the correct aspect ratio.
 */
void GLDisplay::resizeGL(int width, int height)
{
    // Calculate window aspect ratio
    float w_aspect = float(width) / float(height ? height : 1);

    // Calculate texture aspect ratio
    float t_aspect = float(textures->width()) / float(textures->height() ? textures->height() : 1);

    // Reset projection
    projection.setToIdentity();

    // Set othro projection
    projection.ortho(-1 * w_aspect / t_aspect, 1 * w_aspect / t_aspect, -1, 1, -1, 1);
}

/*!
 * \brief GLDisplay::makeObject
 * 
 * Function to create a quad object bound the the full -1,1 Space
 */
void GLDisplay::makeObject()
{
    // Create QUAD object
    const int coords[4][3] = {{-1, -1, 0}, {+1, -1, 0}, {+1, +1, 0}, {-1, +1, 0}};

    // Create vertex
    QList<GLfloat> vertData;
    for (int j = 0; j < 4; ++j) {
        // vertex position
        vertData.append(1.0 * coords[j][0]);
        vertData.append(1.0 * coords[j][1]);
        vertData.append(1.0 * coords[j][2]);
        // Text coord
        vertData.append(j == 1 || j == 2);
        vertData.append(j == 2 || j == 3);
    }

    vbo.create();
    vbo.bind();
    vbo.allocate(vertData.constData(), vertData.count() * sizeof(GLfloat));
}

/*!
 * \brief GLDisplay::initShaders
 * function to compile and link the opengl custom shaders
 * Also set-up the program to work with the correct atributes
 */
void GLDisplay::initShaders()
{
    program = new QOpenGLShaderProgram;

    // Compile vertex shader
    if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shd/shader.vert"))
        close();

    // Compile fragment shader
    if (!program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shd/shader.frag"))
        close();

    program->bindAttributeLocation("vertex", PROGRAM_VERTEX_ATTRIBUTE);
    program->bindAttributeLocation("texCoord", PROGRAM_TEXCOORD_ATTRIBUTE);

    // Link shader pipeline
    if (!program->link())
        close();

    // Bind shader pipeline for use
    if (!program->bind())
        close();
}

/*!
 * \brief GLDisplay::initTexture
 * 
 * Function to initialize the texture
 * 
 * for thorlabs camera to an array of unsigned short
 * For test purpose this array is currently random value
 * 
 * TO DO : support for other camera and data types
 */
void GLDisplay::initTexture()
{
    // Init texture to random values
    buffer = (unsigned short *) malloc(texWidth * texHeigth * sizeof(unsigned short));
    for (int i = 0; i < texWidth; ++i) {
        for (int j = 0; j < texHeigth; ++j) {
            buffer[i * texHeigth + j] = (unsigned short) (rand() % 65535);
        }
    }

    textures = new QOpenGLTexture(QOpenGLTexture::Target2D);

    textures->setMinificationFilter(QOpenGLTexture::Nearest);
    textures->setMagnificationFilter(QOpenGLTexture::Nearest);
    textures->setFormat(QOpenGLTexture::R16U);
    textures->setSize(texWidth, texHeigth);
    textures->setMipLevels(10);
    textures->allocateStorage(QOpenGLTexture::Red_Integer, QOpenGLTexture::UInt16);
    textures->setData(QOpenGLTexture::Red_Integer, QOpenGLTexture::UInt16, buffer);
    //textures = new QOpenGLTexture(QImage(QString(":/test_image.png")).mirrored());
}

/*!
 * \brief GLDisplay::screenToNDC
 * \param QPointF p pointer to a position in screen space 
 * \return QPointF position in NDC spacec
 * 
 * Function to convert screen space in NDC space
 */
QPointF GLDisplay::screenToNDC(QPointF *p)
{
    float xnorm = (p->x() - width() / 2) / (width() / 2);
    float ynorm = -(p->y() - height() / 2) / (height() / 2);

    QVector4D p4;
    p4.setX(xnorm);
    p4.setY(ynorm);
    p4.setZ(0.0);
    p4.setW(1.0);

    QVector4D shift = projection.inverted() * p4;

    return shift.toPointF();
}

/*!
 * \brief GLDisplay::NDCToScreen
 * \param QPointF p pointer to a position in NDC space
 * \return QPoint position in screen space (top left corner 0,0)
 * 
 * function to convert NDC in screen space ROI in top left corner
 */
QPoint GLDisplay::NDCToScreen(QPointF *p)
{
    QVector4D p4;
    p4.setX(p->x());
    p4.setY(p->y());
    p4.setZ(0.0);
    p4.setW(1.0);

    QVector4D pv = projection * p4;

    int xScreen = (pv.x() * (width() / 2)) + (width() / 2);
    int yScreen = (-pv.y() * (height() / 2)) + (height() / 2);

    return QPoint(xScreen, yScreen);
}

/*!
 * \brief GLDisplay::NDCToScreenOpengGL
 * \param QPointF p pointer to a position in NDC space
 * \return QPoint position in screen space (lower left corner 0,0)
 * 
 * function to convert NDC in screen space ROI in lower left corner
 * same as screen space for OpenGl context
 */
QPoint GLDisplay::NDCToScreenOpengGL(QPointF *p)
{
    QVector4D p4;
    p4.setX(p->x());
    p4.setY(p->y());
    p4.setZ(0.0);
    p4.setW(1.0);

    QVector4D pv = projection * p4;

    int xScreen = (pv.x() * (width() / 2)) + (width() / 2);
    int yScreen = (pv.y() * (height() / 2)) + (height() / 2);

    return QPoint(xScreen, yScreen);
}
