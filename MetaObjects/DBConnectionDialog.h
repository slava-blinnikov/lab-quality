#ifndef DBCONNECTIONDIALOG_H
#define DBCONNECTIONDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include <QComboBox>
#include <QDialog>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QVBoxLayout>

#include <QStringList>
#include <QSqlDatabase>

class DBConnectionDialog : public QDialog
{
	Q_OBJECT
public:
	DBConnectionDialog(QWidget *parent = 0);

	void setUserList(QStringList *lst);
	void setLastUser(const QString &lu);
	void setDatabase(QSqlDatabase *db);
	const QString &getUserName();
	const QString &getPassword();

protected:
	void setLoginMode();
	void setChangePasswordMode();

private:
	QSqlDatabase *database;
	bool is_login_mode;
	QString user_name;
	QString password;

	QVBoxLayout *verticalLayout;
	QLabel *lbUser;
	QComboBox *cbUser;
	QLabel *lbPassword;
	QLineEdit *lePassword;
	QLabel *lbNewPassword;
	QLineEdit *leNewPassword;
	QLabel *lbNewPasswordConfirm;
	QLineEdit *leNewPasswordConfirm;
	QHBoxLayout *horizontalLayout;
	QPushButton *pbOk;
	QPushButton *pbCancel;
	QSpacerItem *horizontalSpacer;
	QPushButton *pbChangePassword;

protected slots:
	void okButtonClicked();
	void cancelButtonClicked();
	void changePasswordButtonClicked();
};

#endif // DBCONNECTIONDIALOG_H
