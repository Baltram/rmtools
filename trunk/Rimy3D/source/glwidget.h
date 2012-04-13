#ifndef GLWIDGET_H_INCLUDED
#define GLWIDGET_H_INCLUDED

#include <QGLWidget>
#include <GLC_Factory>
#include <GLC_Viewport>
#include <GLC_World>
#include <GLC_Light>
#include <GLC_MoverController>

class GLWidget :
    public QGLWidget
{
public:
    GLWidget( QWidget * a_pParent );
   ~GLWidget( void );
public:
    void setWorld( GLC_World const & a_Source );
private:
    void initializeGL( void );
    void mousePressEvent( QMouseEvent * a_pEvent );
    void mouseMoveEvent( QMouseEvent * a_pEvent );
    void mouseReleaseEvent( QMouseEvent * );
    void paintGL( void );
    void resizeGL( int a_iWidth, int a_iHeight );
private:
    GLC_Light           m_Light;
    GLC_World           m_World;
    GLC_Viewport        m_GlView;
    GLC_MoverController m_MoverController;
};

#endif  // GLWIDGET_H_INCLUDED
