#include <gui/screen_screen/screenView.hpp>

const uint32_t UpperLimit = 1024;
const uint32_t LowerLimit = 0;

screenView::screenView() : 
    count(0)
{

}

void screenView::setupScreen()
{
    screenViewBase::setupScreen();
}

void screenView::tearDownScreen()
{
    screenViewBase::tearDownScreen();
}

void screenView::PressedUpButton()
{
    count = (count++ > UpperLimit) ? UpperLimit : count;
    Unicode::snprintf(CounterTextBuffer, COUNTERTEXT_SIZE, "%d", count);
    CounterText.invalidate();
}

void screenView::PressedDownButton()
{
    count = (count-- <= LowerLimit) ? LowerLimit : count;
    Unicode::snprintf(CounterTextBuffer, COUNTERTEXT_SIZE, "%d", count);
    CounterText.invalidate();
}