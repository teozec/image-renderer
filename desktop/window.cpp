#include <QtWidgets>
#include <QPrinter>
#include <QPrintDialog>
#include <QRegularExpressionValidator>

#include "window.h"

Wizard::Wizard(QWidget *parent)
    : QWizard(parent)
{
    setPage(Page_Intro, new IntroPage);
    setPage(Page_Menu, new MenuPage);
    setPage(Page_Register, new RegisterPage);
    setPage(Page_Details, new DetailsPage);
    setPage(Page_Conclusion, new ConclusionPage);

    setStartId(Page_Intro);

#ifndef Q_OS_MAC
    setWizardStyle(ModernStyle);
#endif
    setOption(HaveHelpButton, true);
    setPixmap(QWizard::LogoPixmap, QPixmap(":/images/logo.png"));

    connect(this, &QWizard::helpRequested, this, &Wizard::showHelp);

    setWindowTitle(tr("image-renderer Desktop"));
}

void Wizard::showHelp()
{
    static QString lastHelpMessage;

    QString message;

    switch (currentId()) {
    case Page_Menu:
        message = tr("Choose the job you want us to do for you.");
        break;
    case Page_Conclusion:
        message = tr("You must accept the terms and conditions of the "
                     "license to proceed.");
        break;
    default:
        message = tr("Credits: Luca Nigro & Matteo Zeccoli Marazzini");
    }

    if (lastHelpMessage == message)
        message = tr("Sorry, I already gave what help I could. "
                     "Maybe you should try asking a human?");

    QMessageBox::information(this, tr("image-renderer help"), message);

    lastHelpMessage = message;
}

IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Welcome!"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark.png"));

    topLabel = new QLabel(tr("image-renderer is a C++ tool to generate photo-realistic images. "
        "It is based on the lectures of the Numerical tecniques for photorealistic image generation course,"
        "held by Prof. Maurizio Tomasi in 2021."));
    topLabel->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(topLabel);
    setLayout(layout);
}

MenuPage::MenuPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("What can we do for you?"));
    setSubTitle(tr("This program can do two things: it can raytrace an image of your choice or it can convert any .pfm file into a LDR file."
                    "Please check what you wish: if both are checked we will take as input of pfm2ldr the output of raytrace."));
    QButtonGroup *group = new QButtonGroup(this);
    group->setExclusive(false);
    raytraceRadioButton = new QRadioButton(tr("&pfm2ldr"));
    pfm2ldrRadioButton = new QRadioButton(tr("&RayTracer"));
    raytraceRadioButton->setChecked(true);
    group->addButton(raytraceRadioButton);
    group->addButton(pfm2ldrRadioButton);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(raytraceRadioButton);
    layout->addWidget(pfm2ldrRadioButton);
    setLayout(layout);
}

int MenuPage::nextId() const
{
    if (raytraceRadioButton->isChecked()) {
        return Wizard::Page_Register;
    } else {
        return Wizard::Page_Conclusion;
    }
}

RegisterPage::RegisterPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Convert PFM into LDR"));
    setSubTitle(tr("Choose options:"));

    nameLabel = new QLabel(tr("Format output:"));
    nameLineEdit = new QLineEdit;
    nameLabel->setBuddy(nameLineEdit);

    upgradeKeyLabel = new QLabel(tr("Filename output:"));
    upgradeKeyLineEdit = new QLineEdit;
    upgradeKeyLabel->setBuddy(upgradeKeyLineEdit);

    registerField("format", nameLineEdit);
    registerField("filename", upgradeKeyLineEdit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(nameLabel, 0, 0);
    layout->addWidget(nameLineEdit, 0, 1);
    layout->addWidget(upgradeKeyLabel, 1, 0);
    layout->addWidget(upgradeKeyLineEdit, 1, 1);
    setLayout(layout);
}

int RegisterPage::nextId() const
{
    if (upgradeKeyLineEdit->text().isEmpty()) {
        return Wizard::Page_Conclusion;
    } else {
        return Wizard::Page_Details;
    }
}

DetailsPage::DetailsPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Convert PFM into LDR"));
    setSubTitle(tr("Choose the parameters you desire:"));

	companyLabel = new QLabel(tr("a factor:"));
	companyLineEdit = new QLineEdit;
	companyLabel->setBuddy(companyLineEdit);

	emailLabel = new QLabel(tr("gamma value:"));
	emailLineEdit = new QLineEdit;
	emailLabel->setBuddy(emailLineEdit);

    registerField("details.company*", companyLineEdit);
    registerField("details.email*", emailLineEdit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(companyLabel, 0, 0);
    layout->addWidget(companyLineEdit, 0, 1);
    layout->addWidget(emailLabel, 1, 0);
    layout->addWidget(emailLineEdit, 1, 1);
    setLayout(layout);
}

int DetailsPage::nextId() const
{
    return Wizard::Page_Conclusion;
}

ConclusionPage::ConclusionPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Completed!"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/watermark.png"));

    bottomLabel = new QLabel;
    bottomLabel->setWordWrap(true);

    agreeCheckBox = new QCheckBox(tr("I'm satisfied."));

    registerField("conclusion.agree*", agreeCheckBox);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(bottomLabel);
    layout->addWidget(agreeCheckBox);
    setLayout(layout);
}

int ConclusionPage::nextId() const
{
    return -1;
}

void ConclusionPage::initializePage()
{
    QString licenseText = tr("<u>Check the box below</u> if you are satisfied.");
    bottomLabel->setText(licenseText);
}

void ConclusionPage::setVisible(bool visible)
{
    QWizardPage::setVisible(visible);

    if (visible) {
        wizard()->setButtonText(QWizard::CustomButton1, tr("&Print"));
        wizard()->setOption(QWizard::HaveCustomButton1, true);
        connect(wizard(), &QWizard::customButtonClicked,
                this, &ConclusionPage::printButtonClicked);
    } else {
        wizard()->setOption(QWizard::HaveCustomButton1, false);
        disconnect(wizard(), &QWizard::customButtonClicked,
                   this, &ConclusionPage::printButtonClicked);
    }
}

void ConclusionPage::printButtonClicked()
{
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec())
        QMessageBox::warning(this, tr("Print License"),
                             tr("As an environmentally friendly measure, the "
                                "license text will not actually be printed."));
#endif
}
