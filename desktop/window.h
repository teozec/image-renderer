#ifndef WINDOW_H
#define WINDOW_H

#include <QWizard>

class QCheckBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QComboBox;
class QDoubleSpinBox;
class QSpinBox;

class Wizard : public QWizard
{
	Q_OBJECT

public:
	enum { Page_Intro, Page_Menu, Page_pfm2ldr, Page_pfm2png, Page_raytracer,
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

class pfm2ldrPage : public QWizardPage
{
	Q_OBJECT

public:
	pfm2ldrPage(QWidget *parent = 0);

	int nextId() const Q_DECL_OVERRIDE;

private:
	QLabel *formatLabel;
	QLabel *ifilenameLabel;
	QLabel *ofilenameLabel;
	QLabel *aFactorLabel;
	QLabel *gammaLabel;
	QComboBox *formatDropdownMenu;
	QString format;
	QLineEdit *ifilenameLineEdit;
	QLineEdit *ofilenameLineEdit;
	QDoubleSpinBox *aFactorSpinner;
	QDoubleSpinBox *gammaSpinner;

public slots:
	void setPlaceholder();
};

class pfm2pngPage : public QWizardPage
{
	Q_OBJECT

public:
	pfm2pngPage(QWidget *parent = 0);

	int nextId() const Q_DECL_OVERRIDE;

private:
};

class raytracerPage : public QWizardPage
{
	Q_OBJECT

public:
	raytracerPage(QWidget *parent = 0);

	int nextId() const Q_DECL_OVERRIDE;

public slots:
	void setupDemo() const;

private:
	QLabel *widthLabel;
	QLabel *heightLabel;
	QLabel *projectionLabel;
	QLabel *angleCamLabel;
	QLabel *ofilenameLabel;

	QPushButton *demoButton;
	QSpinBox *widthSpinner;
	QSpinBox *heightSpinner;
	QComboBox *projectionDropdownMenu;
	QSpinBox *angleCamSpinner;
	QLineEdit *ofilenameLineEdit;
	QCheckBox *advancedCheckBox;
	QComboBox *oformatDropdownMenu;
};

class ConclusionPage : public QWizardPage
{
	Q_OBJECT

public:
	ConclusionPage(QWidget *parent = 0);

	void initializePage() Q_DECL_OVERRIDE;
	int nextId() const Q_DECL_OVERRIDE;

private:
	QLabel *bottomLabel;
	QCheckBox *agreeCheckBox;
};

#endif //WINDOW_H
