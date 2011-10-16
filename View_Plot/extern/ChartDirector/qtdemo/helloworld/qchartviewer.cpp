#include "qchartviewer.h"
#include <QMouseEvent>
#include <QToolTip>
#include <QCursor>
#include <QBitmap>

/////////////////////////////////////////////////////////////////////////////
// QChartViewer

//
// Build in mouse cursors for zooming and scrolling support
//
static QCursor &getZoomInCursor();
static QCursor &getZoomOutCursor();
static QCursor &getNoZoomCursor();
static QCursor &getNoMove2DCursor();
static QCursor &getNoMoveHorizCursor();
static QCursor &getNoMoveVertCursor();

QChartViewer::QChartViewer(QWidget *parent) :
    QLabel(parent)
{
    // current chart and hot spot tester
    m_currentChart = 0;
    m_hotSpotTester = 0;

    // initialize chart configuration
    m_selectBoxLineColor = QColor(0, 0, 0);
    m_selectBoxLineWidth = 2;
    m_mouseUsage = Chart::MouseUsageDefault;
    m_zoomDirection = Chart::DirectionHorizontal;
    m_zoomInRatio = 2;
    m_zoomOutRatio = 0.5;
    m_scrollDirection = Chart::DirectionHorizontal;
    m_minDragAmount = 5;
    m_updateInterval = 20;

    // current state of the mouse
    m_isOnPlotArea = false;
    m_isPlotAreaMouseDown = false;
    m_isDragScrolling = false;
    m_currentHotSpot = -1;
    m_isClickable = false;

    // chart update rate support
    m_needUpdateChart = false;
    m_needUpdateImageMap = false;
    m_holdTimerActive = false;
    m_isInViewPortChanged = false;

    // selection rectangle
    m_LeftLine = 0;
    m_RightLine = 0;
    m_TopLine = 0;
    m_BottomLine = 0;

    setMouseTracking(true);
}

QChartViewer::~QChartViewer()
{
    delete m_hotSpotTester;
}

/////////////////////////////////////////////////////////////////////////////
// QChartViewer message handlers

//
// MouseMove event handler
//
void QChartViewer::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMove(event);

    //
    // Call OnPlotAreaMouseDrag if mouse is dragging inside the plot area
    //

    m_isOnPlotArea = m_isPlotAreaMouseDown || inPlotArea(event->x(), event->y());
    if (m_isPlotAreaMouseDown)
        onPlotAreaMouseDrag(event);

    //
    // Show hot spot tool tips if necessary
    //

    if (event->buttons() != Qt::NoButton)
    {
        // Hide tool tips if mouse button is pressed.
        QToolTip::hideText();
    }
    else
    {
        // Use the ChartDirector ImageMapHandler to determine if the mouse is over a hot spot
        int hotSpotNo = 0;
        if (0 != m_hotSpotTester)
            hotSpotNo = m_hotSpotTester->getHotSpot(event->x(), event->y());

        // If the mouse is in the same hot spot since the last mouse move event, there is no need
        // to update the tool tip.
        if (hotSpotNo != m_currentHotSpot)
        {
            // Hot spot has changed - update tool tip text
            m_currentHotSpot = hotSpotNo;

            if (hotSpotNo == 0)
            {
                // Mouse is not actually on hanlder hot spot - use default tool tip text and reset
                // the clickable flag.
                QToolTip::showText(event->globalPos(), m_defaultToolTip);
                m_isClickable = false;
            }
            else
            {
                // Mouse is on a hot spot. In this implementation, we consider the hot spot as
                // clickable if its href ("path") parameter is not empty.
                const char *path = m_hotSpotTester->getValue("path");
                m_isClickable = ((0 != path) && (0 != *path));

                // Use the title attribute as the tool tip.
                QToolTip::showText(event->globalPos(), QString::fromUtf8(m_hotSpotTester->getValue("title")));
            }
        }
    }

    onSetCursor();
}

//
// Change the mouse cursor.
//
void QChartViewer::onSetCursor()
{
    if (this->m_isDragScrolling)
    {
        switch (m_scrollDirection)
        {
        case Chart::DirectionHorizontal:
            setCursor(getNoMoveHorizCursor());
            break;
        case Chart::DirectionVertical:
            setCursor(getNoMoveVertCursor());
            break;
        default :
            setCursor(getNoMove2DCursor());
            break;
        }

        return;
    }

    if (m_isOnPlotArea)
    {
        switch (m_mouseUsage)
        {
        case Chart::MouseUsageZoomIn:
            if (canZoomIn(m_zoomDirection))
                setCursor(getZoomInCursor());
            else
                setCursor(getNoZoomCursor());
            return;
        case Chart::MouseUsageZoomOut:
            if (canZoomOut(m_zoomDirection))
                setCursor(getZoomOutCursor());
            else
                setCursor(getNoZoomCursor());
            return;
        }
    }

    if (m_isClickable)
        setCursor(Qt::PointingHandCursor);
    else
        unsetCursor();
}

//
// Mouse left button down event.
//

void QChartViewer::mousePressEvent(QMouseEvent *event)
{
    if ((event->button() == Qt::LeftButton) && inPlotArea(event->x(), event->y()) &&
        (m_mouseUsage != Chart::MouseUsageDefault))
    {
        // Mouse usage is for drag to zoom/scroll. Capture the mouse to prepare for dragging and
        // save the mouse down position to draw the selection rectangle.
        m_isPlotAreaMouseDown = true;
        m_plotAreaMouseDownXPos = event->x();
        m_plotAreaMouseDownYPos = event->y();
        startDrag();
    }
}

//
// Mouse left button up event.
//
void QChartViewer::mouseReleaseEvent(QMouseEvent *event)
{
    if ((event->button() == Qt::LeftButton) && m_isPlotAreaMouseDown)
    {
        // Release the mouse capture.
        m_isPlotAreaMouseDown = false;
        setRectVisible(false);
        bool hasUpdate = false;

        switch (m_mouseUsage)
        {
        case Chart::MouseUsageZoomIn :
            if (canZoomIn(m_zoomDirection))
            {
                if (isDrag(m_zoomDirection, event))
                    // Zoom to the drag selection rectangle.
                    hasUpdate = zoomTo(m_zoomDirection,
                        m_plotAreaMouseDownXPos, m_plotAreaMouseDownYPos, event->x(), event->y());
                else
                    // User just click on a point. Zoom-in around the mouse cursor position.
                    hasUpdate = zoomAt(m_zoomDirection, event->x(), event->y(), m_zoomInRatio);
            }
            break;
        case Chart::MouseUsageZoomOut:
            // Zoom out around the mouse cursor position.
            if (canZoomOut(m_zoomDirection))
                hasUpdate = zoomAt(m_zoomDirection, event->x(), event->y(), m_zoomOutRatio);
            break;
        default :
            if (m_isDragScrolling)
                // Drag to scroll. We can update the image map now as scrolling has finished.
                updateViewPort(false, true);
            else
                // Is not zooming or scrolling, so is just a normal click event.
                emit clicked(event);
            break;
        }

        m_isDragScrolling = false;
        if (hasUpdate)
            // View port has changed - update it.
            updateViewPort(true, true);
    }
    else
        emit clicked(event);

    onSetCursor();
}

//
// Chart hold timer.
//
void QChartViewer::timerEvent(QTimerEvent *event)
{
    // Reset the timer
    if (m_holdTimerActive && (event->timerId() == m_holdTimerId))
    {
        killTimer(m_holdTimerId);
        m_holdTimerActive = false;

        // If has pending chart update request, handles them now.
        if (m_needUpdateChart || m_needUpdateImageMap)
            updateViewPort(m_needUpdateChart, m_needUpdateImageMap);
    }
}

//
// Set the chart to the control
//
void QChartViewer::setChart(BaseChart *c)
{
    // Get the image and metrics for the chart
    QPixmap image;
    const char *chartMetrics = 0;

    if (0 != c)
    {
        // Output chart as Device Indpendent Bitmap with file headers
        MemBlock m = c->makeChart(Chart::BMP);
        image.loadFromData((uchar *)m.data, (uint)m.len);
        setFixedSize(c->getWidth(), c->getHeight());

        // Get chart metrics
        chartMetrics = c->getChartMetrics();
    }

    // Set the HBITMAP for display
    m_currentChart = c;
    setPixmap(image);

    // Set the chart metrics and clear the image map
    setChartMetrics(chartMetrics);
    setImageMap(0);
}

/////////////////////////////////////////////////////////////////////////////
// QChartViewer properties

//
// Get back the same BaseChart pointer provided by the previous setChart call.
//
BaseChart *QChartViewer::getChart()
{
    return m_currentChart;
}

//
// Set image map used by the chart
//
void QChartViewer::setImageMap(const char *imageMap)
{
    //delete the existing ImageMapHandler
    if (0 != m_hotSpotTester)
        delete m_hotSpotTester;
    m_currentHotSpot = -1;
    m_isClickable = false;

    //create a new ImageMapHandler to represent the image map
    if ((0 == imageMap) || (0 == *imageMap))
        m_hotSpotTester = 0;
    else
        m_hotSpotTester = new ImageMapHandler(imageMap);
}

//
// Get the image map handler for the chart
//
ImageMapHandler *QChartViewer::getImageMapHandler()
{
    return m_hotSpotTester;
}

//
// Set the default tool tip to use
//
void QChartViewer::setDefaultToolTip(QString text)
{
    m_defaultToolTip = text;
}

//
// Set the border width of the selection box
//
void QChartViewer::setSelectionBorderWidth(int width)
{
    m_selectBoxLineWidth = width;
}

//
// Get the border with of the selection box.
//
int QChartViewer::getSelectionBorderWidth()
{
    return m_selectBoxLineWidth;
}

//
// Set the border color of the selection box
//
void QChartViewer::setSelectionBorderColor(QColor c)
{
    m_selectBoxLineColor = c;
    if (m_TopLine != 0)
    {
        QPalette p(c, c);
        m_TopLine->setPalette(p);
        m_LeftLine->setPalette(p);
        m_BottomLine->setPalette(p);
        m_RightLine->setPalette(p);
    }
}

//
// Get the border color of the selection box.
//
QColor QChartViewer::getSelectionBorderColor()
{
    return m_selectBoxLineColor;
}

//
// Set the mouse usage mode
//
void QChartViewer::setMouseUsage(int mouseUsage)
{
    m_mouseUsage = mouseUsage;
}

//
// Get the mouse usage mode
//
int QChartViewer::getMouseUsage()
{
    return m_mouseUsage;
}

//
// Set the zoom direction
//
void QChartViewer::setZoomDirection(int direction)
{
    m_zoomDirection = direction;
}

//
// Get the zoom direction
//
int QChartViewer::getZoomDirection()
{
    return m_zoomDirection;
}

//
// Set the scroll direction
//
void QChartViewer::setScrollDirection(int direction)
{
    m_scrollDirection = direction;
}

//
// Get the scroll direction
//
int QChartViewer::getScrollDirection()
{
    return m_scrollDirection;
}

//
// Set the zoom-in ratio for mouse click zoom-in
//
void QChartViewer::setZoomInRatio(double ratio)
{
    m_zoomInRatio = ratio;
}

//
// Get the zoom-in ratio for mouse click zoom-in
//
double QChartViewer::getZoomInRatio()
{
    return m_zoomInRatio;
}

//
// Set the zoom-out ratio
//
void QChartViewer::setZoomOutRatio(double ratio)
{
    m_zoomOutRatio = ratio;
}

//
// Get the zoom-out ratio
//
double QChartViewer::getZoomOutRatio()
{
    return m_zoomOutRatio;
}

//
// Set the minimum mouse drag before the dragging is considered as real. This is to avoid small
// mouse vibrations triggering a mouse drag.
//
void QChartViewer::setMinimumDrag(int offset)
{
    m_minDragAmount = offset;
}

//
// Get the minimum mouse drag before the dragging is considered as real.
//
int QChartViewer::getMinimumDrag()
{
    return m_minDragAmount;
}

//
// Set the minimum interval between ViewPortChanged events. This is to avoid the chart being
// updated too frequently. (Default is 20ms between chart updates.) Multiple update events
// arrived during the interval will be merged into one chart update and executed at the end
// of the interval.
//
void QChartViewer::setUpdateInterval(int interval)
{
    m_updateInterval = interval;
}

//
// Get the minimum interval between ViewPortChanged events.
//
int QChartViewer::getUpdateInterval()
{
    return m_updateInterval;
}

//
// Check if there is a pending chart update request.
//
bool QChartViewer::needUpdateChart()
{
    return m_needUpdateChart;
}

//
// Check if there is a pending image map update request.
//
bool QChartViewer::needUpdateImageMap()
{
    return m_needUpdateImageMap;
}

/////////////////////////////////////////////////////////////////////////////
// QChartViewer methods

//
// Create the edges for the selection rectangle
//
void QChartViewer::initRect()
{
    m_LeftLine = new QLabel(this);
    m_LeftLine->setAutoFillBackground(true);
    m_RightLine = new QLabel(this);
    m_RightLine->setAutoFillBackground(true);
    m_TopLine = new QLabel(this);
    m_TopLine->setAutoFillBackground(true);
    m_BottomLine = new QLabel(this);
    m_BottomLine->setAutoFillBackground(true);

    setSelectionBorderColor(getSelectionBorderColor());
}

//
// Set selection rectangle position and size
//
void QChartViewer::drawRect(int x, int y, int width, int height)
{
    // Create the edges of the rectangle if not already created
    if (m_TopLine == 0)
        initRect();

    // width < 0 is interpreted as the rectangle extends to the left or x.
    // height <0 is interpreted as the rectangle extends to above y.
    if (width < 0)
        x -= (width = -width);
    if (height < 0)
        y -= (height = -height);

    // Put the edges along the sides of the rectangle
    m_TopLine->move(x, y);
    m_TopLine->setFixedSize(width, m_selectBoxLineWidth);
    m_LeftLine->move(x, y);
    m_LeftLine->setFixedSize(m_selectBoxLineWidth, height);
    m_BottomLine->move(x, y + height - m_selectBoxLineWidth + 1);
    m_BottomLine->setFixedSize(width, m_selectBoxLineWidth);
    m_RightLine->move(x + width - m_selectBoxLineWidth + 1, y);
    m_RightLine->setFixedSize(m_selectBoxLineWidth, height);
}

//
// Show/hide selection rectangle
//
void QChartViewer::setRectVisible(bool b)
{
    // Create the edges of the rectangle if not already created
    if (b && (m_TopLine == 0))
        initRect();

    // Show/hide the edges
    if (m_TopLine != 0)
    {
        m_TopLine->setVisible(b);
        m_LeftLine->setVisible(b);
        m_BottomLine->setVisible(b);
        m_RightLine->setVisible(b);
    }
}

//
// Determines if the mouse is dragging.
//
bool QChartViewer::isDrag(int direction, QMouseEvent *event)
{
    // We only consider the mouse is dragging it is has dragged more than m_minDragAmount. This is
    // to avoid small mouse vibrations triggering a mouse drag.
    int spanX = abs(event->x() - m_plotAreaMouseDownXPos);
    int spanY = abs(event->y() - m_plotAreaMouseDownYPos);
    return ((direction != Chart::DirectionVertical) && (spanX >= m_minDragAmount)) ||
        ((direction != Chart::DirectionHorizontal) && (spanY >= m_minDragAmount));
}

//
// Process mouse dragging over the plot area
//
void QChartViewer::onPlotAreaMouseDrag(QMouseEvent *event)
{
    switch (m_mouseUsage)
    {
        case Chart::MouseUsageZoomIn :
        {
            //
            // Mouse is used for zoom in. Draw the selection rectangle if necessary.
            //

            bool isDragZoom = canZoomIn(m_zoomDirection) && isDrag(m_zoomDirection, event);
            if (isDragZoom)
            {
                int spanX = m_plotAreaMouseDownXPos - event->x();
                int spanY = m_plotAreaMouseDownYPos - event->y();

                switch (m_zoomDirection)
                {
                case Chart::DirectionHorizontal:
                    drawRect(event->x(), getPlotAreaTop(), spanX, getPlotAreaHeight());
                    break;
                case Chart::DirectionVertical:
                    drawRect(getPlotAreaLeft(), event->y(), getPlotAreaWidth(), spanY);
                    break;
                default:
                    drawRect(event->x(), event->y(), spanX, spanY);
                    break;
                }
            }
            setRectVisible(isDragZoom);
            break;
        }
        case Chart::MouseUsageScroll :
        {
            //
            // Mouse is used for drag scrolling. Scroll and update the view port.
            //

            if (m_isDragScrolling || isDrag(m_scrollDirection, event))
            {
                m_isDragScrolling = true;
                if (dragTo(m_scrollDirection,
                    event->x() - m_plotAreaMouseDownXPos, event->y() - m_plotAreaMouseDownYPos))
                    updateViewPort(true, false);
            }
        }
    }
}

//
// Trigger the ViewPortChanged event
//
void QChartViewer::updateViewPort(bool needUpdateChart, bool needUpdateImageMap)
{
    // Already updating, no need to update again
    if (m_isInViewPortChanged)
        return;

    // Merge the current update requests with any pending requests.
    m_needUpdateChart = m_needUpdateChart || needUpdateChart;
    m_needUpdateImageMap = needUpdateImageMap;

    // Hold timer has not expired, so do not update chart immediately. Keep the requests pending.
    if (m_holdTimerActive)
        return;

    // Can update chart now. Trigger the ViewPortChanged event.
    validateViewPort();
    m_isInViewPortChanged = true;
    emit viewPortChanged();
    m_isInViewPortChanged = false;

    // Clear any pending updates.
    m_needUpdateChart = false;
    m_needUpdateImageMap = false;

    // Set hold timer to prevent multiple chart updates within a short period.
    if (m_updateInterval > 0)
    {
        m_holdTimerActive = true;
        m_holdTimerId = startTimer(m_updateInterval);
    }
}

/////////////////////////////////////////////////////////////////////////////
// Build in mouse cursors for zooming and scrolling support
//

static const int zoomInCursorA[] =
{
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xff3ff8ff,
0xff0fe0ff,
0xff07c0ff,
0xff07c0ff,
0xff0380ff,
0xff0380ff,
0xff0380ff,
0xff0380ff,
0xff0380ff,
0xff07c0ff,
0xff07c0ff,
0xff01e0ff,
0xff30f8ff,
0x7ff0ffff,
0x3ff8ffff,
0x1ffcffff,
0x0ffeffff,
0x0fffffff,
0x9fffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff
};

static const int zoomInCursorB[] =
{
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00c00700,
0x00f01f00,
0x00f01e00,
0x00f83e00,
0x00f83e00,
0x00183000,
0x00f83e00,
0x00f83e00,
0x00f01e00,
0x00f01f00,
0x00c00700,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000
};

static const int zoomOutCursorA[] =
{
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xff3ff8ff,
0xff0fe0ff,
0xff07c0ff,
0xff07c0ff,
0xff0380ff,
0xff0380ff,
0xff0380ff,
0xff0380ff,
0xff0380ff,
0xff07c0ff,
0xff07c0ff,
0xff01e0ff,
0xff30f8ff,
0x7ff0ffff,
0x3ff8ffff,
0x1ffcffff,
0x0ffeffff,
0x0fffffff,
0x9fffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff
};

static const int zoomOutCursorB[] =
{
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00c00700,
0x00f01f00,
0x00f01f00,
0x00f83f00,
0x00f83f00,
0x00183000,
0x00f83f00,
0x00f83f00,
0x00f01f00,
0x00f01f00,
0x00c00700,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000
};

static const int noZoomCursorA[] =
{
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xff3ff8ff,
0xff0fe0ff,
0xff07c0ff,
0xff07c0ff,
0xff0380ff,
0xff0380ff,
0xff0380ff,
0xff0380ff,
0xff0380ff,
0xff07c0ff,
0xff07c0ff,
0xff01e0ff,
0xff30f8ff,
0x7ff0ffff,
0x3ff8ffff,
0x1ffcffff,
0x0ffeffff,
0x0fffffff,
0x9fffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff
};

static const int noZoomCursorB[] =
{
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00c00700,
0x00f01f00,
0x00f01f00,
0x00f83f00,
0x00f83f00,
0x00f83f00,
0x00f83f00,
0x00f83f00,
0x00f01f00,
0x00f01f00,
0x00c00700,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000
};

static int noMove2DCursorA[] =
{
0xffffffff,
0xffffffff,
0xffffffff,
0xff7ffeff,
0xff3ffcff,
0xff1ff8ff,
0xff0ff0ff,
0xff07e0ff,
0xff03c0ff,
0xff03c0ff,
0xfffc3fff,
0x7ffc3ffe,
0x3ffc3ffc,
0x1f7c3ef8,
0x0f3c3cf0,
0x071c38e0,
0x071c38e0,
0x0f3c3cf0,
0x1f7c3ef8,
0x3ffc3ffc,
0x7ffc3ffe,
0xfffc3fff,
0xff03c0ff,
0xff03c0ff,
0xff07e0ff,
0xff0ff0ff,
0xff1ff8ff,
0xff3ffcff,
0xff7ffeff,
0xffffffff,
0xffffffff,
0xffffffff
};

static int noMove2DCursorB[] =
{
0x00000000,
0x00000000,
0x00000000,
0x00800100,
0x00400200,
0x00200400,
0x00100800,
0x00081000,
0x00042000,
0x00fc3f00,
0x0003c000,
0x80024001,
0x40024002,
0x20824104,
0x10424208,
0x08224410,
0x08224410,
0x10424208,
0x20824104,
0x40024002,
0x80024001,
0x0003c000,
0x00fc3f00,
0x00042000,
0x00081000,
0x00100800,
0x00200400,
0x00400200,
0x00800100,
0x00000000,
0x00000000,
0x00000000
};

static int noMoveHorizCursorA[] =
{
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xfffc3fff,
0x7ffc3ffe,
0x3ffc3ffc,
0x1f7c3ef8,
0x0f3c3cf0,
0x071c38e0,
0x071c38e0,
0x0f3c3cf0,
0x1f7c3ef8,
0x3ffc3ffc,
0x7ffc3ffe,
0xfffc3fff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff,
0xffffffff
};

static int noMoveHorizCursorB[] =
{
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x0003c000,
0x80024001,
0x40024002,
0x20824104,
0x10424208,
0x08224410,
0x08224410,
0x10424208,
0x20824104,
0x40024002,
0x80024001,
0x0003c000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000
};

static int noMoveVertCursorA[] =
{
0xffffffff,
0xffffffff,
0xffffffff,
0xff7ffeff,
0xff3ffcff,
0xff1ff8ff,
0xff0ff0ff,
0xff07e0ff,
0xff03c0ff,
0xff03c0ff,
0xffffffff,
0xffffffff,
0xffffffff,
0xff7ffeff,
0xff3ffcff,
0xff1ff8ff,
0xff1ff8ff,
0xff3ffcff,
0xff7ffeff,
0xffffffff,
0xffffffff,
0xffffffff,
0xff03c0ff,
0xff03c0ff,
0xff07e0ff,
0xff0ff0ff,
0xff1ff8ff,
0xff3ffcff,
0xff7ffeff,
0xffffffff,
0xffffffff,
0xffffffff
};

static int noMoveVertCursorB[] =
{
0x00000000,
0x00000000,
0x00000000,
0x00800100,
0x00400200,
0x00200400,
0x00100800,
0x00081000,
0x00042000,
0x00fc3f00,
0x00000000,
0x00000000,
0x00000000,
0x00800100,
0x00400200,
0x00200400,
0x00200400,
0x00400200,
0x00800100,
0x00000000,
0x00000000,
0x00000000,
0x00fc3f00,
0x00042000,
0x00081000,
0x00100800,
0x00200400,
0x00400200,
0x00800100,
0x00000000,
0x00000000,
0x00000000
};

static QCursor *hZoomInCursor = 0;
static QCursor *hZoomOutCursor = 0;
static QCursor *hNoZoomCursor = 0;
static QCursor *hNoMove2DCursor = 0;
static QCursor *hNoMoveHorizCursor = 0;
static QCursor *hNoMoveVertCursor = 0;

class FreeCursors
{
public:
    ~FreeCursors()
    {
        delete hZoomInCursor;
        delete hZoomOutCursor;
        delete hNoZoomCursor;
        delete hNoMove2DCursor;
        delete hNoMoveHorizCursor;
        delete hNoMoveVertCursor;
    }
} dummyFreeCursorObj;

static QCursor *createCursor(const int *andPlane, const int *orPlane, int hotX, int hotY)
{
    // The cursor bitmaps are in and/or plane formats used in the Win32 CreateCursor
    // function. We need to change it to the bitmap/mask format used by QT.
    int buffer[32];
    for (int i = 0; i < 32; ++i)
        buffer[i] = ~(orPlane[i] + andPlane[i]);
    QBitmap bitmap = QBitmap::fromData(QSize(32, 32), (uchar *)buffer, QImage::Format_Mono);
    for (int i = 0; i < 32; ++i)
        buffer[i] = orPlane[i] | ~andPlane[i];
    QBitmap mask = QBitmap::fromData(QSize(32, 32), (uchar *)buffer, QImage::Format_Mono);

    return new QCursor(bitmap, mask, hotX, hotY);
}

static QCursor &getZoomInCursor()
{
    if (0 == hZoomInCursor)
        hZoomInCursor = createCursor(zoomInCursorA, zoomInCursorB, 15, 15);
    return *hZoomInCursor;
}

static QCursor &getZoomOutCursor()
{
    if (0 == hZoomOutCursor)
        hZoomOutCursor = createCursor(zoomOutCursorA, zoomOutCursorB, 15, 15);
    return *hZoomOutCursor;
}

static QCursor &getNoZoomCursor()
{
    if (0 == hNoZoomCursor)
        hNoZoomCursor = createCursor(noZoomCursorA, noZoomCursorB, 15, 15);
    return *hNoZoomCursor;
}

static QCursor &getNoMove2DCursor()
{
    if (0 == hNoMove2DCursor)
        hNoMove2DCursor = createCursor(noMove2DCursorA, noMove2DCursorB, 15, 15);
    return *hNoMove2DCursor;
}

static QCursor &getNoMoveHorizCursor()
{
    if (0 == hNoMoveHorizCursor)
        hNoMoveHorizCursor = createCursor(noMoveHorizCursorA, noMoveHorizCursorB, 15, 15);
    return *hNoMoveHorizCursor;
}

static QCursor &getNoMoveVertCursor()
{
    if (0 == hNoMoveVertCursor)
        hNoMoveVertCursor = createCursor(noMoveVertCursorA, noMoveVertCursorB, 15, 15);
    return *hNoMoveVertCursor;
}
