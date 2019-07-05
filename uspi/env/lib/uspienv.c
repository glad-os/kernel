//
// uspienv.c
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
#include "../../../uspi/env/include/uspienv.h"
#include "../../../uspi/env/include/uspienv/sysconfig.h"
#include "../../../kernel/include/irq.h"

static TUSPiEnv s_Env;

int USPiEnvInitialize (void)
{

	MemorySystem (&s_Env.m_Memory, TRUE);
	Timer (&s_Env.m_Timer);
	Logger (&s_Env.m_Logger, LogDebug, &s_Env.m_Timer);

	if ( !TimerInitialize (&s_Env.m_Timer) )
	{
		_Logger (&s_Env.m_Logger);
		_Timer (&s_Env.m_Timer);
		return 0;
	}

	return 1;
}

void _USPiEnvInitialize (void)
{
	_Logger (&s_Env.m_Logger);
	_Timer (&s_Env.m_Timer);
	_MemorySystem(&s_Env.m_Memory);
}

void USPiEnvClose (void)
{
	_Logger (&s_Env.m_Logger);
	_Timer (&s_Env.m_Timer);
	_MemorySystem(&s_Env.m_Memory);
}

