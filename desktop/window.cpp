#include <QtWidgets>
#include <string.h>
#include <stdio.h>
#include "argh.h"
#include "actions.h"
#include "window.h"

//#define DEFAULT_THEME true

Wizard::Wizard(QWidget *parent) : QWizard(parent)
{
	setPage(Page_Intro, new IntroPage);
	setPage(Page_Menu, new MenuPage);
	setPage(Page_pfm2ldr, new pfm2ldrPage);
	setPage(Page_raytracer, new raytracerPage);
	setPage(Page_Conclusion, new ConclusionPage);
	setStartId(Page_Intro);

#ifdef DEFAULT_THEME
	#ifndef Q_OS_MAC
		setWizardStyle(ModernStyle);
	#endif
#else
	QString path = QDir::currentPath();
	path.append("/../../themes/blue.qss");
	QFile styleSheetFile(path);
	if (!styleSheetFile.exists())
		qDebug() <<"Non esiste il file";
	//qDebug() <<path;
	styleSheetFile.open(QIODevice::ReadOnly);
	QString styleSheet = QLatin1String(styleSheetFile.readAll());
	setStyleSheet(styleSheet);
#endif

	setOption(HaveHelpButton, true);
	connect(this, &QWizard::helpRequested, this, &Wizard::showHelp);
	setWindowTitle(tr("Image Renderer Desktop"));
	setButtonText(QWizard::HelpButton, tr(" ? "));
	button(QWizard::HelpButton)->setStyleSheet(
		"QPushButton {"
			"background-color: #e7e9e7;"
			"color: #39444A;"
			"font-size: 11px;"
			"font-weight: bold;"
			"border: none;"
			"height: 10px;"
			"width:	10px;"
			"border-radius: 10px;"
			"padding: 5px;"
		"}"
		"QPushButton:hover {"
			"background-color: #e7e9e7;"
			"color: #39444A;"
			"font-size: 11px;"
			"border: 1px solid #39444A;"
			"height: 10px;"
			"width:	10px;"
			"border-radius: 10px;"
			"padding: 5px;"
		"}"
		"QPushButton::pressed {"
			"background-color: #39444A;"
			"color: #e7e9e7;"
			"font-size: 11px;"
			"font-weight: bold;"
			"height: 10px;"
			"width:	10px;"
			"border-radius: 10px;"
			"padding: 5px;"
		"}"
		"QPushButton:disabled {"
			"background-color: transparent;"
			"color: transparent;"
			"border: transparent;"
			"height: 10px;"
			"width:	10px;"
			"border-radius: 10px;"
			"padding: 5px;"
		"}"
	);
}

void Wizard::showHelp()
{
	static QString lastHelpMessage;

	QString message;

	switch (currentId()) {
	case Page_Menu:
		message = tr("'pfm2ldr'	- Convert PFM images into a supported LDR format.\n"
					"'demo'	- Run a basic render.\n");
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
	setSubTitle(tr("This program can do two things: "
					"it can raytrace an image of your choice or it can convert "
					"any .pfm file into a supported LDR format."));
	QButtonGroup *group = new QButtonGroup(this);
	group->setExclusive(true);
	pfm2ldrRadioButton = new QRadioButton(tr(" pfm2ldr"));
	raytraceRadioButton = new QRadioButton(tr(" RayTracer"));
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
	if (raytraceRadioButton->isChecked())
		return Wizard::Page_raytracer;
	else
		return Wizard::Page_pfm2ldr;
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

	registerField("ifilename", ifilenameLineEdit);
	registerField("ofilename", ofilenameLineEdit);
	registerField("aFactor", aFactorSpinner);
	registerField("gamma", gammaSpinner);

	pfm2ldrButton = new QPushButton(tr("Run"));
	connect(pfm2ldrButton, SIGNAL (clicked(bool)), this, SLOT (setupPfm2ldr()));

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
	layout2->addWidget(pfm2ldrButton, 2, 0, 1, 2);
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
	return Wizard::Page_Conclusion;
}

void pfm2ldrPage::setupPfm2ldr() const
{
	QString formatQstr  = formatDropdownMenu->currentText();

	std::string progName = "./image-renderer ";
	std::string	action = "pfm2ldr ";
	std::string format = formatQstr.toStdString()+"\x20";
	std::string infile = field("ifilename").toString().toStdString()+"\x20";
	std::string	outfile = field("ofilename").toString().toStdString()+"\x20";
	
	std::string cmdl = "cd ..;" + progName+action+format+infile+outfile;
	std::system(cmdl.c_str());
}

raytracerPage::raytracerPage(QWidget *parent) : QWizardPage(parent)
{
	setTitle(tr("Try our demo!"));
	setSubTitle(tr("Set your options."));

	widthLabel = new QLabel(tr("width:"));
	widthSpinner = new QSpinBox;
	widthSpinner->setRange(0, 3840);
	widthSpinner->setSuffix(tr(" px"));
	widthSpinner->setSingleStep(10);
	widthSpinner->setValue(400);
	widthLabel->setBuddy(widthSpinner);

	heightLabel = new QLabel(tr("height:"));
	heightSpinner = new QSpinBox;
	heightSpinner->setRange(0, 2160);
	heightSpinner->setSuffix(tr(" px"));
	heightSpinner->setSingleStep(10);
	heightSpinner->setValue(300);
	heightLabel->setBuddy(heightSpinner);

	ofilenameLabel = new QLabel(tr("Output filename:"));
	ofilenameLineEdit = new QLineEdit;
	ofilenameLineEdit->setText(tr("demo"));
	ofilenameLineEdit->setPlaceholderText(tr("e.g. \x20 demo"));
	ofilenameLabel->setBuddy(ofilenameLineEdit);
	oformatDropdownMenu = new QComboBox;
	QStringList supportedFormats{"pfm", "bmp", "gif", "jpeg", "png", "tiff", "webp"};
	oformatDropdownMenu->addItems(supportedFormats);

	projectionLabel = new QLabel(tr("projection:"));
	projectionDropdownMenu = new QComboBox;
	QStringList supportedProjections{"Perspective", "Orthogonal"};
	projectionDropdownMenu->addItems(supportedProjections);
	projectionDropdownMenu->setVisible(false);
	projectionLabel->setBuddy(projectionDropdownMenu);
	projectionLabel->setVisible(false);

	angleCamLabel = new QLabel(tr("angle-offset of cam:"));
	angleCamSpinner = new QSpinBox;
	angleCamSpinner->setRange(0, 360);
	angleCamSpinner->setSuffix(tr("°"));
	angleCamSpinner->setSingleStep(1);
	angleCamSpinner->setValue(0);
	angleCamSpinner->setVisible(false);
	angleCamLabel->setBuddy(angleCamSpinner);
	angleCamLabel->setVisible(false);

	advancedCheckBox = new QCheckBox(tr(" Advanced options"));
	connect(advancedCheckBox, SIGNAL(clicked(bool)), angleCamLabel, SLOT(setVisible(bool)));
	connect(advancedCheckBox, SIGNAL(clicked(bool)), angleCamSpinner, SLOT(setVisible(bool)));
	connect(advancedCheckBox, SIGNAL(clicked(bool)), projectionLabel, SLOT(setVisible(bool)));
	connect(advancedCheckBox, SIGNAL(clicked(bool)), projectionDropdownMenu, SLOT(setVisible(bool)));

	registerField("width", widthSpinner);
	registerField("height", heightSpinner);
	registerField("angleDeg", angleCamSpinner);
	registerField("output filename", ofilenameLineEdit);

	demoButton = new QPushButton(tr("Run"));
	connect(demoButton, SIGNAL (clicked(bool)), this, SLOT (setupDemo()));

	QGridLayout *layout1 = new QGridLayout;
	layout1->addWidget(widthLabel, 0, 0);
	layout1->addWidget(widthSpinner, 0, 1);
	layout1->addWidget(heightLabel, 0, 2);
	layout1->addWidget(heightSpinner, 0, 3);
	QGridLayout *layout2 = new QGridLayout;
	layout2->addWidget(ofilenameLabel, 0, 0);
	layout2->addWidget(ofilenameLineEdit, 0, 1);
	layout2->addWidget(oformatDropdownMenu, 0, 3);
	QGridLayout *layout3 = new QGridLayout;
	layout3->addWidget(advancedCheckBox, 0, 0);
	QGridLayout *layout4 = new QGridLayout;
	layout4->addWidget(angleCamLabel, 0, 0);
	layout4->addWidget(angleCamSpinner, 0, 1);
	layout4->addWidget(projectionLabel, 1, 0);
	layout4->addWidget(projectionDropdownMenu, 1, 1);
	layout4->addWidget(demoButton, 2, 0, 1, 2);
	QGridLayout *layout = new QGridLayout;
	layout->addLayout(layout1, 0, 0);
	layout->addLayout(layout2, 1, 0);
	layout->addLayout(layout3, 2, 0);
	layout->addLayout(layout4, 3, 0);
	setLayout(layout);
}

int raytracerPage::nextId() const
{
	return Wizard::Page_Conclusion;
}

void raytracerPage::setupDemo() const
{
	QString oformat  = oformatDropdownMenu->currentText();
	QString projection  = projectionDropdownMenu->currentText();

	std::string progName = "./image-renderer ";
	std::string width = field("width").toString().toStdString()+"\x20";
	std::string height = field("height").toString().toStdString()+"\x20";
	std::string proj = projection.toLower().toStdString()+"\x20";
	std::string angle = field("angleDeg").toString().toStdString()+"\x20";
	std::string	outfile = field("output filename").toString().toStdString()+".pfm ";

	std::cout << "Rendering..." <<endl;
	std::string cmdl = "cd ..;"+progName+"demo "+"-w "+width+"-h "+height+"-p "+proj+"--angleDeg "+angle+"-o "+outfile;
	std::system(cmdl.c_str());
	if (oformat != tr("pfm")){
		std::cout << "\rConverting into "+oformat.toStdString() <<endl;
		std::string cmdl2 = "cd ..;"+progName+"pfm2ldr "+oformat.toStdString()+'\x20'+outfile+field("output filename").toString().toStdString()+"."+oformat.toStdString();
		std::system(cmdl2.c_str());
	}
	std::cout <<"Finished!" <<endl;
}

ConclusionPage::ConclusionPage(QWidget *parent) : QWizardPage(parent)
{
	setTitle(tr("Completed!"));

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
