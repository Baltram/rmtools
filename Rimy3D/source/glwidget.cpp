#include <QtDebug>
#include "glwidget.h"

GLWidget::GLWidget(QWidget * a_pParent ) :
    QGLWidget( a_pParent ),
    m_Circle( GLC_Factory::instance()->createCircle( 0.2 ) ),
    m_GlView( this )
{
    m_Circle.geomAt( 0 )->setWireColor( Qt::green );
}

GLWidget::~GLWidget( void )
{
}

void GLWidget::initializeGL( void )
{
	m_GlView.initGl();
}

void GLWidget::paintGL( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glLoadIdentity();
	m_GlView.glExecuteCam();
	m_Circle.render();
}

void GLWidget::resizeGL( int a_iWidth, int a_iHeight )
{
    m_GlView.setWinGLSize( a_iWidth, a_iHeight );
}
