#ifndef WINDOW_H
#define WINDOW_H

#include <QWizard>

class QCheckBox;
class QLabel;
class QLineEdit;
class QRadioButton;

class Wizard : public QWizard
{
    Q_OBJECT

public:
    enum { Page_Intro, Page_Menu, Page_Register, Page_Details,
           Page_Conclusion };

    Wizard(QWidget *parent = 0);

private slots:
    void showHelp();
};

class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    IntroPage(QWidget *parent = 0);

private:
    QLabel *topLabel;
};

class MenuPage : public QWizardPage
{
    Q_OBJECT

public:
    MenuPage(QWidget *parent = 0);

    int nextId() const Q_DECL_OVERRIDE;

private:
    QRadioButton *raytraceRadioButton;
    QRadioButton *pfm2ldrRadioButton;
};

class RegisterPage : public QWizardPage
{
    Q_OBJECT

public:
    RegisterPage(QWidget *parent = 0);

    int nextId() const Q_DECL_OVERRIDE;

private:
    QLabel *nameLabel;
    QLabel *upgradeKeyLabel;
    QLineEdit *nameLineEdit;
    QLineEdit *upgradeKeyLineEdit;
};

class DetailsPage : public QWizardPage
{
    Q_OBJECT

public:
    DetailsPage(QWidget *parent = 0);

    int nextId() const Q_DECL_OVERRIDE;

private:
    QLabel *companyLabel;
    QLabel *emailLabel;
    QLabel *postalLabel;
    QLineEdit *companyLineEdit;
    QLineEdit *emailLineEdit;
    QLineEdit *postalLineEdit;
};

class ConclusionPage : public QWizardPage
{
    Q_OBJECT

public:
    ConclusionPage(QWidget *parent = 0);

    void initializePage() Q_DECL_OVERRIDE;
    int nextId() const Q_DECL_OVERRIDE;
    void setVisible(bool visible) Q_DECL_OVERRIDE;

private slots:
    void printButtonClicked();

private:
    QLabel *bottomLabel;
    QCheckBox *agreeCheckBox;
};

#endif //WINDOW_H
