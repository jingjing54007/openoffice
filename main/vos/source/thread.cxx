/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#include <osl/time.h>
#include <vos/diagnose.hxx>
#include <vos/object.hxx>
#include <vos/thread.hxx>

using namespace vos;

void vos::threadWorkerFunction_impl(void * pthis)
{
	vos::OThread* pThis= (vos::OThread*)pthis;

	// call Handler-Function of OThread-derived class
	pThis->run();

	// if not already terminating, by a kill do normal shutdown 
	if (! pThis->m_bTerminating)
	{
		pThis->m_bTerminating = sal_True;

		pThis->onTerminated();		// could e.g. delete this
	}
}

/////////////////////////////////////////////////////////////////////////////
//
// Thread class
//

VOS_IMPLEMENT_CLASSINFO(VOS_CLASSNAME(OThread, vos), 
						VOS_NAMESPACE(OThread, vos), 
						VOS_NAMESPACE(OObject, vos), 0);

OThread::OThread() 
{
	m_hThread      = 0;
	m_bTerminating = sal_False;
    m_aCondition   = osl_createCondition();
}

OThread::~OThread() 
{
	if (m_hThread != 0)
	{
		osl_destroyThread(m_hThread);
	}
    
    osl_destroyCondition( m_aCondition );
}

sal_Bool OThread::create() 
{
	VOS_ASSERT(m_hThread == 0);	// only one running thread per instance

    m_hThread = osl_createSuspendedThread(
        threadWorkerFunction_impl, (void*)this);
	if (m_hThread)
		osl_resumeThread(m_hThread);							             

	return m_hThread != 0;
}

sal_Bool OThread::createSuspended() 
{
	VOS_ASSERT(m_hThread == 0);	// only one running thread per instance

	m_hThread= osl_createSuspendedThread(threadWorkerFunction_impl, (void*)this);
	return m_hThread != 0;
}

void OThread::suspend() 
{
	VOS_ASSERT(m_hThread != 0);	// use only on running thread

	osl_suspendThread(m_hThread);
}

void OThread::resume() 
{
	VOS_ASSERT(m_hThread != 0);	// use only on running thread

	osl_resumeThread(m_hThread);
}

sal_Bool OThread::isRunning() 
{
	return m_hThread != 0 && osl_isThreadRunning(m_hThread);
}

OThread::TThreadIdentifier OThread::getIdentifier() const
{
	return (TThreadIdentifier)osl_getThreadIdentifier(m_hThread);
}

OThread::TThreadIdentifier OThread::getCurrentIdentifier()
{
	return (TThreadIdentifier)osl_getThreadIdentifier(0);
}

void OThread::join() 
{
	if (m_hThread) {
		VOS_ASSERT(getCurrentIdentifier() != getIdentifier());
		osl_joinWithThread(m_hThread);
	}
}

OThread::TThreadSleep OThread::sleep(const TimeValue& Delay) 
{
    TThreadSleep eRet;
    
    switch( osl_waitCondition( m_aCondition, &Delay ) )
    {
    case osl_cond_result_ok:
        eRet = TSleep_Normal;
        break;
        
    case osl_cond_result_timeout:
        eRet = TSleep_Cancel;
        break;   
     
    default:
        eRet = TSleep_Error;
        break;
    }
    
    return eRet;
}

void OThread::wait(const TimeValue& Delay) {
	osl_waitThread(&Delay);
}

sal_Bool OThread::awake() 
{
    osl_setCondition( m_aCondition );
    return osl_resetCondition( m_aCondition );
}

void OThread::terminate() 
{
	osl_terminateThread(m_hThread);
}

sal_Bool OThread::schedule() {
	return osl_scheduleThread(m_hThread);
}

void OThread::kill() 
{
	if (osl_isThreadRunning(m_hThread)) 
	{
		// flag we are shutting down
		m_bTerminating = sal_True;

		terminate();
		join();
	}
}

void OThread::setPriority(OThread::TThreadPriority Priority)
{
	osl_setThreadPriority(m_hThread, (oslThreadPriority)Priority);
}

OThread::TThreadPriority OThread::getPriority()
{
	return 	(TThreadPriority)osl_getThreadPriority(m_hThread);
}


void OThread::yield() 
{ 
	osl_yieldThread();
}

void OThread::onTerminated() 
{
}

/////////////////////////////////////////////////////////////////////////////
//
// ThreadData class
//

VOS_IMPLEMENT_CLASSINFO(VOS_CLASSNAME(OThreadData, vos), 
						VOS_NAMESPACE(OThreadData, vos), 
						VOS_NAMESPACE(OObject, vos), 0);

OThreadData::OThreadData( oslThreadKeyCallbackFunction pCallback )
{
    m_hKey = osl_createThreadKey( pCallback );
	VOS_VERIFY(m_hKey);
}

OThreadData::~OThreadData()
{
	osl_destroyThreadKey(m_hKey);
}

sal_Bool OThreadData::setData(void *pData)
{
	VOS_ASSERT(m_hKey != 0);

	return (osl_setThreadKeyData(m_hKey, pData));
}

void *OThreadData::getData()
{
	VOS_ASSERT(m_hKey != 0);

	return (osl_getThreadKeyData(m_hKey));
}

