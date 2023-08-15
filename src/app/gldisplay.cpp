#include "gldisplay.h"
#include "QThread"
#include <random>

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1

/*!
 * \brief GLDisplay::~GLDisplay
 * 
 * Destructor of gldisplay. Make current OpenGL context 
 * then destroy vbo shader programm and texture
 */
GLDisplay::GLDisplay(QWidget *parent)
    : QOpenGLWidget(parent)
{}

GLDisplay::~GLDisplay()
{
    makeCurrent();
    vbo.destroy();
    delete textures;
    delete program;
    doneCurrent();

    switch (buff_type) {
    case U8:
        delete static_cast<unsigned char *>(private_buffer);
        break;
    case U16:
        delete static_cast<unsigned short *>(private_buffer);
        break;
    case BUFF_END:
        break;
    }
    std::clog << "GLDisplay :: Destructor" << std::endl;
}

/*!
 * \brief GLDisplay::resetZoom
 * 
 * Function to resize the window for full display
 * For that you can adject the zoom level ...
 * 
 * TO DO : Test when zoom on cursor is working !
 */
void GLDisplay::resetZoom()
{
    zoom = 1;
    scale.setToIdentity();
    translation.setToIdentity();
    update();
}

/*!
 * \brief GLDisplay::setTexture
 * \param unsigned short *buffer pointer to the data from the camera
 * \param QMutex *mutex pointer to the mutex for locking buffer 
 * 
 * set texture data depending on buffer type
 * 
 */
void GLDisplay::setTexture(void *buffer, std::mutex *mutex)
{
    std::scoped_lock locker{*mutex};
    switch (buff_type) {
    case U8:
        textures->setData(QOpenGLTexture::Red_Integer, QOpenGLTexture::UInt8, buffer);
        //        std::clog << "GlDispay : Set Texture : test Value : "
        //                  << static_cast<unsigned char *>(buffer)[400] << std::endl;
        break;
    case U16:
        textures->setData(QOpenGLTexture::Red_Integer, QOpenGLTexture::UInt16, buffer);
        //        std::clog << "GlDispay : Set Texture : test Value : "
        //                  << static_cast<unsigned short *>(buffer)[400] << std::endl;
        break;
    case BUFF_END:
        break;
    }
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

    if (zoom >= 5) {
        zoom = 5.0;
    }
    if (zoom <= 0.5) {
        zoom = 0.5;
    }

    scale.setToIdentity();
    scale.scale(zoom);

    if (old_zoom != zoom) {
        QPointF mousePos = e->position();

        float xnorm = (mousePos.x() - width() / 2) / (width() / 2);
        float ynorm = -(mousePos.y() - height() / 2) / (height() / 2);

        QVector4D p4;
        p4.setX(xnorm);
        p4.setY(ynorm);
        p4.setZ(0.0);
        p4.setW(1.0);
    }
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

    if (e->buttons() & Qt::LeftButton) {
        croix = screenToNDCObject(&mousePos);
        update();
    }

    if (e->buttons() & Qt::RightButton) {
        lastPos = screenToNDC(&mousePos);
    }
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
    QPointF currentPosObj = screenToNDCObject(&mousePos);

    float dx = currentPos.x() - lastPos.x();
    float dy = currentPos.y() - lastPos.y();

    QMatrix4x4 denormTrans = projection.inverted() * translation;
    denormTrans.translate(dx, dy);

    if (e->buttons() & Qt::LeftButton) {
        croix = currentPosObj;
        update();
        return;
    }

    // Not perfect but i can live with it
    if (e->buttons() & Qt::RightButton) {
        if (std::abs(denormTrans.toTransform().dx()) > zoom) {
            if (std::abs(denormTrans.toTransform().dy()) > zoom) {
                // X/Y Blocked
                //
                translation.translate(0, 0);
            } else {
                // X blocked
                // Y OK
                translation.translate(0, dy);
                lastPos = QPointF(lastPos.x(), currentPos.y());
            }
        } else {
            if (std::abs(denormTrans.toTransform().dy()) > zoom) {
                // X OK
                // Y Blocked
                translation.translate(dx, 0);
                lastPos = QPointF(currentPos.x(), lastPos.y());
            } else {
                // X/Y OK
                //
                translation.translate(dx, dy);
                lastPos = currentPos;
            }
        }
        lastPos = currentPos;
        update();
    }
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

    // Dark Gray background color
    glClearColor(clearCol.redF(), clearCol.greenF(), clearCol.blueF(), 1);

    scale.setToIdentity();
    translation.setToIdentity();

    QPointF pf = QPointF(width() / 2, height() / 2);
    croix = screenToNDCObject(&pf);

    oldWidth = width();
    oldHeight = height();

    makeObject();
    initShaders();
    initTexture();

    glEnable(GL_CULL_FACE);

    // Not used with camera
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

    // Set modelview-projection matrix
    program->setUniformValue("mvp_matrix", projection * translation * scale);
    program->setUniformValue("in_line", NDCObjToScreenOpenGL(&croix));
    program->setUniformValue("myTexture", 0);
    program->setUniformValue("colorChoice", colorChoice);
    program->setUniformValue("maxVal", maxVal);
    //    std::clog << maxVal << std::endl;

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
 * 
 */
void GLDisplay::resizeGL(int width, int height)
{
    // Calculate window aspect ratio
    float w_aspect = float(width) / float(height ? height : 1);

    // Calculate texture aspect ratio
    float t_aspect = float(textures->width()) / float(textures->height() ? textures->height() : 1);

    // Reset projection
    projection.setToIdentity();

    if (w_aspect > t_aspect) {
        // Set othro projection
        projection.ortho(-1 * w_aspect / t_aspect, 1 * w_aspect / t_aspect, -1, 1, -1, 1);
    } else {
        // Set othro projection
        projection.ortho(-1, 1, -1 * t_aspect / w_aspect, 1 * t_aspect / w_aspect, -1, 1);
    }
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
 * Function to initialize the private buffer to the clear color
 * This image will be the the one displayed before camera start evnet
 * 
 * for thorlabs camera to an array of unsigned short
 * For test purpose this array is currently random value
 * 
 * TO DO : support for other camera and data types
 */
void GLDisplay::initTexture()
{
    maxVal = (int) pow(2, bit_depth) - 1;

    // Texture set-up
    textures = new QOpenGLTexture(QOpenGLTexture::Target2D);
    textures->setMinificationFilter(QOpenGLTexture::Nearest);
    textures->setMagnificationFilter(QOpenGLTexture::Nearest);
    textures->setSize(texWidth, texHeigth);
    textures->setMipLevels(10);

    switch (buff_type) {
    case U8:
        //private_buffer = (char *) malloc(texWidth * texHeigth * sizeof(char));
        private_buffer = new char[texWidth * texHeigth];
        textures->setFormat(QOpenGLTexture::R8U);
        textures->allocateStorage(QOpenGLTexture::Red_Integer, QOpenGLTexture::UInt8);
        break;
    case U16:
        //private_buffer = (unsigned short *) malloc(texWidth * texHeigth * sizeof(unsigned short));
        private_buffer = new unsigned short[texWidth * texHeigth];
        textures->setFormat(QOpenGLTexture::R16U);
        textures->allocateStorage(QOpenGLTexture::Red_Integer, QOpenGLTexture::UInt16);
        break;

    case BUFF_END:
        std::cerr << "GLDisplay Error :: No buffer type" << std::endl;
        break;
    };

    // Initialize the private buffer to dark gray (clear color)
    if (buff_type == U8) {
        for (int i = 0; i < texHeigth; ++i) {
            for (int j = 0; j < texWidth; ++j) {
                static_cast<char *>(private_buffer)[i * texWidth + j] = 0;
            }
        }
        textures->setData(QOpenGLTexture::Red_Integer, QOpenGLTexture::UInt8, private_buffer);

    } else if (buff_type == U16) {
        for (int i = 0; i < texHeigth; ++i) {
            for (int j = 0; j < texWidth; ++j) {
                static_cast<unsigned short *>(private_buffer)[i * texWidth + j] = 0;
            }
        }
        textures->setData(QOpenGLTexture::Red_Integer, QOpenGLTexture::UInt16, private_buffer);
    }
}

/*!
 * \brief GLDisplay::setBit_depth
 * \param newBit_depth
 * 
 * Setter for bit depth
 * 
 */
void GLDisplay::setBit_depth(int newBit_depth)
{
    bit_depth = newBit_depth;
}

/*!
 * \brief GLDisplay::setBuff_type
 * \param newBuff_type
 * 
 * Setter for Buff type
 * 
 */
void GLDisplay::setBuff_type(BUFFTYPE newBuff_type)
{
    buff_type = newBuff_type;
}

/*!
 * \brief GLDisplay::setColorChoice
 * \param newColorChoice
 * 
 * Setter for ColorChoice property
 * 
 */
void GLDisplay::setColorChoice(ColorChoice newColorChoice)
{
    colorChoice = newColorChoice;
    update();
}

/*!
 * \brief GLDisplay::getColorChoice
 * \return 
 * 
 * Getter for colorChoice
 * 
 */
ColorChoice GLDisplay::getColorChoice() const
{
    return colorChoice;
}

/*!
 * \brief GLDisplay::setTexHeigth
 * \param newTexHeigth
 * 
 * Setter for property texHeight
 * 
 */
void GLDisplay::setTexHeigth(int newTexHeigth)
{
    texHeigth = newTexHeigth;
}

/*!
 * \brief GLDisplay::setTexWidth
 * \param newTexWidth
 * 
 * Setter for property texWidth
 * 
 */
void GLDisplay::setTexWidth(int newTexWidth)
{
    texWidth = newTexWidth;
}

/*!
 * \brief GLDisplay::getTexWidth
 * \return texWidth
 * 
 * Getter function for texWidth property
 */
int GLDisplay::getTexWidth() const
{
    return texWidth;
}

/*!
 * \brief GLDisplay::getTexHeigth
 * \return texHeight
 * 
 *  Getter function for texHeight property
 */
int GLDisplay::getTexHeigth() const
{
    return texHeigth;
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
 * \brief GLDisplay::screenToNDCObject
 * \param QPointF p pointer to a position in screen space 
 * \return QPointF position in NDC Object spacec
 * 
 * Function to convert screen space in NDC space (taking the scaling matrix
 * into account)
 */
QPointF GLDisplay::screenToNDCObject(QPointF *p)
{
    float xnorm = (p->x() - width() / 2) / (width() / 2);
    float ynorm = -(p->y() - height() / 2) / (height() / 2);

    QVector4D p4;
    p4.setX(xnorm);
    p4.setY(ynorm);
    p4.setZ(0.0);
    p4.setW(1.0);

    QMatrix4x4 trans = projection * translation * scale;
    QVector4D shift = trans.inverted() * p4;

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
 * \brief GLDisplay::NDCObjToScreen
 * \param QPointF p pointer to a position in NDC Object space
 * \return QPoint position in screen space (lower left corner 0,0)
 * 
 * function to convert NDC in screen space ROI in top left corner
 */
QPoint GLDisplay::NDCObjToScreenOpenGL(QPointF *p)
{
    QVector4D p4;
    p4.setX(p->x());
    p4.setY(p->y());
    p4.setZ(0.0);
    p4.setW(1.0);

    QVector4D pv = projection * translation * scale * p4;

    int xScreen = (pv.x() * (width() / 2)) + (width() / 2);
    int yScreen = (pv.y() * (height() / 2)) + (height() / 2);

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

/*!
 * \brief GLDisplay::getCroix
 * \return QPoint croix
 * 
 * return the buffer coordiante from the croix position
 * 
 */
QPoint GLDisplay::getCroix()
{
    int x = (croix.x() + 1.0) / 2.0 * texWidth;
    int y = (croix.y() + 1.0) / 2.0 * texHeigth;

    if (x < 0) {
        x = 0;
    }
    if (x > (texWidth - 1)) {
        x = texWidth - 1;
    }
    if (y < 0) {
        y = 0;
    }
    if (y > (texHeigth - 1)) {
        y = texHeigth - 1;
    }
    return QPoint(x, y);
}
