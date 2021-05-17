#include <QtWidgets>

#include "window.h"

Wizard::Wizard(QWidget *parent)
    : QWizard(parent)
{
    setPage(Page_Intro, new IntroPage);
    setPage(Page_Menu, new MenuPage);
    setPage(Page_pfm2ldr, new pfm2ldrPage);
	setPage(Page_pfm2png, new pfm2pngPage);
    setPage(Page_raytracer, new raytracerPage);
    setPage(Page_Conclusion, new ConclusionPage);

    setStartId(Page_Intro);

#ifndef Q_OS_MAC
    setWizardStyle(ModernStyle);
#endif
    setOption(HaveHelpButton, true);
    setPixmap(QWizard::LogoPixmap, QPixmap(":/images/demo.png"));

    connect(this, &QWizard::helpRequested, this, &Wizard::showHelp);

    setWindowTitle(tr("image-renderer Desktop"));
}

void Wizard::showHelp()
{
    static QString lastHelpMessage;

    QString message;

    switch (currentId()) {
    case Page_Menu:
        message = tr("'pfm2ldr'	- Convert PFM images into a supported LDR format.\n"
					"'demo'	- Run a basic render.\n"
					"If both are checked we will take as input of 'pfm2ldr' the output of 'Raytracer'");
        break;
    default:
        message = tr("Credits: Luca Nigro & Matteo Zeccoli Marazzini.");
    }

    if (lastHelpMessage == message)
        message = tr("Sorry, I already gave what help I could. "
                     "Maybe you should try asking a human?");

    QMessageBox::information(this, tr("Help"), message);

    lastHelpMessage = message;
}

IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Welcome to image-renderer!"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/images/demo.png"));

    topLabel = new QLabel(tr("image-renderer is a C++ tool to generate photo-realistic images.\n"
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
    setTitle(tr("Choose an action"));
    setSubTitle(tr("This program can do two things: it can raytrace an image of your choice or it can convert any .pfm file into a supported LDR format."));
    QButtonGroup *group = new QButtonGroup(this);
    group->setExclusive(false);
    pfm2ldrRadioButton = new QRadioButton(tr("&pfm2ldr"));
    raytraceRadioButton = new QRadioButton(tr("&RayTracer"));
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
		return Wizard::Page_raytracer;
    } else if (pfm2ldrRadioButton->isChecked() && !raytraceRadioButton->isChecked()) {
        return Wizard::Page_pfm2ldr;
	} else {
        return Wizard::Page_Conclusion;
    }
}

pfm2ldrPage::pfm2ldrPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Convert PFM into LDR"));
    setSubTitle(tr("Enter your preferences."));

    formatLabel = new QLabel(tr("Format output:"));
    formatLineEdit = new QLineEdit;
    formatLabel->setBuddy(formatLineEdit);

	ifilenameLabel = new QLabel(tr("Filename input:"));
    ifilenameLineEdit = new QLineEdit;
    ifilenameLabel->setBuddy(ifilenameLineEdit);

    ofilenameLabel = new QLabel(tr("Filename output:"));
    ofilenameLineEdit = new QLineEdit;
    ofilenameLabel->setBuddy(ofilenameLineEdit);

	aFactorLabel = new QLabel(tr("a factor:"));
	aFactorLineEdit = new QLineEdit;
	aFactorLabel->setBuddy(aFactorLineEdit);

	gammaLabel = new QLabel(tr("gamma value:"));
	gammaLineEdit = new QLineEdit;
	gammaLabel->setBuddy(gammaLineEdit);

    registerField("format", formatLineEdit);
	registerField("ifilename", ifilenameLineEdit);
    registerField("ofilename", ofilenameLineEdit);
    registerField("aFactor", aFactorLineEdit);
    registerField("gamma", gammaLineEdit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(formatLabel, 0, 0);
    layout->addWidget(formatLineEdit, 0, 1);
    layout->addWidget(ifilenameLabel, 1, 0);
    layout->addWidget(ifilenameLineEdit, 1, 1);
    layout->addWidget(ofilenameLabel, 2, 0);
    layout->addWidget(ofilenameLineEdit, 2, 1);
	layout->addWidget(aFactorLabel, 3, 0);
    layout->addWidget(aFactorLineEdit, 3, 1);
    layout->addWidget(gammaLabel, 4, 0);
    layout->addWidget(gammaLineEdit, 4, 1);

    setLayout(layout);
}

int pfm2ldrPage::nextId() const
{
    if (field("format").toString() == "png")
        return Wizard::Page_pfm2png;
    else
		return Wizard::Page_Conclusion;
}


pfm2pngPage::pfm2pngPage(QWidget *parent)
: QWizardPage(parent)
{
    setTitle(tr("Convert PFM into PNG"));
    setSubTitle(tr("Enter the fine parameters."));

}

int pfm2pngPage::nextId() const {
	return Wizard::Page_Conclusion;
}

raytracerPage::raytracerPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Try our demo!"));
    setSubTitle(tr("Set your options."));

	widthLabel = new QLabel(tr("width:"));
	widthLineEdit = new QLineEdit;
	widthLabel->setBuddy(widthLineEdit);

	heightLabel = new QLabel(tr("height:"));
	heightLineEdit = new QLineEdit;
	heightLabel->setBuddy(heightLineEdit);

	projectionLabel = new QLabel(tr("projection:"));
	projectionLineEdit = new QLineEdit;
	projectionLabel->setBuddy(projectionLineEdit);

    registerField("width", widthLineEdit);
    registerField("height", heightLineEdit);
	registerField("projection", projectionLineEdit);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(widthLabel, 0, 0);
    layout->addWidget(widthLineEdit, 0, 1);
    layout->addWidget(heightLabel, 1, 0);
    layout->addWidget(heightLineEdit, 1, 1);
	layout->addWidget(projectionLabel, 2, 0);
    layout->addWidget(projectionLineEdit, 2, 1);
    setLayout(layout);
}

int raytracerPage::nextId() const
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
