/*
Copyright (c) 2014-2016 stoyan shopov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#ifndef MAINWINDOW_HXX
#define MAINWINDOW_HXX

#include <QMainWindow>
#include <QFormLayout>
#include <QLabel>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QMap>
#include <QMessageBox>
#include <QPushButton>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QFile>
#include <QTimer>

extern "C"
{
#include <stdint.h>
#include "engine.h"
#include "dictionary.h"
#include "sf-arch.h"

#include "qsfdict.h"

#include <pff.h>
}


Q_DECLARE_METATYPE(QList<cell>)
Q_DECLARE_METATYPE(QList<scell>)
Q_DECLARE_METATYPE(QList<QString>)

namespace Ui {
class MainWindow;
}

class SForth : public QObject
{
	Q_OBJECT
private:
	QString		cmds;
	QMutex		mtx_ui_input;
	QMutex		mtx_response_sync;
	QWaitCondition	cond_response;
	QWaitCondition	cond;
	QList<cell>	results;

	void retrieve_results(void);
public slots:
	void run(void);
	void appendCommand(const QString & cmd);
public:
	QString get_input(void);
	void write_output(const QString & s);
	void createWidget(const QString & data);
	void put_results(const QList<cell> & res);
	void dump_stack(const QList<cell> & stack);
	void create_graphic_item(const QString & type, QList<scell> params);
	void do_graphic_item_op(const QString & type, QList<scell> params);
	void do_widget_op(const QString & type, QList<scell> iparams, QList<QString> sparams);
	void do_sleep_ms(QList<scell> params);
signals:
	void outputReady(QString s);
	void createQtWidget(QString data);
	void widgetOp(QString data, QList<scell> iparams, QList<QString> sparams);
	void currentStack(QList<cell> stack);
	void drawGraphicItem(QString type, QList<scell> params);
	void graphicItemOp(QString type, QList<scell> params);
	void sleepMs(QList<scell> params);
};

extern class SForth * sfobj;

extern "C"
{

int sfgetc(void);
int sffgetc(cell file_id);
int sfputc(int c);
int sfsync(void);
cell sfopen(const char * pathname, int flags);
int sfclose(cell file_id);
int sffseek(cell stream, long offset);


}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_pushButtonSend_clicked();
	void sf_output_ready(QString s);
	void createWidget(QString data);
	void dynamicButtonPressed(void);
	void dynamicSpinboxEvent(void);
	void dynamicSliderEvent(void);
	void dynamicCheckboxEvent(void);
	void dynamicLineeditEvent(void);
	void dynamicTimerEvent(void);
	void currentStack(QList<cell> stack);
	void drawGraphicItem(QString type, QList<scell> params);
	void graphicItemOp(QString type, QList<scell> params);
	void widgetOp(QString type, QList<scell> iparams, QList<QString> sparams);
	void sleepMs(QList<scell> params);
	void timerFired(void);

protected:
	void closeEvent(QCloseEvent * evt);

signals:
	void start_sforth(void);
private:
	QGraphicsScene	canvas;
	Ui::MainWindow *ui;
	SForth	* sf;
	QThread	sfthread;
	QMap<QPushButton *, QString> btn_map;
	QMap<QSpinBox *, QString> spinbox_map;
	QMap<QSlider *, QString> slider_map;
	QMap<QCheckBox *, QString> checkbox_map;
	QMap<QLineEdit *, QString> lineedit_map;
	QMap<QTimer *, QString> timer_map;
	QMap<QObject *, int> qobject_map;


	QTimer	timer;
	FATFS	fatfs;
	QFormLayout	form_layouts[1];
};

#endif // MAINWINDOW_HXX
