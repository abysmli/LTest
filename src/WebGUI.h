//============================================================================
// Name        : WebGUI.h
// Author      : Li, Yuan
// email	   : abysmli@gmail.com
// Version     : 1.3
// Copyright   : All rights reserved by Anhalt University of Applied Sciences
// Description : Web Graphical User Interface of LTest
//============================================================================

#ifndef WEBGUI_H_
#define WEBGUI_H_
#pragma GCC diagnostic ignored "-Wwrite-strings"

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <Wt/WMenu>
#include <Wt/WMessageBox>
#include <Wt/WNavigationBar>
#include <Wt/WPopupMenu>
#include <Wt/WPopupMenuItem>
#include <Wt/WStackedWidget>
#include <Wt/WBootstrapTheme>
#include <Wt/WGroupBox>
#include <Wt/WTemplate>
#include <Wt/WLabel>
#include <Wt/WIntValidator>
#include <Wt/WComboBox>
#include <Wt/WCheckBox>
#include <Wt/WProgressBar>
#include <Wt/WTimer>
#include <Wt/WTable>
#include <Wt/WTableCell>
#include <Wt/Chart/WCartesianChart>
#include <Wt/Chart/WDataSeries>
#include <Wt/Chart/WPieChart>
#include <Wt/WAbstractItemModel>
#include <Wt/WAbstractItemView>
#include <Wt/WDate>
#include <Wt/WEnvironment>
#include <Wt/WPaintedWidget>
#include <Wt/WItemDelegate>
#include <Wt/WShadow>
#include <Wt/WStandardItemModel>
#include <Wt/WTableView>
#include <Wt/WBorderLayout>
#include <Wt/WFitLayout>
#include <Wt/WStandardItem>
#include <Wt/WAbstractItemModel>
#include <Wt/WDoubleValidator>
#include <Wt/WIntValidator>
#include <Wt/WLocale>
#include <Wt/WPanel>
#include <Wt/WPainterPath>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/WObject>
#include <Wt/WResource>
#include <Wt/WPainter>
#include <Wt/WPdfImage>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/Render/WPdfRenderer>
#include <Wt/WTimer>
#include <Wt/WDialog>
#include <Wt/WRegExpValidator>
#include <Wt/WRadioButton>
#include <Wt/WButtonGroup>
#include <Wt/WFileUpload>
#include <Wt/WException>

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio.hpp>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <hpdf.h>
#include <math.h>
#include "strtok.h"
#include "Client.h"
#include "cstats.h"
#include "SecurityCheck.h"
#include <thread>

/*! \struct SessionData
 *  \brief save Session Data
 */
struct SessionData
{
	/// receiver data address
	std::string sReceiverDataAddress;
	/// address of the receiver GSOAP daemon
	std::string sReceiverGsoapAddress;
	/// local data address
	std::string sLocalDataAddress;
	/// address of local GSOAP daemon
	std::string sGsoapLocalAddress;
	/// address of the remote GSOAP daemon
	std::string sGsoapRemoteAddress;
	/// Protocol type of test session
	proto_t eProtocolType;
	/// Source type of test session
	SourceType eSourceType;
	/// session start time 
	std::string sStartTime;
	/// session finish time
	std::string sFinishTime;
	/// session duration
	std::string sSessionDuration;
	/// test session packet size
	uint32_t iPacketSize;
	/// amount of test packets
	uint32_t iAmountOfPackets;
	/// amount of test bytes 
	uint64_t iAmountOfBytes;
	/// data rate of test session
	uint64_t iDataRate;
	/// test priority
	uint16_t iPriority;
	/// TOS of ip header
	uint16_t iTOS;
	/// max deviation of time correction
	uint64_t iTimeCorrection;
	/// fifo file name
	std::string sFifoFileName;
	/// switch of time correction function
	bool bTimeCorrection;
	/// blocking mode flag
	bool bBlockingMode;
	/// switch of sending modes
	bool bSendingModeFlag;
	/// switch of time modes
	bool bUnixTimeFlag;
	/// switch of ICA
	bool bICA;
};

/*! \struct SourceData
 *  \brief get measure result and display them in HTML page
 */
struct SourceData
{	
	/// min sent interpacket time
	HPTimer::HighPerTimer mSentTMin;  
	/// max sent interpacket time
	HPTimer::HighPerTimer mSentTMax;
	/// position of maximum sent interpacket time
	unsigned long mSentMaxPos; 
	/// position of minimum sent interpacket time
	unsigned long mSentMinPos; 
	/// sent time of the previous valid sent data packet
	HPTimer::HighPerTimer mPrevSentT; 
	/// receivint time of the previous received data packet
	HPTimer::HighPerTimer mPrevRecvT; 
	HPTimer::HighPerTimer mFirstSentT;
	HPTimer::HighPerTimer mLastSentT;
	/// sum of sending interpacket times
	double mSentTSum;   
	/// square sum of sending interpacket times
	double mSentTSumqv;
	/// mean of sending interpacket times
	double mSentAvg; 
	/// standard deviation of interpacket times
	double mSentStdv;  
	/// min receiving interpacket time
	HPTimer::HighPerTimer mRecvTMin; 
	/// max receiving interpacket time
	HPTimer::HighPerTimer mRecvTMax;
	/// position of maximum receiving interpacket time
	unsigned long mRecvMaxPos;
	/// position of minimum receiving interpacket time
	unsigned long mRecvMinPos; 
	HPTimer::HighPerTimer mRecv_im1T;
	HPTimer::HighPerTimer mRecv_i0;
	HPTimer::HighPerTimer mRecvDT;
	double mRecvTSum;
	double mRecvTSumqv;
	double mRecvAvg;
	double mRecvStdv;
	/// min delay
	HPTimer::HighPerTimer mDelayMin;  
	/// max delay
	HPTimer::HighPerTimer mDelayMax; 
	/// mean delay 
	double mDelayAvg;     
	double hDel;
	double mDelaySum;
	double mDelaySumqv;
	double mDelayStdv;
	/// min packet size
	unsigned int mPktSizeMin; 
	/// max packet size
	unsigned int mPktSizeMax; 
	/// sent packets counter
	unsigned int mSentPackets;  
	/// sent bytes counter
	unsigned long long mSentBytes;
	/// overall bytes sum (sum of processed bytes) 
	double mPktBytesSum;      
	/// square sum of bytes processed
	double mPktBytesSumqv;    
	/// average packet size
	double mPackAvg;         
	/// stdv of packet sizes 
	double mSizeStdv;        
	/// sum of bytes lost
	double mPktBytesSumLoss;  
	///overall counter for procLine calls
	unsigned long mLineCount; 
	/// sending packets counter
	unsigned long mSentCount; 
	/// receiving packets counter
	unsigned long mRecvCount; 
	/// position of maximum delay
	unsigned long mDelayMaxPos;  
	/// position of minimum delay
	unsigned long mDelayMinPos;
	/// position of maximum packet size  
	unsigned long mSizeMaxPos; 
	/// position of minimum packet size
	unsigned long mSizeMinPos; 
	HPTimer::HighPerTimer mDeltaTime;
	HPTimer::HighPerTimer mTempT;
};

/*! \enum ValidatorType
 *	\brief control Validator's Type
 */
enum ValidatorType
{
	Address, Size, Time,
};

/*! \fn
 * 	\brief main entrance of WebGUI
 */
uint16_t RunWebGUI(void);

/*! \class Exception
 *  \brief Exception system for LTest GUI
 *  \brief Show the Fault Message and save them in syslog
 */
class Exception
{
private:
	//! Error Level
	uint16_t iFaultCode;
	//! Fault Message
	std::string sFaultMessage;

public:

	/*! \brief Constructor
	 *  \param iFaultCode [Error code]
	 *  \param sFaultMessage [Error informations]
	 */
	Exception(uint16_t, std::string);

	/// FIXME do we actually need this
	/*! \brief save Faultmessages in Log file or in syslog
	 */
	void saveInLog();

	/*! \brief dispose errors
	 *  \details iFaultCode = -1# : gsoap errors
	 *  \details iFaultCode = 1 : Session Configuration errors
	  * \return sFaultMessage [return Error message]
	 */
	std::string what();
};

/*! \class WebGUIUtils
 *  \brief useful tools for strings of WebGUI
 */
class WebGUIUtils
{
public:

	/*! \brief validator of WebGUI, in order to make sure the value of parameters are valid
	 *  \param eValidatorType [set which type of value should be validate]
	 *  \param string [the string (value), which will be checked]
	 *  \return false when there are errors by parsing value from widget
	 */
	static bool validator(ValidatorType eValidatorType, std::string string);

	/*! \brief convert the string to double and recalculate the value with unit
	 *  \param string [the string (value), which will be converted]
	 *  \param eUnit [the unit of the string value]
	 *  \return the value, which has been converted
	 */
	static double StringToDoubleWithUnit(std::string string, UNIT eUnit);

	 /*! \brief get the unit of a string value
	  *  \param string [the string (value), which will be checked]
	  *  \return UNIT [the unit of value]
	  */
	static UNIT checkUnit(std::string string);

	/*! \brief parse the time string with the format <sec>[.<usec>], sets seconds and microseconds parameters.
	 * 	\param pTimeStr [the time which will be parsed]
	 * 	\param pSec [second from parsing]
	 * 	\param pUsec [microsecond from parsing]
	 *  \return true if no parse errors have occurred, false otherways
	 */
	static bool parseTimeString(const std::string pTimeStr, uint64_t & pSec,
			uint64_t & pUsec);

	/*! \brief convert plain text file to csv file
	 * 	\param sInputFilename [the plain text file name]
	 *  \return csv file name
	 */
	static std::string TxtToCsv(std::string sFilename);

	/*! \brief convert csv file to data model
	 * 	\param csvFile [the csv file name]
	 * 	\param *parent [parent container]
	 *  \return StandardItemModel, it is the data model, which created according to csv-file's content
	 */
	static Wt::WStandardItemModel *csvToModel(const std::string csvFile, Wt::WObject *parent);
};

/*! \class GetImgResource
 *  \brief generate the PDF image from chart
 */
class GetImgResource: public Wt::WPdfImage
{
public:

	/*! \brief a constructor
	 *  \param *parent [parent container]
	 */
	GetImgResource(Wt::WObject *parent, Wt::Chart::WCartesianChart *chart);
};

/*! \class GetResource
 *  \brief generate CSV file from the measure result
 */
class GetResource: public Wt::WResource
{
private:
	std::string sCsvFilename;
public:

	/*! \brief a constructor
	 *  \brief generate CSV file from the measure result
	 *  \param *parent [parent container]
	 *  \param sCsvFilename [ setting csv Filename for downloading ]
	 */
	GetResource(Wt::WObject *parent,std::string sCsvFilename);

	/*! \brief destructor
	 */
	~GetResource();

	/*! \brief handle the "GET" request, and response it
	 *  \param &request [HTTP Request]
	 *  \param &response [HTTP Response]
	 */
	void handleRequest(const Wt::Http::Request &request,
			Wt::Http::Response &response);
};

/*! \class PanelList
 * 	\brief generate a PanelList widget with controller
 */
class PanelList: public Wt::WContainerWidget
{
public:

	/*! \brief a Constructor
	 *  \param *parent [parent container]
	 */
	PanelList(Wt::WContainerWidget *parent);

	/*! \brief add panellist widget and set up this widget
	 *  \param &text [the title of panellist]
	 *  \param *w [the widget, which will be set into panellist]
	 *  \param bOpenFlag [false: by default close panellist | true: by default open panellist]
	 *  \param bSetCollapsibel [true: open dynamic feature | false: close it]
	 *  \return p [return panellist widget]
	 */
	Wt::WPanel *addWidget(const Wt::WString& text, WWidget *w, bool bOpenFlag,
			bool bSetCollapsibel);

	/*! \brief set up panellist
	 *  \param *panel [get panellist widget, which will be set up]
	 *  \param bOpenFlag [false: by default close panellist | true: by default open panellist]
	 *  \param bSetCollapsibel [true: open dynamic feature | false: close it]
	 */
	void addPanel(Wt::WPanel *panel, bool bOpenFlag, bool bSetCollapsibel);

	/// FIXME not implement yet and do we need this?
	//! \brief remove a PanelList widget from PanelList group
	void removePanel(Wt::WPanel *panel);

private:

	/*! \brief make sure in one time can only one panellist open
	 *  \param notUndo [true: already opened | close: closed]
	 */
	void onExpand(bool notUndo);

	//! \brief save state of PanelList
	int8_t wasExpanded_;
};

/*! \class SessionConfig
 *  \brief configure the measure session
 *  \brief generate the "Configure Session" HTML page
 */
class SessionConfig: public Wt::WContainerWidget
{
public:
	/*! \brief A constructor.
	 *  \brief set up the widgets in HTML Configuration Session page
	 *  \*parent [parent container]
	 */
	SessionConfig(Wt::WContainerWidget *parent);

	/*! \brief check whether parameters in configuration valid or not
	 *  \return Error message
	 */
	std::string checkParameter();

	//! \brief ReceiverIP widget
	Wt::WLineEdit *ReceiverIP_LineEdit;

	//! \brief Protocol widget
	Wt::WComboBox *Protocol_ComboBox;

	//! \brief PacketSize widget
	Wt::WLineEdit *PacketSize_LineEdit;

	//! \brief Datarate widget
	Wt::WLineEdit *Datarate_LineEdit;

	//! \brief SessionDuration widget
	Wt::WLineEdit *SessionDuration_LineEdit;

	//! \brief AmountOfPackets widget
	Wt::WLineEdit *AmountOfPackets_LineEdit;

	//! \brief SenderIP widget
	Wt::WLineEdit *SenderIP_LineEdit;

	//! \brief LocalSoapAddress widget
	Wt::WLineEdit *LocalSoapAddress_LineEdit;

	//! \brief receiverSoapAddress widget
	Wt::WLineEdit *receiverSoapAddress_LineEdit;

	//! \brief TypeOfSource widget
	Wt::WComboBox *TypeOfSource_ComboBox;

	//! \brief StartTime mode widget
	Wt::WCheckBox *StartTime_CheckBox;

	//! \brief StartTime widget
	Wt::WLineEdit *StartTime_LineEdit;

	//! \brief FinishTime widget
	Wt::WLineEdit *FinishTime_LineEdit;

	//! \brief AmountOfBytesP widget
	Wt::WLineEdit *AmountOfBytes_LineEdit;

	//! \brief Priority widget
	Wt::WLineEdit *Priority_LineEdit;

	//! \brief TOS widget
	Wt::WLineEdit *TOS_LineEdit;

	//! \brief TimeCorrection widget
	Wt::WCheckBox *TimeCorrection_CheckBox;

	//! \brief TimeCorrection widget
	Wt::WLineEdit *TimeCorrection_LineEdit;

	//! \brief ICA widget
	Wt::WCheckBox *ICA_CheckBox;

	//! \brief BlockingMode widget
	Wt::WCheckBox *BlockingMode_CheckBox;

	//! \brief FileUpload widget
	Wt::WFileUpload *FileUpload;

	///FIXME vInterface* used for interfaces detected, do we need them?
	// std::vector<std::string> vInterfaceNames;
	// std::vector<std::string> vInterfaceIPs;

	//! \brief save the sending mode, that user chose
	bool bSendingModeFlag;

private:

	/*! \brief update and refresh the session configuration widgets by settings
	 */
	void update();

	/*! \brief send widget changed signal to update() method
	 *  \param *w [widget, which will be connected]
	 */
	void connectSignals(Wt::WFormWidget *w);

	///FIXME getInterfaces used for interfaces detected, do we need it?
	// get local network interfaces
	//uint16_t getInterfaces();

	/*! \brief FIFO file upload widget
	 */
	void fileupload();

	//! \brief check whether Receiver SOAP Address widget changed
	bool bReceiverSOAPAddressChangeFlag;

	//! \brief reUpload Push Button
	Wt::WPushButton *reUploadButton;

	//! \brief Sending Mode Time Mode Radio Button
	Wt::WRadioButton *SendingModeRadio_TimeMode_button;

	//! \brief Sending Mode Packet Size Mode Radio Button
	Wt::WRadioButton *SendingModeRadio_PacketSizeMode_button;

	//! \brief SessionDuration container
	Wt::WContainerWidget *SessionDuration_Container;

	//! \brief StartTime container
	Wt::WContainerWidget *StartTime_Container;

	//! \brief FinishTime container
	Wt::WContainerWidget *FinishTime_Container;

	//! \brief AmountOfPackets container
	Wt::WContainerWidget *AmountOfPackets_Container;

	//! \brief AmountOfBytes container
	Wt::WContainerWidget *AmountOfBytes_Container;

	//! \brief Information container
	Wt::WContainerWidget *Information_Container;

	//! \brief FIFO_File_upload container
	Wt::WContainerWidget *FIFO_File_upload_Container;

	//! \brief RelativeTime container
	Wt::WContainerWidget *RelativeTime_Container;

	//! \brief UnixTime container
	Wt::WContainerWidget *UnixTime_Container;

	//! \brief interval Timer, used as multithread pool in Web GUI
	Wt::WTimer *intervalTimer;

	//! \brief Information Label
	Wt::WText *Information_Label;

	//! \brief Error informations of FIFO File upload widget
	std::string sFIFOFileParseErrorMessage;

	//! \brief Error informations
	std::string sErrorMessage;

	//! \brief true: no Errors, false: Errors
	bool bFIFOFileParseErrorFlag;

	//! \brief true: no Errors, false: Errors
	bool bErrorFlag;
};

/*! \class ChartConfig
 *  \brief configure the chart synchronized
 *  \brief generate the chart canvas
 */
class ChartConfig: public Wt::WContainerWidget
{
public:
	/*! \brief A constructor
	 *  \brief configuration the chart and update it dynamically
	 *  \param *chart [get chart widget, which will be set up]
	 *  \param *parent [get parent container]
	 */
	ChartConfig(Wt::Chart::WCartesianChart *chart,
			Wt::WContainerWidget *parent);
private:
	/*! \struct AxisControl
	 *  \brief axis controlling needed widgets
	 */
	struct AxisControl
	{
		//! \brief visible CheckBox
		Wt::WCheckBox *visibleEdit;

		//! \brief auto CheckBox
		Wt::WCheckBox *autoEdit;

		//! \brief minimum widget
		Wt::WLineEdit *minimumEdit;

		//! \brief maximum widget
		Wt::WLineEdit *maximumEdit;

		//! \brief gridline CheckBox
		Wt::WCheckBox *gridLinesEdit;
	};

	//! \brief type ComboBox
	Wt::WComboBox *typeEdit;

	//! \brief marker ComboBox
	Wt::WComboBox *markerEdit;

	//! \brief valueLabel CheckBox
	Wt::WCheckBox *valueLabelEdit;

	//! \brief XAxis ComboBox
	Wt::WComboBox *XAxisEdit;

	//! \brief YAxis ComboBox
	Wt::WComboBox *YAxisEdit;

	//! \brief chart widget
	Wt::Chart::WCartesianChart *chart_;
	Wt::Chart::FillRangeType fill_;

	//! \brief set to control axises
	std::vector<AxisControl> axisControls_;

	/*! \brief connect widget change signal to update method
	 *  \param *w [widget, which will be connected]
	 */
	void connectSignals(Wt::WFormWidget *w);

	/*! \brief update the chart by settings
	 */
	void update();

	/*! \brief set up the header of model
	 *  \param *t [the table widget, which the header set for]
	 *  \param *value [the name of header]
	 */
	void addHeader(Wt::WTable *t, const char *value);

	/*! \brief get double value from HTML element
	 *  \param *edit [HTML input element]
	 *  \param &value [the value from input element]
	 *  \return [true: convert no error, false: errors occurred]
	 *  \details in order to avoid crash the WebServer by error input
	 */
	bool getDouble(
			Wt::WLineEdit *edit, double& value);

	/*! \brief set up the content of model, insert rows and data
	 *  \param *model [data model]
	 *  \param *value [data]
	 */
	void addEntry(Wt::WAbstractItemModel *model, const char *value);

	/*! \brief validator of chart configuration
	 *  \param *w [widget, which will be validated]
	 *  \return valid [true: valid, false: invalid]
	 */
	static bool validate(Wt::WFormWidget *w);

	//! \brief parent container
	Wt::WContainerWidget *parent;

	//! \brief getChart push button
	Wt::WPushButton *getChart_Button;
};

/*! \class CSourceData
 *  \brief middleware between WebGUI and gSOAPtunnel
 */
class CSourceData: public CBasicStats
{
public:
	/*! \brief get measurement results from data source and save them in stSourceData struct
	 *  \param *pTestResult_ptr [Source data from ltest main program]
	 */
	SourceData getSourceData(CDataSource *pTestResult_ptr);
};

/*! \class CLTestWebGUI
 *  \details this class generate all HTML pages, get parameters from user and transmit them to ltest main client program
 *  \details running measurement and get results
 *  \details display the results and create graphic from dumpfile
 */
class CLTestWebGUI: public Wt::WApplication, CBasicStats
{
public:
	/*! \brief Constructor of CLTestWebGUI Class
	 * 	\param env [the environment parameters, which needed by WApplication class]
	 */
	CLTestWebGUI(const Wt::WEnvironment& env);

	/*! \brief center router of WebGUI. Router pages to corresponding internal path.
	 */
	void Router(void);

	/*! \brief create Navigationbar
	 */
	void NavigationBar(void);

	/*! \brief create homepage
	 */
	void Homepage(void);

	/*! \brief create configure session html page
	 */
	void Configure_Session(void);

	/*! \brief create test_result html page
	 */
	void Test_Result(void);

	/*! \brief create graphic html page
	 */
	void Graphic(void);

	/*! \brief reset all containers
	 */
	void ClearContainers(void);

	/*! \brief create footer of every pages
	 *  \param *container [parent container]
	 */
	void Footer(Wt::WContainerWidget *container);

	/*! \brief get all parameters from WebGUI and start a measure session with those parameters
	 *  \brief this is the tunnel between WebGUI and gSOAPtunnel
	 *  \param stSessionData [stSession saved all parameters of session configuration and will be transmitted to gSOAPtunnel]
	 */
	void gSOAPInterface(SessionData stSessionData);

	/*! \brief override redundant parameters, takes proper preferences of parameters and switches
	 *  \param bSendingModeFlag [save the sending mode, true: sending by time, false: sending by amount of packets]
	 */
	void normalize(bool bSendingModeFlag);

	/*! \brief get measurement results from class CSourceData
	 */
	void getResult();

	/*! \brief Display the parameters of Session
	 *  \param stSessionData [the values, which set by users in HTML page]
	 *  \param *pWebConf_ptr [Source data which have been already transmitted to gSOAPtunnel]
	 */
	void displayParameters(SessionData stSessionData, ConfLtestStruct *pWebConf_ptr);

private:
	//! \brief application instance
	Wt::WApplication *app;

	//! \brief left menu widget
	Wt::WMenu *leftMenu;

	//! \brief navigationbar container
	Wt::WContainerWidget *NavigationBarContainer;

	//! \brief homepage container
	Wt::WContainerWidget *HomepageContainer;

	//! \brief ConfigureSession container
	Wt::WContainerWidget *ConfigureSessionContainer;

	//! \brief TestResult container
	Wt::WContainerWidget *TestResultContainer;

	//! \brief Graphic container
	Wt::WContainerWidget *GraphicContainer;

	//! \brief save the CSV file name, this file used for create graphic and table. Users can also download it from WebGUI
	std::string sCsvFilename;

	//! \brief Flag of initialization
	bool bInitFlag;

	//! \brief Flag of Errors, true means there are errors, false means no error
	bool bErrorFlag;

	//! \brief Flag of starting state of measurement session
	bool bStartFlag;


protected:
	//! \brief get the session setup informations from data source
	std::string sDisplayBufSetup;

	//! \brief get the session results from data source
	std::string sDisplayBufResult;

	//! \brief transmit session configuration parameters to ltest
	ConfLtestStruct *pWebConf_ptr;

	//! \brief save the session configuration parameters
	CDataSource *pTestResult_ptr;

	//! \brief save the session configuration parameters from HTML page
	SourceData stSourceData;

	//! \brief Error informations
	std::string sErrorMessage;
};

#endif /* WEBGUI_H_ */
