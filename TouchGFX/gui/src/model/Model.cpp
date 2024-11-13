#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include "cmsis_os.h"
#include "FreeRTOS.h"

extern osMessageQueueId_t CounterQueueHandle;

Model::Model() : modelListener(0)
{

}

void Model::tick()
{

}

void Model::AppUpdateCounterValue(int value)
{
  osMessageQueuePut(CounterQueueHandle, &value, 0, 0);
}
