#include <QtDebug>

#include <QMessageBox>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>

#include "DBConnectionDialog.h"
#include "DBConnection.h"

DBConnectionDialog::DBConnectionDialog(QWidget *parent)
		: QDialog(parent)
{
	resize(314, 173);
	QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(this->sizePolicy().hasHeightForWidth());
	setSizePolicy(sizePolicy);
	setMinimumSize(QSize(314, 173));
	setMaximumSize(QSize(314, 173));
	setModal(true);
	verticalLayout = new QVBoxLayout(this);
	verticalLayout->setSpacing(1);
	verticalLayout->setContentsMargins(5, 1, 5, 1);
	lbUser = new QLabel(this);
	QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Fixed);
	sizePolicy1.setHorizontalStretch(0);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(lbUser->sizePolicy().hasHeightForWidth());
	lbUser->setSizePolicy(sizePolicy1);
	verticalLayout->addWidget(lbUser);
	cbUser = new QComboBox(this);
	cbUser->setEditable(true);
	verticalLayout->addWidget(cbUser);
	lbPassword = new QLabel(this);
	sizePolicy1.setHeightForWidth(lbPassword->sizePolicy().hasHeightForWidth());
	lbPassword->setSizePolicy(sizePolicy1);
	verticalLayout->addWidget(lbPassword);
	lePassword = new QLineEdit(this);
	lePassword->setEchoMode(QLineEdit::Password);
	verticalLayout->addWidget(lePassword);
	lbNewPassword = new QLabel(this);
	sizePolicy1.setHeightForWidth(lbNewPassword->sizePolicy().hasHeightForWidth());
	lbNewPassword->setSizePolicy(sizePolicy1);
	verticalLayout->addWidget(lbNewPassword);
	leNewPassword = new QLineEdit(this);
	leNewPassword->setEchoMode(QLineEdit::Password);
	verticalLayout->addWidget(leNewPassword);
	lbNewPasswordConfirm = new QLabel(this);
	sizePolicy1.setHeightForWidth(lbNewPasswordConfirm->sizePolicy().hasHeightForWidth());
	lbNewPasswordConfirm->setSizePolicy(sizePolicy1);
	verticalLayout->addWidget(lbNewPasswordConfirm);
	leNewPasswordConfirm = new QLineEdit(this);
	leNewPasswordConfirm->setEchoMode(QLineEdit::Password);
	verticalLayout->addWidget(leNewPasswordConfirm);
	horizontalLayout = new QHBoxLayout();
	pbOk = new QPushButton(this);
	pbOk->setIconSize(QSize(16, 16));
	horizontalLayout->addWidget(pbOk);
	pbCancel = new QPushButton(this);
	horizontalLayout->addWidget(pbCancel);
	horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	horizontalLayout->addItem(horizontalSpacer);
	pbChangePassword = new QPushButton(this);
	QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
	sizePolicy2.setHorizontalStretch(0);
	sizePolicy2.setVerticalStretch(0);
	sizePolicy2.setHeightForWidth(pbChangePassword->sizePolicy().hasHeightForWidth());
	pbChangePassword->setSizePolicy(sizePolicy2);
	pbChangePassword->setMinimumSize(QSize(90, 0));
	horizontalLayout->addWidget(pbChangePassword);
	verticalLayout->addLayout(horizontalLayout);
	setWindowTitle(tr("Регистрация пользователя"));
	lbUser->setText(tr("Пользователь"));
	lbPassword->setText(tr("Пароль"));
	lbNewPassword->setText(tr("Новый пароль"));
	lbNewPasswordConfirm->setText(tr("Подтвердите новый пароль"));
	pbOk->setText(tr("Ok"));
	pbCancel->setText(tr("Отмена"));
	pbChangePassword->setText(tr("Смена пароля"));
	lbNewPassword->setVisible(false);
	leNewPassword->setVisible(false);
	lbNewPasswordConfirm->setVisible(false);
	leNewPasswordConfirm->setVisible(false);
	connect(pbOk, SIGNAL(clicked(bool)), this, SLOT(okButtonClicked()));
	connect(pbCancel, SIGNAL(clicked(bool)), this, SLOT(cancelButtonClicked()));
	connect(pbChangePassword, SIGNAL(clicked(bool)), this, SLOT(changePasswordButtonClicked()));
	is_login_mode = true;
	setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
	setMinimumHeight(111);
	setMaximumHeight(111);
}

void DBConnectionDialog::setUserList(QStringList *lst)
{
	cbUser->clear();
	lst->sort();
	cbUser->addItems(*lst);
}

void DBConnectionDialog::setLastUser(const QString &lu)
{
	int i = cbUser->findText(lu);
	if (i >= 0) cbUser->setCurrentIndex(i);
		else cbUser->setEditText(lu);
}

void DBConnectionDialog::setDatabase(QSqlDatabase *db)
{
	database = db;
}

const QString &DBConnectionDialog::getUserName()
{
	return user_name;
}

const QString &DBConnectionDialog::getPassword()
{
	return password;
}

void DBConnectionDialog::setLoginMode()
{
	setMinimumHeight(111);
	setMaximumHeight(111);
	pbChangePassword->setVisible(true);
	lbNewPassword->setVisible(false);
	leNewPassword->setVisible(false);
	lbNewPasswordConfirm->setVisible(false);
	leNewPasswordConfirm->setVisible(false);
	is_login_mode = true;
}

void DBConnectionDialog::setChangePasswordMode()
{
	setMinimumHeight(173);
	setMaximumHeight(173);
	lbNewPassword->setVisible(true);
	leNewPassword->setVisible(true);
	lbNewPasswordConfirm->setVisible(true);
	leNewPasswordConfirm->setVisible(true);
	pbChangePassword->setVisible(false);
	leNewPassword->clear();
	leNewPasswordConfirm->clear();
	is_login_mode = false;
}

void DBConnectionDialog::okButtonClicked()
{
	QString u = cbUser->currentText().trimmed();
	if (u.isEmpty())
	{
		QMessageBox::critical(this, windowTitle(), tr("Укажите имя пользователя!"));
		cbUser->setFocus();
		return;
	}
	QString p = lePassword->text().trimmed();
	if (p.isEmpty())
	{
		QMessageBox::critical(this, windowTitle(), tr("Наберите пароль!"));
		lePassword->setFocus();
		return;
	}
	if (database->isOpen()) database->close();
	database->setUserName(u);
	database->setPassword(p);
	if (!database->open())
	{
		qDebug() << database->lastError().text();
		QString err = database->lastError().text();
		if (err.indexOf("password authentication failed") >= 0)
			QMessageBox::critical(this, windowTitle(), tr("Неверно указаны имя пользователя или пароль."));
		else
			QMessageBox::critical(this, windowTitle(), tr("Не могу подключиться к базе данных!\nВозможно, сервер базы данных не работает."));
		return;
	}
	if (is_login_mode)
	{
		user_name = u;
		password = p;
		setResult(QDialog::Accepted);
		hide();
	}
	else
	{
		QString p_new = leNewPassword->text().trimmed();
		if (p_new.isEmpty())
		{
			QMessageBox::critical(this, windowTitle(), tr("Укажите новый пароль!"));
			leNewPassword->setFocus();
			return;
		}
		QString p_new_conf = leNewPasswordConfirm->text().trimmed();
		if (p_new_conf.isEmpty())
		{
			QMessageBox::critical(this, windowTitle(), tr("Подтвердите новый пароль!"));
			leNewPasswordConfirm->setFocus();
			return;
		}
		if (p_new != p_new_conf)
		{
			QMessageBox::critical(this, windowTitle(), tr("Новый пароль не подтверждён!\nНаберите новый пароль и его подтверждение заново."));
			leNewPassword->clear();
			leNewPasswordConfirm->clear();
			leNewPassword->setFocus();
		}
		QSqlQuery *qry = new QSqlQuery(*database);
        DBConnection::QueryKiller killer(qry);
		QString s = "alter user \"" + u + "\" with password \'" + p_new + "\'";
		if (!qry->exec(s))
		{
			QMessageBox::critical(this, windowTitle(), tr("Ошибка базы данных при смене пароля!\n") + database->lastError().text());
			database->close();
			return;
		}
		database->close();
		lePassword->setText(p_new);
		setLoginMode();
		QMessageBox::StandardButton r = (QMessageBox::StandardButton)QMessageBox::question(this, windowTitle(), tr("Пароль успешно изменён!\nЖелаете начать работу с этим пользователем?"), QMessageBox::Yes, QMessageBox::No);
		if (r == QMessageBox::Yes)
		{
			database->setUserName(u);
			database->setPassword(p_new);
			database->open();
			user_name = u;
			password = p_new;
			setResult(QDialog::Accepted);
			hide();
			return;
		}
		cbUser->setFocus();
	}
}

void DBConnectionDialog::cancelButtonClicked()
{
	if (is_login_mode)
	{
		setResult(QDialog::Rejected);
		hide();
	}
	else
	{
		setLoginMode();
	}
}

void DBConnectionDialog::changePasswordButtonClicked()
{
	setChangePasswordMode();
}
