#ifndef GLWIDGET_H_INCLUDED
#define GLWIDGET_H_INCLUDED

#include <QGLWidget>

#include <GLC_Factory>
#include <GLC_Viewport>

class GLWidget :
    public QGLWidget
{
public:
    GLWidget( QWidget * a_pParent );
    ~GLWidget( void );
private:
    void initializeGL( void );
    void paintGL( void );
    void resizeGL( int a_iWidth, int a_iHeight );
private:
	GLC_3DViewInstance m_Circle;
	GLC_Viewport m_GlView;
};

#endif  // GLWIDGET_H_INCLUDED
