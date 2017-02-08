/***************************************************************************
    *   Copyright (C) 2007 by GangsTER  *
 ***************************************************************************/

#include <GL/glew.h>
#include <GL/glut.h>
#include <QApplication>

#include "BBWindow.h"
#include <QSplashScreen>

int main( int argc, char ** argv ) {
	glutInit(&argc, argv);
  Q_INIT_RESOURCE(application);
  QApplication app(argc, argv);

  QPixmap pixmap(":/icones/splashlogo.png");
	if(pixmap.isNull())
	{
		QMainWindow * mw = new BBWindow();
		QRect r = mw->geometry();
		r.moveCenter(QApplication::desktop()->screenGeometry().center());
		mw->setGeometry(r);
		mw->show();
	}
	else
	{
  	QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);
		//Pour la transparence du splash screen
		splash.setMask(pixmap.mask());
  	splash.show();
  	splash.showMessage("Billboard Cloud : v1.0, Equipe GangsTER : 2007");
  	app.processEvents();
  	QMainWindow * mw = new BBWindow();
		QRect r = mw->geometry();
		r.moveCenter(QApplication::desktop()->screenGeometry().center());
		mw->setGeometry(r);
		mw->show();
    sleep(1);  
    splash.finish(mw);  
	}
  app.connect( &app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()) );
  return app.exec();
}
