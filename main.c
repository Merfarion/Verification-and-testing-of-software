

 /*
	FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
	All rights reserved

	VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

	This file is part of the FreeRTOS distribution.

	FreeRTOS is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License (version 2) as published by the
	Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

	***************************************************************************
	>>!   NOTE: The modification to the GPL is included to allow you to     !<<
	>>!   distribute a combined work that includes FreeRTOS without being   !<<
	>>!   obliged to provide the source code for proprietary components     !<<
	>>!   outside of the FreeRTOS kernel.                                   !<<
	***************************************************************************

	FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
	FOR A PARTICULAR PURPOSE.  Full license text is available on the following
	link: http://www.freertos.org/a00114.html

	http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
	the FAQ page "My application does not run, what could be wrong?".  Have you
	defined configASSERT()?

	http://www.FreeRTOS.org/support - In return for receiving this top quality
	embedded software for free we request you assist our global community by
	participating in the support forum.

	http://www.FreeRTOS.org/training - Investing in training allows your team to
	be as productive as possible as early as possible.  Now you can receive
	FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
	Ltd, and the world's leading authority on the world's leading RTOS.

	http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
	including FreeRTOS+Trace - an indispensable productivity tool, a DOS
	compatible FAT file system, and our tiny thread aware UDP/IP stack.

	http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
	Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

	http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
	Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
	licenses offer ticketed support, indemnification and commercial middleware.

	http://www.SafeRTOS.com - High Integrity Systems also provide a safety
	engineered and independently SIL3 certified version for use in safety and
	mission critical applications that require provable dependability.

	1 tab == 4 spaces!
*/

/* ����������� ��������� FreeRTOS.org */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* ����������� ��������������� ������� */
#include "supporting_functions.h"

/* ������� ��� �������� � ������ ������ */
static void vSenderTask(void* pvParameters);
static void vReceiverTask(void* pvParameters);

/*-----------------------------------------------------------*/

/* ���������� ���������� ���� QueueHandle_t. ��� ������������ ��
�������� �������, � ������� ����� ���������� ������*/

QueueHandle_t xQueue;
typedef enum
{
	eSender1,
	eSender2
} DataSource_t;

/* ����������� ���� ���������, ������� ����� ������������ ����� ������� */
typedef struct
{
	uint8_t ucValue;
	DataSource_t eDataSource;
} Data_t;

/*����������� ���� ���������� ���� Data_t, ������� ����� ������������ ����� ������� */
static const Data_t xStructsToSend[2] =
{
	{ 100, eSender1 }, /* ������������ Sender1. */
	{ 200, eSender2 }  /* ������������ Sender2. */
};

int main(void)
{
	/* ������� ��������� ��� ��������� � ���� �������� 3 ��������� ���� Data_t. */
	xQueue = xQueueCreate(3, sizeof(Data_t));

	if (xQueue != NULL)
	{
		/* ��������� ��� ���������� ������, ������� ����� ���������� � �������.
		�������� ������������ ��� �������� ���������, ������� ����� �������� � �������,
		������� ���� ������ ����� ��������� ���������� xStructsToSend[0], � ������ - 
		xStructsToSend[1]. ��� ������ ��������� � ���������� �����������, ����
		���������� ������-����������*/
		xTaskCreate(vSenderTask, "Sender1", 1000, (void*)&(xStructsToSend[0]), 2, NULL);
		xTaskCreate(vSenderTask, "Sender2", 1000, (void*)&(xStructsToSend[1]), 2, NULL);

		/* ��������� ������ ��� ������ �� �������. ��������� ������ �������� ����, ��� � �����-������������*/
		xTaskCreate(vReceiverTask, "Receiver", 1000, NULL, 1, NULL);

		/* ������ ���������, ����� ���� ��������� ������ ������ ����������� */
		vTaskStartScheduler();
	}
	else
	{
		/* ������� �� ����� ���� ������� */
		vPrintString("The queue could not be created.\r\n");
	}
	for (;; );
	return 0;
}
/*-----------------------------------------------------------*/

static void vSenderTask(void* pvParameters)
{
	BaseType_t xStatus;
	const TickType_t xTicksToWait = pdMS_TO_TICKS(100UL);

	/* ��� � ����������� ������ ������, ������ ������ ����������� ��� ����������� ���� */
	for (;; )
	{
		/* ������ �������� �������� ��������, � ������� ������������ ������.
		������� ���� ������� �� ������� ������������.
		
		������ �������� - ��� ����� ������������ ���������. ����� ���������� ��� 
		�������� ������.
		
		������ �������� �������� �������� ���������� - ��������, � ������� �������� 
		������ ������ ���������� � ��������������� ���������, ����� �����
		������������ ����� � �������, ���� ������� ��� ���������. 
		����� ������������ ����������� �� ���� ���������� �������. ��������
		����� ������� �� ������� ������ �����, ����� ������-����������� �����
		� ��������� "Blocked"*/
		xStatus = xQueueSendToBack(xQueue, pvParameters, xTicksToWait);

		if (xStatus != pdPASS)
		{
			/* �� �� ������ ������ ��������, ��� ��� ������� ���� ��������� � ��� ������, 
			��� ��� ������� �� ������ ��������� ����� ������ ��������  */
			vPrintString("Could not send to the queue.\r\n");
		}
	}
}
/*-----------------------------------------------------------*/

static void vReceiverTask(void* pvParameters)
{
	/* ���������� ���������, ������� ����� ��������� ��������, ���������� � ������� */
	Data_t xReceivedStructure;
	BaseType_t xStatus;

	/* ����������� ���� */
	for (;; )
	{
		/* ��������� ��� ������ ����������� ������ �����, ����� ������������ ������ ��������� � ��������������� ���������,
		� ������������ ������ ����������� ������ �����, ����� ������� ���������, ��� ������ ������
		������ ��������, ��� ������� ���������. 3 - ����� �������. */
		if (uxQueueMessagesWaiting(xQueue) != 3)
		{
			vPrintString("Queue should have been full!\r\n");
		}

		/* ������������� ���������� ������� ������ ������ �� ����� ������� �
		������� API ������� xQueueReceive � ������������ �������, 
		���������� �� ������ � ��������� ������ �� ������� � ����������� �������� */
		xStatus = xQueueReceive(xQueue, &xReceivedStructure, 0);

		if (xStatus == pdPASS)
		{
			/* ������ ���� ������� �������� �� �������, ����� ���������� 
			�������� � ��� ���������*/
			if (xReceivedStructure.eDataSource == eSender1)
			{
				vPrintStringAndNumber("From Sender 1 = ", xReceivedStructure.ucValue);
			}
			else
			{
				vPrintStringAndNumber("From Sender 2 = ", xReceivedStructure.ucValue);
			}
		}
		else
		{
			/* ����� ���������� �� ������, ��� �� ���������� ������� �� �������, 
			��� ��� ������� �� ��������� */
			vPrintString("Could not receive from the queue.\r\n");
		}
	}
}




