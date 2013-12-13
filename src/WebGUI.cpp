//============================================================================
// Name        : WebGUI.cpp
// Author      : Li, Yuan
// email	   : abysmli@gmail.com
// Version     : 1.3
// Copyright   : All rights reserved by Anhalt University of Applied Sciences
// Description : Web Graphical User Interface of LTest
//============================================================================

#include "WebGUI.h"

/*! \brief Constructor
 *  \param iFaultCode [Error code]
 *  \param sFaultMessage [Error informations]
 */
Exception::Exception(uint16_t iFaultCode, std::string sFaultMessage)
{
	this->iFaultCode = iFaultCode;
	this->sFaultMessage = sFaultMessage;
}

// FIXME do we actually need this
/*! \brief save Faultmessages in Log file or in syslog
 */
void Exception::saveInLog()
{
//	std::ofstream outfile("Log", std::ios::app);
//	outfile << sFaultMessage << std::endl;
//	outfile.close();
}

/*! \brief dispose errors
 *  \details iFaultCode = -1# : gsoap errors
 *  \details iFaultCode = 1 : Session Configuration errors
  * \return sFaultMessage [return Error message]
 */
std::string Exception::what()
{
//	saveInLog();
	return sFaultMessage;
}

/*! \brief generate a PDF image from chart
 *  \param *parent [parent container]
 */
GetImgResource::GetImgResource(Wt::WObject *parent,
		Wt::Chart::WCartesianChart *chart) :
		Wt::WPdfImage(1200, 700, parent)
{
	try
	{
		// name the PDF image
		suggestFileName("chart.pdf");
		Wt::WPainter painter(this);
		chart->initLayout();
		// generate the PDF image
		chart->paint(painter);
	} catch (Wt::WException &e)
	{
		std::cout << "\nWException: " << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "\nWException: Unexpected Error!\n";
	}
}

/*! \brief generate CSV file from the measure result
 *  \param *parent [parent container]
 *  \param sCsvFilename [ setting csv Filename for downloading ]
 */
GetResource::GetResource(Wt::WObject *parent = 0, std::string sCsvFilename = "") :
		Wt::WResource(parent)
{
	// set up csv File name
	this->sCsvFilename = sCsvFilename;
	// name the measure result
	suggestFileName(sCsvFilename);
}

/*! \brief destructor
 */
GetResource::~GetResource()
{
	// remove widget and release memory space
	beingDeleted();
}

/*! \brief handle Request from HTML page
 *  \param &request [HTTP Request]
 *  \param &response [HTTP Response]
 */
void GetResource::handleRequest(const Wt::Http::Request &request,
		Wt::Http::Response &response)
{
	// set up the format for the file
	response.setMimeType("plain/text");
	std::ifstream infile(sCsvFilename);
	std::string sLine = "";
	// get datum from original file
	while (getline(infile, sLine))
	{
		// create csv file for downloading
		response.out() << sLine << std::endl;
	}
	infile.close();
}

/*! \brief constructor
 *  \param *parent [parent container]
 */
PanelList::PanelList(WContainerWidget *parent) :
		WContainerWidget(parent)
{
	// default close the panel list
	wasExpanded_ = -1;
}

/*! \brief add panellist widget and set up this widget
 *  \param &text [the title of panellist]
 *  \param *w [the widget, which will be set into panellist]
 *  \param bOpenFlag [false: by default close panellist | true: by default open panellist]
 *  \param bSetCollapsibel [true: open dynamic feature | false: close it]
 *  \return p [return panellist widget]
 */
Wt::WPanel *PanelList::addWidget(const Wt::WString& text, WWidget *w,
		bool bOpenFlag = false, bool bSetCollapsibel = true)
{
	Wt::WPanel *p = new Wt::WPanel();
	// set title of panellist 
	p->setTitle(text);
	// set a sub widget into panellist
	p->setCentralWidget(w);
	// set up panellist
	addPanel(p, bOpenFlag, bSetCollapsibel);
	return p;
}

/*! \brief set up panellist
 *  \param *panel [get panellist widget, which will be set up]
 *  \param bOpenFlag [false: by default close panellist | true: by default open panellist]
 *  \param bSetCollapsibel [true: open dynamic feature | false: close it]
 */
void PanelList::addPanel(Wt::WPanel *panel, bool bOpenFlag,
		bool bSetCollapsibel)
{
	// set up dynamic feature
	panel->setCollapsible(bSetCollapsibel);
	if (!bOpenFlag)
	{
		// set up default state of panellist
		panel->collapse();
	}
	//panel->expandedSS().connect(this, &PanelList::onExpand);
	WContainerWidget::addWidget(panel);
}

/*! \brief make sure in one time can only one panellist open
 *  \param notUndo [true: already opened | close: closed]
 */
void PanelList::onExpand(bool notUndo)
{
	Wt::WPanel *panel = dynamic_cast<Wt::WPanel *>(sender());
	if (notUndo)
	{
		wasExpanded_ = -1;
		for (unsigned i = 0; i < children().size(); ++i)
		{
			Wt::WPanel *p = dynamic_cast<Wt::WPanel *>(children()[i]);
			if (p != panel)
			{
				if (!p->isCollapsed())
					wasExpanded_ = i;
				p->collapse();
			}
		}
	}
	else
	{
		if (wasExpanded_ != -1)
		{
			Wt::WPanel *p = dynamic_cast<Wt::WPanel *>(children()[wasExpanded_]);
			p->expand();
		}
	}
}

/*! \brief configuration the chart and update it dynamically
 *  \param *chart [get chart widget, which will be set up]
 *  \param *parent [get parent container]
 */
ChartConfig::ChartConfig(Wt::Chart::WCartesianChart *chart,
		Wt::WContainerWidget *parent) :
		Wt::WContainerWidget(parent), chart_(chart)
{
	// set up legend style and position
	chart_->setLegendStyle(chart_->legendFont(), Wt::WPen(Wt::black),
			Wt::WBrush(Wt::WColor(0xFF, 0xFA, 0xE5)));
	// initialize the chart widget
	chart->initLayout();
	// prepare the panellist and later set the chart configuration widget into the panellist
	PanelList *list = new PanelList(this);

	// validator for float values
	Wt::WDoubleValidator *anyNumberValidator = new Wt::WDoubleValidator(this);
	anyNumberValidator->setMandatory(true);
	WContainerWidget *ChartProperiesContainer = new WContainerWidget();

	//-------------------Line Type Editor---------------------//
	Wt::WStandardItemModel *types = new Wt::WStandardItemModel(0, 1, this);
	addEntry(types, "Line");
	addEntry(types, "Points");
	addEntry(types, "Curve");

	Wt::WStandardItemModel *markers = new Wt::WStandardItemModel(0, 1, this);
	addEntry(markers, "None");
	addEntry(markers, "Square");
	addEntry(markers, "Circle");
	addEntry(markers, "Cross");
	addEntry(markers, "X Cross");
	addEntry(markers, "Triangle");

	Wt::WStandardItemModel *xAxis_name = new Wt::WStandardItemModel(0, 1, this);
	addEntry(xAxis_name, "Relative Time");
	addEntry(xAxis_name, "Sample Number");

	Wt::WStandardItemModel *yAxis_name = new Wt::WStandardItemModel(0, 1, this);
	addEntry(yAxis_name, "Corrected Delay");
	addEntry(yAxis_name, "Delay");

	Wt::WText *LineEditorTitle_Text = new Wt::WText("<h4>Line Editor: </h4>",
			Wt::XHTMLText, ChartProperiesContainer);
	// set up the Style of widget
	LineEditorTitle_Text->setStyleClass("control-label");

	// -----------------Line Typer Editor----------------------//
	Wt::WLabel *LineTypeLabel = new Wt::WLabel("Line Type: ",
			ChartProperiesContainer);
	// set up the Style of widget
	LineTypeLabel->setStyleClass("control-label");
	typeEdit = new Wt::WComboBox(ChartProperiesContainer);
	typeEdit->setModel(types);
	connectSignals(typeEdit);
	Wt::WText *BR = new Wt::WText("<br/>", Wt::XHTMLText,
			ChartProperiesContainer);

	// ------------Line Marker Editor----------------------//
	Wt::WLabel *LineMarkerLabel = new Wt::WLabel("Marker Type: ",
			ChartProperiesContainer);
	// set up the Style of widget
	LineMarkerLabel->setStyleClass("control-label");
	markerEdit = new Wt::WComboBox(ChartProperiesContainer);
	markerEdit->setModel(markers);
	connectSignals(markerEdit);
	BR = new Wt::WText("<br/>", Wt::XHTMLText, ChartProperiesContainer);

	// ------------Value Label-----------------------------//
	Wt::WLabel *ValueLabel = new Wt::WLabel("Value Label: ",
			ChartProperiesContainer);
	// set up the Style of widget
	ValueLabel->setStyleClass("control-label");
	valueLabelEdit = new Wt::WCheckBox(ChartProperiesContainer);
	connectSignals(valueLabelEdit);
	BR = new Wt::WText("<br/><hr/>", Wt::XHTMLText, ChartProperiesContainer);
	// set up the Style of widget
	BR->setStyleClass("control-label");
	Wt::WText *AxisSelectorTitle_Text = new Wt::WText(
			"<h4>Axis Selector: </h4>", Wt::XHTMLText, ChartProperiesContainer);
	AxisSelectorTitle_Text->setStyleClass("control-label");

	// ------------------------XAxis----------------------//
	Wt::WLabel *XAxisLabel = new Wt::WLabel("x-Axis: ",
			ChartProperiesContainer);
	// set up the Style of widget
	XAxisLabel->setStyleClass("control-label");
	XAxisEdit = new Wt::WComboBox(ChartProperiesContainer);
	XAxisEdit->setModel(xAxis_name);
	connectSignals(XAxisEdit);
	BR = new Wt::WText("<br/>", Wt::XHTMLText, ChartProperiesContainer);

	// ------------------------YAxis----------------------//
	Wt::WLabel *YAxisLabel = new Wt::WLabel("y-Axis: ",
			ChartProperiesContainer);
	// set up the Style of widget
	YAxisLabel->setStyleClass("control-label");
	YAxisEdit = new Wt::WComboBox(ChartProperiesContainer);
	YAxisEdit->setModel(yAxis_name);
	connectSignals(YAxisEdit);
	BR = new Wt::WText("<br/><hr/>", Wt::XHTMLText, ChartProperiesContainer);
	Wt::WText *AxisConfigTitle_Text = new Wt::WText("<h4>Axis Config: </h4>",
			Wt::XHTMLText, ChartProperiesContainer);
	// set up the Style of widget
	AxisConfigTitle_Text->setStyleClass("control-label");

	// ---- Axis properties ----
	Wt::WTable *axisConfig = new Wt::WTable(ChartProperiesContainer);
	axisConfig->setMargin(Wt::WLength::Auto, Wt::Left | Wt::Right);
	axisConfig->setWidth("100%");
	addHeader(axisConfig, "Axis");
	addHeader(axisConfig, "Visible");
	addHeader(axisConfig, "Automatic");
	addHeader(axisConfig, "Minimum");
	addHeader(axisConfig, "Maximum");
	addHeader(axisConfig, "Gridlines");

	// loop for setting up the control widget
	for (uint16_t i = 0; i < 2; ++i)
	{
		const char *axisName[] =
		{ "X:", "Y:" };
		uint16_t j = i + 1;

		const Wt::Chart::WAxis& axis = chart_->axis(
				static_cast<Wt::Chart::Axis>(i));

		AxisControl sc;
		// set up the name of axis
		new Wt::WText(Wt::WString(axisName[i], Wt::UTF8),
				axisConfig->elementAt(j, 0));

		// set up the axis visible of not
		sc.visibleEdit = new Wt::WCheckBox(axisConfig->elementAt(j, 1));
		sc.visibleEdit->setChecked(axis.isVisible());
		connectSignals(sc.visibleEdit);

		// set up the size or range of axises
		bool autoValues = axis.autoLimits()
				== (Wt::Chart::MinimumValue | Wt::Chart::MaximumValue);

		sc.minimumEdit = new Wt::WLineEdit(axisConfig->elementAt(j, 3));
		sc.minimumEdit->setText(
				Wt::WLocale::currentLocale().toString(axis.minimum()));
		sc.minimumEdit->setValidator(anyNumberValidator);
		sc.minimumEdit->setEnabled(!autoValues);
		connectSignals(sc.minimumEdit);

		sc.maximumEdit = new Wt::WLineEdit(axisConfig->elementAt(j, 4));
		sc.maximumEdit->setText(
				Wt::WLocale::currentLocale().toString(axis.maximum()));
		sc.maximumEdit->setValidator(anyNumberValidator);
		sc.maximumEdit->setEnabled(!autoValues);
		connectSignals(sc.maximumEdit);

		sc.autoEdit = new Wt::WCheckBox(axisConfig->elementAt(j, 2));
		sc.autoEdit->setChecked(autoValues);
		connectSignals(sc.autoEdit);
		sc.autoEdit->checked().connect(sc.maximumEdit, &Wt::WLineEdit::disable);
		sc.autoEdit->unChecked().connect(sc.maximumEdit,
				&Wt::WLineEdit::enable);
		sc.autoEdit->checked().connect(sc.minimumEdit, &Wt::WLineEdit::disable);
		sc.autoEdit->unChecked().connect(sc.minimumEdit,
				&Wt::WLineEdit::enable);

		// open grid line
		sc.gridLinesEdit = new Wt::WCheckBox(axisConfig->elementAt(j, 5));
		connectSignals(sc.gridLinesEdit);
		axisConfig->rowAt(j)->setStyleClass("control");
		axisControls_.push_back(sc);
	}
	// set up the Style of widget
	axisConfig->setStyleClass("table table-bordered table-condensed");
	// set the control widget into panellist widget
	Wt::WPanel *p = list->addWidget("Chart properties",	ChartProperiesContainer);
	// set up the size and position and margin of panellist
	p->setMargin(Wt::WLength::Auto, Wt::Left | Wt::Right);
	p->resize(880, Wt::WLength::Auto);
	p->setMargin(20, Wt::Top | Wt::Bottom);

	// get Chart and create pdf file for downloading
	getChart_Button = new Wt::WPushButton("Get Chart", parent);
	getChart_Button->setLink(new GetImgResource(parent, chart));
	getChart_Button->setStyleClass("btn-warning");

	/*
	 * If we do not have JavaScript, then add a button to reflect changes to
	 * the chart.
	 */
	if (!Wt::WApplication::instance()->environment().javaScript())
	{
		Wt::WPushButton *b = new Wt::WPushButton(this);
		b->setText("Update chart");
		b->setInline(false); // so we can add margin to center horizontally
		b->setMargin(Wt::WLength::Auto, Wt::Left | Wt::Right);
		b->clicked().connect(this, &ChartConfig::update);
	}
}

/*! \brief set up the header of model
 *  \param *t [the table widget, which the header set for]
 *  \param *value [the name of header]
 */
void ChartConfig::addHeader(Wt::WTable *t, const char *value)
{
	t->elementAt(0, t->columnCount())->addWidget(new Wt::WText(value));
}

/*! \brief set up the content of model, insert rows and data
 *  \param *model [data model]
 *  \param *value [data]
 */
void ChartConfig::addEntry(Wt::WAbstractItemModel *model, const char *value)
{
	model->insertRows(model->rowCount(), 1);
	model->setData(model->rowCount() - 1, 0, boost::any(std::string(value)));
}

/*! \brief get double value from HTML element
 *  \param *edit [HTML input element]
 *  \param &value [the value from input element]
 *  \return [true: convert no error, false: errors occurred]
 *  \details in order to avoid crash the WebServer by error input
 */ 
bool ChartConfig::getDouble(Wt::WLineEdit *edit, double& value)
{
	try
	{
		value = Wt::WLocale::currentLocale().toDouble(edit->text());
		return true;
	} catch (...)
	{
		return false;
	}
}

/*! \brief update the chart by settings
 */
void ChartConfig::update()
{
	try
	{
		// select the 7th column of model as YAxis
		uint16_t iYAxisModelIndex(7);
		// select the 3th column of model as YAxis
		uint16_t iXAxisModelIndex(3);
		// set which column of model YAxis used 
		switch (YAxisEdit->currentIndex())
		{
		case 0:
			iYAxisModelIndex = 7;
			break;
		case 1:
			iYAxisModelIndex = 4;
			break;
		default:
			iYAxisModelIndex = 7;
			break;
		}
		Wt::Chart::WDataSeries s(iYAxisModelIndex);
		// set which column of model XAxis used 
		switch (XAxisEdit->currentIndex())
		{
		case 0:
			iXAxisModelIndex = 3;
			chart_->axis(static_cast<Wt::Chart::Axis>(0)).setTitle(
					"Relative Time [s]");
			if (typeEdit->count() == 4)
			{
				typeEdit->removeItem(3);
			}
			break;
		case 1:
			iXAxisModelIndex = 0;
			chart_->axis(static_cast<Wt::Chart::Axis>(0)).setTitle(
					"Sample Number");
			if (typeEdit->count() == 3)
			{
				typeEdit->addItem("Bar");
			}
			break;
		default:
			iXAxisModelIndex = 3;
			chart_->axis(static_cast<Wt::Chart::Axis>(0)).setTitle(
					"Relative Time [s]");
			if (typeEdit->count() == 4)
			{
				typeEdit->removeItem(3);
			}
			break;
		}
		// update chart with new settings
		s.setXSeriesColumn(iXAxisModelIndex);
		std::vector<Wt::Chart::WDataSeries> series;

		// set up line type: point, line, curve or bar
		switch (typeEdit->currentIndex())
		{
		case 0:
			s.setType(Wt::Chart::LineSeries);
			break;
		case 1:
			s.setType(Wt::Chart::PointSeries);
			if (markerEdit->currentIndex() == 0)
			{
				markerEdit->setCurrentIndex(1);
			}
			break;
		case 2:
			s.setType(Wt::Chart::CurveSeries);
			break;
		case 3:
			s.setType(Wt::Chart::BarSeries);
			break;
		}
		// set up point type
		s.setMarker(
				static_cast<Wt::Chart::MarkerType>(markerEdit->currentIndex()));
		if (valueLabelEdit->isChecked())
		{
			s.setLabelsEnabled(Wt::Chart::YAxis);
		}
		else
		{
			s.setLabelsEnabled(Wt::Chart::YAxis, false);
		}
		// set all the parameters in a vector
		series.push_back(s);
		chart_->setSeries(series);
		for (uint16_t i = 0; i < 2; ++i)
		{
			// set up size and range of axises
			AxisControl& sc = axisControls_[i];
			Wt::Chart::WAxis& axis = chart_->axis(
					static_cast<Wt::Chart::Axis>(i));
			axis.setVisible(sc.visibleEdit->isChecked());
			if (sc.autoEdit->isChecked())
			{
				axis.setAutoLimits(
						Wt::Chart::MinimumValue | Wt::Chart::MaximumValue);
			}
			else
			{
				if (validate(sc.minimumEdit) && validate(sc.maximumEdit))
				{
					double min, max;
					getDouble(sc.minimumEdit, min);
					getDouble(sc.maximumEdit, max);
					axis.setRange(min, max);
				}

			}
			// enable grid line or disable
			axis.setGridLinesEnabled(sc.gridLinesEdit->isChecked());
		}
		// create pdf image from chart for downloading
		if (markerEdit->currentIndex() != 2)
		{
			getChart_Button->setLink(new GetImgResource(parent, chart_));
		}
	} catch (Wt::WException &e)
	{
		std::cout << "\nWException: " << e.what() << std::endl;
	} catch (...)
	{
		std::cout << "\nWException: Unexpected Error!\n";
	}
}

/*! \brief validator of chart configuration
 *  \param *w [widget, which will be validated]
 *  \return valid [true: valid, false: invalid]
 */
bool ChartConfig::validate(Wt::WFormWidget *w)
{
	bool valid = w->validate() == Wt::WValidator::Valid;
	if (!Wt::WApplication::instance()->environment().javaScript())
	{
		w->setStyleClass(valid ? "" : "Wt-invalid");
		w->setToolTip(valid ? "" : "Invalid value");
	}
	return valid;
}

/*! \brief connect widget change signal to update method
 *  \param *w [widget, which will be connected]
 */
void ChartConfig::connectSignals(Wt::WFormWidget *w)
{
	w->changed().connect(this, &ChartConfig::update);
	if (dynamic_cast<Wt::WLineEdit *>(w))
		w->enterPressed().connect(this, &ChartConfig::update);
}

/*! \brief set up the widgets in HTML Configuration Session page
 *  \*parent [parent container]
 */
SessionConfig::SessionConfig(Wt::WContainerWidget *parent) :
		Wt::WContainerWidget(parent)
{
	// Flag of error, when some errors in session configuration bErrorFlag = false
	bErrorFlag = true;
	// Flag of Sending mode, when sending by time bSendingModeFlag=true, when sending by amount of packets bSendingModeFlag=false
	bSendingModeFlag = true;
	Wt::WContainerWidget *ConfigureSessionSubContainer_1 =
			new Wt::WContainerWidget(parent);
	ConfigureSessionSubContainer_1->setStyleClass("row");

	Wt::WGroupBox *groupBox = new Wt::WGroupBox("Configure Session",
			ConfigureSessionSubContainer_1);
	groupBox->addStyleClass("container");

	Wt::WContainerWidget *PanelListContainer = new Wt::WContainerWidget(
			ConfigureSessionSubContainer_1);
	PanelList *list = new PanelList(PanelListContainer);

	Wt::WContainerWidget *AdvancedSetting_Container =
			new Wt::WContainerWidget();
	AdvancedSetting_Container->setStyleClass("row");

	Wt::WContainerWidget *BasicSetting_Container = new Wt::WContainerWidget();
	BasicSetting_Container->setStyleClass("row");

	//----------------------------Integer validator-------------------------------------//
	Wt::WIntValidator *IntNumberValidator = new Wt::WIntValidator();
	IntNumberValidator->setMandatory(true);

	//----------------------------Sending Mode--------------------------------------//
	Wt::WContainerWidget *SendingMode_Container = new Wt::WContainerWidget();
	SendingMode_Container->setStyleClass("control");
	Wt::WLabel *SendingMode_Label = new Wt::WLabel("Sending Mode: ",
			SendingMode_Container);
	SendingMode_Label->setStyleClass("control-label");
	Wt::WText *BR = new Wt::WText("<br/>", Wt::XHTMLText,
			SendingMode_Container);
	Wt::WButtonGroup *SendingModeRadio_group = new Wt::WButtonGroup(
			SendingMode_Container);
	SendingModeRadio_TimeMode_button = new Wt::WRadioButton("Sending by Time",
			SendingMode_Container);
	SendingModeRadio_group->addButton(SendingModeRadio_TimeMode_button);
	SendingModeRadio_PacketSizeMode_button = new Wt::WRadioButton(
			"Sending by amount of Packets", SendingMode_Container);
	SendingModeRadio_group->addButton(SendingModeRadio_PacketSizeMode_button);
	SendingModeRadio_group->setSelectedButtonIndex(0);
	connectSignals(SendingModeRadio_TimeMode_button);
	connectSignals(SendingModeRadio_PacketSizeMode_button);

	//----------------------------ReceiverIP-----------------------------------//
	Wt::WContainerWidget *ReceiverIP_Container = new Wt::WContainerWidget(
			BasicSetting_Container);
	ReceiverIP_Container->setStyleClass("span4");
	Wt::WLabel *ReceiverIP_Label = new Wt::WLabel(
			"Receiver's Address [IP:Port]: ", ReceiverIP_Container);
	ReceiverIP_Label->setStyleClass("control-label");
	BR = new Wt::WText("<br/>", Wt::XHTMLText, ReceiverIP_Container);
	ReceiverIP_LineEdit = new Wt::WLineEdit("10.33.21.14",
			ReceiverIP_Container);
	connectSignals(ReceiverIP_LineEdit);

	//----------------------------Protocol-----------------------------------//
	Wt::WContainerWidget *Protocol_Container = new Wt::WContainerWidget(
			BasicSetting_Container);
	Protocol_Container->setStyleClass("span4");
	Wt::WLabel *Protocol_Label = new Wt::WLabel("Protocol: ",
			Protocol_Container);
	Protocol_Label->setStyleClass("control-label");
	BR = new Wt::WText("<br/>", Wt::XHTMLText, Protocol_Container);
	Protocol_ComboBox = new Wt::WComboBox(Protocol_Container);
	Protocol_ComboBox->addItem("UDP");
	Protocol_ComboBox->addItem("TCP");

	//----------------------------DataRate-----------------------------------//
	Wt::WContainerWidget *DataRate_Container = new Wt::WContainerWidget(
			BasicSetting_Container);
	DataRate_Container->setStyleClass("span4");
	Wt::WLabel *DataRate_Label = new Wt::WLabel("Data Rate [bit/s]: ",
			DataRate_Container);
	DataRate_Label->setStyleClass("control-label");
	BR = new Wt::WText("<br/>", Wt::XHTMLText, DataRate_Container);
	Datarate_LineEdit = new Wt::WLineEdit("1000000", DataRate_Container);
	connectSignals(Datarate_LineEdit);

	//----------------------------PacketSize_Container-----------------------------------//
	Wt::WContainerWidget *PacketSize_Container = new Wt::WContainerWidget(
			BasicSetting_Container);
	PacketSize_Container->setStyleClass("span4");
	Wt::WLabel *PacketSize_Label = new Wt::WLabel("Packet Size [B]: ",
			PacketSize_Container);
	PacketSize_Label->setStyleClass("control-label");
	BR = new Wt::WText("<br/>", Wt::XHTMLText, PacketSize_Container);
	PacketSize_LineEdit = new Wt::WLineEdit("1024", PacketSize_Container);
	connectSignals(PacketSize_LineEdit);

	//----------------------------SessionDuration-----------------------------------//
	SessionDuration_Container = new Wt::WContainerWidget(
			BasicSetting_Container);
	SessionDuration_Container->setStyleClass("span4");
	Wt::WLabel *SessionDuration_Label = new Wt::WLabel(
			"Session Duration [sec(.usec)]: ", SessionDuration_Container);
	SessionDuration_Label->setStyleClass("control-label");
	BR = new Wt::WText("<br/>", Wt::XHTMLText, SessionDuration_Container);
	SessionDuration_LineEdit = new Wt::WLineEdit("5",
			SessionDuration_Container);
	connectSignals(SessionDuration_LineEdit);

	//----------------------------StartTime-----------------------------------//
	StartTime_Container = new Wt::WContainerWidget(BasicSetting_Container);
	StartTime_Container->setStyleClass("span4");
	Wt::WLabel *StartTime_Label = new Wt::WLabel("Start Time [sec(.usec)]: ",
			StartTime_Container);
	StartTime_Label->setStyleClass("control-label");
	StartTime_CheckBox = new Wt::WCheckBox("Unix Time", StartTime_Container);
	BR = new Wt::WText("<br/>", Wt::XHTMLText, StartTime_Container);
	StartTime_LineEdit = new Wt::WLineEdit("0", StartTime_Container);
	connectSignals(StartTime_LineEdit);

	//----------------------------FinishTime-----------------------------------//
	FinishTime_Container = new Wt::WContainerWidget(BasicSetting_Container);
	FinishTime_Container->setStyleClass("span4");
	Wt::WLabel *FinishTime_Label = new Wt::WLabel("Finish Time [sec(.usec)]: ",
			FinishTime_Container);
	FinishTime_Label->setStyleClass("control-label");
	BR = new Wt::WText("<br/>", Wt::XHTMLText, FinishTime_Container);
	FinishTime_LineEdit = new Wt::WLineEdit("5", FinishTime_Container);
	connectSignals(FinishTime_LineEdit);

	//----------------------------RelativeTime-------------------------------//
	RelativeTime_Container = new Wt::WContainerWidget(BasicSetting_Container);
	RelativeTime_Container->setStyleClass("span3 alert alert-info");
	Wt::WText *RelativeTime_Label =
			new Wt::WText(
					"Start measurement session with a given time shift from now. If [Unix Time] selected, seconds are interpreted as absolute start time in seconds and microseconds from Unix Zero Time.",
					RelativeTime_Container);
	RelativeTime_Label->setStyleClass("control-label");

	//----------------------------UnixTime-----------------------------------//
	UnixTime_Container = new Wt::WContainerWidget(BasicSetting_Container);
	UnixTime_Container->setStyleClass("span4");
	Wt::WLabel *CurrentUnixTime_Label = new Wt::WLabel(
			"Current Unix Time [sec(.usec)]: ", UnixTime_Container);
	CurrentUnixTime_Label->setStyleClass("control-label");
	BR = new Wt::WText("<br/>", Wt::XHTMLText, UnixTime_Container);
	Wt::WText *CurrentUnixTime = new Wt::WText(UnixTime_Container);
	BR = new Wt::WText("<br/>", Wt::XHTMLText, UnixTime_Container);
	Wt::WPushButton *getUnixTime_Button = new Wt::WPushButton(
			"Get Current Unix Time", UnixTime_Container);
	// set a timer for calculating the Unix time
	intervalTimer = new Wt::WTimer(UnixTime_Container);
	// refresh interval 1 second
	intervalTimer->setInterval(1000);
	HPTimer::HighPerTimer *pCurrentTime_ptr = new HPTimer::HighPerTimer();
	// display the unix time in html page
	intervalTimer->timeout().connect(
			std::bind(
					[=]()
					{
						pCurrentTime_ptr->SetNow();
						CurrentUnixTime->setText(Wt::WLocale::currentLocale().toString(pCurrentTime_ptr->Seconds())+"."+Wt::WLocale::currentLocale().toString(pCurrentTime_ptr->NSeconds()));
					}));
	// set current unix time into element
	getUnixTime_Button->clicked().connect(std::bind([=]()
	{
		StartTime_LineEdit->setText(CurrentUnixTime->text());
		StartTime_LineEdit->setFocus();
		update();
	}));
	UnixTime_Container->hide();
	// sending mode by time
	StartTime_CheckBox->changed().connect(
			std::bind(
					[=]()
					{
						// display current unix time in html page
						if(StartTime_CheckBox->isChecked())
						{
							RelativeTime_Container->hide();
							pCurrentTime_ptr->SetNow();
							CurrentUnixTime->setText(Wt::WLocale::currentLocale().toString(pCurrentTime_ptr->Seconds())+"."+Wt::WLocale::currentLocale().toString(pCurrentTime_ptr->NSeconds()/1000));
							UnixTime_Container->show();
							intervalTimer->start();
						}
						//
						else
						{
							StartTime_LineEdit->setText("0");
							StartTime_LineEdit->setFocus();
							UnixTime_Container->hide();
							intervalTimer->stop();
							update();
							RelativeTime_Container->show();
						}
					}));

	//----------------------------AmountOfPackets-----------------------------------//
	AmountOfPackets_Container = new Wt::WContainerWidget(
			BasicSetting_Container);
	AmountOfPackets_Container->setStyleClass("span4");
	Wt::WLabel *AmountOfPackets_Label = new Wt::WLabel("Amount of packets: ",
			AmountOfPackets_Container);
	AmountOfPackets_Label->setStyleClass("control-label");
	BR = new Wt::WText("<br/>", Wt::XHTMLText, AmountOfPackets_Container);
	AmountOfPackets_LineEdit = new Wt::WLineEdit("1000",
			AmountOfPackets_Container);
	connectSignals(AmountOfPackets_LineEdit);

	//----------------------------AmountOfBytes-----------------------------------//
	AmountOfBytes_Container = new Wt::WContainerWidget(BasicSetting_Container);
	AmountOfBytes_Container->setStyleClass("span4");
	Wt::WLabel *AmountOfBytes_Label = new Wt::WLabel("Amount of bytes: ",
			AmountOfBytes_Container);
	AmountOfBytes_Label->setStyleClass("control-label");
	BR = new Wt::WText("<br/>", Wt::XHTMLText, AmountOfBytes_Container);
	AmountOfBytes_LineEdit = new Wt::WLineEdit("1024000",
			AmountOfBytes_Container);
	connectSignals(AmountOfBytes_LineEdit);

	//----------------------------SenderIP-----------------------------------//
	Wt::WContainerWidget *SenderIP_Container = new Wt::WContainerWidget(
			AdvancedSetting_Container);
	SenderIP_Container->setStyleClass("span4");
	Wt::WLabel *SenderIP_Label = new Wt::WLabel(
			"Sender's Interface [IP:Port]: ", SenderIP_Container);
	SenderIP_Label->setStyleClass("control-label");
	BR = new Wt::WText("<br/>", Wt::XHTMLText, SenderIP_Container);
	SenderIP_LineEdit = new Wt::WLineEdit(SenderIP_Container);
	connectSignals(SenderIP_LineEdit);

	//----------------------------LocalSoapAddress-----------------------------------//
	Wt::WContainerWidget *LocalSoapAddress_Container = new Wt::WContainerWidget(
			AdvancedSetting_Container);
	LocalSoapAddress_Container->setStyleClass("span4");
	Wt::WLabel *LocalSoapAddress_Label = new Wt::WLabel(
			"Local SOAP Address [IP:Port]: ", LocalSoapAddress_Container);
	LocalSoapAddress_Label->setStyleClass("control-label");
	BR = new Wt::WText("<br/>", Wt::XHTMLText, LocalSoapAddress_Container);
	LocalSoapAddress_LineEdit = new Wt::WLineEdit("0.0.0.0:5554",
			LocalSoapAddress_Container);
	connectSignals(LocalSoapAddress_LineEdit);

	//----------------------------receiverSoapAddress-----------------------------------//
	Wt::WContainerWidget *receiverSoapAddress_Container =
			new Wt::WContainerWidget(AdvancedSetting_Container);
	receiverSoapAddress_Container->setStyleClass("span4");
	Wt::WLabel *receiverSoapAddress_Label = new Wt::WLabel(
			"Receiver SOAP Address [IP:Port]: ", receiverSoapAddress_Container);
	receiverSoapAddress_Label->setStyleClass("control-label");
	BR = new Wt::WText("<br/>", Wt::XHTMLText, receiverSoapAddress_Container);
	receiverSoapAddress_LineEdit = new Wt::WLineEdit(
			receiverSoapAddress_Container);
	connectSignals(receiverSoapAddress_LineEdit);

	//----------------------------TypeOfSource-----------------------------------//
	Wt::WContainerWidget *TypeOfSource_Container = new Wt::WContainerWidget(
			AdvancedSetting_Container);
	TypeOfSource_Container->setStyleClass("span4");
	Wt::WLabel *TypeOfSource_Label = new Wt::WLabel("Type of source: ",
			TypeOfSource_Container);
	TypeOfSource_Label->setStyleClass("control-label");
	BR = new Wt::WText("<br/>", Wt::XHTMLText, TypeOfSource_Container);
	TypeOfSource_ComboBox = new Wt::WComboBox(TypeOfSource_Container);
	TypeOfSource_ComboBox->addItem("CBR");
	TypeOfSource_ComboBox->addItem("Poisson");
	TypeOfSource_ComboBox->addItem("FIFO");
	connectSignals(TypeOfSource_ComboBox);

	//----------------------------BlockingMode-----------------------------------//
	Wt::WContainerWidget *BlockingMode_Container = new Wt::WContainerWidget(
			AdvancedSetting_Container);
	BlockingMode_Container->setStyleClass("span4");
	Wt::WLabel *BlockingMode_Label = new Wt::WLabel("Blocking Mode: ",
			BlockingMode_Container);
	BlockingMode_Label->setStyleClass("control-label");
	BR = new Wt::WText("<br/>", Wt::XHTMLText, BlockingMode_Container);
	BlockingMode_CheckBox = new Wt::WCheckBox("Enable", BlockingMode_Container);
	BR = new Wt::WText("<br/>", Wt::XHTMLText, BlockingMode_Container);
	BR = new Wt::WText("<br/>", Wt::XHTMLText, BlockingMode_Container);
	BR->setStyleClass("control-label");

	//----------------------------FIFO File upload---------------------------------//
	FIFO_File_upload_Container = new Wt::WContainerWidget(
			AdvancedSetting_Container);
	fileupload();
	FIFO_File_upload_Container->hide();

	//----------------------------TimeCorrection-----------------------------------//
	Wt::WContainerWidget *TimeCorrection_Container = new Wt::WContainerWidget(
			AdvancedSetting_Container);
	TimeCorrection_Container->setStyleClass("span4");
	Wt::WLabel *TimeCorrection_Label = new Wt::WLabel("Time Correction [us]: ",
			TimeCorrection_Container);
	TimeCorrection_Label->setStyleClass("control-label");
	TimeCorrection_CheckBox = new Wt::WCheckBox("Enable",
			TimeCorrection_Container);
	TimeCorrection_CheckBox->setChecked();
	BR = new Wt::WText("<br/>", Wt::XHTMLText, TimeCorrection_Container);
	Wt::WLabel *ICA_Label = new Wt::WLabel("With Linear Interpolation: ",
				TimeCorrection_Container);
	ICA_CheckBox = new Wt::WCheckBox("Enable",
			TimeCorrection_Container);
	ICA_CheckBox->setChecked();
	BR = new Wt::WText("<br/>", Wt::XHTMLText, TimeCorrection_Container);
	TimeCorrection_LineEdit = new Wt::WLineEdit("1000", TimeCorrection_Container);
	TimeCorrection_CheckBox->changed().connect(std::bind([=]()
	{
		if(TimeCorrection_CheckBox->checkState()==Wt::Checked)
		{
			TimeCorrection_LineEdit->enable();
		}
		else
		{
			TimeCorrection_LineEdit->disable();
			ICA_CheckBox->setUnChecked();
		}
	}));
	TimeCorrection_LineEdit->setValidator(IntNumberValidator);
	Wt::WLabel *TimeCorrection_Explain_Label = new Wt::WLabel(
				"Max Deviation accepted for Time correction",
				TimeCorrection_Container);
	TimeCorrection_Explain_Label->setStyleClass("control-label");

	//----------------------------Priority-----------------------------------//
	Wt::WContainerWidget *Priority_Container = new Wt::WContainerWidget(
			AdvancedSetting_Container);
	Priority_Container->setStyleClass("span4");
	Wt::WLabel *Priority_Label = new Wt::WLabel(
			"Priority: (0 highest ~ 99 lowest)", Priority_Container);
	Priority_Label->setStyleClass("control-label");
	BR = new Wt::WText("<br/>", Wt::XHTMLText, Priority_Container);
	Priority_LineEdit = new Wt::WLineEdit("50", Priority_Container);
	Wt::WLabel *Priority_Explain_Label = new Wt::WLabel(
			"Priority of the process (root permission needed)",
			Priority_Container);
	Priority_Explain_Label->setStyleClass("control-label");
	Priority_LineEdit->setValidator(IntNumberValidator);
	connectSignals(Priority_LineEdit);

	//----------------------------TOS-----------------------------------//
	Wt::WContainerWidget *TOS_Container = new Wt::WContainerWidget(
			AdvancedSetting_Container);
	TOS_Container->setStyleClass("span4");
	Wt::WLabel *TOS_Label = new Wt::WLabel("TOS: (0 ~ 7)", TOS_Container);
	TOS_Label->setStyleClass("control-label");
	BR = new Wt::WText("<br/>", Wt::XHTMLText, TOS_Container);
	TOS_LineEdit = new Wt::WLineEdit("0", TOS_Container);
	Wt::WLabel *TOS_Explain_Label = new Wt::WLabel(
			"TOS: Type of Service for IP Header", TOS_Container);
	TOS_Explain_Label->setStyleClass("control-label");
	TOS_LineEdit->setValidator(IntNumberValidator);
	connectSignals(TOS_LineEdit);

	//---------------------------Set in Panel-------------------------------------//
	Wt::WPanel *p = list->addWidget("Basic Settings", BasicSetting_Container,
			true, false);
	p->setMargin(Wt::WLength::Auto, Wt::Left | Wt::Right);
	p->resize(Wt::WLength::Auto, Wt::WLength::Auto);
	p->setMargin(20, Wt::Top | Wt::Bottom);

	p = list->addWidget("Advanced Settings", AdvancedSetting_Container, false,
			true);
	p->setMargin(Wt::WLength::Auto, Wt::Left | Wt::Right);
	p->resize(Wt::WLength::Auto, Wt::WLength::Auto);
	p->setMargin(20, Wt::Top | Wt::Bottom);

	Information_Container = new Wt::WContainerWidget();
	Information_Container->setStyleClass("alert alert-error");
	Information_Label = new Wt::WText("", Wt::XHTMLText, Information_Container);
	groupBox->addWidget(SendingMode_Container);
	groupBox->addWidget(PanelListContainer);
	groupBox->addWidget(Information_Container);
	// refresh the Elements in Session Configuration page
	bReceiverSOAPAddressChangeFlag = false;
	update();
}

/*! \brief send widget changed signal to update() method
 *  \param *w [widget, which will be connected]
 */
void SessionConfig::connectSignals(Wt::WFormWidget *w)
{
	w->changed().connect(this, &SessionConfig::update);
	if (dynamic_cast<Wt::WLineEdit *>(w))
	{
		w->keyWentUp().connect(this, &SessionConfig::update);
	}
}

/*! \brief update and refresh the session configuration widgets by settings
 */
void SessionConfig::update()
{
	// information display container, by starting update hide
	Information_Container->hide();
	// initialize sErrorMessage
	sErrorMessage = "";
	// initialize bErrorFlag, when error occurred, bErrorFlag=false, when no error, bErrorFlag=true
	bErrorFlag = true;
	// FIFO_File_upload_container by default hide
	FIFO_File_upload_Container->hide();
	// check which mode will be used
	if (SendingModeRadio_TimeMode_button->isChecked())
	{
		// initialize all widgets and set visible attributes
		bSendingModeFlag = true;
		AmountOfBytes_LineEdit->setDisabled(true);
		AmountOfPackets_LineEdit->setDisabled(true);
		SessionDuration_LineEdit->setEnabled(true);
		StartTime_LineEdit->setEnabled(true);
		FinishTime_LineEdit->setEnabled(true);

		AmountOfBytes_Container->setHidden(true);
		AmountOfPackets_Container->setHidden(true);
		SessionDuration_Container->setHidden(false);
		StartTime_Container->setHidden(false);
		FinishTime_Container->setHidden(false);
		// by time sending mode
		if (!StartTime_CheckBox->isChecked())
		{
			RelativeTime_Container->setHidden(false);
		}
		else
		{
			intervalTimer->start();
			UnixTime_Container->show();
		}
		// check whether the value of session duration valid or not
		try
		{
			WebGUIUtils::validator(Time,
					SessionDuration_LineEdit->text().narrow());
		} catch (Exception &Exception)
		{
			bErrorFlag = false;
			sErrorMessage += "<p>By Session Duration: " + Exception.what()
					+ "</p>";
		} catch (...)
		{
			bErrorFlag = false;
			sErrorMessage += "<p>By Session Duration: Unexpected Error!</p>";
		}
		// check whether the value of start time valid or not
		try
		{
			WebGUIUtils::validator(Time, StartTime_LineEdit->text().narrow());
		} catch (Exception &Exception)
		{
			bErrorFlag = false;
			sErrorMessage += "<p>By Start Time: " + Exception.what() + "</p>";
		} catch (...)
		{
			bErrorFlag = false;
			sErrorMessage += "<p>By Start Time: Unexpected Error!</p>";
		}
		// check whether the value of finish time valid or not
		try
		{
			WebGUIUtils::validator(Time, FinishTime_LineEdit->text().narrow());
		} catch (Exception &Exception)
		{
			bErrorFlag = false;
			sErrorMessage += "<p>By Finish Time: " + Exception.what() + "</p>";
		} catch (...)
		{
			bErrorFlag = false;
			sErrorMessage += "<p>By Finish Time: Unexpected Error!</p>";
		}
		// when no errors occurred
		if (bErrorFlag)
		{
			// when user change the value of Session duration, will automatically check, whether the parameter valid, and calculate the value of finish time
			if (SessionDuration_LineEdit->hasFocus())
			{
				try
				{
					uint64_t iSec(0), iUsec(0), i_Sec(0), i_Usec(0);
					// calculate the value of finish time
					WebGUIUtils::parseTimeString(
							StartTime_LineEdit->text().narrow(), iSec, iUsec);
					WebGUIUtils::parseTimeString(
							SessionDuration_LineEdit->text().narrow(), i_Sec,
							i_Usec);
					FinishTime_LineEdit->setText(
							Wt::WLocale::currentLocale().toString(iSec + i_Sec)
									+ "."
									+ Wt::WLocale::currentLocale().toString(
											iUsec + i_Usec));
				} catch (Exception &Exception)
				{
					bErrorFlag = false;
					sErrorMessage += "<p>By Session Duration: "
							+ Exception.what() + "</p>";
				} catch (...)
				{
					bErrorFlag = false;
					sErrorMessage +=
							"<p>By Session Duration: Unexpected Error!</p>";
				}
			}
			// when user change the value of start time, will automatically check, whether the parameter valid, and calculate the value of finish time
			if (StartTime_LineEdit->hasFocus())
			{
				try
				{
					uint64_t iSec(0), iUsec(0), i_Sec(0), i_Usec(0);
					WebGUIUtils::parseTimeString(
							StartTime_LineEdit->text().narrow(), iSec, iUsec);
					WebGUIUtils::parseTimeString(
							SessionDuration_LineEdit->text().narrow(), i_Sec,
							i_Usec);
					// calculate the value of finish time
					FinishTime_LineEdit->setText(
							Wt::WLocale::currentLocale().toString(iSec + i_Sec)
									+ "."
									+ Wt::WLocale::currentLocale().toString(
											iUsec + i_Usec));
				} catch (Exception &Exception)
				{
					bErrorFlag = false;
					sErrorMessage += "<p>By Start Time: " + Exception.what()
							+ "</p>";
				} catch (...)
				{
					bErrorFlag = false;
					sErrorMessage += "<p>By Start Time: Unexpected Error!</p>";
				}
			}
			// when user change the value of finish time, will automatically check, whether the parameter valid, and calculate the value of session duration
			if (FinishTime_LineEdit->hasFocus())
			{
				try
				{
					uint64_t iSec(0), iUsec(0), i_Sec(0), i_Usec(0);
					WebGUIUtils::parseTimeString(
							FinishTime_LineEdit->text().narrow(), iSec, iUsec);
					WebGUIUtils::parseTimeString(
							StartTime_LineEdit->text().narrow(), i_Sec, i_Usec);
					if (iUsec < i_Usec)
					{
						iUsec += 1000000;
						iSec -= 1;
					}
					if (iSec < i_Sec)
					{
						throw Exception(1,
								"Finish Time must bigger than Start Time!");
					}
					// calculate the value of session duration
					SessionDuration_LineEdit->setText(
							Wt::WLocale::currentLocale().toString(iSec - i_Sec)
									+ "."
									+ Wt::WLocale::currentLocale().toString(
											iUsec - i_Usec));
				} catch (Exception &Exception)
				{
					bErrorFlag = false;
					sErrorMessage += "<p>By Finish Time: " + Exception.what()
							+ "</p>";
				} catch (...)
				{
					bErrorFlag = false;
					sErrorMessage += "<p>By Finish Time: Unexpected Error!</p>";
				}
			}
		}
	}
	// by amount of packets sending mode
	else
	{
		// initialize all widgets and set visible attributes
		bSendingModeFlag = false;
		AmountOfBytes_LineEdit->setEnabled(true);
		AmountOfPackets_LineEdit->setEnabled(true);
		SessionDuration_LineEdit->setDisabled(true);
		StartTime_LineEdit->setDisabled(true);
		FinishTime_LineEdit->setDisabled(true);

		AmountOfBytes_Container->setHidden(false);
		AmountOfPackets_Container->setHidden(false);
		SessionDuration_Container->setHidden(true);
		StartTime_Container->setHidden(true);
		FinishTime_Container->setHidden(true);
		RelativeTime_Container->setHidden(true);
		intervalTimer->stop();
		UnixTime_Container->hide();
		// check whether the value of packet size valid or not
		try
		{
			WebGUIUtils::validator(Size, PacketSize_LineEdit->text().narrow());
		} catch (Exception &Exception)
		{
			bErrorFlag = false;
			sErrorMessage += "<p>By Packet Size: " + Exception.what() + "</p>";
		} catch (...)
		{
			bErrorFlag = false;
			sErrorMessage += "<p>By Packet Size: Unexpected Error!</p>";
		}
		// check whether the value of amount of packets valid or not
		try
		{
			WebGUIUtils::validator(Size,
					AmountOfPackets_LineEdit->text().narrow());
		} catch (Exception &Exception)
		{
			bErrorFlag = false;
			sErrorMessage += "<p>By Amount Of Packets: " + Exception.what()
					+ "</p>";
		} catch (...)
		{
			bErrorFlag = false;
			sErrorMessage += "<p>By Amount Of Packets: Unexpected Error!</p>";
		}
		// check whether the value of amount of bytes valid or not
		try
		{
			WebGUIUtils::validator(Size,
					AmountOfBytes_LineEdit->text().narrow());
		} catch (Exception &Exception)
		{
			bErrorFlag = false;
			sErrorMessage += "<p>By Amount Of Bytes: " + Exception.what()
					+ "</p>";
		} catch (...)
		{
			bErrorFlag = false;
			sErrorMessage += "<p>By Amount Of Bytes: Unexpected Error!</p>";
		}
		// when no errors occurred
		if (bErrorFlag)
		{
			// when user change the value of amount of packets, will automatically check, whether the parameter valid, and calculate the value of amount of bytes
			if (AmountOfPackets_LineEdit->hasFocus())
			{
				try
				{
					// calculate the value of amount of bytes
					AmountOfBytes_LineEdit->setText(
							Wt::WLocale::currentLocale().toString(
									WebGUIUtils::StringToDoubleWithUnit(
											AmountOfPackets_LineEdit->text().narrow(),
											WebGUIUtils::checkUnit(
													AmountOfPackets_LineEdit->text().narrow()))
											* WebGUIUtils::StringToDoubleWithUnit(
													PacketSize_LineEdit->text().narrow(),
													WebGUIUtils::checkUnit(
															PacketSize_LineEdit->text().narrow()))));
				} catch (...)
				{
					bErrorFlag = false;
					sErrorMessage +=
							"<p>By Amount Of Bytes: Unexpected Error!</p>";
				}
			}
			// when user change the value of amount of bytes, will automatically check, whether the parameter valid, and calculate the value of amount of packets
			if (AmountOfBytes_LineEdit->hasFocus())
			{
				try
				{
					// calculate the value of amount of packets
					AmountOfPackets_LineEdit->setText(
							Wt::WLocale::currentLocale().toString(
									static_cast<uint64_t>(WebGUIUtils::StringToDoubleWithUnit(
											AmountOfBytes_LineEdit->text().narrow(),
											WebGUIUtils::checkUnit(
													AmountOfBytes_LineEdit->text().narrow()))
											/ WebGUIUtils::StringToDoubleWithUnit(
													PacketSize_LineEdit->text().narrow(),
													WebGUIUtils::checkUnit(
															PacketSize_LineEdit->text().narrow())))));
				} catch (...)
				{
					bErrorFlag = false;
					sErrorMessage +=
							"<p>By Amount Of Bytes: Unexpected Error!</p>";
				}
			}
			// when user change the value of packet size, will automatically check, whether the parameter valid, and calculate the value of amount of bytes
			if (PacketSize_LineEdit->hasFocus())
			{
				try
				{
					// calculate the value of amount of bytes
					AmountOfBytes_LineEdit->setText(
							Wt::WLocale::currentLocale().toString(
									WebGUIUtils::StringToDoubleWithUnit(
											AmountOfPackets_LineEdit->text().narrow(),
											WebGUIUtils::checkUnit(
													AmountOfPackets_LineEdit->text().narrow()))
											* WebGUIUtils::StringToDoubleWithUnit(
													PacketSize_LineEdit->text().narrow(),
													WebGUIUtils::checkUnit(
															PacketSize_LineEdit->text().narrow()))));
				} catch (...)
				{
					bErrorFlag = false;
					sErrorMessage += "<p>By Packet Size: Unexpected Error!</p>";
				}
			}
		}
	}
	// check whether the receiverSoapAddress LineEdit widget has been changed
	// when it has been changed, the value of this widget will be changed manually
	// when not, it will copy the value from the receiver Address widget and change its port!
	if (receiverSoapAddress_LineEdit->hasFocus())
	{
		bReceiverSOAPAddressChangeFlag = true;
	}
	// when by type of source widget FIFO has been chose
	if (TypeOfSource_ComboBox->currentIndex() == 2)
	{
		// display FIFO container
		FIFO_File_upload_Container->show();
	}
	try
	{
		// check the widget ReceiverIP whether is it empty or not
		// this widget can not be empty, when it is empty, it will be error!
		if (ReceiverIP_LineEdit->text().empty())
		{
			bErrorFlag = false;
			sErrorMessage +=
					"<p>By Receiver IP: Receiver IP Address can not be empty!</p>";
		}
		// check whether the value of ReceiverIP widget valid
		if (WebGUIUtils::validator(Address,
				ReceiverIP_LineEdit->text().narrow())
				&& !bReceiverSOAPAddressChangeFlag)
		{
			std::string sAddress = ReceiverIP_LineEdit->text().narrow();
			for (uint16_t i = sAddress.length() - 1; i > 0; i--)
			{
				// ASCII [58] = :
				if (sAddress[i] == 58)
				{
					sAddress = sAddress.substr(0, i);
				}
			}
			// change the port of receiverSOAPAddress widget to 5555
			receiverSoapAddress_LineEdit->setText(sAddress + ":5555");
		}
	} catch (Exception &Exception)
	{
		bErrorFlag = false;
		sErrorMessage += "<p>By Receiver IP: " + Exception.what() + "</p>";
	} catch (...)
	{
		bErrorFlag = false;
		sErrorMessage += "<p>By Receiver IP: Unexpected Error!</p>";
	}
	// check whether the value of sender ip valid or not
	try
	{
		WebGUIUtils::validator(Address, SenderIP_LineEdit->text().narrow());
	} catch (Exception &Exception)
	{
		bErrorFlag = false;
		sErrorMessage += "<p>By Sender Interface: " + Exception.what() + "</p>";
	} catch (...)
	{
		bErrorFlag = false;
		sErrorMessage += "<p>By Sender Interface: Unexpected Error!</p>";
	}
	// check whether the value of local soap address valid or not
	try
	{
		WebGUIUtils::validator(Address,
				LocalSoapAddress_LineEdit->text().narrow());
	} catch (Exception &Exception)
	{
		bErrorFlag = false;
		sErrorMessage += "<p>By Local SOAP Address: " + Exception.what()
				+ "</p>";
	} catch (...)
	{
		bErrorFlag = false;
		sErrorMessage += "<p>By Local SOAP Address: Unexpected Error!</p>";
	}
	// check whether the value of receiverSoapAddress valid or not
	try
	{
		WebGUIUtils::validator(Address,
				receiverSoapAddress_LineEdit->text().narrow());
	} catch (Exception &Exception)
	{
		bErrorFlag = false;
		sErrorMessage += "<p>By Receiver SOAP Address: " + Exception.what()
				+ "</p>";
	} catch (...)
	{
		bErrorFlag = false;
		sErrorMessage += "<p>By Receiver SOAP Address: Unexpected Error!</p>";
	}
	// check whether the value of Datarate valid or not
	try
	{
		WebGUIUtils::validator(Size, Datarate_LineEdit->text().narrow());
	} catch (Exception &Exception)
	{
		bErrorFlag = false;
		sErrorMessage += "<p>By Datarate: " + Exception.what() + "</p>";
	} catch (...)
	{
		bErrorFlag = false;
		sErrorMessage += "<p>By Datarate: Unexpected Error!</p>";
	}
	// check whether the Priority empty or not
	try
	{
		if (Priority_LineEdit->text().empty())

		{
			bErrorFlag = false;
			sErrorMessage += "<p>By Priority: Please set a value!</p>";
		}
		else if (Wt::WLocale::currentLocale().toInt(Priority_LineEdit->text())
				> 99
				|| Wt::WLocale::currentLocale().toInt(Priority_LineEdit->text())
						< 0)
		{
			bErrorFlag = false;
			sErrorMessage +=
					"<p>By Priority: Priority must be in range 0 to 99!</p>";
		}
	} catch (...)
	{
		bErrorFlag = false;
		sErrorMessage += "<p>By TOS: Unexpected Error!</p>";
	}
	// check whether the TOS_LineEdit empty or not
	try
	{
		if (TOS_LineEdit->text().empty())
		{
			bErrorFlag = false;
			sErrorMessage += "<p>By TOS: Please set a value!</p>";
		}
		else if (Wt::WLocale::currentLocale().toInt(TOS_LineEdit->text()) > 7
				|| Wt::WLocale::currentLocale().toInt(TOS_LineEdit->text()) < 0)
		{
			bErrorFlag = false;
			sErrorMessage += "<p>By TOS: TOS must be in range 0 to 7!</p>";
		}
	} catch (...)
	{
		bErrorFlag = false;
		sErrorMessage += "<p>By TOS: Unexpected Error!</p>";
	}

	// check whether errors occurred by all widgets or not
	if (!bErrorFlag)
	{
		Information_Container->show();
		Information_Label->setText("<p>Error!</p>" + sErrorMessage);
	}

}

/*! \brief FIFO file upload widget
 */
void SessionConfig::fileupload()
{
	// initialize FIFO file upload container
	FIFO_File_upload_Container->clear();
	// initialize FIFO Error Message
	sFIFOFileParseErrorMessage =
			"<p>By Source Of Type: No FIFO File uploaded!</p>";
	bFIFOFileParseErrorFlag = false;
	Wt::WLabel *FIFO_File_upload_Label = new Wt::WLabel("FIFO File Upload: ",
			FIFO_File_upload_Container);
	FIFO_File_upload_Label->setStyleClass("control-label");
	FileUpload = new Wt::WFileUpload(FIFO_File_upload_Container);
	Wt::WText *BR = new Wt::WText("<br/>", Wt::XHTMLText,
			FIFO_File_upload_Container);
	// set up FIFO FileUPload widget
	FileUpload->setFileTextSize(1000);
	FileUpload->setProgressBar(new Wt::WProgressBar());
	FileUpload->setMargin(50);
	FileUpload->setStyleClass("span4");
	FIFO_File_upload_Container->setStyleClass("span4");
	BR = new Wt::WText("<br/><br/>", Wt::XHTMLText, FIFO_File_upload_Container);
	BR->setStyleClass("control-label");
	Wt::WContainerWidget *Upload_Information_Container =
			new Wt::WContainerWidget(FIFO_File_upload_Container);
	Upload_Information_Container->setStyleClass("span4");
	Wt::WText *Upload_Information = new Wt::WText(Upload_Information_Container);
	Wt::WText *_BR = new Wt::WText("<br/>", Wt::XHTMLText,
			FIFO_File_upload_Container);
	// Upload automatically when the user entered a file.
	FileUpload->changed().connect(std::bind([=] ()
	{
		FileUpload->upload();
	}));
	// React to a succesFileUploadll upload.
	FileUpload->uploaded().connect(
			std::bind(
					[=] ()
					{
						// by successful uploading, remove error messages
						bFIFOFileParseErrorFlag=true;
						sFIFOFileParseErrorMessage="";
						Upload_Information->setText("Uploaded Successfully! Filename: "+FileUpload->clientFileName());
						Upload_Information_Container->setStyleClass("alert alert-success");
						std::ifstream infile(FileUpload->spoolFileName());
						std::string sLine("");
						Wt::WString sContentOfFIFOFile("", Wt::UTF8);
						// get the uploaded file and prepare to display it
						// parse this FIFO File and estimate whether is it valid
						while (getline(infile, sLine))
						{
							if (sLine.length()>1)
							{
								CStringTokenizer tok(sLine, "|" );
								if (tok.countTokens() != 3)
								{
									bFIFOFileParseErrorFlag=false;
								}
								// add html comment tags in form of XHTML in order to display it in HTML pages
								sContentOfFIFOFile+="<p>"+sLine+"</p>\n";
							}
						}
						// close ifstream
						infile.close();
						if(!bFIFOFileParseErrorFlag)
						{
							Upload_Information->setText("Parse error in FIFO source! Filename: "+FileUpload->clientFileName());
							Upload_Information_Container->setStyleClass("alert alert-error");
							sFIFOFileParseErrorMessage="<p>By Source Of Type: Parse error in FIFO source!</p>";
						}
						reUploadButton = new Wt::WPushButton("Change File",FIFO_File_upload_Container);
						reUploadButton->setMargin(10, Wt::Right | Wt::Bottom);
						reUploadButton->clicked().connect(
								std::bind(
										[=] ()
										{
											fileupload();
										}));
						// set a button in order to display the uploaded file
						Wt::WPushButton *SeeFileButton=new Wt::WPushButton("See File",FIFO_File_upload_Container);
						SeeFileButton->setMargin(10, Wt::Right | Wt::Bottom);
						SeeFileButton->clicked().connect(
								std::bind(
										[=] ()
										{
											Wt::WMessageBox *messageBox = new Wt::WMessageBox
											("Content of " + FileUpload->clientFileName(),
													sContentOfFIFOFile,
													Wt::Information, Wt::Ok);
											messageBox->buttonClicked().connect(std::bind([=] ()
															{
																delete messageBox;
															}));
											messageBox->show();

										}));
						_BR->hide();
					}));
	// React to a file upload problem.
	FileUpload->fileTooLarge().connect(
			std::bind(
					[=] ()
					{
						bFIFOFileParseErrorFlag=false;
						sFIFOFileParseErrorMessage="<p>By Source Of Type: Uploaded file is too large!</p>";
						Upload_Information->setText("Uploaded file is too large.");
						Upload_Information_Container->setStyleClass("alert alert-error");
						reUploadButton = new Wt::WPushButton("Change File",FIFO_File_upload_Container);
						reUploadButton->setMargin(10, Wt::Right | Wt::Bottom);
						reUploadButton->clicked().connect(
								std::bind(
										[=] ()
										{
											fileupload();
										}));
						_BR->hide();
					}));
}

/*! \brief check whether parameters in configuration valid or not
 *  \return Error message
 */
std::string SessionConfig::checkParameter()
{
	if (TypeOfSource_ComboBox->currentIndex() != 2)
	{
		bFIFOFileParseErrorFlag = true;
		sFIFOFileParseErrorMessage = "";
	}
	if (bErrorFlag && bFIFOFileParseErrorFlag)
	{
		return "";
	}
	else
	{
		return sErrorMessage + sFIFOFileParseErrorMessage;
	}
}
/// FIXME is it necessary?
/*! \brief get interfaces of network card from operating system
 *  \return return the number of interfaces
 */
//uint16_t SessionConfig::getInterfaces()
//{
//	uint16_t iAmountOfInterfaces = 0;
//	std::string sIPAddress = "";
//	std::string sInterfaceBuffer = "";
//
//	struct ifaddrs * ifAddrStruct = NULL;
//	struct ifaddrs * ifa = NULL;
//	void * tmpAddrPtr = NULL;
//
//	getifaddrs(&ifAddrStruct);
//	// check it is IP4
//	for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next)
//	{
//		if (ifa->ifa_addr->sa_family == AF_INET)
//		{
//			// is a valid IP4 Address
//			tmpAddrPtr = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
//			char addressBuffer[INET_ADDRSTRLEN];
//			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
//			sInterfaceBuffer = ifa->ifa_name;
//			sIPAddress = addressBuffer;
//			vInterfaceNames.push_back(sInterfaceBuffer);
//			vInterfaceIPs.push_back(sIPAddress);
//			iAmountOfInterfaces++;
//		}
//		// check it is IP6
//		//		else if (ifa->ifa_addr->sa_family == AF_INET6)
//		//		{
//		//			// is a valid IP6 Address
//		//			tmpAddrPtr = &((struct sockaddr_in6 *) ifa->ifa_addr)->sin6_addr;
//		//			char addressBuffer[INET6_ADDRSTRLEN];
//		//			inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
//		//			sInterfaceBuffer = ifa->ifa_name;
//		//		}
//	}
//	return iAmountOfInterfaces;
//}

/*! \brief validator of WebGUI, in order to make sure the value of parameters are valid
 *  \param eValidatorType [set which type of value should be validate]
 *  \param string [the string (value), which will be checked]
 *  \return false when there are errors by parsing value from widget
 */bool WebGUIUtils::validator(ValidatorType eValidatorType, std::string string)
{
	// checking the type of time
	if (eValidatorType == Time)
	{
		if (string == "")
		{
			throw Exception(1, "Please set a value!");
			return false;
		}
		if (!(string[0] > 47 && string[0] < 58))
		{
			throw Exception(1, "The value must be in format of sec[.usec]!");
			return false;
		}
		bool bFloatPointFlag = false;
		for (uint16_t i = string.length() - 1; i > 0; i--)
		{
			if (string[i] == 44)
			{
				throw Exception(1, "Float Point should be a \".\"!");
				return false;
			}
			else if ((string[i] > 47 && string[i] < 58) || string[i] == 46)
			{
				if (string[i] == 46 && bFloatPointFlag)
				{
					throw Exception(1, "Two Decimal Points!");
					return false;
				}
				if (string[i] == 46)
				{
					bFloatPointFlag = true;
				}
			}
			else
			{
				throw Exception(1,
						"The value must be in format of sec[.usec]!");
				return false;
			}
		}
	}
	// checking the type of packet size
	else if (eValidatorType == Size)
	{
		if (string == "")
		{
			throw Exception(1, "Please set a value!");
			return false;
		}
		if (!(string[0] > 47 && string[0] < 58))
		{
			throw Exception(1,
					"The value must be a number or a number with unit!\n The unit must be in form of [G, M, k, Gi, Mi, ki]!");
			return false;
		}
		for (uint16_t i = string.length() - 1; i > 0; i--)
		{
			if (string[i] == 44 || string[i] == 46)
			{
				throw Exception(1, "Must be a integer!");
				return false;
			}
			else if ((string[i] > 47 && string[i] < 58) || string[i] == 71
					|| string[i] == 77 || string[i] == 107 || string[i] == 105)
			{
				if ((string[i] == 71 || string[i] == 77 || string[i] == 107
						|| string[i] == 105) && (i < string.length() - 2))
				{
					throw Exception(1, "The Unit must be at the end!");
					return false;
				}
				else if ((string[i] > 47 && string[i] < 58)
						&& (i == string.length() - 1)
						&& (!((string[i - 1] > 47 && string[i - 1] < 58))))
				{
					throw Exception(1, "The Unit must be at the end!");
					return false;
				}
				else if ((i == string.length() - 1) && (string[i] == 105)
						&& !(string[i - 1] == 71 || string[i - 1] == 77
								|| string[i - 1] == 107))
				{
					throw Exception(1,
							"The Unit must be in form of [G, M, k, Gi, Mi, ki]!");
					return false;
				}
				else if ((i == string.length() - 1)
						&& (string[i] == 71 || string[i] == 77
								|| string[i] == 107)
						&& (!((string[i - 1] > 47 && string[i - 1] < 58)
								|| (string[i - 1] == 46))))
				{
					throw Exception(1,
							"The Unit must be in form of [G, M, k, Gi, Mi, ki]!");
					return false;
				}
			}
			else
			{
				throw Exception(1,
						"The value must be a number or a number with unit!\n The unit must be in form of [G, M, k, Gi, Mi, ki]!");
				return false;
			}
		}
	}
	// checking the type of address
	else if (eValidatorType == Address)
	{
		if (string == "")
		{
			return true;
		}
		std::string sAddress("");
		std::string sPort("");
		bool bWithPortFlag = false;
		for (uint16_t i = string.length() - 1; i > 0; i--)
		{
			if (string[i] == 58)
			{
				bWithPortFlag = true;
				sAddress = string.substr(0, i);
				sPort = string.substr(i + 1, string.length() - i);
			}
		}
		if (bWithPortFlag)
		{
			try
			{
				boost::asio::ip::address::from_string(sAddress);
			} catch (const boost::system::system_error& e)
			{
				throw Exception(1, "Invalid IP Address!");
				return false;
			}
			if (sPort == "")
			{
				throw Exception(1, "Invalid IP Port!");
				return false;
			}
			for (uint16_t i = 0; i < sPort.length(); i++)
			{
				if (!(sPort[i] > 47 && sPort[i] < 58))
				{
					throw Exception(1, "Invalid IP Port!");
					return false;
				}
			}
		}
		else
		{
			try
			{
				boost::asio::ip::address::from_string(string);
			} catch (const boost::system::system_error& e)
			{
				throw Exception(1, "Invalid IP Address!");
				return false;
			}
		}
	}
	return true;
}


 /*! \brief get the unit of value
  *  \param string [the string (value), which will be checked]
  *  \return UNIT [the unit of value]
  */
UNIT WebGUIUtils::checkUnit(std::string string)
{
	UNIT eUnit;
	if (string == "")
	{
		eUnit = None;
	}
	else if (string[string.length() - 1] == 105)
	{
		if (string[string.length() - 2] == 71)
		{
			eUnit = Gi;
		}
		else if (string[string.length() - 2] == 77)
		{
			eUnit = Mi;
		}
		else if (string[string.length() - 2] == 107)
		{
			eUnit = ki;
		}
	}
	else if (string[string.length() - 1] == 71)
	{
		eUnit = G;
	}
	else if (string[string.length() - 1] == 77)
	{
		eUnit = M;
	}
	else if (string[string.length() - 1] == 107)
	{
		eUnit = k;
	}
	else
	{
		eUnit = None;
	}
	return eUnit;
}

/*! \brief convert the string to double and recalculate the value with unit
 *  \param string [the string (value), which will be converted]
 *  \param eUnit [the unit of the string value]
 *  \return the value, which has been converted
 */
double WebGUIUtils::StringToDoubleWithUnit(std::string string, UNIT eUnit)
{
	double dValue = 0;
	switch (eUnit)
	{
	case None:
		dValue = CStringUtils::stof(string);
		break;
	case G:
		dValue = CStringUtils::stof(string.substr(0, string.length() - 1))
				* pow(10, 9);
		break;
	case M:
		dValue = CStringUtils::stof(string.substr(0, string.length() - 1))
				* pow(10, 6);
		break;
	case k:
		dValue = CStringUtils::stof(string.substr(0, string.length() - 1))
				* pow(10, 3);
		break;
	case Gi:
		dValue = CStringUtils::stof(string.substr(0, string.length() - 2))
				* pow(2, 30);
		break;
	case Mi:
		dValue = CStringUtils::stof(string.substr(0, string.length() - 2))
				* pow(2, 20);
		break;
	case ki:
		dValue = CStringUtils::stof(string.substr(0, string.length() - 2))
				* pow(2, 10);
		break;
	}
	return dValue;
}

/*! \brief parse the time string with the format <sec>[.<usec>], sets seconds and microseconds parameters.
 * 	\param pTimeStr [the time which will be parsed]
 * 	\param pSec [second from parsing]
 * 	\param pUsec [microsecond from parsing]
 *  \return true if no parse errors have occurred, false otherways
 */bool WebGUIUtils::parseTimeString(const std::string pTimeStr, uint64_t & pSec,
		uint64_t & pUsec)
{
	CStringTokenizer tok(pTimeStr, ".", false, false);
	if (tok.countTokens() > 2)
	{
		throw Exception(1, "Invalid Time format!");
		return false;
	}
	pSec = CStringUtils::stoi(tok.nextToken());
	pUsec = 0;
	if (tok.hasMoreTokens())
		pUsec = CStringUtils::stoi(tok.nextToken());
	return true;
}

/*! \brief convert plain text file to csv file
 * 	\param sInputFilename [the plain text file name]
 *  \return csv file name
 */
std::string WebGUIUtils::TxtToCsv(std::string sInputFilename)
{
	std::ifstream infile(sInputFilename);
	std::string sOutputFilename("");
	if (infile)
	{
		for (uint16_t i = sInputFilename.length() - 1; i > 0; i--)
		{
			if (sInputFilename[i] == 46)
			{
				sOutputFilename = sInputFilename.substr(0, i);
				break;
			}
		}
		sOutputFilename += ".csv";
		std::ofstream outfile(sOutputFilename, std::ios::out);
		// set header of csv file
		std::string sLine = "";
		outfile
				<< "\"Number\",\"Sent Time [s]\",\"Receive Time [s]\",\"Relative Time [s]\",\"Delay [s]\",\"Packet length [bytes]\",\"Correction Time [s]\",\"Corrected Delay [s]\"\n";
		double dSentTimeMin = 0;
		// kernel of conversion
		while (getline(infile, sLine))
		{
			if (sLine[0] != '#')
			{

				int iColumnNr = 0;
				int iPosition_before = 0;
				bool bGetStrFlag = true;
				int iCounter = 0;
				double dCurrentSentTime(0.0);
				for (uint16_t i = 0; i < sLine.length(); i++)
				{
					if ((sLine[i] == ' ' || sLine[i] == '\t'
							|| i == sLine.length() - 1) && bGetStrFlag)
					{
						iColumnNr++;
						std::string sBuffer = sLine.substr(iPosition_before,
								iCounter);
						if (iColumnNr == 2)
						{
							dCurrentSentTime = CStringUtils::stof(sBuffer);
							if (!dSentTimeMin)
							{
								dSentTimeMin = dCurrentSentTime;
							}
						}
						else if (iColumnNr == 4)
						{
							outfile
									<< CStringUtils::ftos(
											dCurrentSentTime - dSentTimeMin)
											+ ",";
						}
						outfile << sBuffer;
						if (i != sLine.length() - 1)
						{
							outfile << ",";
						}
						bGetStrFlag = false;
						iCounter = 0;
					}
					if (!(sLine[i] == ' ' || sLine[i] == '\t'
							|| sLine[i] == '\n') && !bGetStrFlag)
					{
						iPosition_before = i;
						bGetStrFlag = true;
					}
					if (bGetStrFlag)
					{
						iCounter++;
					}
				}
				outfile << std::endl;
			}
		}
		outfile.close();
	}
	else
	{
		throw Exception(2, "Can not open the Dumpfile: " + sInputFilename);
	}
	infile.close();
	return sOutputFilename;
}

/*! \brief convert csv file to data model
 * 	\param csvFile [the csv file name]
 * 	\param *parent [parent container]
 *  \return StandardItemModel, it is the data model, which created according to csv-file's content
 */
Wt::WStandardItemModel *WebGUIUtils::csvToModel(const std::string csvFile,
		Wt::WObject *parent)
{
	Wt::WStandardItemModel *result = new Wt::WStandardItemModel(0, 0, parent);
	std::ifstream f(csvFile);
	if (f)
	{
		Wt::WAbstractItemModel *model = result;
		int numRows = -1;
		bool firstLineIsHeaders = true;
		int csvRow = 0;
		while (f)
		{
			std::string line;
			getline(f, line);
			if (f)
			{
				typedef boost::tokenizer<boost::escaped_list_separator<char> > CsvTokenizer;
				CsvTokenizer tok(line);
				int col = 0;
				for (CsvTokenizer::iterator i = tok.begin(); i != tok.end();
						++i, ++col)
				{
					if (col >= model->columnCount())
						model->insertColumns(model->columnCount(),
								col + 1 - model->columnCount());
					if (firstLineIsHeaders && csvRow == 0)
						model->setHeaderData(col,
								boost::any(Wt::WString::fromUTF8(*i)));
					else
					{
						int dataRow = firstLineIsHeaders ? csvRow - 1 : csvRow;

						if (numRows != -1 && dataRow >= numRows)
							break;

						if (dataRow >= model->rowCount())
							model->insertRows(model->rowCount(),
									dataRow + 1 - model->rowCount());
						std::string s = *i;
						boost::any data;
						char *end;
						int i = std::strtol(s.c_str(), &end, 10);
						if (*end == 0)
							data = boost::any(i);
						else
						{
							double d = std::strtod(s.c_str(), &end);
							if (*end == 0)
								data = boost::any(d);
							else
								data = boost::any(Wt::WString::fromUTF8(s));
						}
						model->setData(dataRow, col, data);
					}
				}
			}
			++csvRow;
		}
	}
	else
	{
		throw Exception(2, "Can not convert '.csv' file to Data Model");
	}
	return result;
}

/*! \brief Constructor of CLTestWebGUI Class
 * 	\param env [the environment parameters, which needed by WApplication class]
 */
CLTestWebGUI::CLTestWebGUI(const Wt::WEnvironment& env) :
		Wt::WApplication(env)
{
    // set Title and Theme of Web site
	setTitle("LTest");
	root()->resize(Wt::WLength::Auto, Wt::WLength::Auto);
	// create app instance to control all actions of WebGUI
	app = Wt::WApplication::instance();
	// use bootstrap css theme
	app->useStyleSheet("resources/themes/bootstrap/bootstrap.css");
	app->useStyleSheet("resources/themes/bootstrap/bootstrap-responsive.css");
	setTheme(new Wt::WBootstrapTheme());

	// set default page "/"
	app->setInternalPath("/");

	// initialize all flags
	bInitFlag = false;
	bErrorFlag = false;
	bStartFlag = false;
	sErrorMessage = "";

	// initialize the containers
	NavigationBarContainer = new Wt::WContainerWidget(app->root());
	//NavigationBarContainer->setStyleClass("container");
	HomepageContainer = new Wt::WContainerWidget(app->root());
	HomepageContainer->setStyleClass("container");
	ConfigureSessionContainer = new Wt::WContainerWidget(app->root());
	ConfigureSessionContainer->setStyleClass("container");
	TestResultContainer = new Wt::WContainerWidget(app->root());
	TestResultContainer->setStyleClass("container");
	GraphicContainer = new Wt::WContainerWidget(app->root());
	GraphicContainer->setStyleClass("container");

	// create the main page
	NavigationBar();
	Homepage();
	// route the internal Path
	app->internalPathChanged().connect(std::bind([=]()
	{
		Router();
	}));
}

/*! \brief center router of WebGUI. Router pages to corresponding internal path.
 */
void CLTestWebGUI::Router()
{

	std::cout << "\n\nInternalPath: " << app->internalPath() << "\n\n";

	// when internal path is "/", request homepage() method
	if (app->internalPath() == "/")
	{
		Homepage();
	}
	// when internal path is "/Configure_Session", request Configure_Session() method
	else if (app->internalPath() == "/Configure_Session")
	{
		Configure_Session();
	}
	// when internal path is "/Test_Result", request Test_Result() method
	else if (app->internalPath() == "/Test_Result")
	{
		Test_Result();
	}
	// when internal path is "/Graphic", request Graphic() method
	else if (app->internalPath() == "/Graphic")
	{
		Graphic();
	}
}

/*! \brief create Navigationbar
 */
void CLTestWebGUI::NavigationBar()
{
	// Create a navigation bar with a link to a web page.
	Wt::WNavigationBar *navigation = new Wt::WNavigationBar(
			NavigationBarContainer);
	navigation->setTitle("LTest", "/");
	navigation->setResponsive(true);
	navigation->setStyleClass("navbar");
	Wt::WStackedWidget *contentsStack = new Wt::WStackedWidget(
			NavigationBarContainer);
	contentsStack->addStyleClass("container");
	// set up a Left-aligned menu.
	leftMenu = new Wt::WMenu(contentsStack, NavigationBarContainer);
	navigation->addMenu(leftMenu);

	leftMenu->addItem("Homepage")->setLink(
			Wt::WLink(Wt::WLink::InternalPath, "/"));

	leftMenu->addItem("Configure Session")->setLink(
			Wt::WLink(Wt::WLink::InternalPath, "/Configure_Session"));

	leftMenu->addItem("Test Result")->setLink(
			Wt::WLink(Wt::WLink::InternalPath, "/Test_Result"));

	leftMenu->addItem("Graphic")->setLink(
			Wt::WLink(Wt::WLink::InternalPath, "/Graphic"));
	leftMenu->setItemHidden(2, true);
	leftMenu->setItemHidden(3, true);
	// set up a Right-aligned menu.
	Wt::WMenu *rightMenu = new Wt::WMenu();
	navigation->addMenu(rightMenu, Wt::AlignRight);
	// Create a popup submenu for the Help menu.
	Wt::WMenuItem *item = new Wt::WMenuItem("Help");
	Wt::WPopupMenu *popup = new Wt::WPopupMenu();
	popup->addItem("Help");
	popup->addItem("License");
	popup->addSeparator();
	popup->addItem("About");
	popup->itemSelected().connect(
			std::bind(
					[=] (Wt::WMenuItem *item)
					{
						Wt::WMessageBox *messageBox;
						// create help message box
						if(item->text()=="Help")
						{
							messageBox = new Wt::WMessageBox
							("Help",
									"<h4>Usage:</h4>"
									"<p class=\"text-info\">Step 1: Configure a session, and click \"<b>Start Session</b>\" button.</p>"
									"<p class=\"text-info\">Step 2: Wait a moment, you will see the result.</p>"
									"<p class=\"text-info\">Step 3: Go into the \"<b>Graphic</b>\" page, configure the graphic</p>"
									"<h4>Have fun!</h4>",
									Wt::Information, Wt::Ok);
						}
						// create about message box
						else if (item->text()=="About")
						{
							messageBox = new Wt::WMessageBox
							("About",
									"<div style=\"text-align: center; width: 500px\">"
									"<img style=\"heigth: 150px; width: 150px;\" src=\"resources\\logo.png\"></img>"
									"<h4>LTest: a tool for measurement of network performance</h4>"
									"<p class=\"text-info\">Website     : www.hs-anhalt.de</p>"
									"<p class=\"text-info\">Version     : 0.4</p>"
									"<p class=\"text-info\">Copyright   : All rights reserved by Anhalt University of Applied sciences</p>"
									"<p class=\"text-info\">Description : Web Graphical User Interface of LTest</p>"
									"</div>",
									Wt::Information, Wt::Ok);
						}
						// create license message box
						else if(item->text()=="License")
						{
							SecurityCheck securityInstance;
							messageBox = new Wt::WMessageBox
							("License Information",
									"<div style=\"width: 500px; margin: 20px\">"
									"<h4>Copyright (C) 2013 - Hochschule Anhalt</h4>"
									"<h5>License valid duration: "+securityInstance.getStringLeftValidity()+" or "+CStringUtils::itos(securityInstance.getIntLeftValidity()) +" seconds</h5>"
									"<img style=\"height: 149px; width: 95px; float:left; margin:0 30px 10px 0;\" src=\"resources\\logo_hs_anhalt.jpg\"></img>"
									"<p>LTest is free. You don't have to pay for it, and you can use it any way you want.</p>"
									"<p>If you simply wish to install and use this software, you need only be aware of the disclaimer conditions in the license, which are set out below.</p>"
									"<h5>NO WARRANTY:</h5>"
									"<p>Because the program is licensed free of charge, there is no warranty for the program, to the extent permitted by applicable law.  Except when otherwise stated in writing the copyright holders and other parties provide the program \"as is\" without warranty of any kind, either expressed or implied, including, but not limited to, the implied warranties of merchantability and fitness for a particular purpose.  The entire risk as to the quality and performance of the program is with you.  Should the program prove defective, you assume the cost of all necessary servicing, repair or correction.</p>"
									"<p>In no event unless required by applicable law or agreed to in writing will any copyright holder, or any other party who may modify and redistribute the program as permitted above, be liable to you for damages, including any general, special, incidental or consequential damages arising out of the use or inability to use the program (including but not limited to loss of data or data being rendered inaccurate or losses sustained by you or third parties or a failure of the program to operate with any other programs), even if such holder or other party has been advised of the possibility of such damages.</p>"
									"</div>"
									, Wt::Information, Wt::Ok);

						}
						// click ok button, remove message box
						messageBox->buttonClicked().connect(std::bind([=] ()
										{
											delete messageBox;
										}));

						messageBox->show();
					}, std::placeholders::_1));
	item->setMenu(popup);
	rightMenu->addItem(item);
	NavigationBarContainer->addWidget(contentsStack);
}

/*! \brief reset all containers
 */
void CLTestWebGUI::ClearContainers()
{
	HomepageContainer->clear();
	ConfigureSessionContainer->clear();
	TestResultContainer->clear();
	GraphicContainer->clear();
}

/*! \brief create homepage
 */
void CLTestWebGUI::Homepage()
{
	leftMenu->select(0);
	// reset all containers
	ClearContainers();
	//----------------------------Welcome-----------------------------------//
	Wt::WText *WelcomeText =
			new Wt::WText(
							"<div class=\"hero-unit\">"
							"<div style=\"height: 150px; width: 150px; border: 1px solid black; box-shadow: 2px 2px 5px #585858; display: inline-block; margin: 0 50px 50px 0; position: relative;\">"
							"<img style=\"height: 150px; width: 150px;\" src=\"resources\\logo.png\"></img>"
							"</div>"
							"<div style=\"display: inline-block; clear: left;\">"
							"<h1 style=\"text-shadow: 2px 2px 2px #585858;\">Welcome to LTest</h1>"
							"</div>"
							"<div style=\"display: inline-block; clear: both;\">"
							"<p class=\"muted\"><b>LTest</b> is a tool for measurement of network performance between two or more hosts within the Internet. It is designed in a client-server fashion like other well known tools like ttcp, netperf and iperf.</p>"
							"<p class=\"text-info\">Usage:</p>"
							"<p class=\"text-info\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Step 1: Configure a session, and click \"<b>Start Session</b>\" button.</p>"
							"<p class=\"text-info\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Step 2: Wait a moment, you will see the result.</p>"
							"<p class=\"text-info\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Step 3: Go into the \"<b>Graphic</b>\" page, configure the graphic</p>"
							"<p class=\"text-info\">Have fun!</p>"
							"</div>"
							"</div>", Wt::XHTMLText, HomepageContainer);
	WelcomeText->setStyleClass("control");
	Footer(HomepageContainer);
}

/*! \brief create configure session html page
 */
void CLTestWebGUI::Configure_Session()
{
	leftMenu->select(1);
	// reset all containers
	ClearContainers();
	SessionConfig *pSessionConfig_ptr = new SessionConfig(
			ConfigureSessionContainer);

	//-----------------------Dialog for waiting-----------------------------//
	Wt::WDialog *WaitingDialog = new Wt::WDialog("Please Wait");
	Wt::WText *text = new Wt::WText("<p>Please wait a minute!</p>"
			"<p>it will be finished soon...</p>", Wt::XHTMLText,
			WaitingDialog->contents());
	text->setStyleClass("control-label");
	Wt::WProgressBar *bar = new Wt::WProgressBar(WaitingDialog->contents());
	bar->setStyleClass("progress progress-striped active");
	bar->setRange(0, 1000);
	bar->setValue(1000);
	bar->setFormat("");

	//-----------------------start Session-------------------------//
	// create a wt-timer to start the session, it is a pseudo multithread which be used in html page
	// in order to create the progress bar
	Wt::WTimer *intervalTimer = new Wt::WTimer(ConfigureSessionContainer);
	intervalTimer->setInterval(1000);
	intervalTimer->timeout().connect(
			std::bind(
					[=]()
					{

						bErrorFlag=false;
						leftMenu->setItemHidden(2,false);
						leftMenu->setItemHidden(3,false);
						// get all parameters from widgets
						SessionData stSessionData;
						stSessionData.bSendingModeFlag=pSessionConfig_ptr->bSendingModeFlag;
						stSessionData.sReceiverDataAddress=pSessionConfig_ptr->ReceiverIP_LineEdit->text().narrow();
						stSessionData.sReceiverGsoapAddress=pSessionConfig_ptr->receiverSoapAddress_LineEdit->text().narrow();
						stSessionData.sLocalDataAddress=pSessionConfig_ptr->SenderIP_LineEdit->text().narrow();
						stSessionData.sGsoapLocalAddress=pSessionConfig_ptr->LocalSoapAddress_LineEdit->text().narrow();
						// bInitFlag=true means the session was already initialized
						bInitFlag=true;
						if(pSessionConfig_ptr->Protocol_ComboBox->currentText()=="TCP")
						{
							stSessionData.eProtocolType=P_TCP;
						}
						else if(pSessionConfig_ptr->Protocol_ComboBox->currentText()=="UDP")
						{
							stSessionData.eProtocolType=P_UDP;
						}

						if(pSessionConfig_ptr->TypeOfSource_ComboBox->currentText()=="CBR")
						{
							stSessionData.eSourceType=ST_CBR;
						}
						else if(pSessionConfig_ptr->TypeOfSource_ComboBox->currentText()=="Poisson")
						{
							stSessionData.eSourceType=ST_POISSON;
						}
						else if(pSessionConfig_ptr->TypeOfSource_ComboBox->currentText()=="FIFO")
						{
							stSessionData.eSourceType=ST_FIFO;
							stSessionData.sFifoFileName=pSessionConfig_ptr->FileUpload->spoolFileName();
						}
						stSessionData.bUnixTimeFlag=pSessionConfig_ptr->StartTime_CheckBox->isChecked();
						stSessionData.sStartTime=pSessionConfig_ptr->StartTime_LineEdit->text().narrow();
						stSessionData.sFinishTime=pSessionConfig_ptr->FinishTime_LineEdit->text().narrow();
						stSessionData.sSessionDuration=pSessionConfig_ptr->SessionDuration_LineEdit->text().narrow();

						stSessionData.iPacketSize=static_cast<uint32_t>(WebGUIUtils::StringToDoubleWithUnit(
										pSessionConfig_ptr->PacketSize_LineEdit->text().narrow(),WebGUIUtils::checkUnit(pSessionConfig_ptr->PacketSize_LineEdit->text().narrow())));
						stSessionData.iAmountOfPackets=static_cast<uint32_t>(WebGUIUtils::StringToDoubleWithUnit(
										pSessionConfig_ptr->AmountOfPackets_LineEdit->text().narrow(),WebGUIUtils::checkUnit(pSessionConfig_ptr->AmountOfPackets_LineEdit->text().narrow())));
						stSessionData.iAmountOfBytes=static_cast<uint32_t>(WebGUIUtils::StringToDoubleWithUnit(
										pSessionConfig_ptr->AmountOfBytes_LineEdit->text().narrow(),WebGUIUtils::checkUnit(pSessionConfig_ptr->AmountOfBytes_LineEdit->text().narrow())));
						stSessionData.iDataRate=static_cast<uint32_t>(WebGUIUtils::StringToDoubleWithUnit(
										pSessionConfig_ptr->Datarate_LineEdit->text().narrow(),WebGUIUtils::checkUnit(pSessionConfig_ptr->Datarate_LineEdit->text().narrow())));
						stSessionData.iPriority=CStringUtils::stoi(pSessionConfig_ptr->Priority_LineEdit->text().narrow());
						stSessionData.iTOS=CStringUtils::stoi(pSessionConfig_ptr->TOS_LineEdit->text().narrow());
						if(pSessionConfig_ptr->TimeCorrection_CheckBox->checkState()==Wt::Checked)
						{
							stSessionData.bTimeCorrection=true;
							stSessionData.iTimeCorrection=CStringUtils::stoi(pSessionConfig_ptr->TimeCorrection_LineEdit->text().narrow())*1000;
							if(pSessionConfig_ptr->ICA_CheckBox->isChecked())
							{
								stSessionData.bICA=true;
							}
							else
							{
								stSessionData.bICA=false;
							}
						}
						else
						{
							stSessionData.bTimeCorrection=false;
							stSessionData.iTimeCorrection=0;
						}
						if(pSessionConfig_ptr->BlockingMode_CheckBox->checkState()==Wt::Checked)
						{
							stSessionData.bBlockingMode=true;
						}
						else
						{
							stSessionData.bBlockingMode=false;
						}
						// transmit all parameters to ltest gsoap tunnel
						gSOAPInterface(stSessionData);
						// end timer or end this thread
						intervalTimer->stop();
						// remove dialog window
						delete(WaitingDialog);
						// route to internal path "/Test_Result"
						app->setInternalPath("/Test_Result");
						Router();
					}));

	//----------------------------StartButton & ProgressBar-----------------------------------//
	Wt::WText *BR = new Wt::WText("<hr />", Wt::XHTMLText,
			ConfigureSessionContainer);
	Wt::WContainerWidget *StartButton_Container = new Wt::WContainerWidget(
			ConfigureSessionContainer);
	StartButton_Container->setStyleClass("container");
	Wt::WPushButton *StartButton = new Wt::WPushButton("Start Session",
			StartButton_Container);
	BR = new Wt::WText("<br/>", Wt::XHTMLText, ConfigureSessionContainer);
	BR->setStyleClass("control-label");
	StartButton->setStyleClass("btn-primary btn-large");
	StartButton->clicked().connect(
			std::bind(
					[=]()
					{
						std::string sMessage=pSessionConfig_ptr->checkParameter();
						if(sMessage.empty())
						{
							WaitingDialog->show();
							intervalTimer->start();
						}
						else
						{
							Wt::WMessageBox *messageBox = new Wt::WMessageBox
							("Error!",
									"<p>Starting session failed. Errors occurred by configuration!</p>"
									"<p>Please check the parameters!</p>"
									"<h4>Details:</h4>"+sMessage,
									Wt::Warning, Wt::Ok);
							messageBox->buttonClicked().connect(std::bind([=] ()
											{
												delete messageBox;
											}));
							messageBox->show();
						}
					}));
	Footer(ConfigureSessionContainer);
}

/*! \brief create test_result html page
 */
void CLTestWebGUI::Test_Result()
{
	// reset all containers
	ClearContainers();
	leftMenu->select(2);
	// no error occurred by measurement's processing
	if (!bErrorFlag)
	{
		if (bInitFlag)
		{
			getResult();
			bInitFlag = false;
		}
		//----------------------set up Group-----------------------------------//
		Wt::WContainerWidget *TestResultSubContainer_1 =
				new Wt::WContainerWidget(TestResultContainer);
		TestResultSubContainer_1->setStyleClass("row");
		Wt::WGroupBox *groupBox = new Wt::WGroupBox("Test Result",
				TestResultSubContainer_1);
		groupBox->addStyleClass("span12");

		//-----------------------Display Buffer set up--------------------------//
		Wt::WContainerWidget *DisplayBufSetup_Container =
				new Wt::WContainerWidget();
		DisplayBufSetup_Container->setStyleClass("span12");
		Wt::WText *BR = new Wt::WText("<h4>Source description:</h4>",
				Wt::XHTMLText, DisplayBufSetup_Container);
		BR = new Wt::WText(sDisplayBufSetup, Wt::PlainText,
				DisplayBufSetup_Container);
		BR = new Wt::WText("<br/>", Wt::XHTMLText, DisplayBufSetup_Container);
		BR->setStyleClass("control");
		groupBox->addWidget(DisplayBufSetup_Container);

		//-----------------------Display Buffer Result-------------------------//
		Wt::WContainerWidget *DisplayBufResult_Container =
				new Wt::WContainerWidget();
		DisplayBufResult_Container->setStyleClass("span12");
		BR = new Wt::WText("<hr />"
				"<h4>Session description:</h4>", Wt::XHTMLText,
				DisplayBufResult_Container);
		BR = new Wt::WText(sDisplayBufResult, Wt::PlainText,
				DisplayBufResult_Container);
		BR = new Wt::WText("<br/>", Wt::XHTMLText, DisplayBufResult_Container);
		BR->setStyleClass("control");
		groupBox->addWidget(DisplayBufResult_Container);

		//-------------------------------------table_1----------------------------------------//
		// get all result from SourceData struct and display them in a table
		Wt::WContainerWidget *table_1_Container = new Wt::WContainerWidget();
		table_1_Container->setStyleClass("span12");
		BR = new Wt::WText("<hr />"
				"<h4>Statistics:</h4>", Wt::XHTMLText,
				DisplayBufResult_Container);
		Wt::WTable *table_1 = new Wt::WTable(table_1_Container);
		table_1->setHeaderCount(1);
		table_1->elementAt(0, 0)->addWidget(new Wt::WText("Parameter"));
		table_1->elementAt(0, 1)->addWidget(new Wt::WText("Packets"));
		table_1->elementAt(0, 2)->addWidget(new Wt::WText("Bytes"));
		table_1->elementAt(1, 0)->addWidget(new Wt::WText("Sent"));
		table_1->elementAt(1, 1)->addWidget(
				new Wt::WText(
						CStringUtils::decades_formatted(
								stSourceData.mSentCount)));
		table_1->elementAt(1, 2)->addWidget(
				new Wt::WText(
						CStringUtils::decades_formatted(
								stSourceData.mSentCount
										* stSourceData.mPackAvg)));
		table_1->elementAt(2, 0)->addWidget(new Wt::WText("Received"));
		table_1->elementAt(2, 1)->addWidget(
				new Wt::WText(
						CStringUtils::decades_formatted(
								stSourceData.mRecvCount)));
		table_1->elementAt(2, 2)->addWidget(
				new Wt::WText(
						CStringUtils::decades_formatted(
								stSourceData.mRecvCount
										* stSourceData.mPackAvg)));
		table_1->elementAt(3, 0)->addWidget(new Wt::WText("Dropped"));
		table_1->elementAt(3, 1)->addWidget(
				new Wt::WText(
						CStringUtils::decades_formatted(
								stSourceData.mSentCount
										- stSourceData.mRecvCount)));
		table_1->elementAt(3, 2)->addWidget(
				new Wt::WText(
						CStringUtils::decades_formatted(
								stSourceData.mPktBytesSumLoss)));
		table_1->elementAt(4, 0)->addWidget(new Wt::WText("Loss(%)"));
		table_1->elementAt(4, 1)->addWidget(
				new Wt::WText(
						CStringUtils::decades_formatted(
								100.00
										* ((double) stSourceData.mSentCount
												- (double) stSourceData.mRecvCount)
										/ (double) stSourceData.mSentCount)));
		table_1->elementAt(4, 2)->addWidget(
				new Wt::WText(
						CStringUtils::decades_formatted(
								(double) stSourceData.mPktBytesSumLoss
										/ ((double) stSourceData.mSentCount
												* (double) stSourceData.mPackAvg)
										* 100)));
		table_1->setStyleClass("table table-bordered table-striped");
		groupBox->addWidget(table_1_Container);

		//-------------------------------------table_2----------------------------------------//
		// get all result from SourceData struct and display them in a table
		Wt::WContainerWidget *table_2_Container = new Wt::WContainerWidget();
		table_2_Container->setStyleClass("span12");
		Wt::WTable *table_2 = new Wt::WTable(table_2_Container);
		table_2->setHeaderCount(1);
		table_2->elementAt(0, 0)->addWidget(new Wt::WText(""));
		table_2->elementAt(0, 1)->addWidget(new Wt::WText("Sent"));
		table_2->elementAt(0, 2)->addWidget(new Wt::WText("Received"));
		table_2->elementAt(0, 3)->addWidget(new Wt::WText("Delay"));
		table_2->elementAt(0, 4)->addWidget(new Wt::WText("Length"));
		table_2->elementAt(1, 0)->addWidget(new Wt::WText("Parameter"));
		table_2->elementAt(1, 1)->addWidget(
				new Wt::WText("InterPacketTime [s]"));
		table_2->elementAt(1, 2)->addWidget(
				new Wt::WText("InterPacketTime [s]"));
		table_2->elementAt(1, 3)->addWidget(new Wt::WText("[s]"));
		table_2->elementAt(1, 4)->addWidget(new Wt::WText("[bytes]"));
		table_2->elementAt(2, 0)->addWidget(new Wt::WText("Min"));
		table_2->elementAt(2, 1)->addWidget(
				new Wt::WText(
						CStringUtils::ftos(
								(HPTimer::HighPerTimer::HPTimertoD(
										stSourceData.mSentTMin)))));
		table_2->elementAt(2, 2)->addWidget(
				new Wt::WText(
						CStringUtils::ftos(
								HPTimer::HighPerTimer::HPTimertoD(
										stSourceData.mRecvTMin))));
		table_2->elementAt(2, 3)->addWidget(
				new Wt::WText(
						CStringUtils::ftos(
								HPTimer::HighPerTimer::HPTimertoD(
										stSourceData.mDelayMin))));
		table_2->elementAt(2, 4)->addWidget(
				new Wt::WText(
						CStringUtils::decades_formatted(
								stSourceData.mPktSizeMin)));
		table_2->elementAt(3, 0)->addWidget(new Wt::WText("SeqNo"));
		table_2->elementAt(3, 1)->addWidget(
				new Wt::WText(
						CStringUtils::decades_formatted(
								stSourceData.mSentMinPos)));
		table_2->elementAt(3, 2)->addWidget(
				new Wt::WText(
						CStringUtils::decades_formatted(
								stSourceData.mRecvMinPos)));
		table_2->elementAt(3, 3)->addWidget(
				new Wt::WText(
						CStringUtils::decades_formatted(
								stSourceData.mDelayMinPos)));
		table_2->elementAt(3, 4)->addWidget(
				new Wt::WText(
						CStringUtils::decades_formatted(
								stSourceData.mSizeMinPos)));
		table_2->elementAt(4, 0)->addWidget(new Wt::WText("Max"));
		table_2->elementAt(4, 1)->addWidget(
				new Wt::WText(
						CStringUtils::ftos(
								HPTimer::HighPerTimer::HPTimertoD(
										stSourceData.mSentTMax))));
		table_2->elementAt(4, 2)->addWidget(
				new Wt::WText(
						CStringUtils::ftos(
								HPTimer::HighPerTimer::HPTimertoD(
										stSourceData.mRecvTMax))));
		table_2->elementAt(4, 3)->addWidget(
				new Wt::WText(
						CStringUtils::ftos(
								HPTimer::HighPerTimer::HPTimertoD(
										stSourceData.mDelayMax))));
		table_2->elementAt(4, 4)->addWidget(
				new Wt::WText(
						CStringUtils::decades_formatted(
								stSourceData.mPktSizeMax)));
		table_2->elementAt(5, 0)->addWidget(new Wt::WText("SeqNo"));
		table_2->elementAt(5, 1)->addWidget(
				new Wt::WText(
						CStringUtils::decades_formatted(
								stSourceData.mSentMaxPos)));
		table_2->elementAt(5, 2)->addWidget(
				new Wt::WText(
						CStringUtils::decades_formatted(
								stSourceData.mRecvMaxPos)));
		table_2->elementAt(5, 3)->addWidget(
				new Wt::WText(
						CStringUtils::decades_formatted(
								stSourceData.mDelayMaxPos)));
		table_2->elementAt(5, 4)->addWidget(
				new Wt::WText(
						CStringUtils::decades_formatted(
								stSourceData.mSizeMaxPos)));
		table_2->elementAt(6, 0)->addWidget(new Wt::WText("Mean"));
		table_2->elementAt(6, 1)->addWidget(
				new Wt::WText(CStringUtils::ftos(stSourceData.mSentAvg)));
		table_2->elementAt(6, 2)->addWidget(
				new Wt::WText(CStringUtils::ftos(stSourceData.mRecvAvg)));
		table_2->elementAt(6, 3)->addWidget(
				new Wt::WText(CStringUtils::ftos(stSourceData.mDelayAvg)));
		table_2->elementAt(6, 4)->addWidget(
				new Wt::WText(
						CStringUtils::decades_formatted(
								(long) stSourceData.mPackAvg)));
		table_2->elementAt(7, 0)->addWidget(new Wt::WText("St. deviation"));
		table_2->elementAt(7, 1)->addWidget(
				new Wt::WText(CStringUtils::ftos(stSourceData.mSentStdv)));
		table_2->elementAt(7, 2)->addWidget(
				new Wt::WText(CStringUtils::ftos(stSourceData.mRecvStdv)));
		table_2->elementAt(7, 3)->addWidget(
				new Wt::WText(CStringUtils::ftos(stSourceData.mDelayStdv)));
		table_2->elementAt(7, 4)->addWidget(
				new Wt::WText(CStringUtils::ftos(stSourceData.mSizeStdv)));
		table_2->setStyleClass("table table-bordered table-striped");
		groupBox->addWidget(table_2_Container);

		//-----------------------Display Time Correction----------------------------//
		Wt::WContainerWidget *DisplayTimeCorrection_Container =
				new Wt::WContainerWidget();
		DisplayTimeCorrection_Container->setStyleClass("span12");
		BR = new Wt::WText("<hr />"
				"<h4>Time Correction: </h4>", Wt::XHTMLText,
				DisplayTimeCorrection_Container);

		BR = new Wt::WText("A time correction by ", Wt::PlainText,
				DisplayTimeCorrection_Container);
		BR = new Wt::WText(
				CStringUtils::ftos(
						HPTimer::HighPerTimer::HPTimertoD(
								stSourceData.mDeltaTime)), Wt::PlainText,
				DisplayTimeCorrection_Container);
		BR =
				new Wt::WText(
						" accroding to the sender session initialization is applied to the printed results",
						Wt::PlainText, DisplayTimeCorrection_Container);
		BR->setStyleClass("control");
		groupBox->addWidget(DisplayTimeCorrection_Container);

		//----------------------------Bottons-----------------------------------//
		Wt::WContainerWidget *Buttons_Container = new Wt::WContainerWidget(
				DisplayTimeCorrection_Container);
		Buttons_Container->setStyleClass("container");
		BR = new Wt::WText("<br/><hr />", Wt::XHTMLText, Buttons_Container);
		BR->setStyleClass("control-label");
		Wt::WPushButton *NewSessionButton = new Wt::WPushButton("New Session",
				Buttons_Container);
		NewSessionButton->setStyleClass("btn-primary span2");
		NewSessionButton->setLink(
				Wt::WLink(Wt::WLink::InternalPath, "/Configure_Session"));
		Wt::WPushButton *GraphicButton = new Wt::WPushButton("Graphic",
				Buttons_Container);
		GraphicButton->setStyleClass("btn-primary span2");
		GraphicButton->setLink(Wt::WLink(Wt::WLink::InternalPath, "/Graphic"));
		groupBox->addWidget(Buttons_Container);
		Footer(TestResultContainer);
	}
	else
	{
		Wt::WMessageBox *msgBox =
				new Wt::WMessageBox("Error!",
						"<p>Starting session failed! Please check parameters of session. Especially the addresses!</p>"
								"<h4>Details: </h4>"
								"<p>" + sErrorMessage + "</p>", Wt::Warning,
						Wt::Ok);
		msgBox->buttonClicked().connect(std::bind([=] ()
		{
			delete msgBox;
		}));
		msgBox->show();
		app->setInternalPath("/Configure_Session");
		Router();
	}

}

/*! \brief create graphic html page
 */
void CLTestWebGUI::Graphic()
{
	leftMenu->select(3);
	// reset all containers
	ClearContainers();
	// no error occurred by measurement's processing
	if (!bErrorFlag)
	{
		try
		{
			//----------------------set up Group-----------------------------------//
			Wt::WContainerWidget *GraphicSubContainer_1 =
					new Wt::WContainerWidget(GraphicContainer);
			GraphicSubContainer_1->setStyleClass("row");
			Wt::WGroupBox *groupBox = new Wt::WGroupBox("Graphic",
					GraphicSubContainer_1);
			groupBox->addStyleClass("span12");
			Wt::WStandardItemModel *pDataModel_ptr;
			//-------------------------------get csvModel-------------------------------------//

			pDataModel_ptr = WebGUIUtils::csvToModel(sCsvFilename,
					GraphicSubContainer_1);
			//----------------------------------Table View------------------------------------//
			// Renders the data in a table.
			Wt::WContainerWidget *TableViewContainer =
					new Wt::WContainerWidget();
			TableViewContainer->setStyleClass("span12");
			Wt::WText *BR = new Wt::WText("<h4>Values Table:</h4>",
					Wt::XHTMLText, TableViewContainer);
			Wt::WTableView *table = new Wt::WTableView(TableViewContainer);
			table->setMargin(Wt::WLength::Auto);
			table->setModel(pDataModel_ptr);
			table->setSortingEnabled(false);
			table->setColumnResizeEnabled(true);
			table->setSelectionMode(Wt::NoSelection);
			table->setAlternatingRowColors(true);
			table->setColumnAlignment(0, Wt::AlignCenter);
			table->setHeaderAlignment(0, Wt::AlignCenter);
			table->setRowHeight(30);
			table->setHeaderHeight(30);
			table->resize(1000, 300);
			table->setColumnWidth(0, 60);
			table->setColumnWidth(1, 140);
			table->setColumnWidth(2, 140);
			table->setColumnWidth(3, 120);
			table->setColumnWidth(4, 80);
			table->setColumnWidth(5, 100);
			table->setColumnWidth(6, 140);
			table->setColumnWidth(7, 140);
			// Use a delegate for the numeric data which rounds values sensibly
			Wt::WItemDelegate *delegate = new Wt::WItemDelegate(
					TableViewContainer);
			delegate->setTextFormat("%.6f");
			table->setHeaderAlignment(0, Wt::AlignCenter);
			table->setHeaderAlignment(1, Wt::AlignCenter);
			table->setHeaderAlignment(2, Wt::AlignCenter);
			table->setHeaderAlignment(3, Wt::AlignCenter);
			table->setHeaderAlignment(4, Wt::AlignCenter);
			table->setHeaderAlignment(5, Wt::AlignCenter);
			table->setHeaderAlignment(6, Wt::AlignCenter);
			table->setHeaderAlignment(7, Wt::AlignCenter);
			table->setColumnAlignment(0, Wt::AlignCenter);
			table->setColumnAlignment(1, Wt::AlignCenter);
			table->setColumnAlignment(2, Wt::AlignCenter);
			table->setColumnAlignment(3, Wt::AlignCenter);
			table->setColumnAlignment(4, Wt::AlignCenter);
			table->setColumnAlignment(5, Wt::AlignCenter);
			table->setColumnAlignment(6, Wt::AlignCenter);
			table->setColumnAlignment(7, Wt::AlignCenter);
			table->setItemDelegate(delegate);
			table->setItemDelegateForColumn(0,
					new Wt::WItemDelegate(TableViewContainer));
			table->setItemDelegateForColumn(5,
					new Wt::WItemDelegate(TableViewContainer));
			BR = new Wt::WText("<br/>", Wt::XHTMLText, TableViewContainer);
			Wt::WPushButton *getResource_Button = new Wt::WPushButton(
					"Get Result", TableViewContainer);
			Wt::WResource *getResource = new GetResource(TableViewContainer,
					sCsvFilename);
			getResource_Button->setLink(Wt::WLink(getResource));
			getResource_Button->setStyleClass("btn-warning");

			groupBox->addWidget(TableViewContainer);

			//--------------------------------Chart----------------------------------//
			Wt::WContainerWidget *ChartContainer = new Wt::WContainerWidget();
			ChartContainer->setStyleClass("span12");
			BR = new Wt::WText("<hr /><h4>Chart:</h4>", Wt::XHTMLText,
					ChartContainer);
			BR->setStyleClass("control");
			// Creates the scatter plot
			Wt::Chart::WCartesianChart *chart = new Wt::Chart::WCartesianChart(
					ChartContainer);
			chart->setModel(pDataModel_ptr);
			chart->setXSeriesColumn(3);
			chart->setLegendEnabled(true);
			chart->setLegendColumns(7, 180);
			chart->setLegendLocation(Wt::Chart::LegendOutside, Wt::Top,
					Wt::AlignRight);
			chart->setType(Wt::Chart::ScatterPlot);
			chart->axis(Wt::Chart::XAxis).setScale(Wt::Chart::LinearScale);

			// Provide ample space for the title, the X and Y axis and the legend
			chart->setPlotAreaPadding(80, Wt::Left);
			chart->setPlotAreaPadding(50, Wt::Top | Wt::Bottom);
			chart->setPlotAreaPadding(120, Wt::Right);

			// Add the the seventh column as line series
			Wt::Chart::WDataSeries Line(7, Wt::Chart::LineSeries);

			chart->addSeries(Line);
			chart->resize(1000, 400);
			chart->setMargin(Wt::WLength::Auto);
			// set up x,y Axis
			Wt::Chart::WAxis& xAxis = chart->axis(
					static_cast<Wt::Chart::Axis>(0));
			Wt::Chart::WAxis& yAxis = chart->axis(
					static_cast<Wt::Chart::Axis>(1));
			xAxis.setTitle("Relative Time [s]");
			yAxis.setTitle("Delay [s]");
			new ChartConfig(chart, ChartContainer);
			groupBox->addWidget(ChartContainer);

			//----------------------------Bottons-----------------------------------//
			Wt::WContainerWidget *Buttons_Container = new Wt::WContainerWidget(
					ChartContainer);
			Buttons_Container->setStyleClass("container");
			BR = new Wt::WText("<br/><hr />", Wt::XHTMLText, Buttons_Container);
			BR->setStyleClass("control-label");
			Wt::WPushButton *NewSessionButton = new Wt::WPushButton(
					"New Session", Buttons_Container);
			NewSessionButton->setStyleClass("btn-primary span2");
			NewSessionButton->setLink(
					Wt::WLink(Wt::WLink::InternalPath, "/Configure_Session"));
			Wt::WPushButton *GraphicButton = new Wt::WPushButton("Test Result",
					Buttons_Container);
			GraphicButton->setStyleClass("btn-primary span2");
			GraphicButton->setLink(
					Wt::WLink(Wt::WLink::InternalPath, "/Test_Result"));
			groupBox->addWidget(Buttons_Container);
		}
		// can not get CSV File
		catch (Exception &e)
		{
			Wt::WMessageBox *msgBox = new Wt::WMessageBox("Error!",
					"<p>" + e.what() + "</p>", Wt::Warning, Wt::Ok);
			msgBox->buttonClicked().connect(std::bind([=] ()
			{
				delete msgBox;
			}));
			msgBox->show();
			app->setInternalPath("/Test_Result");
			Router();
		} catch (...)
		{
			Wt::WMessageBox *msgBox = new Wt::WMessageBox("Error!",
					"<p>Unexpected Error occurred by Graphic generating!</p>",
					Wt::Warning, Wt::Ok);
			msgBox->buttonClicked().connect(std::bind([=] ()
			{
				delete msgBox;
			}));
			msgBox->show();
			app->setInternalPath("/Test_Result");
			Router();
		}
		Footer(GraphicContainer);
	}
	else
	{
		Wt::WMessageBox *msgBox =
				new Wt::WMessageBox("Error!",
						"<p>Starting session failed! Please check parameters of session. Especially the addresses!</p>"
								"<h4>Details: </h4>"
								"<p>" + sErrorMessage + "</p>", Wt::Warning,
						Wt::Ok);
		msgBox->buttonClicked().connect(std::bind([=] ()
		{
			delete msgBox;
		}));
		msgBox->show();
		app->setInternalPath("/Configure_Session");
		Router();
	}

}

/*! \brief create footer of every pages
 *  \param *container [parent container]
 */
void CLTestWebGUI::Footer(Wt::WContainerWidget *container)
{
	Wt::WText *BR = new Wt::WText("<br/><br/><hr />", Wt::XHTMLText, container);
	BR->setStyleClass("control");
	Wt::WContainerWidget *FooterContainer = new Wt::WContainerWidget(container);
	Wt::WText *Footer =
			new Wt::WText(
					"<footer>"
							"<p><a href=\"http://www.hs-anhalt.de\">Hochschule Anhalt</a>&nbsp;-&nbsp;<a href=\"http://www.hs-anhalt.de\">Eduard, Siemens</a>&nbsp;-&nbsp;<a href=\"http://www.hs-anhalt.de\">Li, Yuan</a> @ Copyright: 2013</p>"
							"</footer>", Wt::XHTMLText, FooterContainer);

	Footer->setStyleClass("control");
}

/*! \brief get all parameters from WebGUI and start a measure session with those parameters
 *  \brief this is the tunnel between WebGUI and gSOAPtunnel
 *  \param stSessionData [stSession saved all parameters of session configuration and will be transmitted to gSOAPtunnel]
 */
void CLTestWebGUI::gSOAPInterface(SessionData stSessionData)
{
	// Configure WebConf object for starting the gSOAPTunnel
	pWebConf_ptr = new ConfLtestStruct;

	// set up the addresses of Session
	pWebConf_ptr->RemoteSender = false;
	CStringTokenizer *pCST_ptr;
	std::string sReceiverDataIP("");
	std::string sHostName("");
	std::string sPort("");
	pCST_ptr = new CStringTokenizer(stSessionData.sReceiverDataAddress, ":");
	sReceiverDataIP = pCST_ptr->nextToken();
	sPort = pCST_ptr->nextToken();
	pCST_ptr = new CStringTokenizer(stSessionData.sReceiverDataAddress, ":");
	pWebConf_ptr->ReceiverDataAddress.Hostname(sReceiverDataIP);
	pWebConf_ptr->ReceiverDataAddress.Port(CStringUtils::stoi(sPort));
	sHostName = "";
	sPort = "";
	pCST_ptr = new CStringTokenizer(stSessionData.sGsoapLocalAddress, ":");
	sHostName = pCST_ptr->nextToken();
	sPort = pCST_ptr->nextToken();
	if (sHostName.empty())
	{
		sHostName = "0.0.0.0";
	}
	if (sPort.empty())
	{
		sPort = "5554";
	}
	pWebConf_ptr->GsoapLocalAddress.Hostname(sHostName);
	pWebConf_ptr->GsoapLocalAddress.Port(CStringUtils::stoi(sPort));
	sHostName = "";
	sPort = "";
	pCST_ptr = new CStringTokenizer(stSessionData.sReceiverGsoapAddress, ":");
	sHostName = pCST_ptr->nextToken();
	sPort = pCST_ptr->nextToken();
	if (sHostName.empty())
	{
		sHostName = sReceiverDataIP;
	}
	if (sPort.empty())
	{
		sPort = "5555";
	}
	pWebConf_ptr->GsoapRemoteAddress.Hostname(sHostName);
	pWebConf_ptr->GsoapRemoteAddress.Port(CStringUtils::stoi(sPort));
	sHostName = "";
	sPort = "";
	pCST_ptr = new CStringTokenizer(stSessionData.sReceiverGsoapAddress, ":");
	sHostName = pCST_ptr->nextToken();
	sPort = pCST_ptr->nextToken();
	if (sHostName.empty())
	{
		sHostName = sReceiverDataIP;
	}
	if (sPort.empty())
	{
		sPort = "5555";
	}
	pWebConf_ptr->ReceiverGsoapAddress.Hostname(sHostName);
	pWebConf_ptr->ReceiverGsoapAddress.Port(CStringUtils::stoi(sPort));
	if (stSessionData.sLocalDataAddress != "")
	{
		sHostName = "";
		sPort = "";
		pCST_ptr = new CStringTokenizer(stSessionData.sLocalDataAddress, ":");
		sHostName = pCST_ptr->nextToken();
		sPort = pCST_ptr->nextToken();
		pCST_ptr = new CStringTokenizer(stSessionData.sLocalDataAddress, ":");
		pWebConf_ptr->LocalDataAddress.Hostname(sHostName);
		pWebConf_ptr->LocalDataAddress.Port(CStringUtils::stoi(sPort));
	}
	delete pCST_ptr;
	// set up the options of WebConf object
	pWebConf_ptr->Blocking = stSessionData.bBlockingMode;
	pWebConf_ptr->TimeCorrection = stSessionData.bTimeCorrection;
	if (pWebConf_ptr->TimeCorrection)
	{
		pWebConf_ptr->MaxTimeDev = stSessionData.iTimeCorrection;
	}
	pWebConf_ptr->ICA=stSessionData.bICA;
	pWebConf_ptr->Priority = stSessionData.iPriority;
	pWebConf_ptr->TOS = stSessionData.iTOS;
	pWebConf_ptr->Size = stSessionData.iPacketSize;
	pWebConf_ptr->Packets = stSessionData.iAmountOfPackets;
	pWebConf_ptr->Bytes = stSessionData.iAmountOfBytes;
	pWebConf_ptr->BPS = stSessionData.iDataRate;
	pWebConf_ptr->SType = stSessionData.eSourceType;
	if (pWebConf_ptr->SType == ST_FIFO)
	{
		pWebConf_ptr->FifoFile = stSessionData.sFifoFileName;
	}
	pWebConf_ptr->Proto = stSessionData.eProtocolType;

	uint64_t sec(0);
	uint64_t usec(0);
	WebGUIUtils::parseTimeString(stSessionData.sSessionDuration, sec, usec);
	pWebConf_ptr->Duration.SetTimer(sec, usec * 1000);
	if (stSessionData.bUnixTimeFlag)
	{
		WebGUIUtils::parseTimeString(stSessionData.sStartTime, sec, usec);
		pWebConf_ptr->StartTime.SetTimer(sec, usec * 1000);
		WebGUIUtils::parseTimeString(stSessionData.sFinishTime, sec, usec);
		pWebConf_ptr->FinishTime.SetTimer(sec, usec * 1000);
	}
	else
	{
		HPTimer::HighPerTimer currentTime;
		currentTime.SetNow();
		WebGUIUtils::parseTimeString(stSessionData.sStartTime, sec, usec);
		pWebConf_ptr->StartTime.SetTimer(sec + currentTime.Seconds(),
				usec * 1000 + currentTime.NSeconds());
		WebGUIUtils::parseTimeString(stSessionData.sFinishTime, sec, usec);
		pWebConf_ptr->FinishTime.SetTimer(sec + currentTime.Seconds(),
				usec * 1000 + currentTime.NSeconds());
	}
	pWebConf_ptr->DumpSamples = true;
	pWebConf_ptr->DumpFile = "result.txt";

	normalize(stSessionData.bSendingModeFlag);

#ifdef DEBUG_OUTPUT
	// display debug informations
	displayParameters(stSessionData, pWebConf_ptr);
#endif
    // pointer to the struct
	ConfLtestStruct * Conf;

	CCommandLine::SetWebConf(pWebConf_ptr);

	Conf = CCommandLine::conf();

	// start measure session by start GsoapClient
	try
	{
		std::string URL = Conf->GsoapRemoteAddress.GsoapURL();
		ltestProxy Proxy(URL.c_str(), SOAP_IO_KEEPALIVE);
		GsoapClient *client = new GsoapClient(Proxy);
		client->Run();

		pTestResult_ptr = client->GetDataSource();
		sDisplayBufSetup = pTestResult_ptr->PrintSDesc();
		client->FetchResults();

		sDisplayBufResult = pTestResult_ptr->PrintStats();
		sCsvFilename = "";
		// convert DumpFile to .csv File
		sCsvFilename = WebGUIUtils::TxtToCsv(pWebConf_ptr->DumpFile);
		if (sCsvFilename.empty())
		{
			throw Exception(2, "Can not open the CSV File!");
		}
	} catch (const CNetwork_exception & ex)
	{
		sErrorMessage = "CNetwork exception occured. Reason: "
				+ CStringUtils::itos(ex.code()) + " Code: " + ex.what();
		std::cerr << sErrorMessage << std::endl;
		bErrorFlag = true;
	} catch (Exception &e)
	{
		sErrorMessage = e.what();
		std::cerr << sErrorMessage << std::endl;
		bErrorFlag = true;
	} catch (...)
	{
		sErrorMessage = "Unknown exception caught";
		std::cerr << sErrorMessage << std::endl;
		bErrorFlag = true;
	}
}

/*! \brief Display the parameters of Session
 *  \param stSessionData [the values, which set by users in HTML page]
 *  \param *pWebConf_ptr [Source data which have been already transmitted to gSOAPtunnel]
 */
void CLTestWebGUI::displayParameters(SessionData stSessionData,
		ConfLtestStruct *pWebConf_ptr)
{
	std::cout.setf(std::ios::left);
	std::cout << "\nParameter Configuration of Session by WebGUI\n";
	std::cout
			<< "------------------------------------------------------------------------\n";
	std::cout << "\n";
	std::cout << std::setw(30) << "GsoapLocalAddress: "
			<< pWebConf_ptr->GsoapLocalAddress << "\n";
	std::cout << std::setw(30) << "GsoapRemoteAddress: "
			<< pWebConf_ptr->GsoapRemoteAddress << "\n";
	std::cout << std::setw(30) << "ReceiverDataAddress: "
			<< pWebConf_ptr->ReceiverDataAddress << "\n";
	std::cout << std::setw(30) << "ReceiverGsoapAddress: "
			<< pWebConf_ptr->ReceiverGsoapAddress << "\n";
	std::cout << std::setw(30) << "LocalDataAddress: "
			<< pWebConf_ptr->LocalDataAddress << "\n\n";

	if (pWebConf_ptr->Proto == P_TCP)
	{
		std::cout << std::setw(30) << "ProtocolType: " << "TCP\n";
	}
	else if (pWebConf_ptr->Proto == P_UDP)
	{
		std::cout << std::setw(30) << "ProtocolType: " << "UDP\n";
	}
	std::cout << std::setw(30) << "DataRate: " << pWebConf_ptr->BPS
			<< " bit/s\n";
	std::cout << std::setw(30) << "PacketSize: " << pWebConf_ptr->Size
			<< " Byte\n";
	std::cout << "\n";
	if (stSessionData.bSendingModeFlag)
	{
		std::cout << std::setw(30) << "SendindMode: " << "Sending by Time\n";
		std::cout << std::setw(30) << "SessionDuration: "
				<< pWebConf_ptr->Duration << " <sec>.<nanosec>\n";
		std::cout << std::setw(30) << "StartTime: " << pWebConf_ptr->StartTime
				<< " <sec>.<nanosec>\n";
		std::cout << std::setw(30) << "FinishTime: " << pWebConf_ptr->FinishTime
				<< " <sec>.<nanosec>\n";
	}
	else
	{
		std::cout << std::setw(30) << "SendindMode: " << "Sending by amount of Packets\n";
		std::cout << std::setw(30) << "AmountOfPackets: "
				<< pWebConf_ptr->Packets << "\n";
		std::cout << std::setw(30) << "AmountOfBytes: " << pWebConf_ptr->Bytes
				<< " Byte\n";
	}
	std::cout << "\n";
	if (pWebConf_ptr->SType == ST_CBR)
	{
		std::cout << std::setw(30) << "SourceType: " << "CBR\n";
	}
	else if (pWebConf_ptr->SType == ST_POISSON)
	{
		std::cout << std::setw(30) << "SourceType: " << "Poisson\n";
	}
	else if (pWebConf_ptr->SType == ST_FIFO)
	{
		std::cout << std::setw(30) << "SourceType: " << "FIFO\n";
		std::cout << std::setw(30) << "FifoFileName: " << pWebConf_ptr->FifoFile
				<< "\n";
	}
	std::cout << std::setw(30) << "DumpFile: " << pWebConf_ptr->DumpFile
			<< "\n";
	std::cout << std::setw(30) << "Priority: " << pWebConf_ptr->Priority
			<< "\n";
	std::cout << std::setw(30) << "TOS: " << static_cast<int>(pWebConf_ptr->TOS)
			<< "\n";
	if (pWebConf_ptr->TimeCorrection)
	{
		std::cout << std::setw(30) << "TimeCorrection: " << "Enable\n";
		std::cout << std::setw(30) << "TimeCorrection: "
				<< pWebConf_ptr->MaxTimeDev << " ns\n";
	}
	else
	{
		std::cout << std::setw(30) << "TimeCorrection: " << "Disable\n";
	}
	if (pWebConf_ptr->Blocking)
	{
		std::cout << std::setw(30) << "BlockingMode: " << "Enable\n";
	}
	else
	{
		std::cout << std::setw(30) << "BlockingMode: " << "Disable\n";
	}
	std::cout
			<< "\n------------------------------------------------------------------------\n\n";
}

/*! \brief override redundant parameters, takes proper preferences of parameters and switches
 *  \param bSendingModeFlag [save the sending mode, true: sending by time, false: sending by amount of packets]
 */
void CLTestWebGUI::normalize(bool bSendingModeFlag)
{
	// Sending by Time
	if (bSendingModeFlag)
	{
		pWebConf_ptr->Packets = 0;
		pWebConf_ptr->Bytes = 0;
	}
	// Sending by Packet Size
	else
	{
		pWebConf_ptr->Duration = HPTimer::HighPerTimer::Nil();
		pWebConf_ptr->StartTime = HPTimer::HighPerTimer::Nil();
		pWebConf_ptr->FinishTime = HPTimer::HighPerTimer::Nil();
	}
	// test start and stop time as well as the duration
	if ((!(pWebConf_ptr->FinishTime).IsNil())
			&& ((pWebConf_ptr->StartTime > pWebConf_ptr->FinishTime)
					|| (pWebConf_ptr->FinishTime < HPTimer::HighPerTimer::Now())))
	{
		(pWebConf_ptr->StartTime).SetNow();
	}
	// duration overrides the finish time
	if (!(pWebConf_ptr->Duration).IsNil()
			&& (!(pWebConf_ptr->StartTime).IsNil()))
	{
		pWebConf_ptr->FinishTime = pWebConf_ptr->StartTime
				+ pWebConf_ptr->Duration;
	}
	// the amount of bytes takes precedence over the duration and finish time
	if ((pWebConf_ptr->Bytes != 1024 * 1024) && (pWebConf_ptr->Bytes != 0))	//not default
	{
		pWebConf_ptr->Duration = HPTimer::HighPerTimer::Nil();
	}
	// if bytes have not been set, but duration has been set, reset bytes
	else if (!(pWebConf_ptr->Duration).IsNil())
	{
		pWebConf_ptr->Bytes = 0;
	}
	// the amount of packets takes precedence over the duration and finish
	// time and bytes
	if (pWebConf_ptr->Packets != 0)
	{
		pWebConf_ptr->Duration = HPTimer::HighPerTimer::Nil();
		pWebConf_ptr->Bytes = 0;
	}
	// if data rate explicitly set to 0, the full performance source should
	// be started
	if (pWebConf_ptr->BPS == 0)
	{
		pWebConf_ptr->SType = ST_FS;
	}
}

/*! \brief get measurement results from class CSourceData
 */
void CLTestWebGUI::getResult()
{
	CSourceData *pCSourceData_ptr = new CSourceData();
	stSourceData = pCSourceData_ptr->getSourceData(pTestResult_ptr);
	delete pCSourceData_ptr;
}

/*! \brief get measurement results from data source and save them in stSourceData struct
 *  \param *pTestResult_ptr [Source data from ltest main program]
 */
SourceData CSourceData::getSourceData(CDataSource * pTestResult_ptr)
{
    //	calculate basic statistics for the transmission and store result
    //	in human readable form in the string.
    //	Calculations are corrected on the fly by mDeltaTime
	std::vector<CPDUHeader> tmpHeader;
	tmpHeader = pTestResult_ptr->getHeader();
	std::vector<CPDUHeader>::iterator it(tmpHeader.begin());
	for (; it != tmpHeader.end(); it++)
	{
		HPTimer::HighPerTimer S(it->SendTime()), R(it->RecvTime()), D;
		if (!R.IsNil())
		{
			D = R - S;
		}
		else
		{
			D = D = HPTimer::HighPerTimer::Nil();
		}
		uint32_t SeqNo(it->SeqNo()), Len(it->Length());
		procLine(SeqNo, S, R, D, Len);
	}
	postProc();
	CorrectDelays(pTestResult_ptr->getDeltaTime());
	SourceData stSourceData;
	stSourceData.mSentCount = mSentCount;
	stSourceData.mPackAvg = mPackAvg;
	stSourceData.mRecvCount = mRecvCount;
	stSourceData.mPktBytesSumLoss = mPktBytesSumLoss;
	stSourceData.mSentTMin = mSentTMin;
	stSourceData.mRecvTMin = mRecvTMin;
	stSourceData.mDelayMin = mDelayMin;
	stSourceData.mPktSizeMin = mPktSizeMin;
	stSourceData.mSentMinPos = mSentMinPos;
	stSourceData.mRecvMinPos = mRecvMinPos;
	stSourceData.mDelayMinPos = mDelayMinPos;
	stSourceData.mSizeMinPos = mSizeMinPos;
	stSourceData.mSentTMax = mSentTMax;
	stSourceData.mRecvTMax = mRecvTMax;
	stSourceData.mDelayMax = mDelayMax;
	stSourceData.mPktSizeMax = mPktSizeMax;
	stSourceData.mSentMaxPos = mSentMaxPos;
	stSourceData.mRecvMaxPos = mRecvMaxPos;
	stSourceData.mDelayMaxPos = mDelayMaxPos;
	stSourceData.mSizeMaxPos = mSizeMaxPos;
	stSourceData.mSentAvg = mSentAvg;
	stSourceData.mRecvAvg = mRecvAvg;
	stSourceData.mDelayAvg = mDelayAvg;
	stSourceData.mPackAvg = mPackAvg;
	stSourceData.mSentStdv = mSentStdv;
	stSourceData.mRecvStdv = mRecvStdv;
	stSourceData.mDelayStdv = mDelayStdv;
	stSourceData.mSizeStdv = mSizeStdv;
	stSourceData.mDeltaTime = pTestResult_ptr->getDeltaTime();
	return stSourceData;
}

/*! \brief function in order to start WebGUI
 */
Wt::WApplication *createApplication(const Wt::WEnvironment& env)
{
	return new CLTestWebGUI(env);
}

/*! \brief Entrance of WebGUI
 */
uint16_t RunWebGUI()
{
	uint16_t argc=7;
	char *argv[7];
	// web server running's environmental parameters
	argv[1] = "--docroot";
	argv[2] = ".";
	argv[3] = "--http-address";
	argv[4] = "0.0.0.0";
	argv[5] = "--http-port";
	argv[6] = "5600";
	return Wt::WRun(argc, argv, &createApplication);
}
