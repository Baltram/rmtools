#include "glwidget.h"
#include "preferencesdialog.h"

GLWidget::GLWidget( QWidget * a_pParent ) :
    QGLWidget( a_pParent ),
    m_GlView( this )
{
    m_Light.setPosition( 1.0f, 1.0f, 1.0f );
    m_MoverController = GLC_Factory::instance()->createDefaultMoverController( QColor( 255, 30, 30, 255 ), &m_GlView );
    m_GlView.cameraHandle()->setDefaultUpVector( glc::Z_AXIS );
    m_GlView.cameraHandle()->setIsoView();
}

GLWidget::~GLWidget( void )
{
}

void GLWidget::resetCamera( void )
{
    m_GlView.cameraHandle()->setIsoView();
}

void GLWidget::setWorld( GLC_World const & a_Source )
{
    m_World = a_Source;
    if ( !m_World.isEmpty() )
        m_GlView.reframe( m_World.boundingBox() );
    updateGL();
}

void GLWidget::initializeGL( void )
{
    glEnable( GL_CULL_FACE );
    m_GlView.initGl();
}

void GLWidget::mousePressEvent( QMouseEvent * a_pEvent )
{
    if ( m_MoverController.hasActiveMover() )
        return;
    m_uMoverSpeed = ( a_pEvent->modifiers() & Qt::ControlModifier ) ? 3 : 1;
    bool bShift = a_pEvent->modifiers() & Qt::ShiftModifier;
    switch ( a_pEvent->button() )
    {
    case ( Qt::LeftButton ):
        m_MoverController.setActiveMover( bShift ? GLC_MoverController::Zoom : GLC_MoverController::Pan, GLC_UserInput( a_pEvent->x() * m_uMoverSpeed, a_pEvent->y() * m_uMoverSpeed ) );
        updateGL();
        break;
    case ( Qt::RightButton ):
        if ( bShift )
            m_uMoverSpeed = 1;
        m_MoverController.setActiveMover( bShift ? GLC_MoverController::TrackBall : GLC_MoverController::TurnTable, GLC_UserInput( a_pEvent->x() * m_uMoverSpeed, a_pEvent->y() * m_uMoverSpeed ) );
        updateGL();
        break;
    case ( Qt::MidButton ):
        if ( bShift )
            m_uMoverSpeed = 1;
        m_MoverController.setActiveMover( bShift ? GLC_MoverController::Target : GLC_MoverController::Zoom, GLC_UserInput( a_pEvent->x() * m_uMoverSpeed, a_pEvent->y() * m_uMoverSpeed ) );
        updateGL();
    }
}

void GLWidget::mouseMoveEvent( QMouseEvent * a_pEvent )
{
    if ( !m_MoverController.hasActiveMover() )
        return;
    m_MoverController.move( GLC_UserInput( a_pEvent->x() * m_uMoverSpeed, a_pEvent->y() * m_uMoverSpeed ) );
    updateGL();
}

void GLWidget::mouseReleaseEvent( QMouseEvent * )
{
    if ( !m_MoverController.hasActiveMover() )
        return;
    m_MoverController.setNoMover();
    updateGL();
}

void GLWidget::paintGL( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glLoadIdentity();
    m_GlView.setDistMinAndMax( m_World.boundingBox() );
    m_Light.enable();
    m_Light.glExecute();
    m_GlView.glExecuteCam();
    if ( !m_World.isEmpty() )
        m_World.render( 0, glc::ShadingFlag );
    if ( PreferencesDialog::getInstance().showTransformGizmos() )
        m_MoverController.drawActiveMoverRep();
}

void GLWidget::resizeGL( int a_iWidth, int a_iHeight )
{
    m_GlView.setWinGLSize( a_iWidth, a_iHeight );
}
