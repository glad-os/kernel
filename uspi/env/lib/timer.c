//
// timer.c
//
// USPi - An USB driver for Raspberry Pi written in C
// Copyright (C) 2014-2015  R. Stange <rsta2@o2online.de>
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//



#include <stdint.h>
#include "../../../uspi/env/include/uspienv/timer.h"
#include "../../../uspi/env/include/uspienv/alloc.h"
#include "../../../uspi/env/include/uspienv/bcm2835.h"
#include "../../../uspi/env/include/uspienv/logger.h"
#include "../../../uspi/env/include/uspienv/synchronize.h"
#include "../../../uspi/env/include/uspienv/sysconfig.h"
#include "../../../kernel/c/common/include/irq.h"
#include "../../../kernel/c/common/include/define.h"
#include "../../../kernel/c/common/include/video.h"


extern void put_word( unsigned int, unsigned int );
extern unsigned int get_word( unsigned int );
extern void delay_loop (unsigned nCount);
void TimerPollKernelTimers (TTimer *pThis);
void TimerInterruptHandler (void *pParam);
void TimerTuneMsDelay (TTimer *pThis);



static TTimer *s_pThis = 0;



void Timer (TTimer *pThis/*, TInterruptSystem *pInterruptSystem*/)
{

	pThis->m_nTicks = 0;
	pThis->m_nTime = 0;
	#ifdef ARM_DISABLE_MMU
		pThis->m_nMsDelay = 12500;
	#else
		pThis->m_nMsDelay = 350000;
	#endif
	pThis->m_nusDelay = pThis->m_nMsDelay / 1000;

	s_pThis = pThis;

	for (unsigned hTimer = 0; hTimer < KERNEL_TIMERS; hTimer++)
	{
		pThis->m_KernelTimer[hTimer].m_pHandler = 0;
	}
}



void _Timer (TTimer *pThis)
{
	s_pThis = 0;
}



void timerreset(TTimer *pThis) {

	_kernel_interrupt_register_irq_handler( INTERRUPT_IRQ_TIMER3, TimerInterruptHandler, pThis );
	_kernel_interrupt_enable_irq( INTERRUPT_IRQ_TIMER3 );

}



boolean TimerInitialize (TTimer *pThis)
{

	// ACU - Interrupt
	// InterruptSystemConnectIRQ (pThis->m_pInterruptSystem, ARM_IRQ_TIMER3, TimerInterruptHandler, pThis);
	_kernel_interrupt_register_irq_handler( INTERRUPT_IRQ_TIMER3, TimerInterruptHandler, pThis );
	_kernel_interrupt_enable_irq( INTERRUPT_IRQ_TIMER3 );

	DataMemBarrier ();

	put_word (ARM_SYSTIMER_CLO, -(30 * CLOCKHZ));	// timer wraps soon, to check for problems
	put_word (ARM_SYSTIMER_C3, get_word (ARM_SYSTIMER_CLO) + CLOCKHZ / HZ);
	TimerTuneMsDelay (pThis);

	DataMemBarrier ();

	return TRUE;

}



unsigned TimerGetClockTicks (TTimer *pThis)
{

	DataMemBarrier ();

	unsigned nResult = get_word (ARM_SYSTIMER_CLO);

	DataMemBarrier ();

	return nResult;
}



unsigned TimerGetTicks (TTimer *pThis)
{

	return pThis->m_nTicks;

}



unsigned TimerGetTime (TTimer *pThis)
{

	return pThis->m_nTime;

}



TString *TimerGetTimeString (TTimer *pThis)
{

	EnterCritical ();

	unsigned nTime = pThis->m_nTime;
	unsigned nTicks = pThis->m_nTicks;

	LeaveCritical ();

	if (nTicks == 0)
	{
		return 0;
	}

	unsigned nSecond = nTime % 60;
	nTime /= 60;
	unsigned nMinute = nTime % 60;
	nTime /= 60;
	unsigned nHours = nTime;

	nTicks %= HZ;
	#if (HZ != 100)
		nTicks = nTicks * 100 / HZ;
	#endif

	TString *pString = malloc (sizeof (TString));
	String (pString);

	StringFormat (pString, "%02u:%02u:%02u.%02lu", nHours, nMinute, nSecond, nTicks);

	return pString;

}



unsigned TimerStartKernelTimer (TTimer *pThis, unsigned nDelay, TKernelTimerHandler *pHandler, void *pParam, void *pContext)
{

	EnterCritical ();

	unsigned hTimer;
	for (hTimer = 0; hTimer < KERNEL_TIMERS; hTimer++)
	{
		if (pThis->m_KernelTimer[hTimer].m_pHandler == 0)
		{
			break;
		}
	}

	if (hTimer >= KERNEL_TIMERS)
	{
		LeaveCritical ();
		return 0;
	}

	pThis->m_KernelTimer[hTimer].m_pHandler    = pHandler;
	pThis->m_KernelTimer[hTimer].m_nElapsesAt  = pThis->m_nTicks+nDelay;
	pThis->m_KernelTimer[hTimer].m_pParam      = pParam;
	pThis->m_KernelTimer[hTimer].m_pContext    = pContext;

	LeaveCritical ();

	return hTimer+1;

}



void TimerCancelKernelTimer (TTimer *pThis, unsigned hTimer)
{

	pThis->m_KernelTimer[hTimer-1].m_pHandler = 0;

}



void TimerMsDelay (TTimer *pThis, unsigned nMilliSeconds)
{

	if (nMilliSeconds > 0)
	{
		uintptr_t nCycles =  pThis->m_nMsDelay * nMilliSeconds;
		delay_loop (nCycles);
	}

}



void TimerusDelay (TTimer *pThis, unsigned nMicroSeconds)
{

	if (nMicroSeconds > 0)
	{
		unsigned nCycles =  pThis->m_nusDelay * nMicroSeconds;

		delay_loop (nCycles);
	}

}



TTimer *TimerGet (void)
{

	return s_pThis;

}



void TimerSimpleMsDelay (unsigned nMilliSeconds)
{

	if (nMilliSeconds > 0)
	{
		TimerSimpleusDelay (nMilliSeconds * 1000);
	}

}



void TimerSimpleusDelay (unsigned nMicroSeconds)
{

	if (nMicroSeconds > 0)
	{
		unsigned nTicks = nMicroSeconds * (CLOCKHZ / 1000000);

		DataMemBarrier ();

		unsigned nStartTicks = get_word (ARM_SYSTIMER_CLO);
		while (get_word (ARM_SYSTIMER_CLO) - nStartTicks < nTicks)
		{
			// do nothing
		}

		DataMemBarrier ();
	}

}



void TimerPollKernelTimers (TTimer *pThis)
{

	EnterCritical ();

	for (unsigned hTimer = 0; hTimer < KERNEL_TIMERS; hTimer++)
	{
		volatile TKernelTimer *pTimer = &pThis->m_KernelTimer[hTimer];

		TKernelTimerHandler *pHandler = pTimer->m_pHandler;
		if (pHandler != 0)
		{
			if ((int) (pTimer->m_nElapsesAt - pThis->m_nTicks) <= 0)
			{
				pTimer->m_pHandler = 0;

				(*pHandler) (hTimer+1, pTimer->m_pParam, pTimer->m_pContext);
			}
		}
	}

	LeaveCritical ();

}



void TimerInterruptHandler (void *pParam)
{

	TTimer *pThis = (TTimer *) pParam;

	DataMemBarrier ();

	u32 nCompare = get_word (ARM_SYSTIMER_C3) + CLOCKHZ / HZ;
	put_word (ARM_SYSTIMER_C3, nCompare);
	if (nCompare < get_word (ARM_SYSTIMER_CLO))			// time may drift
	{
		nCompare = get_word (ARM_SYSTIMER_CLO) + CLOCKHZ / HZ;
		put_word (ARM_SYSTIMER_C3, nCompare);
	}

	put_word (ARM_SYSTIMER_CS, 1 << 3);

	DataMemBarrier ();

	if (++pThis->m_nTicks % HZ == 0)
	{
		pThis->m_nTime++;
	}

	TimerPollKernelTimers (pThis);

}



void TimerTuneMsDelay (TTimer *pThis)
{

	unsigned nTicks = TimerGetTicks (pThis);
	TimerMsDelay (pThis, 1000);
	nTicks = TimerGetTicks (pThis) - nTicks;

	unsigned nFactor = 100 * HZ / nTicks;

	pThis->m_nMsDelay = pThis->m_nMsDelay * nFactor / 100;
	pThis->m_nusDelay = (pThis->m_nMsDelay + 500) / 1000;

}

