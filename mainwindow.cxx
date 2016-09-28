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
#include <QDebug>

#include "mainwindow.hxx"
#include "ui_mainwindow.h"

class SForth * sfobj;

static QString sfoutbuf;
static QString sfinbuf;
static QMap<int, QFile *>	opened_file_map;
static int			opened_file_idx;

static const char * init_fs =

        "create stack 128 cells allot "
        ": get-stack ( --) depth 0<> if depth 0 do depth i - 1- pick stack i cells + ! loop then depth stack ; immediate "

        "s\" button::^2::dup dup * . cr::test button\" widget-create constant btn1 "
        "0 value spinbox 0 value slider "
        "s\" spinbox::spbox::slider swap widget-setval::super spinbox\" widget-create to spinbox "
        "s\" slider::sld::spinbox swap widget-setval::super slider\" widget-create to slider "

        ": run include ; "
        "0 value cx 0 value cy "
        ": north ( units --) >r cx cy cx cy r> - dup to cy line drop ; "
        ": south ( units --) >r cx cy cx cy r> + dup to cy line drop ; "
        ": west ( units --) >r cx cy cx r> - dup to cx cy line drop ; "
        ": east ( units --) >r cx cy cx r> + dup to cx cy line drop ; "

        "s\\\" checkbox::chkbox::drop s\\\" checkbox toggled!\\\" type cr \" widget-create value chkbox "

	"s\" lineedit::xxx::2dup type cr type cr\"widget-create value ledit "

        "0 0 50 0 line value l1 "
        "100 value delay "
        ": rot 0 do l1 i rotate delay ms loop ; "

        "0 value rot-slider "
        "s\" slider::rotate line::l1 swap rotate\" widget-create to rot-slider "
        ": clock 0 0 0 -50 line 360 0 do dup i rotate 1000 ms 360 60 / +loop delete ; "
        ": fig1 1800 0 do 10 forward drop i right loop ; "
        // repeat 1800 [fd 10 rt repcount + .1]
        // px for [x 1 1000000] [fd :x rt 90]
        ": fig2 10000 0 do i forward drop 90 right loop ; "

	": point 2dup line ; 0 0 point value p1 20 20 point value p2 "

	"0 value tmr : start-timer s\" timer::tmr::.( hello!)cr\" widget-create to tmr ; "
	"0 value timer-btn "
	"s\\\" button::start\\mtimer::timer-btn s\\\\\\\" stop\\mtimer\\\" widget-settext start-timer\"widget-create to timer-btn "
        ;


int sfgetc()
{
	int res, i;
	cell * s;
	QList<cell> ss;

#if 1
	sf_eval("get-stack");
	s = (cell *) sf_pop();
	i = sf_pop();
	while (i --)
		ss << * s ++;
	sfobj->dump_stack(ss);
#endif
	while (sfinbuf.isEmpty())
		sfinbuf = sfobj->get_input();
	res = sfinbuf[0].toLatin1();
	sfinbuf.remove(0, 1);
	return res;
}


int sffgetc(cell file_id)
{
	char c;
	WORD b;
	if (pf_read(& c, 1, & b) != FR_OK || b != 1)
		return EOF;
	return (unsigned) c;
}


int sffgetc_1(int file_id)
{
	char c;
	if (!opened_file_map[file_id]->getChar(&c))
		return EOF;
	return (unsigned) c;
}

int sfputc(int c)
{
	sfoutbuf += QChar(c);
	if (c == '\n')
		return sfsync();
	return 0;
}


int sfsync()
{
	if (!sfoutbuf.isEmpty())
	{
		sfobj->write_output(sfoutbuf);
		sfoutbuf.clear();
	}
	return 0;
}


cell sfopen(const char *pathname, int flags)
{
	if (pf_open(pathname) != FR_OK)
		return 0;
	else
		return 1;
}

int sffseek(cell stream, long offset)
{
	return pf_lseek(offset);
}

int sfopen_1(const char *pathname, int flags)
{
	QFile * f = new QFile(QString(":/forth-code/") + pathname);

	if (!f->open(QFile::ReadOnly))
	{
		delete f;
		return 0;
	}
	opened_file_map[opened_file_idx] = f;
	return opened_file_idx ++;
}


int sfclose(cell file_id)
{
	if (opened_file_map[file_id])
	{
		opened_file_map[file_id]->close();
		delete opened_file_map[file_id];
		opened_file_map[file_id] = 0;
	}
	return 0;
}

void SForth::run()
{
	sf_reset();
	sf_ext_init();
	sf_eval(init_fs);

	while (1)
	{
		do_quit();
		if (1) return;
		emit outputReady("\n\n<<< sforth engine exited; restarting >>>\n\n");
		sf_reset();
		sf_ext_init();
	}
}

void SForth::appendCommand(const QString &cmd)
{
	mtx_ui_input.lock();
	cmds += cmd;
	cond.wakeOne();
	mtx_ui_input.unlock();
}

QString SForth::get_input()
{
	QString res;
	mtx_ui_input.lock();
	while (1)
	{
		if (!cmds.isEmpty())
		{
			res = cmds;
			cmds.clear();
			mtx_ui_input.unlock();
			return res;
		}
		cond.wait(&mtx_ui_input);
	}
}

void SForth::write_output(const QString &s)
{
	emit outputReady(s);
}

void SForth::dump_stack(const QList<cell> &stack)
{
	emit currentStack(stack);
}


void SForth::retrieve_results()
{
	int i;
	for (i = 0; i < results.length(); i ++)
		sf_push(results[i]);
	results.clear();
}

void SForth::create_graphic_item(const QString &type, QList<scell> params)
{
	mtx_response_sync.lock();
	emit drawGraphicItem(type, params);
	cond_response.wait(&mtx_response_sync);
	retrieve_results();
	mtx_response_sync.unlock();
}

void SForth::do_graphic_item_op(const QString &type, QList<scell> params)
{
	mtx_response_sync.lock();
	emit graphicItemOp(type, params);
	cond_response.wait(&mtx_response_sync);
	retrieve_results();
	mtx_response_sync.unlock();
}


void SForth::do_widget_op(const QString &type, QList<scell> iparams, QList<QString> sparams)
{
	mtx_response_sync.lock();
	emit widgetOp(type, iparams, sparams);
	cond_response.wait(&mtx_response_sync);
	retrieve_results();
	mtx_response_sync.unlock();
}

void SForth::do_sleep_ms(QList<scell> params)
{
	mtx_response_sync.lock();
	emit sleepMs(params);
	cond_response.wait(&mtx_response_sync);
	retrieve_results();
	mtx_response_sync.unlock();
}

void SForth::createWidget(const QString &data)
{
	mtx_response_sync.lock();
	emit createQtWidget(data);
	cond_response.wait(&mtx_response_sync);
	retrieve_results();
	mtx_response_sync.unlock();
}

void SForth::put_results(const QList<cell> &res)
{
	mtx_response_sync.lock();
	if (!results.empty())
	{
		*(int*)0=0;
		qDebug() << "!!! previous results not retrieved !!!";
	}
	results = res;
	cond_response.wakeOne();
	mtx_response_sync.unlock();
}

MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
	qRegisterMetaType<QList<cell> >();
	qRegisterMetaType<QList<scell> >();
	qRegisterMetaType<QList<QString> >();

	if (pf_mount(& fatfs) != FR_OK)
		qDebug() << "failed to mount filesystem";

	ui->setupUi(this);
	sfobj = sf = new SForth;
	sf->moveToThread(&sfthread);

	connect(this, SIGNAL(start_sforth()), sf, SLOT(run()));
	connect(sf, SIGNAL(outputReady(QString)), this, SLOT(sf_output_ready(QString)));
	connect(sf, SIGNAL(createQtWidget(QString)), this, SLOT(createWidget(QString)));
	connect(sf, SIGNAL(currentStack(QList<cell>)), this, SLOT(currentStack(QList<cell>)));
	connect(sf, SIGNAL(drawGraphicItem(QString,QList<scell>)), this, SLOT(drawGraphicItem(QString,QList<scell>)));
	connect(sf, SIGNAL(graphicItemOp(QString,QList<scell>)), this, SLOT(graphicItemOp(QString,QList<scell>)));
	connect(sf, SIGNAL(widgetOp(QString,QList<scell>,QList<QString>)), this, SLOT(widgetOp(QString,QList<scell>,QList<QString>)));
	connect(sf, SIGNAL(sleepMs(QList<scell>)), this, SLOT(sleepMs(QList<scell>)));

	connect(ui->pushButtonClearCanvas, SIGNAL(clicked()), &canvas, SLOT(clear()));
	connect(&timer, SIGNAL(timeout()), this, SLOT(timerFired()));

	sfthread.start();
	ui->graphicsView->setScene(& canvas);

	opened_file_map[0] = 0;
	opened_file_idx = 1;
	timer.setSingleShot(true);

	ui->groupBox->setLayout(form_layouts);

	emit start_sforth();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_pushButtonSend_clicked()
{
	sf->appendCommand(ui->lineEdit->text() + "\n");
	ui->lineEdit->clear();
}

void MainWindow::sf_output_ready(QString s)
{
	ui->plainTextEdit->appendPlainText(s);
}

void MainWindow::createWidget(QString data)
{
	QStringList l = data.split("::");
	QObject * w = 0;
	QWidget * wd = 0;
	int msgtype;
	if (l.length() == 0)
		return;
	if (l[0] == "button")
	{
		if (l.length() < 2)
		{
			qDebug() << "bad parameters for" << l[0] << "widget creation";
		}
		else
		{
			QPushButton * b = new QPushButton(l[1], this);
			w = wd = b;
			if (l.length() > 2)
				btn_map[b] = l[2];
			connect(b, SIGNAL(clicked()), this, SLOT(dynamicButtonPressed()));
			form_layouts[0].addWidget(b);
		}
	}
	else if (l[0] == "slider")
	{
		if (l.length() < 2)
		{
			qDebug() << "bad parameters for" << l[0] << "widget creation";
		}
		else
		{
			QSlider * b = new QSlider(Qt::Horizontal, this);
			w = wd = b;
			if (l.length() > 2)
				slider_map[b] = l[2];
			//connect(b, SIGNAL(editingFinished()), this, SLOT(dynamicSpinboxEvent()));
			connect(b, SIGNAL(valueChanged(int)), this, SLOT(dynamicSliderEvent()));
			form_layouts[0].addRow(l[1], b);
		}
	}
	else if (l[0] == "spinbox")
	{
		if (l.length() < 2)
		{
			qDebug() << "bad parameters for" << l[0] << "widget creation";
		}
		else
		{
			QSpinBox * b = new QSpinBox(this);
			w = wd = b;
			if (l.length() > 2)
				spinbox_map[b] = l[2];
			//connect(b, SIGNAL(editingFinished()), this, SLOT(dynamicSpinboxEvent()));
			connect(b, SIGNAL(valueChanged(int)), this, SLOT(dynamicSpinboxEvent()));
			form_layouts[0].addRow(l[1], b);
		}
	}
	else if (l[0] == "checkbox")
	{
		if (l.length() < 2)
		{
			qDebug() << "bad parameters for" << l[0] << "widget creation";
		}
		else
		{
			QCheckBox * b = new QCheckBox(this);
			w = wd = b;
			if (l.length() > 2)
				checkbox_map[b] = l[2];
			connect(b, SIGNAL(stateChanged(int)), this, SLOT(dynamicCheckboxEvent()));
			form_layouts[0].addRow(l[1], b);
		}
	}
	else if (l[0] == "lineedit")
	{
		if (l.length() < 2)
		{
			qDebug() << "bad parameters for" << l[0] << "widget creation";
		}
		else
		{
			QLineEdit * b = new QLineEdit(this);
			w = b;
			if (l.length() > 2)
				lineedit_map[b] = l[2];
			connect(b, SIGNAL(returnPressed()), this, SLOT(dynamicLineeditEvent()));
			form_layouts[0].addRow(l[1], b);
		}
	}
	else if (l[0] == "timer")
	{
		if (l.length() < 2)
		{
			qDebug() << "bad parameters for" << l[0] << "widget creation";
		}
		else
		{
			QTimer * b = new QTimer(this);
			w = b;
			if (l.length() > 2)
				timer_map[b] = l[2];
			connect(b, SIGNAL(timeout()), this, SLOT(dynamicTimerEvent()));
			form_layouts[0].addRow(l[1], new QLabel("timer"));
			b->setInterval(1000);
			b->setSingleShot(false);
			b->start();
		}
	}
	else if ((msgtype = 0, l[0] == "critical") || (msgtype = 1, l[0] == "warning") || (msgtype = 2, l[0] == "info") || (msgtype = 3, l[0] == "question"))
	{
		/* message boxes */
		QString title, msg;
		QList<cell> res;
		if (l.length() > 1)
			title = l[1];
		if (l.length() > 2)
			msg = l[2];
		switch (msgtype)
		{
		default:
		case 0: QMessageBox::critical(0, title, msg); break;
		case 1: QMessageBox::warning(0, title, msg); break;
		case 2: QMessageBox::information(0, title, msg); break;
		case 3: res << ((QMessageBox::question(0, title, msg) == QMessageBox::Yes) ? C_TRUE : C_FALSE); break;
		}
		sf->put_results(res);
		return;
	}
	else
	{
		qDebug() << "requested widget" << QString("'") + l[0] + "'" << "not supported";
	}
	if (wd && l.length() > 3)
		wd->setToolTip(l[3]);
	if (w)
		qobject_map[w] = 1;
	sf->put_results(QList<cell>() << (cell) w);
}

void MainWindow::dynamicButtonPressed()
{
	QString s;
	QPushButton * w = qobject_cast<QPushButton *>(sender());
	s = btn_map[w];
	if (!s.isEmpty())
	{
		qDebug() << s;
		sf->appendCommand(s + "\n");
	}
}

void MainWindow::dynamicSpinboxEvent()
{
	QString s;
	QSpinBox * w = qobject_cast<QSpinBox *>(sender());
	s = spinbox_map[w];
	if (!s.isEmpty())
	{
		sf->appendCommand(QString("#%1 ").arg(w->value()) + s + "\n");
	}
}

void MainWindow::dynamicSliderEvent()
{
	QString s;
	QSlider * w = qobject_cast<QSlider *>(sender());
	s = slider_map[w];
	if (!s.isEmpty())
	{
		sf->appendCommand(QString(" #%1 ").arg(w->value()) + s + "\n");
	}
}

void MainWindow::dynamicCheckboxEvent()
{
	QString s;
	QCheckBox * w = qobject_cast<QCheckBox *>(sender());
	s = checkbox_map[w];
	if (!s.isEmpty())
	{
		sf->appendCommand((w->isChecked() ? QString("TRUE ") : QString("FALSE ")) + s + "\n");
	}
}

void MainWindow::dynamicLineeditEvent()
{
	QString s;
	QLineEdit * w = qobject_cast<QLineEdit *>(sender());
	s = lineedit_map[w];
	if (!s.isEmpty())
	{
		sf->appendCommand(QString(" s\" ") + w->text() + "\" " + s + "\n");
	}
	w->clear();
}

void MainWindow::dynamicTimerEvent()
{
	QString s;
	QTimer * w = qobject_cast<QTimer *>(sender());
	s = timer_map[w];
	if (!s.isEmpty())
	{
		sf->appendCommand(s + "\n");
	}
}

void MainWindow::currentStack(QList<cell> stack)
{
	int i;
	QString s;

	if (0) qDebug() << "current stack:" << stack;
	for (i = 0; i < stack.length(); i ++)
		s += QString("%1 ").arg(stack[i]);
	ui->statusBar->showMessage(s);
}

void MainWindow::drawGraphicItem(QString type, QList<scell> params)
{
	QGraphicsItem * item = 0;
	qDebug() << "requested drawing of a" << type;
	if (type == "circle")
	{
		qreal r = params[2];
		item = canvas.addEllipse(0, 0, 2 * r, 2 * r);
		item->moveBy(params[0] - r, params[1] - r);
	}
	else if (type == "rect")
	{
		item = canvas.addRect(params[0], params[1], params[2], params[3]);
	}
	else if (type == "line")
	{
		item = canvas.addLine(params[0], params[1], params[2], params[3]);
	}
	else if (type == "text")
	{
		/* HACK HACK HACK */
		cell r[2];
		if (sf_get_results(r, 2) != 2)
		{
			qDebug() << "bad parameters for text rendering";
		}
		else
		{
			item = canvas.addText(QByteArray((const char *) r[0], r[1]));
			item->moveBy(params[0], params[1]);
		}
	}
	else
	{
		qDebug() << "graphics primitive" << QString("'") + type + "'" << "not supported";
	}
	sf->put_results(QList<cell>() << (cell) item);
}

void MainWindow::graphicItemOp(QString type, QList<scell> params)
{
	QGraphicsItem * item = 0;
	qDebug() << "requested graphic item operation: " << type;

	/* HACK HACK HACK */
	cell r[1];
	if (sf_get_results(r, 1) != 1)
	{
		qDebug() << "bad parameters for graphic item operation";
	}
	else
	{
		item = (QGraphicsItem *) r[0];
		if (type == "scale") { if (params.length() == 1) item->setScale(params[0]); else qDebug() << "bad parameter count for" << QString("'") + type + "'" << "operation"; }
		else if (type == "move-to") { if (params.length() == 2) item->setPos(params[0], params[1]); else qDebug() << "bad parameter count for" << QString("'") + type + "'" << "operation"; }
		else if (type == "rotate") { if (params.length() == 1) item->setRotation(params[0]); else qDebug() << "bad parameter count for" << QString("'") + type + "'" << "operation"; }
		else if (type == "delete") { if (params.length() == 0) { canvas.removeItem(item); delete item; } else qDebug() << "bad parameter count for" << QString("'") + type + "'" << "operation"; }
		else
		{
			qDebug() << "graphics operation" << QString("'") + type + "'" << "not supported";
		}
	}
	sf->put_results(QList<cell>());
}

void MainWindow::widgetOp(QString type, QList<scell> iparams, QList<QString> sparams)
{
	QWidget * w = 0;
	QList<cell> result;
	//qDebug() << "requested widget operation: " << type;

	/* HACK HACK HACK */
	cell r[1];
	if (sf_get_results(r, 1) != 1)
	{
		qDebug() << "bad parameters for graphic item operation";
	}
	else
	{
		w = (QWidget *) r[0];
		if (qobject_map.find(w) == qobject_map.end())
		{
			qDebug() << "bad widget pointer/id:" << r[0] << "for operation" << QString("'") + type;
		}
		else if (type == "enable")
		{
			w->setEnabled(true);
		}
		else if (type == "disable")
		{
			w->setEnabled(false);
		}
		else if (type == "set-val")
		{
			if (iparams.length() == 1)
			{
				if (slider_map.find((QSlider *) w) != slider_map.end())
				{
					w->blockSignals(true);
					((QSlider *) w)->setValue(iparams[0]);
					w->blockSignals(false);
				}
				else if (spinbox_map.find((QSpinBox *) w) != spinbox_map.end())
				{
					w->blockSignals(true);
					((QSpinBox *) w)->setValue(iparams[0]);
					w->blockSignals(false);
				}
				else qDebug() << "bad widget id for" << QString("'") + type + "'" << " widget operation:" << r[0];
			}
			else qDebug() << "bad parameter count for" << QString("'") + type + "'" << "operation";
		}
		else if (type == "get-val")
		{
			if (iparams.length() == 0)
			{
				if (slider_map.find((QSlider *) w) != slider_map.end())
				{
					result << ((QSlider *) w)->value();
				}
				else if (spinbox_map.find((QSpinBox *) w) != spinbox_map.end())
				{
					result << ((QSpinBox *) w)->value();
				}
				else if (checkbox_map.find((QCheckBox *) w) != checkbox_map.end())
				{
					result << ((((QCheckBox *) w)->checkState() == Qt::Checked) ? C_TRUE : C_FALSE);
				}
				else qDebug() << "bad widget id for" << QString("'") + type + "'" << " widget operation:" << r[0];
			}
			else qDebug() << "bad parameter count for" << QString("'") + type + "'" << "operation";
		}
		else if (type == "set-text")
		{
			if (sparams.length() == 1)
			{
				if (lineedit_map.find((QLineEdit *) w) != lineedit_map.end())
				{
					w->blockSignals(true);
					((QLineEdit *) w)->setText(sparams[0]);
					w->blockSignals(false);
				}
				else if (btn_map.find((QPushButton *) w) != btn_map.end())
				{
					w->blockSignals(true);
					((QPushButton *) w)->setText(sparams[0]);
					w->blockSignals(false);
				}
				else qDebug() << "bad widget id for" << QString("'") + type + "'" << " widget operation:" << r[0];
			}
			else qDebug() << "bad parameter count for" << QString("'") + type + "'" << "operation";
		}
		else if (type == "set-range")
		{
			if (iparams.length() == 2)
			{
				if (slider_map.find((QSlider *) w) != slider_map.end())
				{
					((QSlider *) w)->setRange(iparams[0], iparams[1]);
				}
				else if (spinbox_map.find((QSpinBox *) w) != spinbox_map.end())
				{
					((QSpinBox *) w)->setRange(iparams[0], iparams[1]);
				}
				else qDebug() << "bad widget id for" << QString("'") + type + "'" << " widget operation:" << r[0];
			}
			else qDebug() << "bad parameter count for" << QString("'") + type + "'" << "operation";
		}
		else if (type == "set-state")
		{
			if (iparams.length() == 1)
			{
				if (checkbox_map.find((QCheckBox *) w) != checkbox_map.end())
				{
					((QCheckBox *) w)->setChecked(iparams[0] != false);
				}
				else qDebug() << "bad widget id for" << QString("'") + type + "'" << " widget operation:" << r[0];
			}
			else qDebug() << "bad parameter count for" << QString("'") + type + "'" << "operation";
		}
		else
		{
			qDebug() << "widget operation" << QString("'") + type + "'" << "not supported";
		}
	}
	sf->put_results(result);
}

void MainWindow::sleepMs(QList<scell> params)
{
	if (params.length() == 1)
	{
		timer.start(params[0]);
	}
	else
	{
		qDebug() << "bad arguments for timer operation";
		sf->put_results(QList<cell>());
	}
}

void MainWindow::timerFired()
{
	sf->put_results(QList<cell>());
}

void MainWindow::closeEvent(QCloseEvent *evt)
{
	if (sfthread.isRunning())
	{
		sf->appendCommand(" bye\n");
		sfthread.exit();
		sfthread.wait();
	}
}


