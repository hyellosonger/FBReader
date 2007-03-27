/*
 * Copyright (C) 2004-2007 Nikolay Pultsin <geometer@mawhrin.net>
 * Copyright (C) 2005 Mikhail Sobolev <mss@mawhrin.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

#ifndef __ZLQTSELECTIONDIALOG_H__
#define __ZLQTSELECTIONDIALOG_H__

#include <string>
#include <map>

#include <QtGui/QDialog>
#include <QtGui/QPainter>
#include <QtGui/QListWidget>

#include "../../desktop/dialogs/ZLDesktopSelectionDialog.h"

class QVBox;
class QLineEdit;

class ZLQtSelectionDialogItem : public QListWidgetItem {

public:
	ZLQtSelectionDialogItem(QListWidget *listWidget, const ZLTreeNodePtr node);
	ZLTreeNodePtr node() const { return myNode; }

private:
	ZLTreeNodePtr myNode;
};

class ZLQListWidget : public QListWidget {

Q_OBJECT

public:
	ZLQListWidget(QWidget *parent);

Q_SIGNALS:
	void returnPressed();

private:
	void keyPressEvent(QKeyEvent *event);
};


class ZLQtSelectionDialog : public QDialog, public ZLDesktopSelectionDialog {
	Q_OBJECT

public:
	ZLQtSelectionDialog(const char *caption, ZLTreeHandler &handler); 
	~ZLQtSelectionDialog();
	bool run();

private:
	QIcon &getIcon(const ZLTreeNodePtr node);

protected:
	void keyPressEvent(QKeyEvent *event);

	void setSize(int width, int height) { QDialog::resize(width, height); }
	int width() const { return QDialog::width(); }
	int height() const { return QDialog::height(); }

	void exitDialog();
	void updateStateLine();
	void updateList();
	void selectItem(int index);

private Q_SLOTS:
	void runNodeSlot();
	void accept();

private:
	QLineEdit *myStateLine;
	ZLQListWidget *myListWidget;
	std::map<std::string,QIcon*> myIcons;
};

#endif /* __ZLQTSELECTIONDIALOG_H__ */
