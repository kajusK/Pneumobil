//init uart, init can
//

#include "modules/log.h"
#include "modules/comm/can_transport.h"
#include "modules/comm/uart_transport.h"
#include "modules/comm/session.h"

void Commi_SendLog(log_msg_t *msg)
{

}

void Comm_Init(void)
{
    log_severity_t severity[LOG_SOURCE_COUNT];
    //TODO fill severity

    Comm_SessionInit();
    Comm_CanInit();
    Comm_UartInit();
    Log_Subscribe(Commi_SendLog, severity);
}
