/****************************************************************************

 This file is part of the GLC-lib library.
 Copyright (C) 2005-2008 Laurent Ribon (laumaya@users.sourceforge.net)
 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.

 GLC-lib is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with GLC-lib; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*****************************************************************************/

//! \file glc_viewport.cpp implementation of the GLC_Viewport class.

#include <QtOpenGL>

#include "../glu/glc_glu.h"
#include "glc_viewport.h"
#include "../glc_openglexception.h"
#include "../glc_ext.h"
#include "../shading/glc_selectionmaterial.h"
#include "../glc_state.h"
#include "../sceneGraph/glc_3dviewinstance.h"
#include "../geometry/glc_point.h"
#include "../glc_factory.h"
#include "../glc_context.h"

#include <QtDebug>

using namespace glc;
//////////////////////////////////////////////////////////////////////
// Constructor Destructor
//////////////////////////////////////////////////////////////////////

GLC_Viewport::GLC_Viewport()
    : m_pViewCam(new GLC_Camera())				// Camera
    , m_DistanceMax(500.0)			// Camera Maximum distance
    , m_dDistanceMini(0.01)			// Camera Minimum distance
    , m_ViewAngle(35)					// Camera angle of view
    , m_ViewTangent(tan(glc::toRadian(m_ViewAngle)))
    , m_pImagePlane(NULL)			// Background image
    // OpenGL Window size
    , m_Width(0)				// Horizontal OpenGL viewport size
    , m_Height(1)				// Vertical OpenGL viewport size
    , m_AspectRatio(1.0)
    // the default backgroundColor
    , m_BackgroundColor(Qt::black)
    , m_SelectionSquareSize(4)
    , m_ProjectionMatrix()
    , m_Frustum()
    , m_ClipPlanesHash()
    , m_UseClipPlane(false)
    , m_3DWidgetCollection()
    , m_UseParallelProjection(false)
    , m_MinimumStaticPixelSize(10)
    , m_MinimumStaticRatioSize(0.0)
    , m_MinimumDynamicRatioSize(0.0)
    , m_TextRenderingCollection()
{

}

GLC_Viewport::~GLC_Viewport()
{
	delete m_pViewCam;

	// delete background image
	deleteBackGroundImage();

	// Delete clip planes
	QHash<GLenum, GLC_Plane*>::iterator iClip= m_ClipPlanesHash.begin();
	while (m_ClipPlanesHash.constEnd() != iClip)
	{
		delete iClip.value();
		++iClip;
    }
}

QSize GLC_Viewport::size() const
{
    return QSize(m_Width, m_Height);
}

//////////////////////////////////////////////////////////////////////
// Get Functions
//////////////////////////////////////////////////////////////////////
GLC_Point2d  GLC_Viewport::normalyseMousePosition(int x, int y) const
{
	double nX= 0.0;
	double nY= 0.0;
    if (m_Width != 0)
	{
        nX= static_cast<double>(x) / static_cast<double>(m_Width);
	}

    if (m_Height != 0)
	{
        nY= static_cast<double>(y) / static_cast<double>(m_Height);
	}

	return GLC_Point2d(nX, nY);
}

GLC_Point2d GLC_Viewport::mapToOpenGLScreen(int x, int y) const
{
	GLC_Point2d nPos= normalyseMousePosition(x, y);

	return mapNormalyzeToOpenGLScreen(nPos.x(), nPos.y());
}

GLC_Point2d GLC_Viewport::mapNormalyzeToOpenGLScreen(double x, double y) const
{
	GLC_Point2d pos(x, y);
	pos= pos * 2.0;
	pos.setY(pos.y() * -1.0);
	pos= pos + GLC_Point2d(-1.0, 1.0);
	return pos;
}

GLC_Vector3d GLC_Viewport::mapPosMouse( GLdouble Posx, GLdouble Posy) const
{
	// Change the window origin (Up Left -> centred)
    Posx= Posx - static_cast<double>(m_Width)  / 2.0;
    Posy= static_cast<double>(m_Height) / 2.0 - Posy;

	GLC_Vector3d VectMouse(Posx, Posy,0);

	// Compute the length of camera's field of view
	const double ChampsVision = m_pViewCam->distEyeTarget() *  m_ViewTangent;

	// the side of camera's square is mapped on Vertical length of window
	// Ratio OpenGL/Pixel = dimend GL / dimens Pixel
    const double Ratio= ChampsVision / static_cast<double>(m_Height);

	VectMouse= VectMouse * Ratio;

	return VectMouse;
}

GLC_Vector3d GLC_Viewport::mapNormalyzePosMouse(double Posx, double Posy) const
{
    double screenX= Posx * static_cast<double>(m_Width);
    double screenY= Posy * static_cast<double>(m_Height);
	return mapPosMouse(screenX, screenY);
}

//////////////////////////////////////////////////////////////////////
// Public OpenGL Functions
//////////////////////////////////////////////////////////////////////

void GLC_Viewport::initGl()
{
    QOpenGLFunctions glFuncts(QOpenGLContext::currentContext());

	glClearColor(m_BackgroundColor.redF(), m_BackgroundColor.greenF(), m_BackgroundColor.blueF(), 1.0f);
	glClearDepth(1.0f);                                   // Depth Buffer Setup
    glShadeModel(GL_SMOOTH);                              // Enable Smooth Shading

    // Depth test
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);    // Really Nice Perspective Calculation
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glFuncts.glBlendColor(0.0f, 0.0f, 0.0f, 0.0f);
    glFuncts.glBlendEquation(GL_FUNC_ADD);

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_ALPHA_TEST);
    glDisable(GL_SCISSOR_TEST);
    glEnable(GL_DITHER);
    glDisable(GL_FOG);
    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_INDEX_LOGIC_OP);
    glDisable(GL_COLOR_LOGIC_OP);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_POLYGON_OFFSET_FILL);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glDepthMask(GL_TRUE);

    glEnable(GL_NORMALIZE);
}

void GLC_Viewport::glExecuteCam(const QImage& image, bool preserveRatio)
{
    if (!image.isNull())
    {
        GLC_ImagePlane* pOldImagePlane= m_pImagePlane;
        m_pImagePlane= new GLC_ImagePlane(image, preserveRatio);
        renderImagePlane();
        delete m_pImagePlane;
        m_pImagePlane= pOldImagePlane;
    }
    else
    {
        renderImagePlane();
    }
	m_pViewCam->glExecute();
}

void GLC_Viewport::updateProjectionMat(bool updateOpenGL)
{
	if (m_UseParallelProjection)
	{
        const double height= m_pViewCam->distEyeTarget() * m_ViewTangent;
        const double with= height * m_AspectRatio;
		const double left= -with * 0.5;
		const double right=  -left;
		const double bottom= - height * 0.5;
		const double top= -bottom;
        if (updateOpenGL)
        {
            GLC_Context* pContext= GLC_ContextManager::instance()->currentContext();
            pContext->glcMatrixMode(GL_PROJECTION);
            pContext->glcLoadIdentity();

            pContext->glcOrtho(left, right, bottom, top, m_dDistanceMini, m_DistanceMax);
            m_ProjectionMatrix= pContext->projectionMatrix();
            pContext->glcMatrixMode(GL_MODELVIEW);
        }
        else
        {
            m_ProjectionMatrix= GLC_Matrix4x4::orthonormalMatrix(left, right, bottom, top, m_dDistanceMini, m_DistanceMax);
        }

	}
	else
	{
        const double yMax= m_dDistanceMini * tan(m_ViewAngle * glc::PI / 360.0);
	    const double yMin= -yMax;
	    const double xMax= yMax * m_AspectRatio;
	    const double xMin= -xMax;
        if (updateOpenGL)
        {
            GLC_Context* pContext= GLC_ContextManager::instance()->currentContext();
            pContext->glcMatrixMode(GL_PROJECTION);
            pContext->glcLoadIdentity();

            pContext->glcFrustum(xMin, xMax, yMin, yMax, m_dDistanceMini, m_DistanceMax);
            m_ProjectionMatrix= pContext->projectionMatrix();
            pContext->glcMatrixMode(GL_MODELVIEW);
        }
        else
        {
            m_ProjectionMatrix= GLC_Matrix4x4::frustumMatrix(xMin, xMax, yMin, yMax, m_dDistanceMini, m_DistanceMax);
        }
	}
}

void GLC_Viewport::forceAspectRatio(double ratio)
{
	m_AspectRatio= ratio;
	updateProjectionMat();
}

void GLC_Viewport::updateAspectRatio()
{
	// Update The Aspect Ratio Of The Window
    m_AspectRatio= static_cast<double>(m_Width)/static_cast<double>(m_Height);
}
GLC_Frustum GLC_Viewport::selectionFrustum(int x, int y) const
{
	const int halfSize= m_SelectionSquareSize / 2;
	// Calculate the 4 points of the selection
	//p1->p2
	//
	//p0  p3
	QList<int> coordinates;
	// Point 0
	coordinates << (x - halfSize) << (y + halfSize);
	// Point 1
	coordinates << (x - halfSize) << (y - halfSize);
	// Point 2
	coordinates << (x + halfSize) << (y - halfSize);
	// Point 3
	coordinates << (x + halfSize) << (y + halfSize);

	// Unproject the 4 point
	QList<GLC_Point3d> listOfPoint= unproject(coordinates);

	Q_ASSERT(4 == listOfPoint.size());
	// Create the four frustum planes
	GLC_Point3d eye= m_pViewCam->eye();
	const GLC_Plane leftPlane(listOfPoint.at(0), listOfPoint.at(1), eye);
	const GLC_Plane rightPlane(listOfPoint.at(3), eye , listOfPoint.at(2));
	const GLC_Plane upPlane(listOfPoint.at(2), eye, listOfPoint.at(1));
	const GLC_Plane bottomPlane(listOfPoint.at(0), eye, listOfPoint.at(3));

	GLC_Frustum selectionFrustum(m_Frustum);
	selectionFrustum.setLeftClippingPlane(leftPlane);
	selectionFrustum.setRightClippingPlane(rightPlane);
	selectionFrustum.setTopClippingPlane(upPlane);
	selectionFrustum.setBottomClippingPlane(bottomPlane);

	return selectionFrustum;
}

GLC_Point3d GLC_Viewport::unproject(int x, int y, GLenum buffer, bool onGeometry) const
{
    GLC_Point3d subject;

	// Z Buffer component of the given coordinate is between 0 and 1
	GLfloat Depth;
	// read selected point
    glReadBuffer(buffer);
    glReadPixels(x, m_Height - y , 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &Depth);

    // if on geometry mode and the point is not on geometry return null point
    if (!qFuzzyCompare(Depth, 1.0f) || !onGeometry)
    {
        // The current viewport opengl definition
        GLint viewport[4]= {0, 0, m_Width, m_Height};

        // OpenGL ccordinate of selected point
        GLdouble pX, pY, pZ;
        glc::gluUnProject((GLdouble) x, (GLdouble) (m_Height - y) , Depth
            , m_pViewCam->modelViewMatrix().getData(), m_ProjectionMatrix.getData(), viewport, &pX, &pY, &pZ);

        subject.setVect(pX, pY, pZ);
    }
    return subject;
}

GLC_Point2d GLC_Viewport::project(const GLC_Point3d &point, bool useCameraMatrix) const
{
    GLC_Matrix4x4 modelView;
    GLC_Matrix4x4 projectionMatrix;

    GLint viewport[4]= {0, 0, m_Width, m_Height};
    if (useCameraMatrix)
    {
        modelView= m_pViewCam->modelViewMatrix();
        projectionMatrix= m_ProjectionMatrix;
    }
    else
    {
        modelView= GLC_Context::current()->modelViewMatrix();
        glGetIntegerv(GL_VIEWPORT, viewport);
        projectionMatrix= GLC_Context::current()->projectionMatrix();
    }

    double x;
    double y;
    double z;
    glc::gluProject(point.x(), point.y(), point.z(), modelView.getData(), projectionMatrix.getData(), viewport, &x, &y, &z);

    GLC_Vector2d subject;
    subject.setX(x);
    subject.setY(y);

    return subject;
}

QList<GLC_Point2d> GLC_Viewport::project(const QList<GLC_Point3d> &points, bool useCameraMatrix) const
{
    QList<GLC_Point2d> subject;

    GLC_Matrix4x4 modelView;
    GLC_Matrix4x4 projectionMatrix;

    GLint viewport[4]= {0, 0, m_Width, m_Height};
    if (useCameraMatrix)
    {
        modelView= m_pViewCam->modelViewMatrix();
        projectionMatrix= m_ProjectionMatrix;
    }
    else
    {
        modelView= GLC_Context::current()->modelViewMatrix();
        glGetIntegerv(GL_VIEWPORT, viewport);
        projectionMatrix= GLC_Context::current()->projectionMatrix();
    }

    double x;
    double y;
    double z;

    const int count= points.count();
    for (int i= 0; i < count; ++i)
    {
        const GLC_Point3d point= points.at(i);
        glc::gluProject(point.x(), point.y(), point.z(), modelView.getData(), projectionMatrix.getData(), viewport, &x, &y, &z);
        subject.append(GLC_Point2d(x, y));
    }

    return subject;
}

GLC_Point3d GLC_Viewport::fuzzyUnproject(int x, int y, double z) const
{
    const GLC_Point2d position= mapToOpenGLScreen(x, y);
    const GLC_Point3d position3d(position.x(), position.y(), z);
    const GLC_Point3d subject= compositionMatrix().inverted() * position3d;

    return subject;
}

QList<GLC_Point3d> GLC_Viewport::unproject(const QList<int>& list, GLenum buffer)const
{
    const int size= list.size();
	Q_ASSERT((size % 2) == 0);

	// The current viewport opengl definition
    GLint viewport[4]= {0, 0, m_Width, m_Height};

	// Z Buffer component of the given coordinate is between 0 and 1
	GLfloat Depth;

	// Coordinate of readed points
	GLdouble pX, pY, pZ;
	QList<GLC_Point3d> unprojectedPoints;
	for (int i= 0; i < size; i+= 2)
	{
        const int x= list.at(i);
        const int y= m_Height - list.at(i + 1);
        glReadBuffer(buffer);
		glReadPixels(x, y , 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &Depth);

		glc::gluUnProject(static_cast<GLdouble>(x), static_cast<GLdouble>(y) , Depth , m_pViewCam->modelViewMatrix().getData()
                , m_ProjectionMatrix.getData(), viewport, &pX, &pY, &pZ);
		unprojectedPoints.append(GLC_Point3d(pX, pY, pZ));
	}

    return unprojectedPoints;
}

void GLC_Viewport::renderText(const GLC_Point3d& point, const QString &text, const QColor &color, const QFont &font)
{
    m_TextRenderingCollection.clear();
    if (!text.isEmpty())
    {
        QFontMetrics fontMetrics(font);
        const double width= fontMetrics.width(text);
        const double height= fontMetrics.height();

        // Compute the ratio betwwen screen and world
        const GLC_Matrix4x4 invertedViewMatrix(GLC_Context::current()->modelViewMatrix().rotationMatrix().invert());
        const GLC_Vector2d projectedPoint1(project(point, false));
        const GLC_Vector3d vector(invertedViewMatrix * GLC_Vector3d(0.0, height, 0.0));
        const GLC_Vector2d projectedPoint2(project((point + vector), false));
        const double ratio= height / (projectedPoint2 - projectedPoint1).length();

        QPixmap pixmap(width, height);
        pixmap.fill(Qt::transparent);
        QPainter painter;

        painter.begin(&pixmap);
        painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing);
        painter.setFont(font);
        painter.setPen(color);
        painter.drawText(0, fontMetrics.ascent(), text);
        painter.end();

        QImage image= pixmap.toImage();

        GLC_Texture *pTexture= new GLC_Texture(image);
        GLC_Material* pMaterial= new GLC_Material(Qt::black);
        pMaterial->setTexture(pTexture);
        pMaterial->setOpacity(0.99);

        GLC_3DViewInstance rectangle= GLC_Factory::instance()->createRectangle(width, height);

        GLC_Matrix4x4 scaleMatrix;
        scaleMatrix.setMatScaling(ratio, ratio, ratio);
        rectangle.setMatrix(scaleMatrix);
        rectangle.multMatrix(invertedViewMatrix);
        rectangle.multMatrix(GLC_Matrix4x4(point));
        rectangle.geomAt(0)->addMaterial(pMaterial);
        m_TextRenderingCollection.add(rectangle);

        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glDisable(GL_DEPTH_TEST);
        m_TextRenderingCollection.render(0, glc::TransparentRenderFlag);
        glEnable(GL_DEPTH_TEST);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        m_TextRenderingCollection.clear();
    }
}

//////////////////////////////////////////////////////////////////////
// Private OpenGL Functions
//////////////////////////////////////////////////////////////////////

void GLC_Viewport::renderImagePlane()
{
	if(!GLC_State::isInSelectionMode())
	{
		if (m_pImagePlane != NULL)
		{
            m_pImagePlane->render(m_AspectRatio);
		}
	}
}

void GLC_Viewport::render3DWidget()
{
	m_3DWidgetCollection.render(0, glc::WireRenderFlag);
	m_3DWidgetCollection.render(0, glc::TransparentRenderFlag);
}
//////////////////////////////////////////////////////////////////////
// Set Functions
//////////////////////////////////////////////////////////////////////

void GLC_Viewport::setWinGLSize(int width, int height, bool updateOpenGL)
{
    m_Width= width;
    m_Height= height;

    // Prevent A Divide By Zero By
    if (m_Height == 0)
    {
        m_Height= 1;
    }

    updateAspectRatio();
    updateMinimumRatioSize();

    if (updateOpenGL)
    {
        glViewport(0,0,m_Width,m_Height);
        updateProjectionMat();
    }
}

void GLC_Viewport::setWinGLSize(const QSize &size, bool updateOpenGL)
{
    setWinGLSize(size.width(), size.height(), updateOpenGL);
}

GLC_uint GLC_Viewport::renderAndSelect(int x, int y, GLenum buffer)
{
	const QColor clearColor(Qt::black);
	glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), 1.0f);
	GLC_State::setSelectionMode(true);
	// Draw the scene
	emit updateOpenGL();
	GLC_State::setSelectionMode(false);

    return selectOnPreviousRender(x, y, buffer);
}

GLC_uint GLC_Viewport::selectOnPreviousRender(int x, int y, GLenum buffer)
{
	GLsizei width= m_SelectionSquareSize;
	GLsizei height= width;
	GLint newX= x - width / 2;
    GLint newY= (m_Height - y) - height / 2;
	if (newX < 0) newX= 0;
	if (newY < 0) newY= 0;

    return meaningfullIdInsideSquare(newX, newY, width, height, buffer);
}
GLC_uint GLC_Viewport::selectBody(GLC_3DViewInstance* pInstance, int x, int y, GLenum buffer)
{
    GLC_Context* pContext= GLC_ContextManager::instance()->currentContext();

	const QColor clearColor(Qt::black);
	glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), 1.0f);
	GLC_State::setSelectionMode(true);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    pContext->glcLoadIdentity();

	glExecuteCam();

	// Draw the scene
	glDisable(GL_BLEND);
    pContext->glcEnableLighting(false);
	glDisable(GL_TEXTURE_2D);

	pInstance->renderForBodySelection();
	GLC_State::setSelectionMode(false);

	GLsizei width= 6;
	GLsizei height= width;
	GLint newX= x - width / 2;
    GLint newY= (m_Height - y) - height / 2;
	if (newX < 0) newX= 0;
	if (newY < 0) newY= 0;

    return meaningfullIdInsideSquare(newX, newY, width, height, buffer);
}

QPair<int, GLC_uint> GLC_Viewport::selectPrimitive(GLC_3DViewInstance* pInstance, int x, int y, GLenum buffer)
{
    GLC_Context* pContext= GLC_ContextManager::instance()->currentContext();

	QPair<int, GLC_uint> result;

	const QColor clearColor(Qt::black);
	glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), 1.0f);
	GLC_State::setSelectionMode(true);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    pContext->glcLoadIdentity();

	glExecuteCam();

	// Draw the scene
	glDisable(GL_BLEND);
    pContext->glcEnableLighting(false);
	glDisable(GL_TEXTURE_2D);

	pInstance->renderForBodySelection();


	GLsizei width= 6;
	GLsizei height= width;
	GLint newX= x - width / 2;
    GLint newY= (m_Height - y) - height / 2;
	if (newX < 0) newX= 0;
	if (newY < 0) newY= 0;

    GLC_uint bodyId= meaningfullIdInsideSquare(newX, newY, width, height, buffer);
	if (bodyId == 0)
	{
		result.first= -1;
		result.second= 0;
	}
	else
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		result.first= pInstance->renderForPrimitiveSelection(bodyId);
        result.second= meaningfullIdInsideSquare(newX, newY, width, height, buffer);
	}
	GLC_State::setSelectionMode(false);
	return result;
}

QSet<GLC_uint> GLC_Viewport::selectInsideSquare(int x1, int y1, int x2, int y2, GLenum buffer)
{
	if (x1 > x2)
	{
		int xTemp= x1;
		x1= x2;
		x2= xTemp;
	}
	if (y2 > y1)
	{
		int yTemp= y1;
		y1= y2;
		y2= yTemp;
	}
	const QColor clearColor(Qt::black);
	glClearColor(clearColor.redF(), clearColor.greenF(), clearColor.blueF(), 1.0f);
	GLC_State::setSelectionMode(true);
	// Draw the scene
	updateOpenGL();
	GLC_State::setSelectionMode(false);

	GLsizei width= x2 - x1;
	GLsizei height= y1 - y2;
	GLint newX= x1;
    GLint newY= (m_Height - y1);
	if (newX < 0) newX= 0;
	if (newY < 0) newY= 0;

    return listOfIdInsideSquare(newX, newY, width, height, buffer);
}

GLC_uint GLC_Viewport::meaningfullIdInsideSquare(GLint x, GLint y, GLsizei width, GLsizei height, GLenum buffer)
{
	const int squareSize= width * height;
	const GLsizei arraySize= squareSize * 4; // 4 -> R G B A
	QVector<GLubyte> colorId(arraySize);

	// Get the array of pixels
    glReadBuffer(buffer);
	glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, colorId.data());

	// Restore Background color
	glClearColor(m_BackgroundColor.redF(), m_BackgroundColor.greenF(), m_BackgroundColor.blueF(), 1.0f);

	QHash<GLC_uint, int> idHash;
	QList<int> idWeight;

	// Find the most meaningful color
	GLC_uint returnId= 0;
	// There is nothing at the center
	int maxWeight= 0;
	int currentIndex= 0;
	for (int i= 0; i < squareSize; ++i)
	{
		GLC_uint id= glc::decodeRgbId(&colorId[i * 4]);
		if (idHash.contains(id))
		{
			const int currentWeight= ++(idWeight[idHash.value(id)]);
			if (maxWeight < currentWeight)
			{
				returnId= id;
				maxWeight= currentWeight;
			}
		}
		else if (id != 0)
		{
			idHash.insert(id, currentIndex++);
			idWeight.append(1);
			if (maxWeight < 1)
			{
				returnId= id;
				maxWeight= 1;
			}
		}
	}

	return returnId;
}

QSet<GLC_uint> GLC_Viewport::listOfIdInsideSquare(GLint x, GLint y, GLsizei width, GLsizei height, GLenum buffer)
{
	const int squareSize= width * height;
	const GLsizei arraySize= squareSize * 4; // 4 -> R G B A
	QVector<GLubyte> colorId(arraySize);

	// Get the array of pixels
    glReadBuffer(buffer);
	glReadPixels(x, y, width, height, GL_RGBA, GL_UNSIGNED_BYTE, colorId.data());

	// Restore Background color
	glClearColor(m_BackgroundColor.redF(), m_BackgroundColor.greenF(), m_BackgroundColor.blueF(), 1.0f);

	QSet<GLC_uint> idSet;

	// get the color inside square
	for (int i= 0; i < squareSize; ++i)
	{
		GLC_uint id= glc::decodeRgbId(&colorId[i * 4]);
		idSet << id;
	}

	return idSet;
}

void GLC_Viewport::updateMinimumRatioSize()
{
    int size= qMax(m_Width, m_Height);
	m_MinimumStaticRatioSize=  static_cast<double>(m_MinimumStaticPixelSize) / static_cast<double>(size) * 100.0;
	m_MinimumDynamicRatioSize= m_MinimumStaticRatioSize * 2.0;
}

void GLC_Viewport::loadBackGroundImage(const QString& ImageFile, bool preserveRatio)
{
	delete m_pImagePlane;
    m_pImagePlane= new GLC_ImagePlane(ImageFile, preserveRatio);
}

void GLC_Viewport::loadBackGroundImage(const QImage& image, bool preserveRatio)
{
	delete m_pImagePlane;
    m_pImagePlane= new GLC_ImagePlane(image, preserveRatio);
}

void GLC_Viewport::deleteBackGroundImage()
{
	delete m_pImagePlane;
	m_pImagePlane= NULL;
}

void GLC_Viewport::clearBackground(const QColor& color) const
{
    glClearColor(color.redF(), color.greenF(), color.blueF(), 1.0f);
}

void GLC_Viewport::setToOrtho(bool useOrtho)
{
	if (m_UseParallelProjection != useOrtho)
	{
		m_UseParallelProjection= useOrtho;
        updateProjectionMat(false); // doesn't update OpenGL
	}
}

void GLC_Viewport::reframe(const GLC_BoundingBox& box, double coverFactor)
{
    GLC_Camera targetCamera= reframedCamera(box, coverFactor);
    m_pViewCam->setCam(targetCamera);
}

GLC_Camera GLC_Viewport::reframedCamera(const GLC_BoundingBox &box, double coverFactor) const
{
    Q_ASSERT(!box.isEmpty());
    GLC_Camera subject(*m_pViewCam);

    // Center view on the BoundingBox
    const GLC_Vector3d deltaVector(box.center() - m_pViewCam->target());
    subject.translate(deltaVector);

    if (coverFactor > 0.0) {
        double cameraCover= box.boundingSphereRadius() * coverFactor;

        // Compute Camera distance
        const double distance = cameraCover / m_ViewTangent;

        // Update Camera position
        subject.setDistEyeTarget(distance);
    }

    return subject;
}

bool GLC_Viewport::reframeFromDeltaCover(double deltaCover)
{
    bool subject= false;
    const double cameraCover = m_pViewCam->distEyeTarget() *  m_ViewTangent;
    const double newCover= cameraCover+ deltaCover;

    if (!qFuzzyCompare(newCover, cameraCover))
    {
        // Compute Camera distance
        const double distance = newCover / m_ViewTangent;

        // Update Camera position
        m_pViewCam->setDistEyeTarget(distance);

        subject= true;
    }

    return subject;
}

bool GLC_Viewport::setDistMin(double DistMin, bool updateOpenGL)
{
	if (m_UseParallelProjection) DistMin= fabs(DistMin);
	if (DistMin < m_DistanceMax)
	{
		m_dDistanceMini= DistMin;

        updateProjectionMat(updateOpenGL);	// Update OpenGL projection matrix

		return true;
	}
	else
	{
		qDebug("GLC_Viewport::SetDistMin : KO");
		return false;
	}

}

bool GLC_Viewport::setDistMax(double DistMax, bool updateOpenGL)
{
	if (m_UseParallelProjection) DistMax= fabs(DistMax);
	if (DistMax > m_dDistanceMini)
	{
		m_DistanceMax= DistMax;

		// Update OpenGL projection matrix
        updateProjectionMat(updateOpenGL);

		return true;
	}
	else
	{
		qDebug("GLC_Viewport::SetDistMax : KO");
		return false;
	}
}

void GLC_Viewport::setDistMinAndMax(const GLC_BoundingBox& bBox, bool updateOpenGL)
{
    if(!bBox.isEmpty())
	{
		// The scene is not empty
		GLC_Matrix4x4 matComp(m_pViewCam->modelViewMatrix());

		// The bounding Box in Camera coordinate
		GLC_BoundingBox boundingBox(bBox);
		boundingBox.transform(matComp);

		// Increase size of the bounding box
		const double increaseFactor= 1.1;
		// Convert box distance in sphere distance
		const double center= fabs(boundingBox.center().z());
		const double radius= boundingBox.boundingSphereRadius();
		const double min= center - radius * increaseFactor;
		const double max= center + radius * increaseFactor;

		GLC_Point3d camEye(m_pViewCam->eye());
		camEye= matComp * camEye;

		if ((min > 0.0) || (m_UseParallelProjection))
		{
			// Outside bounding Sphere
			m_dDistanceMini= min;
			m_DistanceMax= max;
            //qDebug() << "distmin" << m_dDistanceMini;
            //qDebug() << "distmax" << m_DistanceMax;
		}
		else
		{
			// Inside bounding Sphere
			m_dDistanceMini= qMin(0.01 * radius, m_pViewCam->distEyeTarget() / 4.0);
			m_DistanceMax= max;
            //qDebug() << "inside distmin" << m_dDistanceMini;
            //qDebug() << "inside distmax" << m_DistanceMax;
		}
	}
	else
	{
		// The scene is empty
		m_dDistanceMini= m_pViewCam->distEyeTarget() / 2.0;
		m_DistanceMax= m_pViewCam->distEyeTarget();
	}

    // Update View projection matrix
    updateProjectionMat(updateOpenGL);
}

void GLC_Viewport::setBackgroundColor(QColor setColor)
{
	m_BackgroundColor= setColor;
}

void GLC_Viewport::addClipPlane(GLenum planeGlEnum,GLC_Plane* pPlane)
{
	if (m_ClipPlanesHash.contains(planeGlEnum))
	{
		delete m_ClipPlanesHash.value(planeGlEnum);
		m_ClipPlanesHash.remove(planeGlEnum);
	}
	m_ClipPlanesHash.insert(planeGlEnum, pPlane);
}

void GLC_Viewport::removeClipPlane(GLenum planeGlEnum)
{
	if (m_ClipPlanesHash.contains(planeGlEnum))
	{
		delete m_ClipPlanesHash.value(planeGlEnum);
		m_ClipPlanesHash.remove(planeGlEnum);
	}
	else
	{
		qDebug() << "GLC_Viewport::removeClipPlane Clipp plane " << planeGlEnum << " Not found";
	}
}

void GLC_Viewport::removeAllClipPlane()
{
	// Delete clip planes
	QHash<GLenum, GLC_Plane*>::iterator iClip= m_ClipPlanesHash.begin();
	while (m_ClipPlanesHash.constEnd() != iClip)
	{
		delete iClip.value();
		++iClip;
	}
}

void GLC_Viewport::useClipPlane(bool flag)
{
	m_UseClipPlane= flag;
	if (m_UseClipPlane)
	{
		QHash<GLenum, GLC_Plane*>::iterator iClip= m_ClipPlanesHash.begin();
		while (m_ClipPlanesHash.constEnd() != iClip)
		{
			GLenum planeKey= iClip.key();
			GLC_Plane* pPlane= iClip.value();

			glClipPlane (planeKey, pPlane->data());
			glEnable (planeKey);
			++iClip;
		}
	}
	else
	{
		QHash<GLenum, GLC_Plane*>::iterator iClip= m_ClipPlanesHash.begin();
		while (m_ClipPlanesHash.constEnd() != iClip)
		{
			glDisable(iClip.key());
			++iClip;
		}
	}

}

void GLC_Viewport::clearBackground() const
{
    glClearColor(m_BackgroundColor.redF(), m_BackgroundColor.greenF(), m_BackgroundColor.blueF(), 1.0f);
}
