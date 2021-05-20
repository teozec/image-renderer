#include <QtWidgets>

#include "window.h"

Wizard::Wizard(QWidget *parent) : QWizard(parent)
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
	connect(this, &QWizard::helpRequested, this, &Wizard::showHelp);
	setWindowTitle(tr("Image Renderer Desktop"));
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

IntroPage::IntroPage(QWidget *parent) : QWizardPage(parent)
{
	setTitle(tr("Welcome to image-renderer!"));

	topLabel = new QLabel(tr("image-renderer is a C++ tool to generate photo-realistic images.\n"
		"It is based on the lectures of the Numerical tecniques for photorealistic image generation course,"
		"held by Prof. Maurizio Tomasi in 2021."));
	topLabel->setWordWrap(true);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(topLabel);
	setLayout(layout);
}

MenuPage::MenuPage(QWidget *parent) : QWizardPage(parent)
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

pfm2ldrPage::pfm2ldrPage(QWidget *parent) : QWizardPage(parent)
{
	setTitle(tr("Convert PFM into LDR"));
	setSubTitle(tr("Enter your preferences."));

	formatLabel = new QLabel(tr("Format output:"));
	formatDropdownMenu = new QComboBox;
	QStringList supportedFormats{"bmp", "gif", "jpeg", "png", "tiff", "webp"};
	formatDropdownMenu->addItems(supportedFormats);
	formatLabel->setBuddy(formatDropdownMenu);

	ifilenameLabel = new QLabel(tr("Filename input:"));
	ifilenameLineEdit = new QLineEdit;
	ifilenameLineEdit->setPlaceholderText(tr("demo.pfm"));
	ifilenameLabel->setBuddy(ifilenameLineEdit);

	ofilenameLabel = new QLabel(tr("Filename output:"));
	ofilenameLineEdit = new QLineEdit;
	connect(formatDropdownMenu, SIGNAL(currentTextChanged(QString)), this, SLOT(setPlaceholder()));
	ofilenameLabel->setBuddy(ofilenameLineEdit);

	aFactorLabel = new QLabel(tr("a factor:"));
	aFactorSpinner = new QDoubleSpinBox;
	aFactorSpinner->setRange(0, 1);
	aFactorSpinner->setSingleStep(0.02);
	aFactorLabel->setBuddy(aFactorSpinner);

	gammaLabel = new QLabel(tr("gamma value:"));
	gammaSpinner = new QDoubleSpinBox;
	gammaSpinner->setRange(0, 1);
	gammaSpinner->setSingleStep(0.02);
	gammaLabel->setBuddy(gammaSpinner);

	format  = formatDropdownMenu->currentText();
	registerField("ifilename", ifilenameLineEdit);
	registerField("ofilename", ofilenameLineEdit);
	registerField("aFactor", aFactorSpinner);
	registerField("gamma", gammaSpinner);

	QGridLayout *layout1 = new QGridLayout;
	layout1->addWidget(formatLabel, 0, 0);
	layout1->addWidget(formatDropdownMenu, 0, 2);
	layout1->addWidget(ifilenameLabel, 1, 0);
	layout1->addWidget(ifilenameLineEdit, 1, 2);
	layout1->addWidget(ofilenameLabel, 2, 0);
	layout1->addWidget(ofilenameLineEdit, 2, 2);
	QGridLayout *layout2 = new QGridLayout;
	layout2->addWidget(aFactorLabel, 0, 0);
	layout2->addWidget(aFactorSpinner, 1, 0);
	layout2->addWidget(gammaLabel, 0, 2);
	layout2->addWidget(gammaSpinner, 1, 2);
	QGridLayout *layout = new  QGridLayout;
	layout->addLayout(layout1, 0, 0);
	layout->addLayout(layout2, 2, 0);
	setLayout(layout);
}

void pfm2ldrPage::setPlaceholder() {
	ofilenameLineEdit->setPlaceholderText(tr("demo.").append(formatDropdownMenu->currentText()));
}

int pfm2ldrPage::nextId() const
{
	if (format == tr("png"))
		return Wizard::Page_pfm2png;
	else
		return Wizard::Page_Conclusion;
}


pfm2pngPage::pfm2pngPage(QWidget *parent) : QWizardPage(parent)
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
