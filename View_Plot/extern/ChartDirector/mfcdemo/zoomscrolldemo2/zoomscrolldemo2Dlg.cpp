// zoomscrolldemo2Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "zoomscrolldemo2.h"
#include "zoomscrolldemo2Dlg.h"
#include "HotSpotDlg.h"
#include "chartdir.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CZoomscrolldemo2Dlg dialog

//
// Constructor
//
CZoomscrolldemo2Dlg::CZoomscrolldemo2Dlg(CWnd* pParent /*=NULL*/)
    : CDialog(CZoomscrolldemo2Dlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CZoomscrolldemo2Dlg)
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

//
// Destructor
//
CZoomscrolldemo2Dlg::~CZoomscrolldemo2Dlg()
{
    delete m_ChartViewer.getChart();
}


void CZoomscrolldemo2Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CZoomscrolldemo2Dlg)
    DDX_Control(pDX, IDC_CenterPB, m_CenterPB);
    DDX_Control(pDX, IDC_PointerPB, m_PointerPB);
    DDX_Control(pDX, IDC_NavigatePad, m_NavigatePad);
    DDX_Control(pDX, IDC_NavigateWindow, m_NavigateWindow);
    DDX_Control(pDX, IDC_ZoomBar, m_ZoomBar);
    DDX_Control(pDX, IDC_UpRightPB, m_UpRightPB);
    DDX_Control(pDX, IDC_UpPB, m_UpPB);
    DDX_Control(pDX, IDC_UpLeftPB, m_UpLeftPB);
    DDX_Control(pDX, IDC_RightPB, m_RightPB);
    DDX_Control(pDX, IDC_LeftPB, m_LeftPB);
    DDX_Control(pDX, IDC_DownRightPB, m_DownRightPB);
    DDX_Control(pDX, IDC_DownPB, m_DownPB);
    DDX_Control(pDX, IDC_DownLeftPB, m_DownLeftPB);
    DDX_Control(pDX, IDC_ChartViewer, m_ChartViewer);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CZoomscrolldemo2Dlg, CDialog)
    //{{AFX_MSG_MAP(CZoomscrolldemo2Dlg)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_PointerPB, OnPointerPB)
    ON_BN_CLICKED(IDC_ZoomInPB, OnZoomInPB)
    ON_BN_CLICKED(IDC_ZoomOutPB, OnZoomOutPB)
    ON_BN_CLICKED(IDC_LeftPB, OnLeftPB)
    ON_BN_CLICKED(IDC_RightPB, OnRightPB)
    ON_BN_CLICKED(IDC_UpPB, OnUpPB)
    ON_BN_CLICKED(IDC_DownPB, OnDownPB)
    ON_BN_CLICKED(IDC_UpLeftPB, OnUpLeftPB)
    ON_BN_CLICKED(IDC_DownLeftPB, OnDownLeftPB)
    ON_BN_CLICKED(IDC_UpRightPB, OnUpRightPB)
    ON_BN_CLICKED(IDC_DownRightPB, OnDownRightPB)
    ON_BN_CLICKED(IDC_CenterPB, OnCenterPB)
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_NavigateWindow, OnNavigateWindow)
    ON_BN_CLICKED(IDC_ChartViewer, OnChartViewer)
    ON_CONTROL(CVN_ViewPortChanged, IDC_ChartViewer, OnViewPortChanged)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZoomscrolldemo2Dlg message handlers

//
// Initialization
//
BOOL CZoomscrolldemo2Dlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // *** code automatically generated by VC++ MFC AppWizard ***
    // Set the icon for this dialog.  The framework does this automatically
    // when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon
    
    //
    // Initialize member variables
    //
    m_extBgColor = getDefaultBgColor();     // Default background color
    m_minX = m_maxX = m_minY = m_maxY = 0;  // x and y axes ranges

    //
    // Initialize controls
    //

    // Load icons to mouse usage buttons
    loadButtonIcon(IDC_PointerPB, IDI_PointerPB, 100, 20);  
    loadButtonIcon(IDC_ZoomInPB, IDI_ZoomInPB, 100, 20);    
    loadButtonIcon(IDC_ZoomOutPB, IDI_ZoomOutPB, 100, 20);  

    // Load icons to navigation buttons
    loadButtonIcon(IDC_UpPB, IDI_UpArrow, 16, 16);
    loadButtonIcon(IDC_DownPB, IDI_DownArrow, 16, 16);
    loadButtonIcon(IDC_LeftPB, IDI_LeftArrow, 16, 16);
    loadButtonIcon(IDC_RightPB, IDI_RightArrow, 16, 16);
    loadButtonIcon(IDC_UpRightPB, IDI_UpRightArrow, 16, 16);
    loadButtonIcon(IDC_DownRightPB, IDI_DownRightArrow, 16, 16);
    loadButtonIcon(IDC_UpLeftPB, IDI_UpLeftArrow, 16, 16);
    loadButtonIcon(IDC_DownLeftPB, IDI_DownLeftArrow, 16, 16);
    loadButtonIcon(IDC_CenterPB, IDI_Dot, 16, 16);

    // Set Pointer pushbutton into clicked state
    m_PointerPB.SetCheck(1);

    // Initialize zoom bar
    m_ZoomBar.SetRange(1, 100);
    m_ZoomBar.SetPageSize(5);
    m_ZoomBar.SetTicFreq(10);

    // Set initial mouse usage for ChartViewer
    m_ChartViewer.setMouseUsage(Chart::MouseUsageScroll);
    m_ChartViewer.setScrollDirection(Chart::DirectionHorizontalVertical);
    m_ChartViewer.setZoomDirection(Chart::DirectionHorizontalVertical);

    // Display the chart
    m_ChartViewer.updateViewPort(true, true);
    return TRUE;
}

// *** code automatically generated by VC++ MFC AppWizard ***
// If you add a minimize button to your dialog, you will need the code below
// to draw the icon.  For MFC applications using the document/view model,
// this is automatically done for you by the framework.
void CZoomscrolldemo2Dlg::OnPaint() 
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

// *** code automatically generated by VC++ MFC AppWizard ***
// The system calls this to obtain the cursor to display while the user drags
// the minimized window.
HCURSOR CZoomscrolldemo2Dlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

//
// User clicks on the Pointer pushbutton
//
void CZoomscrolldemo2Dlg::OnPointerPB() 
{
    m_ChartViewer.setMouseUsage(Chart::MouseUsageScroll);   
}

//
// User clicks on the Zoom In pushbutton
//
void CZoomscrolldemo2Dlg::OnZoomInPB() 
{
    m_ChartViewer.setMouseUsage(Chart::MouseUsageZoomIn);   
}

//
// User clicks on the Zoom Out pushbutton
//
void CZoomscrolldemo2Dlg::OnZoomOutPB() 
{
    m_ChartViewer.setMouseUsage(Chart::MouseUsageZoomOut);  
}

//
// User clicks on the left navigation pushbutton
//
void CZoomscrolldemo2Dlg::OnLeftPB() 
{
    // Scroll left by 10%
    handleButton(&m_LeftPB, -0.1, 0);
}

//
// User clicks on the right navigation pushbutton
//
void CZoomscrolldemo2Dlg::OnRightPB() 
{
    // Scroll right by 10%
    handleButton(&m_RightPB, 0.1, 0);
}

//
// User clicks on the up navigation pushbutton
//
void CZoomscrolldemo2Dlg::OnUpPB() 
{
    // Scroll up by 10%
    handleButton(&m_UpPB, 0, -0.1);
}

//
// User clicks on the down navigation pushbutton
//
void CZoomscrolldemo2Dlg::OnDownPB() 
{
    // Scroll down by 10%
    handleButton(&m_DownPB, 0, 0.1);
}

//
// User clicks on the up-left navigation pushbutton
//
void CZoomscrolldemo2Dlg::OnUpLeftPB() 
{
    // Scroll up and left by 10%
    handleButton(&m_UpLeftPB, -0.1, -0.1);
}

//
// User clicks on the down-left navigation pushbutton
//
void CZoomscrolldemo2Dlg::OnDownLeftPB() 
{
    // Scroll down and left by 10%
    handleButton(&m_DownLeftPB, -0.1, 0.1);
}

//
// User clicks on the up-right navigation pushbutton
//
void CZoomscrolldemo2Dlg::OnUpRightPB() 
{
    // Scroll up and right by 10%
    handleButton(&m_UpRightPB, 0.1, -0.1);
}

//
// User clicks on the down-right navigation pushbutton
//
void CZoomscrolldemo2Dlg::OnDownRightPB() 
{
    // Scroll down and right by 10%
    handleButton(&m_DownRightPB, 0.1, 0.1);
}

//
// User clicks on the center navigation pushbutton
//
void CZoomscrolldemo2Dlg::OnCenterPB() 
{
    // Center the view port at the origin (0, 0) 
    m_ChartViewer.setViewPortLeft(0.5 - m_ChartViewer.getViewPortWidth() / 2);
    m_ChartViewer.setViewPortTop(0.5 - m_ChartViewer.getViewPortHeight() / 2);

    // Update the chart
    m_ChartViewer.updateViewPort(true, true);
}

//
// User moves the Zoom slider control
//
void CZoomscrolldemo2Dlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
    if ((nSBCode != SB_ENDSCROLL) && (nSBCode != SB_THUMBPOSITION))
    {
        //
        // The slider is moving. Update the chart.
        //

        // Remember the center point
        double centerX = m_ChartViewer.getViewPortLeft() + 
            m_ChartViewer.getViewPortWidth() / 2;
        double centerY = m_ChartViewer.getViewPortTop() + 
            m_ChartViewer.getViewPortHeight() / 2;
            
        // Aspect ratio and zoom factor
        double aspectRatio = m_ChartViewer.getViewPortWidth() / 
            m_ChartViewer.getViewPortHeight();
        double zoomTo = ((double)m_ZoomBar.GetPos()) / m_ZoomBar.GetRangeMax();

        // Zoom by adjusting ViewPortWidth and ViewPortHeight while maintaining the aspect 
        // ratio
        m_ChartViewer.setViewPortWidth(zoomTo * ((aspectRatio < 1) ? 1 : aspectRatio));
        m_ChartViewer.setViewPortHeight(zoomTo * ((aspectRatio > 1) ? 1 : (1 / aspectRatio)));
        
        // Adjust ViewPortLeft and ViewPortTop to keep center point unchanged
        m_ChartViewer.setViewPortLeft(centerX - m_ChartViewer.getViewPortWidth() / 2);
        m_ChartViewer.setViewPortTop(centerY - m_ChartViewer.getViewPortHeight() / 2);
        
        // Update the chart image only, but no need to update the image map.
        m_ChartViewer.updateViewPort(true, false);
    }
    else if (nSBCode == SB_ENDSCROLL)
    {
        // Slider has stoped moving. Can update image map now.
        m_ChartViewer.updateViewPort(false, true);
    }
        
    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

//
// User drags the navigate window
//
void CZoomscrolldemo2Dlg::OnNavigateWindow() 
{
    if (m_NavigateWindow.IsMouseDrag())
    {
        //
        // Get the position of the navigate window inside the navigate pad as a ratio between 
        // 0 - 1. (Note: we allowed for a 2-pixel margin for the frame border in the following
        // computations.)
        //

        CRect windowPos;
        m_NavigateWindow.GetWindowRect(&windowPos);

        CRect containerPos;
        m_NavigatePad.GetWindowRect(&containerPos);

        double viewPortLeft = ((double)windowPos.left + m_NavigateWindow.GetMouseDragPos().x 
            - m_NavigateWindow.GetMouseDownPos().x - containerPos.left - 2) / 
            (containerPos.Width() - 4);
        double viewPortTop = ((double)windowPos.top + m_NavigateWindow.GetMouseDragPos().y
            - m_NavigateWindow.GetMouseDownPos().y - containerPos.top - 2) / 
            (containerPos.Height() - 4);
        
        //
        // Ensures the view port is within valid range.
        //
        if (viewPortLeft < 0)
            viewPortLeft = 0;
        if (viewPortLeft > 1 - m_ChartViewer.getViewPortWidth())
            viewPortLeft = 1 - m_ChartViewer.getViewPortWidth();
        if (viewPortTop < 0)
            viewPortTop = 0;
        if (viewPortTop > 1 - m_ChartViewer.getViewPortHeight())
            viewPortTop = 1 - m_ChartViewer.getViewPortHeight();

        //
        // Update the chart view port
        //
        m_ChartViewer.setViewPortLeft(viewPortLeft);
        m_ChartViewer.setViewPortTop(viewPortTop);

        // Update the chart image only, but no need to update the image map.
        m_ChartViewer.updateViewPort(true, false);
    }
    else if (!m_NavigateWindow.IsMouseDown())
    {
        // User has finished dragging. Can update image map now.
        m_ChartViewer.updateViewPort(false, true);
    }
}

//
// CChartViewer ViewPortChanged event
//
void CZoomscrolldemo2Dlg::OnViewPortChanged()
{
    //
    // Update the Zoom slider to reflect the current zoom level of the view port
    //
    double smallerSide = m_ChartViewer.getViewPortWidth() > m_ChartViewer.getViewPortHeight() 
        ? m_ChartViewer.getViewPortHeight() : m_ChartViewer.getViewPortWidth();
    m_ZoomBar.SetPos((int)(smallerSide * m_ZoomBar.GetRangeMax() + 0.5));

    //
    // Update chart and image map if necessary
    //
    if (m_ChartViewer.needUpdateChart())
        drawChart(&m_ChartViewer);
    if (m_ChartViewer.needUpdateImageMap())
        updateImageMap(&m_ChartViewer);

    //
    // Update the navigate window to reflect the current view port position and size. (Note: 
    // we allowed for a 2-pixel margin for the frame border in the following computations.)
    //
    CRect container;
    m_NavigatePad.GetWindowRect(&container);
    ScreenToClient(&container);
    
    int left = (int)(m_ChartViewer.getViewPortLeft() * (container.Width() - 4) + 2 + 0.5);
    int top = (int)(m_ChartViewer.getViewPortTop() * (container.Height() - 4) + 2 + 0.5);
    int width = (int)(m_ChartViewer.getViewPortWidth() * (container.Width() - 4) + 0.5);
    int height = (int)(m_ChartViewer.getViewPortHeight() * (container.Height() - 4) + 0.5);
    m_NavigateWindow.MoveWindow(left + container.left, container.top + top, width, height);
}

//
// User clicks on the CChartViewer
//
void CZoomscrolldemo2Dlg::OnChartViewer() 
{
    ImageMapHandler *handler = m_ChartViewer.getImageMapHandler();
    if (0 != handler)
    {
        //
        // Query the ImageMapHandler to see if the mouse is on a clickable hot spot. We 
        // consider the hot spot as clickable if its href ("path") parameter is not empty.
        //
        const char *path = handler->getValue("path");
        if ((0 != path) && (0 != *path))
        {
            // In this sample code, we just show all hot spot parameters.
            CHotSpotDlg hs;
            hs.SetData(handler);
            hs.DoModal();
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
// CZoomscrolldemo2Dlg methods

//
// Scroll the chart when the user presses the navigate button
//
void CZoomscrolldemo2Dlg::handleButton(CRepeatButton *b, double deltaX, double deltaY)
{
    if (b->isPressed())
    {
        //
        // User is still pressing the button.
        //
        
        // Scroll the view port by the specified ratio
        double vpLeft = m_ChartViewer.getViewPortLeft() + deltaX * 
            m_ChartViewer.getViewPortWidth();
        double vpTop = m_ChartViewer.getViewPortTop() + deltaY * 
            m_ChartViewer.getViewPortHeight();

        //
        // Ensures the view port is within valid range.
        //
        if (vpLeft < 0)
            vpLeft = 0;
        if (vpLeft > 1 - m_ChartViewer.getViewPortWidth())
            vpLeft = 1 - m_ChartViewer.getViewPortWidth();
        if (vpTop < 0)
            vpTop = 0;
        if (vpTop > 1 - m_ChartViewer.getViewPortHeight())
            vpTop = 1 - m_ChartViewer.getViewPortHeight();

        //
        // Update chart only if the view port has actually changed
        //
        if ((vpLeft != m_ChartViewer.getViewPortLeft()) || 
            (vpTop != m_ChartViewer.getViewPortTop()))
        {
            m_ChartViewer.setViewPortLeft(vpLeft);
            m_ChartViewer.setViewPortTop(vpTop);

            // Update the chart image only, but no need to update the image map.
            m_ChartViewer.updateViewPort(true, false);
        }
    }
    else
    {
        // User has finished pressing the button. Can update image map now.
        m_ChartViewer.updateViewPort(false, true);
    }
}

//
// Draw the chart and display it in the given viewer
//
void CZoomscrolldemo2Dlg::drawChart(CChartViewer *viewer)
{
    //
    // For simplicity, in this demo, we just use hard coded data. In a real application,
    // the data probably read from a dynamic source such as a database. (See the
    // ChartDirector documentation on "Using Data Sources with ChartDirector" if you need
    // some sample code on how to read data from database to array variables.)
    //
    double dataX0[] = {10, 15, 6, -12, 14, -8, 13, -3, 16, 12, 10.5, -7, 3, -10, -5, 2, 5};
    double dataY0[] = {130, 150, 80, 110, -110, -105, -130, -15, -170, 125,  125, 60, 25, 150,
        150,15, 120};
    double dataX1[] = {6, 7, -4, 3.5, 7, 8, -9, -10, -12, 11, 8, -3, -2, 8, 4, -15, 15};
    double dataY1[] = {65, -40, -40, 45, -70, -80, 80, 10, -100, 105, 60, 50, 20, 170, -25, 
        50, 75};
    double dataX2[] = {-10, -12, 11, 8, 6, 12, -4, 3.5, 7, 8, -9, 3, -13, 16, -7.5, -10, -15};
    double dataY2[] = {65, -80, -40, 45, -70, -80, 80, 90, -100, 105, 60, -75, -150, -40, 120,
        -50, -30};
                
    // Create an XYChart object 500 x 480 pixels in size, with the same background color
    // as the container
    XYChart *c = new XYChart(500, 480, m_extBgColor);

    // Set the plotarea at (50, 40) and of size 400 x 400 pixels. Use light grey (c0c0c0)
    // horizontal and vertical grid lines. Set 4 quadrant coloring, where the colors of 
    // the quadrants alternate between lighter and deeper grey (dddddd/eeeeee). 
    c->setPlotArea(50, 40, 400, 400, -1, -1, -1, 0xc0c0c0, 0xc0c0c0
        )->set4QBgColor(0xdddddd, 0xeeeeee, 0xdddddd, 0xeeeeee, 0x000000);

    // Enable clipping mode to clip the part of the data that is outside the plot area.
    c->setClipping();

    // Set 4 quadrant mode, with both x and y axes symetrical around the origin
    c->setAxisAtOrigin(Chart::XYAxisAtOrigin, Chart::XAxisSymmetric + Chart::YAxisSymmetric);

    // Add a legend box at (450, 40) (top right corner of the chart) with vertical layout
    // and 8 pts Arial Bold font. Set the background color to semi-transparent grey.
    LegendBox *legendBox = c->addLegend(450, 40, true, "arialbd.ttf", 8);
    legendBox->setAlignment(Chart::TopRight);
    legendBox->setBackground(0x40dddddd);

    // Add a titles to axes
    c->xAxis()->setTitle("Alpha Index");
    c->yAxis()->setTitle("Beta Index");

    // Set axes width to 2 pixels
    c->xAxis()->setWidth(2);
    c->yAxis()->setWidth(2);

    // The default ChartDirector settings has a denser y-axis grid spacing and less-dense
    // x-axis grid spacing. In this demo, we want the tick spacing to be symmetrical.
    // We use around 50 pixels between major ticks and 25 pixels between minor ticks.
    c->xAxis()->setTickDensity(50, 25);
    c->yAxis()->setTickDensity(50, 25);

    //
    // In this example, we represent the data by scatter points. If you want to represent
    // the data by somethings else (lines, bars, areas, floating boxes, etc), just modify
    // the code below to use the layer type of your choice. 
    //

    // Add scatter layer, using 11 pixels red (ff33333) X shape symbols
    c->addScatterLayer(DoubleArray(dataX0, sizeof(dataX0) / sizeof(dataX0[0])), 
        DoubleArray(dataY0, sizeof(dataY0) / sizeof(dataY0[0])), "Group A", 
        Chart::Cross2Shape(), 11, 0xff3333);

    // Add scatter layer, using 11 pixels green (33ff33) circle symbols
    c->addScatterLayer(DoubleArray(dataX1, sizeof(dataX1) / sizeof(dataX1[0])),
        DoubleArray(dataY1, sizeof(dataY1) / sizeof(dataY1[0])), 
        "Group B", Chart::CircleShape, 11,  0x33ff33);

    // Add scatter layer, using 11 pixels blue (3333ff) triangle symbols
    c->addScatterLayer(DoubleArray(dataX2, sizeof(dataX2) / sizeof(dataX2[0])),
        DoubleArray(dataY2, sizeof(dataY2) / sizeof(dataY2[0])), 
        "Group C", Chart::TriangleSymbol, 11, 0x3333ff);

    if (m_maxX == m_minX)
    {
        // The axis scale has not yet been set up. So this is the first time the chart is
        // drawn and it is drawn with no zooming. We can use auto-scaling to determine the
        // axis-scales, then remember them for future use. 

        // Explicitly auto-scale axes so we can get the axis scales
        c->layout();

        // Save the axis scales for future use
        m_minX = c->xAxis()->getMinValue();
        m_maxX = c->xAxis()->getMaxValue();
        m_minY = c->yAxis()->getMinValue();
        m_maxY = c->yAxis()->getMaxValue();
    }
    else
    {
        // Compute the zoomed-in axis scales using the overall axis scales and ViewPort size
        double xScaleMin = m_minX + (m_maxX - m_minX) * viewer->getViewPortLeft();
        double xScaleMax = m_minX + (m_maxX - m_minX) * (viewer->getViewPortLeft() + 
            viewer->getViewPortWidth());
        double yScaleMin = m_maxY - (m_maxY - m_minY) * (viewer->getViewPortTop() + 
            viewer->getViewPortHeight());
        double yScaleMax = m_maxY - (m_maxY - m_minY) * viewer->getViewPortTop();
        // *** use the following formula if you are using a log scale axis ***
        // double xScaleMin = minX * pow(maxX / minX, viewer->getViewPortLeft());
        // double xScaleMax = minX * pow(maxX / minX, viewer->getViewPortLeft() + 
        //     viewer->getViewPortWidth());
        // double yScaleMin = maxY * pow(minY / maxY, viewer->getViewPortTop() + 
        //    viewer->getViewPortHeight);
        // double yScaleMax = maxY * pow(minY / maxY, viewer->getViewPortTop());

        // Set the axis scales
        c->xAxis()->setLinearScale(xScaleMin, xScaleMax);
        c->xAxis()->setRounding(false, false);
        c->yAxis()->setLinearScale(yScaleMin, yScaleMax);
        c->yAxis()->setRounding(false, false);
    }
    
    // Set the chart image to the WinChartViewer
    delete m_ChartViewer.getChart();
    m_ChartViewer.setChart(c);
}

//
// Update the image map
//
void CZoomscrolldemo2Dlg::updateImageMap(CChartViewer *viewer)
{
    if (0 == viewer->getImageMapHandler())
    {
        // no existing image map - creates a new one
        viewer->setImageMap(viewer->getChart()->getHTMLImageMap("clickable", "",
                "title='[{dataSetName}] Alpha = {x}, Beta = {value}'"));
    }
}

/////////////////////////////////////////////////////////////////////////////
// General utilities

//
// Get the default background color
//
int CZoomscrolldemo2Dlg::getDefaultBgColor()
{
    LOGBRUSH LogBrush; 
    HBRUSH hBrush = (HBRUSH)SendMessage(WM_CTLCOLORDLG, (WPARAM)CClientDC(this).m_hDC, 
        (LPARAM)m_hWnd); 
    ::GetObject(hBrush, sizeof(LOGBRUSH), &LogBrush); 
    int ret = LogBrush.lbColor;
    return ((ret & 0xff) << 16) | (ret & 0xff00) | ((ret & 0xff0000) >> 16);
}

//
// Load an icon resource into a button
//
void CZoomscrolldemo2Dlg::loadButtonIcon(int buttonId, int iconId, int width, int height)
{
    GetDlgItem(buttonId)->SendMessage(BM_SETIMAGE, IMAGE_ICON, (LPARAM)::LoadImage(
        AfxGetResourceHandle(), MAKEINTRESOURCE(iconId), IMAGE_ICON, width, height, 
        LR_DEFAULTCOLOR));  
}
