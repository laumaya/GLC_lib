/****************************************************************************
 This file is part of the documentation of GLC-lib library.
 Copyright (C) 2012 Laurent Ribon (laumaya@users.sourceforge.net)
 http://glc-lib.sourceforge.net

 GLC-lib is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This file is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 http://www.fsf.org/licensing/licenses/info/GPLv2.html and
 http://www.gnu.org/copyleft/gpl.html.
*****************************************************************************/

#ifndef GLCSCENE_H_
#define GLCSCENE_H_

#include <QGraphicsScene>
#include <QDialog>
#include <QString>
#include <QLabel>
#include <QTreeWidget>

#include <GLC_World>
#include <GLC_Light>
#include <GLC_Viewport>
#include <GLC_MoverController>

class GLCScene : public QGraphicsScene
{
	Q_OBJECT

public:
	GLCScene();
	virtual ~GLCScene();

	void drawBackground(QPainter *painter, const QRectF &rect);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
	QDialog* createDialog() const;
	void createSceneWidgets();
	void loadModel(const QString &filePath);
	void updateLabels(const QString& filePath);
	void initGl();
	void renderWorld();

private slots:
	void loadModel();
	void worldLoaded();
	void reframe();
	void updateCameraPos();

private:
	GLC_Light m_Light;
	GLC_World m_World;
	QString m_CurrentPath;
	QString m_CurrentFilePath;
	QFutureWatcher<GLC_World *> m_WorldLoader;
	QList<QLabel*> m_Labels;
	QWidget* m_pLoadWorldButton;

	GLC_Viewport m_Viewport;
	GLC_MoverController m_MoverController;
	QPoint m_SavedMousePos;
	QTime m_MouseTimer;

	GLC_Vector3d m_Axis;
	double m_Angle;

	GLC_Vector3d m_SavedCamForwadVector;

};

#endif /* GLCSCENE_H_ */
