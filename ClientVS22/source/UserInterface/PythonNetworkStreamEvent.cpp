#include "StdAfx.h"
#include "PythonNetworkStream.h"

void CPythonNetworkStream::OnRemoteDisconnect()
{
	if (m_bChangingChannel)
	{
		m_bChangingChannel = false;
		m_bPendingChannelReconnect = true;
		return;
	}
	if (m_bChannelHopInProgress)
	{
		m_bChannelHopInProgress = false;
		return;
	}
	PyCallClassMemberFunc(m_poHandler, "SetLoginPhase", Py_BuildValue("()"));
}

void CPythonNetworkStream::OnDisconnect()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main Game
void CPythonNetworkStream::OnScriptEventStart(int iSkin, int iIndex)
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenQuestWindow", Py_BuildValue("(ii)", iSkin, iIndex));
}