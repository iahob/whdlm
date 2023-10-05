#include "stdafx.h"
#include "Resource.h"
#include "ClientDebugItemSink.h"

////////////////////////////////////////////////////////////////////////////////

// ���ԶԻ���
IMPLEMENT_DYNAMIC(CClientDebugItemSinkDlg, CDialog)
BEGIN_MESSAGE_MAP(CClientDebugItemSinkDlg, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_CUSTOM, OnTcnSelchangeTabOptisons)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

// ���캯��
CClientDebugItemSinkDlg::CClientDebugItemSinkDlg(CWnd *pParent /*=NULL*/) : CDialog(IDD_DIALOG_MAIN, pParent)
{
	// ���ñ���
	m_pParentWnd = NULL;
	m_pIClientDebugCallback = NULL;

	m_bInit = false;
	m_nResetCount = 0;
	m_lEndStorageScore = 0;
	m_lTotalSystemStorage = 0;
	m_lTotalUserStorage = 0;


	m_dwRoomDebugIndex = 0;
	m_lRoomInitSystemStorage = 0;
	m_lRoomInitUserStorage = 0;
	m_nRoomParameterK = 0;
	m_lRoomEndStorageScore = 0;
	m_lRoomTotalSystemStorage = 0;
	m_lRoomTotalUserStorage = 0;

	m_dwUserDebugCount = 0;
	m_dwUserDebugIndex = 0;
	m_dwGameID = 0;
	m_lUserInitSystemStorage = 0;
	m_lUserInitUserStorage = 0;
	m_nUserParameterK = 0;
	m_lUserEndStorageScore = 0;
	m_lUserTotalSystemStorage = 0;
	m_lUserTotalUserStorage = 0;


}

// ��������
CClientDebugItemSinkDlg::~CClientDebugItemSinkDlg()
{
}

// ��������
void CClientDebugItemSinkDlg::OnDestroy()
{
	// ���ٴ���
	for (int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
	{
		m_DialogCustom[nIndex]->DestroyWindow();
		delete m_DialogCustom[nIndex];
	}

	CDialog::OnDestroy();
}

// �ؼ���
VOID CClientDebugItemSinkDlg::DoDataExchange(CDataExchange *pDX)
{
	__super::DoDataExchange(pDX);
}

// �ͷŽӿ�
void CClientDebugItemSinkDlg::Release()
{
	delete this;
}

// ��������
bool CClientDebugItemSinkDlg::Create(CWnd *pParentWnd, IClientDebugCallback *pIClientDebugCallback)
{
	// ���ñ���
	m_pParentWnd = pParentWnd;
	m_pIClientDebugCallback = pIClientDebugCallback;

	// ��������
	__super::Create(IDD_DIALOG_MAIN, pParentWnd);

	// ��ȡ��ҳ
	CTabCtrl *pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM);

	// ����ѡ��
	pTabCtrl->InsertItem(0, _T("ͨ������"));
	pTabCtrl->InsertItem(1, _T("��־��¼"));
	pTabCtrl->SetCurSel(0);

	// ��ȡ��ҳ
	CRect RectWindow;
	CWnd *pWindowShow = GetDlgItem(IDC_STATIC_SHOW);
	pWindowShow->ShowWindow(SW_HIDE);
	pWindowShow->GetWindowRect(RectWindow);
	ScreenToClient(RectWindow);

	// ��������
	m_DialogCustom[CUSTOM_GENERAL] = new CDialogGeneral(this);
	m_DialogCustom[CUSTOM_GENERAL]->Create(IDD_CUSTOM_GENERAL, this);

	m_DialogCustom[CUSTOM_ADVANCED] = new CDialogAdvanced(this);
	m_DialogCustom[CUSTOM_ADVANCED]->Create(IDD_CUSTOM_ADVANCED, this);

	// ����λ��
	for (int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
	{
		CRect RectCustom;
		CSize SizeCustom;
		m_DialogCustom[nIndex]->GetClientRect(RectCustom);
		SizeCustom.SetSize(min(RectWindow.Width(), RectCustom.Width()), min(RectWindow.Height(), RectCustom.Height()));

		m_DialogCustom[nIndex]->SetWindowPos(NULL, RectWindow.left + RectWindow.Width() / 2 - SizeCustom.cx / 2, RectWindow.top, SizeCustom.cx, SizeCustom.cy, SWP_NOZORDER);
		m_DialogCustom[nIndex]->ShowWindow((nIndex == 0) ? SW_SHOW : SW_HIDE);
	}

	// �رմ���
	__super::ShowWindow(SW_HIDE);

	
	return true;

}

// ��ʾ����
bool CClientDebugItemSinkDlg::ShowWindow(bool bShow)
{
	// ��ʾ����
	__super::ShowWindow(bShow ? SW_SHOW : SW_HIDE);

	// ��ʾ����
	if (bShow && m_pParentWnd)
	{
		// ��ȡ������
		CRect RectParent;
		m_pParentWnd->GetWindowRect(RectParent);

		// ��ȡ��ǰ����
		CRect RectWindow;
		GetWindowRect(RectWindow);

		// �ƶ�λ��
		SetWindowPos(NULL, RectParent.left + RectParent.Width() / 2 - RectWindow.Width() / 2, RectParent.top + RectParent.Height() / 2 - RectWindow.Height() / 2, 0, 0, SWP_NOSIZE);
	}

	return true;
}

// ��Ϣ����
bool CClientDebugItemSinkDlg::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize)
{
	// ��Ч�ӿ�
	if (m_pIClientDebugCallback == NULL)
	{
		return false;
	}


	//��ʼ��δ���
	if (!m_bInit && nMessageID != SUB_S_REFRESH_STORAGE)
	{
		return true;
	}

	switch (nMessageID)
	{
	case SUB_S_DEBUG_TEXT:
	{
		ASSERT(nSize == sizeof(CMD_S_DebugText));
		if (nSize != sizeof(CMD_S_DebugText))
		{
			return false;
		}

		CMD_S_DebugText *pDebugText = (CMD_S_DebugText *)pData;

		// ��ʾ��Ϣ
		::MessageBox(GetSafeHwnd(), pDebugText->szMessageText, _T("������ʾ"), MB_OK);
		return true;
	}
	case SUB_S_REFRESH_STORAGE:			//ˢ�¿����Ϣ
		{
			ASSERT(nSize == sizeof(CMD_S_BaseStorageInfo));
			if (nSize != sizeof(CMD_S_BaseStorageInfo)) return false;

			CMD_S_BaseStorageInfo *pBaseStorageInfo = (CMD_S_BaseStorageInfo *)pData;
			m_tStartDebugTime = CTime(pBaseStorageInfo->lStartDebugTime);
			m_tResetSystemStorage = CTime(pBaseStorageInfo->lResetSystemStorage);
			m_dwSysDebugIndex = pBaseStorageInfo->dwSysDebugIndex;
			m_bSystemStorageEnd = pBaseStorageInfo->bSystemStorageEnd;
			m_lInitSystemStorage = pBaseStorageInfo->lLastInitSystemStorage;
			m_lSystemStorage = pBaseStorageInfo->lLastInitSystemStorage;
			m_lInitUserStorage = pBaseStorageInfo->lLastInitUserStorage;
			m_lUserStorage = pBaseStorageInfo->lLastInitUserStorage;
			m_nInitParameterK = pBaseStorageInfo->nInitParameterK;


			//�µ���
			m_nResetCount++; 
			tagHistoryRoomDebugInfo HistoryRoomDebugInfo;
			HistoryRoomDebugInfo.dwDebugIndex = m_dwSysDebugIndex;
			HistoryRoomDebugInfo.bSystemStorageEnd = m_bSystemStorageEnd;
			HistoryRoomDebugInfo.lDebugTime = pBaseStorageInfo->lResetSystemStorage;
			HistoryRoomDebugInfo.lUpdateTime = pBaseStorageInfo->lResetSystemStorage;
			HistoryRoomDebugInfo.lInitSystemStorage = pBaseStorageInfo->lLastInitSystemStorage;
			HistoryRoomDebugInfo.lSystemStorage = m_lSystemStorage;
			HistoryRoomDebugInfo.lInitUserStorage = pBaseStorageInfo->lLastInitUserStorage;
			HistoryRoomDebugInfo.lUserStorage = m_lUserStorage;
			HistoryRoomDebugInfo.nParameterK = m_nInitParameterK;
			HistoryRoomDebugInfo.nDebugStatus = RUNNNING_STATUS;
			m_listHistorySysDebugInfo.AddTail(HistoryRoomDebugInfo);

			m_bInit = true;

			//����ϵͳ����
			UpdateSysDebugInfo(&HistoryRoomDebugInfo);

			//����ϵͳ����
			UpdateSysDebugInfo(m_dwSysDebugIndex, pBaseStorageInfo->lSystemStorage, pBaseStorageInfo->lUserStorage, RUNNNING_STATUS, pBaseStorageInfo->lResetSystemStorage);

			return true;
		}
	case SUB_S_SET_ROOM_STORAGE:
	{
		ASSERT(nSize == sizeof(CMD_S_SetRoomStorageInfo));
		if (nSize != sizeof(CMD_S_SetRoomStorageInfo)) return false;

		CMD_S_SetRoomStorageInfo *pSetRoomStorageInfo = (CMD_S_SetRoomStorageInfo *)pData;
		
		//�����¼
		tagHistoryRoomDebugInfo HistoryRoomDebugInfo;
		HistoryRoomDebugInfo.dwDebugIndex = pSetRoomStorageInfo->dwDebugIndex;
		HistoryRoomDebugInfo.bSystemStorageEnd = pSetRoomStorageInfo->bSystemStorageEnd;
		HistoryRoomDebugInfo.lDebugTime = pSetRoomStorageInfo->lUpdateTime;
		HistoryRoomDebugInfo.lUpdateTime = pSetRoomStorageInfo->lUpdateTime;
		HistoryRoomDebugInfo.lInitSystemStorage = pSetRoomStorageInfo->lSystemStorage;
		HistoryRoomDebugInfo.lSystemStorage = pSetRoomStorageInfo->lSystemStorage;
		HistoryRoomDebugInfo.lInitUserStorage = pSetRoomStorageInfo->lUserStorage;
		HistoryRoomDebugInfo.lUserStorage = pSetRoomStorageInfo->lUserStorage;
		HistoryRoomDebugInfo.nParameterK = pSetRoomStorageInfo->nParameterK;
		HistoryRoomDebugInfo.nDebugStatus = pSetRoomStorageInfo->nDebugStatus;;
		m_listHistoryRoomDebugInfo.AddTail(HistoryRoomDebugInfo);

		if (m_dwRoomDebugIndex == 0)
		{
			m_dwRoomDebugIndex = HistoryRoomDebugInfo.dwDebugIndex;
			m_lRoomInitSystemStorage = HistoryRoomDebugInfo.lInitSystemStorage;
			m_lRoomInitUserStorage = HistoryRoomDebugInfo.lInitUserStorage;
			m_nRoomParameterK = HistoryRoomDebugInfo.nParameterK;
		}

		UpdateRoomDebugInfo(&HistoryRoomDebugInfo);

		return true;
	}	
	case SUB_S_DEL_ROOM_STORAGE:
	{
		ASSERT(nSize == sizeof(CMD_S_DeleteStorageInfo));
		if (nSize != sizeof(CMD_S_DeleteStorageInfo)) return false;

		CMD_S_DeleteStorageInfo *pDeleteStorageInfo = (CMD_S_DeleteStorageInfo *)pData;

		DeleteRoomDebugInfo(pDeleteStorageInfo->dwDebugIndex);
		return true;
	}
	case SUB_S_SET_USER_STORAGE:
	{
		ASSERT(nSize == sizeof(CMD_S_SetUserStorageInfo));
		if (nSize != sizeof(CMD_S_SetUserStorageInfo)) return false;

		CMD_S_SetUserStorageInfo *pSetUserStorageInfo = (CMD_S_SetUserStorageInfo *)pData;

		//�����¼
		tagHistoryRoomUserInfo HistoryUserDebugInfo;
		HistoryUserDebugInfo.dwDebugIndex = pSetUserStorageInfo->dwDebugIndex;
		HistoryUserDebugInfo.dwGameID = pSetUserStorageInfo->dwGameID;
		HistoryUserDebugInfo.bSystemStorageEnd = pSetUserStorageInfo->bSystemStorageEnd;
		HistoryUserDebugInfo.lDebugTime = pSetUserStorageInfo->lUpdateTime;
		HistoryUserDebugInfo.lUpdateTime = pSetUserStorageInfo->lUpdateTime;
		HistoryUserDebugInfo.lInitSystemStorage = pSetUserStorageInfo->lSystemStorage;
		HistoryUserDebugInfo.lSystemStorage = pSetUserStorageInfo->lSystemStorage;
		HistoryUserDebugInfo.lInitUserStorage = pSetUserStorageInfo->lUserStorage;
		HistoryUserDebugInfo.lUserStorage = pSetUserStorageInfo->lUserStorage;
		HistoryUserDebugInfo.nParameterK = pSetUserStorageInfo->nParameterK;
		HistoryUserDebugInfo.nDebugStatus = RUNNNING_STATUS;
		m_listHistoryRoomUserInfo.AddTail(HistoryUserDebugInfo);

		m_dwUserDebugCount++;
		if (m_dwUserDebugIndex == 0)
		{
			m_dwUserDebugIndex = HistoryUserDebugInfo.dwDebugIndex;
			m_lUserInitSystemStorage = HistoryUserDebugInfo.lInitSystemStorage;
			m_lUserInitUserStorage = HistoryUserDebugInfo.lInitUserStorage;
			m_nUserParameterK = HistoryUserDebugInfo.nParameterK;
		}

		UpdateUserDebugInfo(&HistoryUserDebugInfo);

		return true;
	}
	case SUB_S_DEL_USER_STORAGE:
	{
		ASSERT(nSize == sizeof(CMD_S_DeleteStorageInfo));
		if (nSize != sizeof(CMD_S_DeleteStorageInfo)) return false;

		CMD_S_DeleteStorageInfo *pDeleteStorageInfo = (CMD_S_DeleteStorageInfo *)pData;

		DeleteUserDebugInfo(pDeleteStorageInfo->dwDebugIndex);
		return true;
	}
	case SUB_S_REFRESH_USER_DEBUG:
	{
		ASSERT(nSize == sizeof(CMD_S_UpdateStorageInfo));
		if (nSize != sizeof(CMD_S_UpdateStorageInfo)) return false;

		CMD_S_UpdateStorageInfo *pUpdateStorageInfo = (CMD_S_UpdateStorageInfo *)pData;
		UpdateUserDebugInfo(pUpdateStorageInfo->dwDebugIndex, pUpdateStorageInfo->lSystemStorage, pUpdateStorageInfo->lUserStorage, pUpdateStorageInfo->nDebugStatus, pUpdateStorageInfo->lUpdateTime);
		return true;
	}
	case SUB_S_REFRESH_HISTORY_USER_DEBUG:
	{
		ASSERT(nSize % sizeof(tagHistoryRoomUserInfo) == 0 && nSize != 0);
		if (nSize % sizeof(tagHistoryRoomUserInfo) != 0 || nSize == 0) return false;

		tagHistoryRoomUserInfo *pHistoryRoomUserInfo = (tagHistoryRoomUserInfo *)pData;

		for (UINT i = 0; i < nSize / sizeof(tagHistoryRoomUserInfo); i++)
		{
			m_listHistoryRoomUserInfo.AddTail(*pHistoryRoomUserInfo);

			//ͳ������
			m_lUserTotalSystemStorage += pHistoryRoomUserInfo->lInitSystemStorage - pHistoryRoomUserInfo->lSystemStorage;
			m_lUserTotalUserStorage += pHistoryRoomUserInfo->lInitUserStorage - pHistoryRoomUserInfo->lUserStorage;
			if (pHistoryRoomUserInfo->nDebugStatus == FINISH_STATUS && pHistoryRoomUserInfo->bSystemStorageEnd)
			{
				//����ֹ
				m_lUserEndStorageScore += pHistoryRoomUserInfo->lUserStorage;
			}

			UpdateUserDebugInfo(pHistoryRoomUserInfo);

			pHistoryRoomUserInfo++;
		}

		return true;
	}
	case SUB_S_REFRESH_ROOM_DEBUG:
	{
		ASSERT(nSize == sizeof(CMD_S_UpdateStorageInfo));
		if (nSize != sizeof(CMD_S_UpdateStorageInfo)) return false;

		CMD_S_UpdateStorageInfo *pUpdateStorageInfo = (CMD_S_UpdateStorageInfo *)pData;
		UpdateRoomDebugInfo(pUpdateStorageInfo->dwDebugIndex, pUpdateStorageInfo->lSystemStorage, pUpdateStorageInfo->lUserStorage, pUpdateStorageInfo->nDebugStatus, pUpdateStorageInfo->lUpdateTime);
		return true;
	}
	case SUB_S_REFRESH_HISTORY_ROOM_DEBUG:
	{
		ASSERT(nSize % sizeof(tagHistoryRoomDebugInfo) == 0 && nSize != 0);
		if (nSize % sizeof(tagHistoryRoomDebugInfo) != 0 || nSize == 0) return false;

		tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo = (tagHistoryRoomDebugInfo *)pData;
		
		for (UINT i = 0; i < nSize / sizeof(tagHistoryRoomDebugInfo); i++)
		{			
			m_listHistoryRoomDebugInfo.AddTail(*pHistoryRoomDebugInfo);

			//ͳ������
			m_lRoomTotalSystemStorage += pHistoryRoomDebugInfo->lInitSystemStorage - pHistoryRoomDebugInfo->lSystemStorage;
			m_lRoomTotalUserStorage += pHistoryRoomDebugInfo->lInitUserStorage - pHistoryRoomDebugInfo->lUserStorage;
			if (pHistoryRoomDebugInfo->bSystemStorageEnd && pHistoryRoomDebugInfo->nDebugStatus == FINISH_STATUS)
			{
				//����ֹ
				m_lRoomEndStorageScore += pHistoryRoomDebugInfo->lUserStorage;
			}

			if (m_dwRoomDebugIndex == 0 && pHistoryRoomDebugInfo->nDebugStatus == RUNNNING_STATUS)
			{
				m_dwRoomDebugIndex = pHistoryRoomDebugInfo->dwDebugIndex;
			}

			UpdateRoomDebugInfo(pHistoryRoomDebugInfo);

			pHistoryRoomDebugInfo++;
		}
		return true;
	}
	case SUB_S_REFRESH_SYS_DEBUG:
	{
		ASSERT(nSize == sizeof(CMD_S_UpdateStorageInfo));
		if (nSize != sizeof(CMD_S_UpdateStorageInfo)) return false;

		CMD_S_UpdateStorageInfo *pUpdateStorageInfo = (CMD_S_UpdateStorageInfo *)pData;
		UpdateSysDebugInfo(pUpdateStorageInfo->dwDebugIndex, pUpdateStorageInfo->lSystemStorage, pUpdateStorageInfo->lUserStorage, pUpdateStorageInfo->nDebugStatus, pUpdateStorageInfo->lUpdateTime);
		return true;
	}
	case SUB_S_REFRESH_HISTORY_SYS_DEBUG:
	{
		ASSERT(nSize % sizeof(tagHistoryRoomDebugInfo) == 0 && nSize != 0);
		if (nSize % sizeof(tagHistoryRoomDebugInfo) != 0 || nSize == 0) return false;

		tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo = (tagHistoryRoomDebugInfo *)pData;

		for (UINT i = 0; i < nSize / sizeof(tagHistoryRoomDebugInfo); i++)
		{
			m_nResetCount++;
			m_listHistorySysDebugInfo.AddTail(*pHistoryRoomDebugInfo);

			if (pHistoryRoomDebugInfo->nDebugStatus == FINISH_STATUS || pHistoryRoomDebugInfo->nDebugStatus == CANCEL_STATUS)
			{
				//ͳ������
				m_lTotalSystemStorage += pHistoryRoomDebugInfo->lInitSystemStorage - pHistoryRoomDebugInfo->lSystemStorage;
				m_lTotalUserStorage += pHistoryRoomDebugInfo->lInitUserStorage - pHistoryRoomDebugInfo->lUserStorage;

				//����ֹ
				if (pHistoryRoomDebugInfo->nDebugStatus == FINISH_STATUS)
				{
					m_lEndStorageScore += pHistoryRoomDebugInfo->lInitUserStorage - pHistoryRoomDebugInfo->lUserStorage;
				}
			}

			UpdateSysDebugInfo(pHistoryRoomDebugInfo);

			pHistoryRoomDebugInfo++;
		}
		return true;
	}
	default:
		break;
	}

	// �жϴ���
	for (int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
	{
		if (m_DialogCustom[nIndex]->OnDebugMessage(nMessageID, wTableID, pData, nSize))
		{
			return true;
		}
	}

	return true;
}

// ������Ϣ
bool CClientDebugItemSinkDlg::SendDebugMessage(UINT nMessageID, void *pData, UINT nSize)
{
	if (m_pIClientDebugCallback != NULL)
	{
		// ������Ϣ
		m_pIClientDebugCallback->OnDebugInfo(nMessageID, 0, pData, nSize);
	}
	return true;
}

// ������Ϣ
bool CClientDebugItemSinkDlg::SendDebugMessage(UINT nMessageID, WORD wTableID, void *pData, UINT nSize)
{
	if (m_pIClientDebugCallback != NULL)
	{
		// ������Ϣ
		m_pIClientDebugCallback->OnDebugInfo(nMessageID, wTableID, pData, nSize);
	}
	return true;
}

// �任ѡ��
void CClientDebugItemSinkDlg::OnTcnSelchangeTabOptisons(NMHDR *pNMHDR, LRESULT *pResult)
{
	// ��ȡ��ҳ
	CTabCtrl *pTabCtrl = (CTabCtrl *)GetDlgItem(IDC_TAB_CUSTOM);

	// ��ȡѡ����Ŀ
	int nCurSel = pTabCtrl->GetCurSel();
	if (nCurSel >= 0 && nCurSel < MAX_CUSTOM)
	{
		// �жϴ���
		for (int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
		{
			m_DialogCustom[nIndex]->ShowWindow((nIndex == nCurSel) ? SW_SHOW : SW_HIDE);
		}
	}

	*pResult = 0;
}


//������ҵ���
void CClientDebugItemSinkDlg::DeleteUserDebugInfo(DWORD dwDebugIndex)
{
	POSITION posHead = m_listHistoryRoomUserInfo.GetHeadPosition();
	while (posHead)
	{
		POSITION posCur = posHead;
		tagHistoryRoomUserInfo HistoryRoomUserInfo = m_listHistoryRoomUserInfo.GetNext(posHead);
		if (HistoryRoomUserInfo.dwDebugIndex == dwDebugIndex)
		{
			m_dwUserDebugCount--;
			m_listHistoryRoomUserInfo.RemoveAt(posCur);	
			break;
		}
	}

	if (m_dwUserDebugIndex == dwDebugIndex)
	{
		m_dwUserDebugIndex = 0;
	}
	
	// �жϴ���
	for (int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
	{
		m_DialogCustom[nIndex]->DeleteUserDebugInfo(dwDebugIndex);
	}
}

//������ҵ���
void CClientDebugItemSinkDlg::UpdateUserDebugInfo(tagHistoryRoomUserInfo *pHistoryRoomUserInfo)
{
	m_dwUserDebugCount = 0;
	POSITION posHead = m_listHistoryRoomUserInfo.GetHeadPosition();
	while (posHead)
	{
		POSITION posCur = posHead;
		tagHistoryRoomUserInfo HistoryRoomUserInfo = m_listHistoryRoomUserInfo.GetNext(posHead);
		if (HistoryRoomUserInfo.nDebugStatus == RUNNNING_STATUS)
		{
			m_dwUserDebugCount++;
		}
	}

	// �жϴ���
	for (int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
	{
		m_DialogCustom[nIndex]->UpdateUserDebugInfo(pHistoryRoomUserInfo);
	}
}

//������ҵ���
void CClientDebugItemSinkDlg::UpdateUserDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime)
{
	if (nDebugStatus == FINISH_STATUS || nDebugStatus == CANCEL_STATUS)
	{
		m_dwUserDebugCount--;
	}

	//���µ���
	tagHistoryRoomUserInfo HistoryRoomUserInfo;

	//ȡ��ϵͳ����
	POSITION posCur = GetUserDebugInfo(dwDebugIndex, HistoryRoomUserInfo);
	if (posCur)
	{
		//ͳ������
		m_lUserTotalSystemStorage += HistoryRoomUserInfo.lSystemStorage - lSystemStorage;
		m_lUserTotalUserStorage += HistoryRoomUserInfo.lUserStorage - lUserStorage;

		if (nDebugStatus == FINISH_STATUS)
		{			
			if (HistoryRoomUserInfo.bSystemStorageEnd)
			{
				//����ֹ
				m_lUserEndStorageScore += lUserStorage;
			}
			if (m_dwUserDebugIndex == dwDebugIndex)
			{
				
				m_dwUserDebugIndex = 0;
			}
		}

		HistoryRoomUserInfo.lSystemStorage = lSystemStorage;
		HistoryRoomUserInfo.lUserStorage = lUserStorage;
		HistoryRoomUserInfo.nDebugStatus = nDebugStatus;
		HistoryRoomUserInfo.lUpdateTime = lUpdateTime;
		m_listHistoryRoomUserInfo.SetAt(posCur, HistoryRoomUserInfo);

		//������ǰ
		if (m_dwUserDebugIndex != dwDebugIndex && nDebugStatus == RUNNNING_STATUS)
		{
			m_dwUserDebugIndex = dwDebugIndex;
			m_lUserInitSystemStorage = HistoryRoomUserInfo.lInitSystemStorage;
			m_lUserInitUserStorage = HistoryRoomUserInfo.lInitUserStorage;
			m_nUserParameterK = HistoryRoomUserInfo.nParameterK;
		}

		// �жϴ���
		for (int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
		{
			m_DialogCustom[nIndex]->UpdateUserDebugInfo(dwDebugIndex, lSystemStorage, lUserStorage, nDebugStatus, lUpdateTime);
		}
	}
}

//���·������
void CClientDebugItemSinkDlg::DeleteRoomDebugInfo(DWORD dwDebugIndex)
{
	//ɾ������
	POSITION posHead = m_listHistoryRoomDebugInfo.GetHeadPosition();
	while (posHead)
	{
		POSITION posCur = posHead;
		tagHistoryRoomDebugInfo HistoryRoomDebugInfo = m_listHistoryRoomDebugInfo.GetNext(posHead);
		if (HistoryRoomDebugInfo.dwDebugIndex == dwDebugIndex)
		{
			m_listHistoryRoomDebugInfo.RemoveAt(posCur);
			break;
		}
	}

	if (m_dwRoomDebugIndex == dwDebugIndex)
	{
		m_dwRoomDebugIndex = 0;
	}

	// �жϴ���
	for (int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
	{
		m_DialogCustom[nIndex]->DeleteRoomDebugInfo(dwDebugIndex);
	}
}

//���·������
void CClientDebugItemSinkDlg::UpdateRoomDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo)
{
	// �жϴ���
	for (int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
	{
		m_DialogCustom[nIndex]->UpdateRoomDebugInfo(pHistoryRoomDebugInfo);
	}
}

//���·������
void CClientDebugItemSinkDlg::UpdateRoomDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime)
{
	//���µ���
	tagHistoryRoomDebugInfo HistoryRoomDebugInfo;

	//ȡ��ϵͳ����
	POSITION posCur = GetRoomDebugInfo(dwDebugIndex, HistoryRoomDebugInfo);
	if (posCur)
	{
		//ͳ������
		m_lRoomTotalSystemStorage += HistoryRoomDebugInfo.lSystemStorage - lSystemStorage;
		m_lRoomTotalUserStorage += HistoryRoomDebugInfo.lUserStorage - lUserStorage;
		if (nDebugStatus == FINISH_STATUS)
		{
			//����ֹ
			if (HistoryRoomDebugInfo.bSystemStorageEnd)
			{
				m_lRoomEndStorageScore += lUserStorage;
			}
			if (m_dwRoomDebugIndex == dwDebugIndex)
			{
				m_dwRoomDebugIndex = 0;
			}
		}

		HistoryRoomDebugInfo.lSystemStorage = lSystemStorage;
		HistoryRoomDebugInfo.lUserStorage = lUserStorage;
		HistoryRoomDebugInfo.nDebugStatus = nDebugStatus;
		HistoryRoomDebugInfo.lUpdateTime = lUpdateTime;
		m_listHistoryRoomDebugInfo.SetAt(posCur, HistoryRoomDebugInfo);

		//������ǰ
		if (m_dwRoomDebugIndex != dwDebugIndex && nDebugStatus == RUNNNING_STATUS)
		{
			m_dwRoomDebugIndex = dwDebugIndex;
			m_lRoomInitSystemStorage = HistoryRoomDebugInfo.lInitSystemStorage;
			m_lRoomInitUserStorage = HistoryRoomDebugInfo.lInitUserStorage;
			m_nRoomParameterK = HistoryRoomDebugInfo.nParameterK;
		}

		// �жϴ���
		for (int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
		{
			m_DialogCustom[nIndex]->UpdateRoomDebugInfo(dwDebugIndex, lSystemStorage, lUserStorage, nDebugStatus, lUpdateTime);
		}
	}
}

//����ϵͳ����
void CClientDebugItemSinkDlg::UpdateSysDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo)
{
	// �жϴ���
	for (int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
	{
		m_DialogCustom[nIndex]->UpdateSysDebugInfo(pHistoryRoomDebugInfo);
	}
}

//����ϵͳ����
void CClientDebugItemSinkDlg::UpdateSysDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime)
{
	//ͳ������
	if (nDebugStatus == CANCEL_STATUS || nDebugStatus == FINISH_STATUS)
	{
		//��ɵ���
		tagHistoryRoomDebugInfo HistoryRoomDebugInfo;

		//ȡ��ϵͳ����
		POSITION posCur = GetSysDebugInfo(dwDebugIndex, HistoryRoomDebugInfo);
		ASSERT(posCur);

		HistoryRoomDebugInfo.lUpdateTime = lUpdateTime;
		HistoryRoomDebugInfo.lSystemStorage = lSystemStorage;
		HistoryRoomDebugInfo.lUserStorage = lUserStorage;
		HistoryRoomDebugInfo.nDebugStatus = nDebugStatus;
		m_listHistorySysDebugInfo.SetAt(posCur, HistoryRoomDebugInfo);

		//����ϵͳ����
		UpdateSysDebugInfo(&HistoryRoomDebugInfo);

		if (nDebugStatus == FINISH_STATUS)
		{
			m_nResetCount++;		
			m_dwSysDebugIndex++; 

			//����ֹ
			if (m_bSystemStorageEnd)
			{
				m_lEndStorageScore += lUserStorage;
				lUserStorage = 0;
			}

			m_lTotalSystemStorage += m_lSystemStorage - lSystemStorage;
			m_lTotalUserStorage += m_lUserStorage - lUserStorage;
			m_lSystemStorage = m_lInitSystemStorage + lSystemStorage;
			m_lUserStorage = m_lInitUserStorage + lUserStorage;
		
			//����ֹ���⴦��
			if (m_bSystemStorageEnd && (m_lUserStorage < 0 || m_lSystemStorage < 0))
			{
				m_nResetCount++;
				m_dwSysDebugIndex++;

				m_lSystemStorage += m_lInitSystemStorage;
				m_lUserStorage += m_lInitUserStorage;

				if (m_lUserStorage < 0)
				{
					m_lUserStorage = 0;
				}

				if (m_lSystemStorage < 0)
				{
					m_lSystemStorage = 0;
				}

				if (m_lUserStorage <= 0 && m_lSystemStorage <= 0)
				{
					m_nResetCount++;
					m_dwSysDebugIndex++;

					m_lSystemStorage = m_lInitSystemStorage;
					m_lUserStorage = m_lInitUserStorage;
				}
			}

			//�µ���
			HistoryRoomDebugInfo.dwDebugIndex = m_dwSysDebugIndex;
			HistoryRoomDebugInfo.bSystemStorageEnd = m_bSystemStorageEnd;
			HistoryRoomDebugInfo.lDebugTime = lUpdateTime;
			HistoryRoomDebugInfo.lUpdateTime = lUpdateTime;
			HistoryRoomDebugInfo.lInitSystemStorage = m_lSystemStorage;
			HistoryRoomDebugInfo.lSystemStorage = m_lSystemStorage;
			HistoryRoomDebugInfo.lInitUserStorage = m_lUserStorage;
			HistoryRoomDebugInfo.lUserStorage = m_lUserStorage;
			HistoryRoomDebugInfo.nParameterK = m_nInitParameterK;
			HistoryRoomDebugInfo.nDebugStatus = RUNNNING_STATUS;
			m_listHistorySysDebugInfo.AddTail(HistoryRoomDebugInfo);

			//����ϵͳ����
			m_tStartDebugTime = lUpdateTime;

			//����ϵͳ����
			UpdateSysDebugInfo(&HistoryRoomDebugInfo);
		}
	}
	else
	{
		m_lTotalSystemStorage += m_lSystemStorage - lSystemStorage;
		m_lTotalUserStorage += m_lUserStorage - lUserStorage;
		m_lSystemStorage = lSystemStorage;
		m_lUserStorage = lUserStorage;

		// �жϴ���
		for (int nIndex = 0; nIndex < MAX_CUSTOM; ++nIndex)
		{
			m_DialogCustom[nIndex]->UpdateSysDebugInfo(dwDebugIndex, lSystemStorage, lUserStorage, nDebugStatus, lUpdateTime);
		}
	}
}


//ȡ����ҵ���
POSITION CClientDebugItemSinkDlg::GetUserDebugInfo(DWORD dwDebugIndex, tagHistoryRoomUserInfo &HistoryRoomUserInfo)
{
	POSITION posHead = m_listHistoryRoomUserInfo.GetHeadPosition();
	while (posHead)
	{
		POSITION posCur = posHead;
		HistoryRoomUserInfo = m_listHistoryRoomUserInfo.GetNext(posHead);
		if (HistoryRoomUserInfo.dwDebugIndex == dwDebugIndex)
		{
			return posCur;
		}
	}

	return posHead;
}

//ȡ�÷������
POSITION CClientDebugItemSinkDlg::GetRoomDebugInfo(DWORD dwDebugIndex, tagHistoryRoomDebugInfo &HistoryRoomDebugInfo)
{
	POSITION posHead = m_listHistoryRoomDebugInfo.GetHeadPosition();
	while (posHead)
	{
		POSITION posCur = posHead;
		HistoryRoomDebugInfo = m_listHistoryRoomDebugInfo.GetNext(posHead);
		if (HistoryRoomDebugInfo.dwDebugIndex == dwDebugIndex)
		{
			return posCur;
		}
	}

	return posHead;
}

//ȡ��ϵͳ����
POSITION CClientDebugItemSinkDlg::GetSysDebugInfo(DWORD dwDebugIndex, tagHistoryRoomDebugInfo &HistoryRoomDebugInfo)
{
	POSITION posHead = m_listHistorySysDebugInfo.GetHeadPosition();
	while (posHead)
	{
		POSITION posCur = posHead;
		HistoryRoomDebugInfo = m_listHistorySysDebugInfo.GetNext(posHead);
		if (HistoryRoomDebugInfo.dwDebugIndex == dwDebugIndex)
		{
			return posCur;
		}
	}

	return posHead;
}

////////////////////////////////////////////////////////////////////////////////


IMPLEMENT_DYNAMIC(CDialogGeneral, IClientDebugDlg)

BEGIN_MESSAGE_MAP(CDialogGeneral, IClientDebugDlg)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_REFRESHRULE, &CDialogGeneral::OnBnClickedButtonRefreshrule)
	ON_BN_CLICKED(IDC_BUTTON_CUR_STORAGE, &CDialogGeneral::OnBnClickedButtonCurStorage)
	ON_BN_CLICKED(IDC_BUTTON_INIT_STORAGE, &CDialogGeneral::OnBnClickedButtonInitStorage)
	ON_BN_CLICKED(IDC_BTN_REFRESH_ROOM_LIST, &CDialogGeneral::OnBnClickedBtnRefreshRoomList)
	ON_BN_CLICKED(IDC_BTN_SET_ROOM_STORAGE, &CDialogGeneral::OnBnClickedBtnSetRoomStorage)
	ON_BN_CLICKED(IDC_BTN_CANCEL_ROOM_STORAGE, &CDialogGeneral::OnBnClickedBtnCancelRoomStorage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ROOM, &CDialogGeneral::OnLvnItemchangedListRoom)

	ON_BN_CLICKED(IDC_BTN_REFRESH_USER_LIST, &CDialogGeneral::OnBnClickedBtnRefreshUserList)
	ON_BN_CLICKED(IDC_BTN_SET_USER_STORAGE, &CDialogGeneral::OnBnClickedBtnSetUserStorage)
	ON_BN_CLICKED(IDC_BTN_CANCEL_USER_STORAGE, &CDialogGeneral::OnBnClickedBtnCancelUserStorage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_USER, &CDialogGeneral::OnLvnItemchangedListUser)

	ON_EN_CHANGE(IDC_EDIT_SYSTEM_STORAGE, &CDialogGeneral::OnEnChangeEditSystemStorage)
	ON_EN_CHANGE(IDC_EDIT_USER_STORAGE, &CDialogGeneral::OnEnChangeEditUserStorage)
	ON_EN_CHANGE(IDC_EDIT_PARAMETER_K, &CDialogGeneral::OnEnChangeEditParameterK)

	ON_EN_CHANGE(IDC_EDIT_ROOM_SYSTEM_STORAGE, &CDialogGeneral::OnEnChangeEditRoomSystemStorage)
	ON_EN_CHANGE(IDC_EDIT_ROOM_USER_STORAGE, &CDialogGeneral::OnEnChangeEditRoomUserStorage)
	ON_EN_CHANGE(IDC_EDIT_ROOM_PARAMETER_K, &CDialogGeneral::OnEnChangeEditRoomParameterK)

	ON_EN_CHANGE(IDC_EDIT_USER_SYSTEM_STORAGE, &CDialogGeneral::OnEnChangeEditUserSystemStorage)
	ON_EN_CHANGE(IDC_EDIT_USER_USER_STORAGE, &CDialogGeneral::OnEnChangeEditUserUserStorage)
	ON_EN_CHANGE(IDC_EDIT_USER_PARAMETER_K, &CDialogGeneral::OnEnChangeEditUserParameterK)


	ON_BN_CLICKED(IDC_BUTTON_CUR_CAI_JING, &CDialogGeneral::OnBnClickedButtonCurCaiJing)
END_MESSAGE_MAP()

CDialogGeneral::CDialogGeneral(CClientDebugItemSinkDlg* pParent)
	: CDialogDexter(IDD_CUSTOM_GENERAL, pParent)
{

	m_wTableID = INVALID_WORD;
	ZeroMemory(m_szServerName, sizeof(m_szServerName));
	
}

CDialogGeneral::~CDialogGeneral()
{

}

void CDialogGeneral::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}

//��ʼ��
BOOL CDialogGeneral::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText(TEXT("���ɨ�׵��Զ�"));

	((CEdit *)GetDlgItem(IDC_EDIT_PARAMETER_K))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_ROOM_PARAMETER_K))->LimitText(2);
	((CEdit *)GetDlgItem(IDC_EDIT_USER_PARAMETER_K))->LimitText(2);

	CButton *pSystemStorageEnd = (CButton *)GetDlgItem(IDC_CHECK_SYS);
	CButton *pRoomStorageEnd = (CButton *)GetDlgItem(IDC_CHECK_ROOM);
	CButton *pUserStorageEnd = (CButton *)GetDlgItem(IDC_CHECK_USER);

	pSystemStorageEnd->SetCheck(BST_UNCHECKED);
	pRoomStorageEnd->SetCheck(BST_CHECKED);
	pUserStorageEnd->SetCheck(BST_CHECKED);

	//ˢ�¿��
	OnBnClickedButtonRefreshrule();
	return TRUE; 
}

HBRUSH CDialogGeneral::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}

void CDialogGeneral::OnTimer(UINT_PTR nIDEvent)
{
	__super::OnTimer(nIDEvent);
}	


BOOL CDialogGeneral::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			return true;
		}
	}

	return false;
}

//������ҵ���
void CDialogGeneral::DeleteUserDebugInfo(DWORD dwDebugIndex)
{
	if (dwDebugIndex == m_pClientDebugItemSinkDlg->m_dwUserDebugIndex)
	{
		//���·������
		UpdateUserDebugInfo(dwDebugIndex, 0, 0, DELETE_STATUS, 0);
	}
}

//������ҵ���
void CDialogGeneral::UpdateUserDebugInfo(tagHistoryRoomUserInfo *pHistoryRoomUserInfo)
{
	//���·������
	UpdateUserDebugInfo(pHistoryRoomUserInfo->dwDebugIndex, pHistoryRoomUserInfo->lSystemStorage, pHistoryRoomUserInfo->lUserStorage, pHistoryRoomUserInfo->nDebugStatus, pHistoryRoomUserInfo->lUpdateTime);
}

//������ҵ���
void CDialogGeneral::UpdateUserDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime)
{
	if (nDebugStatus == RUNNNING_STATUS)
	{
		CString strInfoL;
		CString strInfoR;
		CString strInfoRR;
		CStatic *pStaticInfoL = (CStatic *)GetDlgItem(IDC_STATIC_USER_INFO_L);
		CStatic *pStaticInfoR = (CStatic *)GetDlgItem(IDC_STATIC_USER_INFO_R);
		CStatic *pStaticInfoRR = (CStatic *)GetDlgItem(IDC_STATIC_USER_INFO_RR);

		strInfoL.Format(
			TEXT("��ǰ����ID��%d\n\n")
			TEXT("��ֹ��ȣ�%I64d\n\n")
			TEXT("ϵͳ��Ӯ������%I64d\n\n")
			TEXT("�����Ӯ������%I64d"),
			dwDebugIndex,
			m_pClientDebugItemSinkDlg->m_lUserEndStorageScore,
			m_pClientDebugItemSinkDlg->m_lUserTotalSystemStorage,
			m_pClientDebugItemSinkDlg->m_lUserTotalUserStorage);

		strInfoR.Format(
			TEXT("��ǰϵͳ��棺%I64d\n\n")
			TEXT("��ǰ��ҿ�棺%I64d\n\n")
			TEXT("��ǰ����ϵ����%d%%\n\n")
			TEXT("��ǰ�н��ʣ�%d%%"),
			lSystemStorage,
			lUserStorage,
			m_pClientDebugItemSinkDlg->m_nUserParameterK,
			CalcWinChance(lSystemStorage, lUserStorage, m_pClientDebugItemSinkDlg->m_nUserParameterK));

		strInfoRR.Format(
			TEXT("��ǰ������Ҹ�����%4d\n\n"),
			m_pClientDebugItemSinkDlg->m_dwUserDebugCount);

		pStaticInfoL->SetWindowText(strInfoL);
		pStaticInfoR->SetWindowText(strInfoR);
		pStaticInfoRR->SetWindowText(strInfoRR);
	}
	else
	{
		CString strInfoL;
		CString strInfoR;
		CString strInfoRR;
		CStatic *pStaticInfoL = (CStatic *)GetDlgItem(IDC_STATIC_USER_INFO_L);
		CStatic *pStaticInfoR = (CStatic *)GetDlgItem(IDC_STATIC_USER_INFO_R);
		CStatic *pStaticInfoRR = (CStatic *)GetDlgItem(IDC_STATIC_USER_INFO_RR);

		strInfoL.Format(
			TEXT("��ǰ����ID��\n\n")
			TEXT("��ֹ��ȣ�%I64d\n\n")
			TEXT("ϵͳ��Ӯ������%I64d\n\n")
			TEXT("�����Ӯ������%I64d"),
			m_pClientDebugItemSinkDlg->m_lUserEndStorageScore,
			m_pClientDebugItemSinkDlg->m_lUserTotalSystemStorage,
			m_pClientDebugItemSinkDlg->m_lUserTotalUserStorage);

		strInfoR.Format(
			TEXT("��ǰϵͳ��棺\n\n")
			TEXT("��ǰ��ҿ�棺\n\n")
			TEXT("��ǰ����ϵ����\n\n")
			TEXT("��ǰ�н��ʣ�"));

		strInfoRR.Format(
			TEXT("��ǰ������Ҹ�����%4d\n\n"),
			m_pClientDebugItemSinkDlg->m_dwUserDebugCount);

		pStaticInfoL->SetWindowText(strInfoL);
		pStaticInfoR->SetWindowText(strInfoR);
		pStaticInfoRR->SetWindowText(strInfoRR);
	}
}

//���·������
void CDialogGeneral::DeleteRoomDebugInfo(DWORD dwDebugIndex)
{
	if (m_pClientDebugItemSinkDlg->m_dwRoomDebugIndex == dwDebugIndex)
	{
		//���·������
		UpdateRoomDebugInfo(dwDebugIndex, 0, 0, DELETE_STATUS, 0);
	}
}

//���·������
void CDialogGeneral::UpdateRoomDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo)
{
	//���·������
	UpdateRoomDebugInfo(pHistoryRoomDebugInfo->dwDebugIndex, pHistoryRoomDebugInfo->lSystemStorage, pHistoryRoomDebugInfo->lUserStorage, pHistoryRoomDebugInfo->nDebugStatus, pHistoryRoomDebugInfo->lUpdateTime);
}

//���·������
void CDialogGeneral::UpdateRoomDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime)
{
	if (nDebugStatus == RUNNNING_STATUS)
	{
		CString strInfoL;
		CString strInfoR;
		CStatic *pStaticInfoL = (CStatic *)GetDlgItem(IDC_STATIC_ROOM_INFO_L);
		CStatic *pStaticInfoR = (CStatic *)GetDlgItem(IDC_STATIC_ROOM_INFO_R);

		strInfoL.Format(
			TEXT("��ǰ����ID��%d\n\n")
			TEXT("��ֹ��ȣ�%I64d\n\n")
			TEXT("ϵͳ��Ӯ������%I64d\n\n")
			TEXT("�����Ӯ������%I64d"),
			dwDebugIndex,
			m_pClientDebugItemSinkDlg->m_lRoomEndStorageScore,
			m_pClientDebugItemSinkDlg->m_lRoomTotalSystemStorage,
			m_pClientDebugItemSinkDlg->m_lRoomTotalUserStorage);

		strInfoR.Format(
			TEXT("��ǰϵͳ��棺%I64d\n\n")
			TEXT("��ǰ��ҿ�棺%I64d\n\n")
			TEXT("��ǰ����ϵ����%d%%\n\n")
			TEXT("��ǰ�н��ʣ�%d%%"),
			lSystemStorage,
			lUserStorage,
			m_pClientDebugItemSinkDlg->m_nRoomParameterK,
			CalcWinChance(lSystemStorage, lUserStorage, m_pClientDebugItemSinkDlg->m_nRoomParameterK));

		pStaticInfoL->SetWindowText(strInfoL);
		pStaticInfoR->SetWindowText(strInfoR);
	}
	else if (m_pClientDebugItemSinkDlg->m_dwRoomDebugIndex == 0)
	{
		CString strInfoL;
		CString strInfoR;
		CStatic *pStaticInfoL = (CStatic *)GetDlgItem(IDC_STATIC_ROOM_INFO_L);
		CStatic *pStaticInfoR = (CStatic *)GetDlgItem(IDC_STATIC_ROOM_INFO_R);

		strInfoL.Format(
			TEXT("\n\n")
			TEXT("��ֹ��ȣ�%I64d\n\n")
			TEXT("ϵͳ��Ӯ������%I64d\n\n")
			TEXT("�����Ӯ������%I64d"),
			/*dwDebugIndex,*/
			m_pClientDebugItemSinkDlg->m_lRoomEndStorageScore,
			m_pClientDebugItemSinkDlg->m_lRoomTotalSystemStorage,
			m_pClientDebugItemSinkDlg->m_lRoomTotalUserStorage);
			
// 		strInfoR.Format(
// 			TEXT("��ǰϵͳ��棺%I64d\n\n")
// 			TEXT("��ǰ��ҿ�棺%I64d\n\n")
// 			TEXT("��ǰ����ϵ����%d%%\n\n")
// 			TEXT("��ǰ�н��ʣ�%d%%"),
// 			lSystemStorage,
// 			lUserStorage,
// 			m_pClientDebugItemSinkDlg->m_nRoomParameterK,
// 			CalcWinChance(lSystemStorage, lUserStorage, m_pClientDebugItemSinkDlg->m_nRoomParameterK));

	

		pStaticInfoL->SetWindowText(strInfoL);
		pStaticInfoR->SetWindowText(strInfoR);
	}
}

//����ϵͳ����
void CDialogGeneral::UpdateSysDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo)
{
	if (pHistoryRoomDebugInfo->nDebugStatus == RUNNNING_STATUS)
	{
		//���·������
		UpdateSysDebugInfo(pHistoryRoomDebugInfo->dwDebugIndex, pHistoryRoomDebugInfo->lSystemStorage, pHistoryRoomDebugInfo->lUserStorage, pHistoryRoomDebugInfo->nDebugStatus, pHistoryRoomDebugInfo->lUpdateTime);
	}
}

//����ϵͳ����
void CDialogGeneral::UpdateSysDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime)
{
	CString strInfoL;
	CString strInfoR;
	CStatic *pStaticInfoL = (CStatic *)GetDlgItem(IDC_STATIC_SYSTEM_INFO_L);
	CStatic *pStaticInfoR = (CStatic *)GetDlgItem(IDC_STATIC_SYSTEM_INFO_R);
	

	strInfoL.Format(
		TEXT("������ô�����%d\n\n")
		TEXT("��ֹ��ȣ�%I64d\n\n")		
		TEXT("ϵͳ��Ӯ������%I64d\n\n")
		TEXT("�����Ӯ������%I64d"),
		m_pClientDebugItemSinkDlg->m_dwSysDebugIndex,
		m_pClientDebugItemSinkDlg->m_lEndStorageScore,
		m_pClientDebugItemSinkDlg->m_lTotalSystemStorage,
		m_pClientDebugItemSinkDlg->m_lTotalUserStorage);

	strInfoR.Format(
		TEXT("��ǰϵͳ��棺%I64d\n\n")
		TEXT("��ǰ��ҿ�棺%I64d\n\n")
		TEXT("��ǰ����ϵ����%d%%\n\n")
		TEXT("��ǰ�н��ʣ�%d%%"),
		lSystemStorage,
		lUserStorage,
		m_pClientDebugItemSinkDlg->m_nInitParameterK,
		CalcWinChance(lSystemStorage, lUserStorage, m_pClientDebugItemSinkDlg->m_nInitParameterK));

	pStaticInfoL->SetWindowText(strInfoL);
	pStaticInfoR->SetWindowText(strInfoR);
}

// ��Ϣ����
bool CDialogGeneral::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD wDataSize)
{
 	switch (nMessageID)
 	{
 	case SUB_GET_ROMM_ALL_USER_RESULT:
 	{
 		ASSERT(wDataSize == sizeof(CMD_CF_UserInfo));
 		if (wDataSize != sizeof(CMD_CF_UserInfo))
 		{
 			return false;
 		}
 
		//CopyMemory(&m_UserInfo, pData, sizeof(m_UserInfo));
 
 		return true;
 	}
	case SUB_S_DEBUG_TEXT:
	{
		ASSERT(wDataSize == sizeof(CMD_S_DebugText));
		if (wDataSize != sizeof(CMD_S_DebugText))
		{
			return false;
		}

		CMD_S_DebugText *pDebugText = (CMD_S_DebugText *)pData;

		// ��ʾ��Ϣ
		::MessageBox(GetSafeHwnd(), pDebugText->szMessageText, _T("������ʾ"), MB_OK);
		return true;
	}
	case SUB_S_DEL_ROOM_STORAGE:								//ɾ�����������
	{
		ASSERT(wDataSize == sizeof(CMD_S_DeleteStorageInfo));
		if (wDataSize != sizeof(CMD_S_DeleteStorageInfo))
		{
			return false;
		}

		CMD_S_DeleteStorageInfo *pDeleteStorageInfo = (CMD_S_DeleteStorageInfo *)pData;

		// ������Ϣ
		CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM);
		CString strInfo;
		int nItemIndex = -1;

		strInfo.Format(TEXT("%d"), pDeleteStorageInfo->dwDebugIndex);
		for (int j = 0; j < pListCtrlRoom->GetItemCount(); j++)
		{
			if (strInfo == pListCtrlRoom->GetItemText(j, 0))
			{
				pListCtrlRoom->DeleteItem(j);
				break;
			}
		}

		return true;
	}
	case SUB_S_DEL_USER_STORAGE:								//ɾ����ҿ�����
	{
		ASSERT(wDataSize == sizeof(CMD_S_DeleteStorageInfo));
		if (wDataSize != sizeof(CMD_S_DeleteStorageInfo))
		{
			return false;
		}

		CMD_S_DeleteStorageInfo *pDeleteStorageInfo = (CMD_S_DeleteStorageInfo *)pData;

		// ������Ϣ
		CListCtrl *pListCtrlUser = (CListCtrl *)GetDlgItem(IDC_LIST_USER);
		CString strInfo;

		strInfo.Format(TEXT("%d"), pDeleteStorageInfo->dwDebugIndex);
		for (int j = 0; j < pListCtrlUser->GetItemCount(); j++)
		{
			if (strInfo == pListCtrlUser->GetItemText(j, 0))
			{
				pListCtrlUser->DeleteItem(j);
				break;
			}
		}

		return true;
	}
 	}
 	return false;
}

//ˢ�¿��
void CDialogGeneral::OnBnClickedButtonRefreshrule()
{
	// ������Ϣ
	SendDebugMessage(SUB_C_REFRESH_STORAGE);
}

//ϵͳ���仯
void CDialogGeneral::OnEnChangeEditSystemStorage()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_SYSTEM_STORAGE, strValue);
	LONGLONG lSystemStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_USER_STORAGE, strValue);
	LONGLONG lUserStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_PARAMETER_K, strValue);
	int nParameterK = _ttoi(strValue);

	strValue.Format(TEXT("�н��ʣ�%d%%"), CalcWinChance(lSystemStorage, lUserStorage, nParameterK));
	SetDlgItemText(IDC_STATIC_ONE_THERE, strValue);
}

//��ҿ��仯
void CDialogGeneral::OnEnChangeEditUserStorage()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_SYSTEM_STORAGE, strValue);
	LONGLONG lSystemStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_USER_STORAGE, strValue);
	LONGLONG lUserStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_PARAMETER_K, strValue);
	int nParameterK = _ttoi(strValue);

	WORD wUserWinChance = CalcWinChance(lSystemStorage, lUserStorage, nParameterK);
	strValue.Format(TEXT("�н��ʣ�%d%%"), wUserWinChance);
	SetDlgItemText(IDC_STATIC_ONE_THERE, strValue);
}

//����ϵ���仯
void CDialogGeneral::OnEnChangeEditParameterK()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_SYSTEM_STORAGE, strValue);
	LONGLONG lSystemStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_USER_STORAGE, strValue);
	LONGLONG lUserStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_PARAMETER_K, strValue);
	int nParameterK = _ttoi(strValue);

	WORD wUserWinChance = CalcWinChance(lSystemStorage, lUserStorage, nParameterK);
	strValue.Format(TEXT("�н��ʣ�%d%%"), wUserWinChance);
	SetDlgItemText(IDC_STATIC_ONE_THERE, strValue);
}

//���õ�ǰ���
void CDialogGeneral::OnBnClickedButtonCurStorage()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_SYSTEM_STORAGE, strValue);
	LONGLONG lSystemStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_USER_STORAGE, strValue);
	LONGLONG lUserStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_PARAMETER_K, strValue);
	int nParameterK = _ttoi(strValue);

	if (lSystemStorage < 0 || lUserStorage < 0)
	{
		// ��ʾ��Ϣ
		::MessageBox( GetSafeHwnd(), _T("��治��С��0,��������Ч������"), _T("��ܰ��ʾ"), MB_OK );
		return;
	}

	if (nParameterK <= 0 || nParameterK >= 100)
	{
		// ��ʾ��Ϣ
		MessageBox( _T("����ϵ������С�ڵ���0����ڵ���100,��������Ч������"), _T("��ܰ��ʾ"), MB_OK);
		return;
	}

	CButton *pSystemStorageEnd = (CButton *)GetDlgItem(IDC_CHECK_SYS);

	//�������
	CMD_C_SetCurStorage SetCurStorage;
	SetCurStorage.lSystemStorage = lSystemStorage;
	SetCurStorage.lUserStorage = lUserStorage;
	SetCurStorage.nParameterK = nParameterK;
	SetCurStorage.bSystemStorageEnd = pSystemStorageEnd->GetCheck() == BST_CHECKED;

	// ������Ϣ
	SendDebugMessage(SUB_C_SET_CUR_STORAGE, &SetCurStorage, sizeof(SetCurStorage));
}

//���ó�ʼ���
void CDialogGeneral::OnBnClickedButtonInitStorage()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_SYSTEM_STORAGE, strValue);
	LONGLONG lSystemStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_USER_STORAGE, strValue);
	LONGLONG lUserStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_PARAMETER_K, strValue);
	int nParameterK = _ttoi(strValue);

	if (lSystemStorage < 0 || lUserStorage < 0)
	{
		// ��ʾ��Ϣ
		MessageBox( _T("��治��С��0,��������Ч������"), _T("��ܰ��ʾ"), MB_OK);
		return;
	}

	if (nParameterK <= 0 || nParameterK >= 100)
	{
		// ��ʾ��Ϣ
		MessageBox( _T("����ϵ������С�ڵ���0����ڵ���100,��������Ч������"), _T("��ܰ��ʾ"), MB_OK);
		return;
	}

	//�������
	CMD_C_SetCurStorage SetCurStorage;
	SetCurStorage.lSystemStorage = lSystemStorage;
	SetCurStorage.lUserStorage = lUserStorage;
	SetCurStorage.nParameterK = nParameterK;

	// ������Ϣ
	SendDebugMessage(SUB_C_SET_INIT_STORAGE, &SetCurStorage, sizeof(SetCurStorage));
}

//ϵͳ���仯
void CDialogGeneral::OnEnChangeEditRoomSystemStorage()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_ROOM_SYSTEM_STORAGE, strValue);
	LONGLONG lSystemStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_ROOM_USER_STORAGE, strValue);
	LONGLONG lUserStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_ROOM_PARAMETER_K, strValue);
	int nParameterK = _ttoi(strValue);

	strValue.Format(TEXT("�н��ʣ�%d%%"), CalcWinChance(lSystemStorage, lUserStorage, nParameterK));
	SetDlgItemText(IDC_STATIC_ROOM, strValue);
}

//��ҿ��仯
void CDialogGeneral::OnEnChangeEditRoomUserStorage()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_ROOM_SYSTEM_STORAGE, strValue);
	LONGLONG lSystemStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_ROOM_USER_STORAGE, strValue);
	LONGLONG lUserStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_ROOM_PARAMETER_K, strValue);
	int nParameterK = _ttoi(strValue);

	strValue.Format(TEXT("�н��ʣ�%d%%"), CalcWinChance(lSystemStorage, lUserStorage, nParameterK));
	SetDlgItemText(IDC_STATIC_ROOM, strValue);
}

//����ϵ���仯
void CDialogGeneral::OnEnChangeEditRoomParameterK()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_ROOM_SYSTEM_STORAGE, strValue);
	LONGLONG lSystemStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_ROOM_USER_STORAGE, strValue);
	LONGLONG lUserStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_ROOM_PARAMETER_K, strValue);
	int nParameterK = _ttoi(strValue);

	strValue.Format(TEXT("�н��ʣ�%d%%"), CalcWinChance(lSystemStorage, lUserStorage, nParameterK));
	SetDlgItemText(IDC_STATIC_ROOM, strValue);
}

//ˢ�·�����
void CDialogGeneral::OnBnClickedBtnRefreshRoomList()
{
	GetDlgItem(IDC_BTN_REFRESH_ROOM_LIST)->EnableWindow(FALSE);
	CListCtrl *pListCtrlUser = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM);
	for (int i = 0; i < pListCtrlUser->GetItemCount(); i++)
	{
		pListCtrlUser->SetItemData(i, -1);
	}

	// ������Ϣ
	SendDebugMessage(SUB_C_REFRESH_ROOM_STORAGE);
}

//���ÿ��
void CDialogGeneral::OnBnClickedBtnSetRoomStorage()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_ROOM_SYSTEM_STORAGE, strValue);
	LONGLONG lSystemStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_ROOM_USER_STORAGE, strValue);
	LONGLONG lUserStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_ROOM_PARAMETER_K, strValue);
	int nParameterK = _ttoi(strValue);

	if (lSystemStorage < 0 || lUserStorage < 0)
	{
		// ��ʾ��Ϣ
		MessageBox( _T("��治��С��0,��������Ч������"), _T("��ܰ��ʾ"), MB_OK);
		return;
	}

	if (nParameterK <= 0 || nParameterK >= 100)
	{
		// ��ʾ��Ϣ
		MessageBox( _T("����ϵ������С�ڵ���0����ڵ���100,��������Ч������"), _T("��ܰ��ʾ"), MB_OK);
		return;
	}

	CButton *pRoomStorageEnd = (CButton *)GetDlgItem(IDC_CHECK_ROOM);

	//�������
	CMD_C_SetRoomStorage SetRoomStorage;
	SetRoomStorage.lSystemStorage = lSystemStorage;
	SetRoomStorage.lUserStorage = lUserStorage;
	SetRoomStorage.nParameterK = nParameterK;
	SetRoomStorage.bSystemStorageEnd = pRoomStorageEnd->GetCheck() == BST_CHECKED;;

	// ������Ϣ
	SendDebugMessage(SUB_C_SET_ROOM_STORAGE, &SetRoomStorage, sizeof(SetRoomStorage));
}

//ȡ��������
void CDialogGeneral::OnBnClickedBtnCancelRoomStorage()
{	
	CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM);
	POSITION pos = pListCtrlRoom->GetFirstSelectedItemPosition();

	if (!pos)
	{
		MessageBox(_T("��ѡ��ĳ��"), _T("��ܰ��ʾ"), MB_OK);
		return;
	}

	int nItem = pListCtrlRoom->GetNextSelectedItem(pos);

	//�������
	CMD_C_CancelRoomStorage CancelRoomStorage;
	CancelRoomStorage.dwDebugIndex = _ttoi(pListCtrlRoom->GetItemText(nItem, 0));

	// ������Ϣ
	SendDebugMessage(SUB_C_CANCEL_ROOM_STORAGE, &CancelRoomStorage, sizeof(CancelRoomStorage));
}

//���������б�
void CDialogGeneral::OnLvnItemchangedListRoom(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if ((pNMLV->uChanged == LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED))
	{
		//��������
		int nItem = pNMLV->iItem;
		if (nItem >= 0)
		{
			CListCtrl *pListRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM);

			SetDlgItemText(IDC_EDIT_ROOM_SYSTEM_STORAGE, pListRoom->GetItemText(nItem, 1));
			SetDlgItemText(IDC_EDIT_ROOM_USER_STORAGE, pListRoom->GetItemText(nItem, 2));
			SetDlgItemText(IDC_EDIT_ROOM_PARAMETER_K, pListRoom->GetItemText(nItem, 3));

			//����ϵ���仯
			OnEnChangeEditRoomParameterK();
		}
	}

	*pResult = 0;
}

//ϵͳ���仯
void CDialogGeneral::OnEnChangeEditUserSystemStorage()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_USER_SYSTEM_STORAGE, strValue);
	LONGLONG lSystemStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_USER_USER_STORAGE, strValue);
	LONGLONG lUserStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_USER_PARAMETER_K, strValue);
	int nParameterK = _ttoi(strValue);

	strValue.Format(TEXT("�н��ʣ�%d%%"), CalcWinChance(lSystemStorage, lUserStorage, nParameterK));
	SetDlgItemText(IDC_STATIC_USER, strValue);
}

//��ҿ��仯
void CDialogGeneral::OnEnChangeEditUserUserStorage()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_USER_SYSTEM_STORAGE, strValue);
	LONGLONG lSystemStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_USER_USER_STORAGE, strValue);
	LONGLONG lUserStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_USER_PARAMETER_K, strValue);
	int nParameterK = _ttoi(strValue);

	strValue.Format(TEXT("�н��ʣ�%d%%"), CalcWinChance(lSystemStorage, lUserStorage, nParameterK));
	SetDlgItemText(IDC_STATIC_USER, strValue);
}

//����ϵ���仯
void CDialogGeneral::OnEnChangeEditUserParameterK()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_USER_SYSTEM_STORAGE, strValue);
	LONGLONG lSystemStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_USER_USER_STORAGE, strValue);
	LONGLONG lUserStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_USER_PARAMETER_K, strValue);
	int nParameterK = _ttoi(strValue);

	strValue.Format(TEXT("�н��ʣ�%d%%"), CalcWinChance(lSystemStorage, lUserStorage, nParameterK));
	SetDlgItemText(IDC_STATIC_USER, strValue);
}

//ˢ����ҿ��
void CDialogGeneral::OnBnClickedBtnRefreshUserList()
{
	GetDlgItem(IDC_BTN_REFRESH_USER_LIST)->EnableWindow(FALSE);
	CListCtrl *pListCtrlUser = (CListCtrl *)GetDlgItem(IDC_LIST_USER);
	for (int i = 0; i < pListCtrlUser->GetItemCount(); i++)
	{
		pListCtrlUser->SetItemData(i, -1);
	}

	// ������Ϣ
	SendDebugMessage(SUB_C_REFRESH_USER_STORAGE);
}

//������ҿ��
void CDialogGeneral::OnBnClickedBtnSetUserStorage()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_USER_SYSTEM_STORAGE, strValue);
	LONGLONG lSystemStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_USER_USER_STORAGE, strValue);
	LONGLONG lUserStorage = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_USER_PARAMETER_K, strValue);
	int nParameterK = _ttoi(strValue);

	DWORD dwGameID = GetDlgItemInt(IDC_EDIT_USER_GAME_ID);
	if (dwGameID == 0)
	{
		MessageBox(TEXT("����д��ȷ����ұ�ʶ��GameID����"), TEXT("��ܰ��ʾ"), MB_OK);
		return;
	}

	if (lSystemStorage < 0 || lUserStorage < 0)
	{
		// ��ʾ��Ϣ
		MessageBox( _T("��治��С��0,��������Ч������"), _T("��ܰ��ʾ"), MB_OK);
		return;
	}

	if (nParameterK <= 0 || nParameterK >= 100)
	{
		// ��ʾ��Ϣ
		MessageBox( _T("����ϵ������С�ڵ���0����ڵ���100,��������Ч������"), _T("��ܰ��ʾ"), MB_OK);
		return;
	}

	//�������
	CMD_C_SetUserStorage SetUserStorage;
	SetUserStorage.dwGameID = dwGameID;
	SetUserStorage.lSystemStorage = lSystemStorage;
	SetUserStorage.lUserStorage = lUserStorage;
	SetUserStorage.nParameterK = nParameterK;

	// ������Ϣ
	SendDebugMessage(SUB_C_SET_USER_STORAGE, &SetUserStorage, sizeof(SetUserStorage));
}

//ȡ����ҿ��
void CDialogGeneral::OnBnClickedBtnCancelUserStorage()
{
	DWORD dwGameID = GetDlgItemInt(IDC_EDIT_USER_GAME_ID);
	if (dwGameID == 0)
	{
		MessageBox(TEXT("����д��ȷ����ұ�ʶ��GameID����"), TEXT("��ܰ��ʾ"), MB_OK);
		return;
	}
	
	
	CListCtrl *pListCtrlUser = (CListCtrl *)GetDlgItem(IDC_LIST_USER);
	POSITION pos = pListCtrlUser->GetFirstSelectedItemPosition();
	if (!pos)
	{
		MessageBox(_T("��ѡ��ĳ��"), _T("��ܰ��ʾ"), MB_OK);
		return;
	}
	
	int nItem = pListCtrlUser->GetNextSelectedItem(pos);


	//�������
	CMD_C_CancelUserStorage CancelUserStorage;
	CancelUserStorage.dwGameID = dwGameID;

	// ������Ϣ
	SendDebugMessage(SUB_C_CANCEL_USER_STORAGE, &CancelUserStorage, sizeof(CancelUserStorage));
}

//��������б�
void CDialogGeneral::OnLvnItemchangedListUser(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if ((pNMLV->uChanged == LVIF_STATE) && (pNMLV->uNewState & LVIS_SELECTED))
	{
		//��������
		int nItem = pNMLV->iItem;
		if (nItem >= 0)
		{
			CListCtrl *pListUser = (CListCtrl *)GetDlgItem(IDC_LIST_USER);
			SetDlgItemText(IDC_EDIT_USER_GAME_ID, pListUser->GetItemText(nItem, 0));
			SetDlgItemText(IDC_EDIT_USER_SYSTEM_STORAGE, pListUser->GetItemText(nItem, 1));
			SetDlgItemText(IDC_EDIT_USER_USER_STORAGE, pListUser->GetItemText(nItem, 2));
			SetDlgItemText(IDC_EDIT_USER_PARAMETER_K, pListUser->GetItemText(nItem, 3));

			//����ϵ���仯
			OnEnChangeEditUserParameterK();
		}
	}

	*pResult = 0;
}

void CDialogGeneral::OnBnClickedButtonCurCaiJing()
{
	CString strValue;
	GetDlgItemText(IDC_EDIT_CAI_JING_TAX, strValue);
	int nCaiJingTax = _ttoi(strValue);

	GetDlgItemText(IDC_EDIT_VIRTUAL_CAI_JING, strValue);
	LONGLONG lVirtualCaiJing = _ttoi64(strValue);

	GetDlgItemText(IDC_EDIT_PAI_CAI_START, strValue);
	LONGLONG lPaiCaiStartScore = _ttoi64(strValue);

	if (lVirtualCaiJing < 0 || lPaiCaiStartScore < 0)
	{
		// ��ʾ��Ϣ
		MessageBox(_T("����ʽ���ɲ���㲻��С��0,��������Ч������"), _T("��ܰ��ʾ"), MB_OK);
		return;
	}

	if (nCaiJingTax < 0 || nCaiJingTax >= 1000)
	{
		// ��ʾ��Ϣ
		MessageBox(_T("�ʽ��ˮ����С��0����ڵ���1000,��������Ч������"), _T("��ܰ��ʾ"), MB_OK);
		return;
	}

// 	//�������
// 	CMD_C_SetCaiJing SetCaiJing;
// 	SetCaiJing.lVirtualCaiJing = lVirtualCaiJing;
// 	SetCaiJing.lPaiCaiStartScore = lPaiCaiStartScore;
// 	SetCaiJing.nCaiJingTax = nCaiJingTax;
// 
// 	// ������Ϣ
// 	SendDebugMessage(SUB_C_SET_CAI_JING, &SetCaiJing, sizeof(SetCaiJing));
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CDialogAdvanced, CDialogDexter)
BEGIN_MESSAGE_MAP(CDialogAdvanced, CDialogDexter)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH_HISTORY, &CDialogAdvanced::OnBnClickedButtonRefreshHistory)
	ON_BN_CLICKED(IDC_BUTTON_ROOM_CANCEL, &CDialogAdvanced::OnBnClickedButtonRoomCancel)
	ON_BN_CLICKED(IDC_BUTTON_USER_CANCEL, &CDialogAdvanced::OnBnClickedButtonUserCancel)
END_MESSAGE_MAP()

// ���캯��
CDialogAdvanced::CDialogAdvanced(CClientDebugItemSinkDlg *pParent /*=NULL*/) : CDialogDexter(IDD_CUSTOM_ADVANCED, pParent)
{
}

// ��������
CDialogAdvanced::~CDialogAdvanced()
{
}

// �ؼ���
void CDialogAdvanced::DoDataExchange(CDataExchange *pDX)
{
	CDialogDexter::DoDataExchange(pDX);
}

// ��Ϣ����
bool CDialogAdvanced::OnDebugMessage(WORD nMessageID, WORD wTableID, void *pData, WORD nSize)
{
	switch (nMessageID)
	{
	case  SUB_S_REFRESH_HISTORY_USER_DEBUG:					//ˢ���û��б�
	{
		ASSERT(nSize % sizeof(tagHistoryRoomUserInfo) == 0);
		if (nSize % sizeof(tagHistoryRoomUserInfo) != 0)
		{
			return false;
		}

		// ������Ϣ
		CListCtrl *pListCtrlUser = (CListCtrl *)GetDlgItem(IDC_LIST_USER_HISTORY);
		CString strInfo;
		int nItemIndex = -1;
		tagHistoryRoomUserInfo *pRoomUserInfo = (tagHistoryRoomUserInfo *)pData;

		for (WORD i = 0; i < nSize / sizeof(tagHistoryRoomUserInfo); i++)
		{
			nItemIndex = -1;
			strInfo.Format(TEXT("%d"), pRoomUserInfo->dwGameID);
			nItemIndex = pListCtrlUser->InsertItem(pListCtrlUser->GetItemCount(), strInfo);

			CString strCtrlVal;
			strCtrlVal.Format(TEXT("%I64d"), pRoomUserInfo->lSystemStorage);
			pListCtrlUser->SetItemText(nItemIndex, 1, strCtrlVal);

			strCtrlVal.Format(TEXT("%I64d"), pRoomUserInfo->lUserStorage);
			pListCtrlUser->SetItemText(nItemIndex, 2, strCtrlVal);

			strCtrlVal.Format(TEXT("%d"), pRoomUserInfo->nParameterK);
			pListCtrlUser->SetItemText(nItemIndex, 3, strCtrlVal);

			pRoomUserInfo++;
			nItemIndex++;
			pListCtrlUser->SetItemData(i, nItemIndex);
		}

		return true;
	}
	case  SUB_S_REFRESH_HISTORY_ROOM_DEBUG:
	{
		ASSERT(nSize % sizeof(tagHistoryRoomDebugInfo) == 0);
		if (nSize % sizeof(tagHistoryRoomDebugInfo) != 0)
		{
			return false;
		}

		// ������Ϣ
		CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM_HISTORY);
		CString strInfo;
		int nItemIndex = -1;
		tagHistoryRoomDebugInfo *pRoomDebugInfo = (tagHistoryRoomDebugInfo *)pData;

		for (WORD i = 0; i < nSize / sizeof(tagHistoryRoomDebugInfo); i++)
		{
			CString strCtrlIndex;
			strCtrlIndex.Format(TEXT("%d"), pRoomDebugInfo->dwDebugIndex);

			int nItem = pListCtrlRoom->GetItemCount();
			nItemIndex = pListCtrlRoom->InsertItem(nItem, strCtrlIndex);

			CString strCtrlVal;
			strCtrlVal.Format(TEXT("%I64d"), pRoomDebugInfo->lSystemStorage);
			pListCtrlRoom->SetItemText(nItem, 1, strCtrlVal);

			strCtrlVal.Format(TEXT("%I64d"), pRoomDebugInfo->lUserStorage);
			pListCtrlRoom->SetItemText(nItem, 2, strCtrlVal);

			strCtrlVal.Format(TEXT("%d"), pRoomDebugInfo->nParameterK);
			pListCtrlRoom->SetItemText(nItem, 3, strCtrlVal);

			pRoomDebugInfo++;
		}

		return true;
	}
	case  SUB_S_REFRESH_HISTORY_SYS_DEBUG:
	{
		ASSERT(nSize % sizeof(tagHistoryRoomDebugInfo) == 0);
		if (nSize % sizeof(tagHistoryRoomDebugInfo) != 0)
		{
			return false;
		}

		// ������Ϣ
		CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_SYS_HISTORY);
		CString strInfo;
		int nItemIndex = -1;
		tagHistoryRoomDebugInfo *pRoomDebugInfo = (tagHistoryRoomDebugInfo *)pData;

		for (WORD i = 0; i < nSize / sizeof(tagHistoryRoomDebugInfo); i++)
		{
			CString strCtrlIndex;
			strCtrlIndex.Format(TEXT("%d"), pRoomDebugInfo->dwDebugIndex);

			int nItem = pListCtrlRoom->GetItemCount();
			nItemIndex = pListCtrlRoom->InsertItem(nItem, strCtrlIndex);

			CString strCtrlVal;
			strCtrlVal.Format(TEXT("%I64d"), pRoomDebugInfo->lSystemStorage);
			pListCtrlRoom->SetItemText(nItem, 1, strCtrlVal);

			strCtrlVal.Format(TEXT("%I64d"), pRoomDebugInfo->lUserStorage);
			pListCtrlRoom->SetItemText(nItem, 2, strCtrlVal);

			strCtrlVal.Format(TEXT("%d"), pRoomDebugInfo->nParameterK);
			pListCtrlRoom->SetItemText(nItem, 3, strCtrlVal);

			pRoomDebugInfo++;
		}

		return true;
	}
	default:
		break;

	}
	return true;
}

//������ҵ���
void CDialogAdvanced::DeleteUserDebugInfo(DWORD dwDebugIndex)
{
	CString strInfo;
	CListCtrl *pListCtrlUser = (CListCtrl *)GetDlgItem(IDC_LIST_USER_HISTORY);

	strInfo.Format(TEXT("%d"), dwDebugIndex);
	for (int i = 0; i < pListCtrlUser->GetItemCount(); i++)
	{
		if (strInfo == pListCtrlUser->GetItemText(i, 0))
		{
			pListCtrlUser->DeleteItem(i);
			break;
		}
	}
}

//������ҵ���
void CDialogAdvanced::UpdateUserDebugInfo(tagHistoryRoomUserInfo *pHistoryRoomUserInfo)
{
	CString strInfo;

	CStatic *pStaticRoomInfo = (CStatic *)GetDlgItem(IDC_STATIC_USER_INFO);
	strInfo.Format(TEXT("ϵͳ��Ӯ������%I64d\t\t\t�����Ӯ������%I64d"),
		m_pClientDebugItemSinkDlg->m_lUserTotalSystemStorage, m_pClientDebugItemSinkDlg->m_lUserTotalUserStorage);
	pStaticRoomInfo->SetWindowText(strInfo);

	DWORD dwDebugIndexItem = 0;
	int nItemInsertIndex = 0;
	bool bAddItem = true;
	CListCtrl *pListCtrlUser = (CListCtrl *)GetDlgItem(IDC_LIST_USER_HISTORY);
	for (int i = 0; i < pListCtrlUser->GetItemCount(); i++)
	{
		dwDebugIndexItem = _ttoi(pListCtrlUser->GetItemText(i, 0));
		if (pHistoryRoomUserInfo->dwDebugIndex == dwDebugIndexItem)
		{
			nItemInsertIndex = i;
			bAddItem = false;
			break;
		}
		if (pHistoryRoomUserInfo->dwDebugIndex < dwDebugIndexItem)
		{
			nItemInsertIndex = i + 1;
			break;
		}
	}

	if (bAddItem)
	{
		strInfo.Format(TEXT("%d"), pHistoryRoomUserInfo->dwDebugIndex);
		pListCtrlUser->InsertItem(nItemInsertIndex, strInfo);
	}

	strInfo.Format(TEXT("%d"), pHistoryRoomUserInfo->dwGameID);
	pListCtrlUser->SetItemText(nItemInsertIndex, 1, strInfo);

	strInfo.Format(TEXT("%I64d"), pHistoryRoomUserInfo->lSystemStorage);
	pListCtrlUser->SetItemText(nItemInsertIndex, 2, strInfo);

	strInfo.Format(TEXT("%I64d"), pHistoryRoomUserInfo->lUserStorage);
	pListCtrlUser->SetItemText(nItemInsertIndex, 3, strInfo);

	strInfo.Format(TEXT("%d"), pHistoryRoomUserInfo->nParameterK);
	pListCtrlUser->SetItemText(nItemInsertIndex, 4, strInfo);

	strInfo.Format(TEXT("%I64d"), (pHistoryRoomUserInfo->bSystemStorageEnd && pHistoryRoomUserInfo->nDebugStatus == FINISH_STATUS) ? pHistoryRoomUserInfo->lUserStorage : 0);
	pListCtrlUser->SetItemText(nItemInsertIndex, 5, strInfo);

	strInfo.Format(TEXT("%I64d"), pHistoryRoomUserInfo->lInitSystemStorage - pHistoryRoomUserInfo->lSystemStorage);
	pListCtrlUser->SetItemText(nItemInsertIndex, 6, strInfo);

	strInfo.Format(TEXT("%I64d"), pHistoryRoomUserInfo->lInitUserStorage - pHistoryRoomUserInfo->lUserStorage);
	pListCtrlUser->SetItemText(nItemInsertIndex, 7, strInfo);

	CTime time = CTime(pHistoryRoomUserInfo->lDebugTime);
	strInfo.Format(TEXT("%02d-%02d %02d:%02d"), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute());
	pListCtrlUser->SetItemText(nItemInsertIndex, 8, strInfo);

	strInfo = GetDebugStatus(pHistoryRoomUserInfo->nDebugStatus);
	pListCtrlUser->SetItemText(nItemInsertIndex, 9, strInfo);

	//����ϵͳ������
	UpdateAllDebugInfo();
}

//������ҵ���
void CDialogAdvanced::UpdateUserDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime)
{
	CString strInfo;

	CStatic *pStaticUserInfo = (CStatic *)GetDlgItem(IDC_STATIC_USER_INFO);
	strInfo.Format(TEXT("ϵͳ��Ӯ������%I64d\t\t\t�����Ӯ������%I64d"),
		m_pClientDebugItemSinkDlg->m_lUserTotalSystemStorage, m_pClientDebugItemSinkDlg->m_lUserTotalUserStorage);
	pStaticUserInfo->SetWindowText(strInfo);

	int nItemInsertIndex = -1;
	CListCtrl *pListCtrlUser = (CListCtrl *)GetDlgItem(IDC_LIST_USER_HISTORY);
	strInfo.Format(TEXT("%d"), dwDebugIndex);
	for (int i = 0; i < pListCtrlUser->GetItemCount(); i++)
	{
		if (strInfo == pListCtrlUser->GetItemText(i, 0))
		{
			nItemInsertIndex = i;
			break;
		}

	}
	if (nItemInsertIndex != -1)
	{
		//���µ���
		tagHistoryRoomUserInfo HistoryRoomUserInfo;

		//ȡ��ϵͳ����
		POSITION posCur = m_pClientDebugItemSinkDlg->GetUserDebugInfo(dwDebugIndex, HistoryRoomUserInfo);
		if (posCur)
		{
			strInfo.Format(TEXT("%I64d"), lSystemStorage);
			pListCtrlUser->SetItemText(nItemInsertIndex, 2, strInfo);

			strInfo.Format(TEXT("%I64d"), lUserStorage);
			pListCtrlUser->SetItemText(nItemInsertIndex, 3, strInfo);

			strInfo.Format(TEXT("%I64d"), (HistoryRoomUserInfo.bSystemStorageEnd && nDebugStatus == FINISH_STATUS) ? lUserStorage : 0);
			pListCtrlUser->SetItemText(nItemInsertIndex, 5, strInfo);

			strInfo.Format(TEXT("%I64d"), HistoryRoomUserInfo.lInitSystemStorage - lSystemStorage);
			pListCtrlUser->SetItemText(nItemInsertIndex, 6, strInfo);

			strInfo.Format(TEXT("%I64d"), HistoryRoomUserInfo.lInitUserStorage - lUserStorage);
			pListCtrlUser->SetItemText(nItemInsertIndex, 7, strInfo);

			strInfo = GetDebugStatus(nDebugStatus);
			pListCtrlUser->SetItemText(nItemInsertIndex, 9, strInfo);
		}
	}

	//����ϵͳ������
	UpdateAllDebugInfo();
}

//���·������
void CDialogAdvanced::DeleteRoomDebugInfo(DWORD dwDebugIndex)
{
	CString strInfo;
	CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM_HISTORY);

	strInfo.Format(TEXT("%d"), dwDebugIndex);
	for (int i = 0; i < pListCtrlRoom->GetItemCount(); i++)
	{
		if (strInfo == pListCtrlRoom->GetItemText(i, 0))
		{
			pListCtrlRoom->DeleteItem(i);
			break;
		}
	}
}

//���·������
void CDialogAdvanced::UpdateRoomDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo)
{
	CString strInfo;

	CStatic *pStaticRoomInfo = (CStatic *)GetDlgItem(IDC_STATIC_ROOM_INFO);
	strInfo.Format(TEXT("ϵͳ��Ӯ������%I64d\t\t\t�����Ӯ������%I64d"),
		m_pClientDebugItemSinkDlg->m_lRoomTotalSystemStorage, m_pClientDebugItemSinkDlg->m_lRoomTotalUserStorage);
	pStaticRoomInfo->SetWindowText(strInfo);

	DWORD dwDebugIndexItem = 0;
	int nItemInsertIndex = 0;
	bool bAddItem = true;
	CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM_HISTORY);
	for (int i = 0; i < pListCtrlRoom->GetItemCount(); i++)
	{
		dwDebugIndexItem = _ttoi(pListCtrlRoom->GetItemText(i, 0));
		if (pHistoryRoomDebugInfo->dwDebugIndex == dwDebugIndexItem)
		{
			nItemInsertIndex = i;
			bAddItem = false;
			break;
		}
		if (pHistoryRoomDebugInfo->dwDebugIndex < dwDebugIndexItem)
		{
			nItemInsertIndex = i + 1;
			break;
		}
	}

	if (bAddItem)
	{
		strInfo.Format(TEXT("%d"), pHistoryRoomDebugInfo->dwDebugIndex);
		pListCtrlRoom->InsertItem(nItemInsertIndex, strInfo);
	}

	strInfo.Format(TEXT("%I64d"), pHistoryRoomDebugInfo->lSystemStorage);
	pListCtrlRoom->SetItemText(nItemInsertIndex, 1, strInfo);

	strInfo.Format(TEXT("%I64d"), pHistoryRoomDebugInfo->lUserStorage);
	pListCtrlRoom->SetItemText(nItemInsertIndex, 2, strInfo);

	strInfo.Format(TEXT("%d"), pHistoryRoomDebugInfo->nParameterK);
	pListCtrlRoom->SetItemText(nItemInsertIndex, 3, strInfo);

	strInfo.Format(TEXT("%I64d"), (pHistoryRoomDebugInfo->bSystemStorageEnd && pHistoryRoomDebugInfo->nDebugStatus == FINISH_STATUS) ? pHistoryRoomDebugInfo->lUserStorage : 0);
	pListCtrlRoom->SetItemText(nItemInsertIndex, 4, strInfo);

	strInfo.Format(TEXT("%I64d"), pHistoryRoomDebugInfo->lInitSystemStorage - pHistoryRoomDebugInfo->lSystemStorage);
	pListCtrlRoom->SetItemText(nItemInsertIndex, 5, strInfo);

	strInfo.Format(TEXT("%I64d"), pHistoryRoomDebugInfo->lInitUserStorage - pHistoryRoomDebugInfo->lUserStorage);
	pListCtrlRoom->SetItemText(nItemInsertIndex, 6, strInfo);

	CTime time = CTime(pHistoryRoomDebugInfo->lDebugTime);
	strInfo.Format(TEXT("%02d-%02d %02d:%02d"), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute());
	pListCtrlRoom->SetItemText(nItemInsertIndex, 7, strInfo);

	strInfo = GetDebugStatus(pHistoryRoomDebugInfo->nDebugStatus);
	pListCtrlRoom->SetItemText(nItemInsertIndex, 8, strInfo);

	//����ϵͳ������
	UpdateAllDebugInfo();
}

//���·������
void CDialogAdvanced::UpdateRoomDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime)
{
	CString strInfo;

	CStatic *pStaticRoomInfo = (CStatic *)GetDlgItem(IDC_STATIC_ROOM_INFO);
	strInfo.Format(TEXT("ϵͳ��Ӯ������%I64d\t\t\t�����Ӯ������%I64d"),
		m_pClientDebugItemSinkDlg->m_lRoomTotalSystemStorage, m_pClientDebugItemSinkDlg->m_lRoomTotalUserStorage);
	pStaticRoomInfo->SetWindowText(strInfo);

	int nItemInsertIndex = -1;
	CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM_HISTORY);
	strInfo.Format(TEXT("%d"), dwDebugIndex);
	for (int i = 0; i < pListCtrlRoom->GetItemCount(); i++)
	{
		if (strInfo == pListCtrlRoom->GetItemText(i, 0))
		{
			nItemInsertIndex = i;
			break;
		}

	}
	if (nItemInsertIndex != -1)
	{
		//���µ���
		tagHistoryRoomDebugInfo HistoryRoomDebugInfo;

		//ȡ��ϵͳ����
		POSITION posCur = m_pClientDebugItemSinkDlg->GetRoomDebugInfo(dwDebugIndex, HistoryRoomDebugInfo);
		if (posCur)
		{
			strInfo.Format(TEXT("%I64d"), lSystemStorage);
			pListCtrlRoom->SetItemText(nItemInsertIndex, 1, strInfo);

			strInfo.Format(TEXT("%I64d"), lUserStorage);
			pListCtrlRoom->SetItemText(nItemInsertIndex, 2, strInfo);

			strInfo.Format(TEXT("%I64d"), (HistoryRoomDebugInfo.bSystemStorageEnd && HistoryRoomDebugInfo.nDebugStatus == FINISH_STATUS) ? HistoryRoomDebugInfo.lUserStorage : 0);
			pListCtrlRoom->SetItemText(nItemInsertIndex, 4, strInfo);

			strInfo.Format(TEXT("%I64d"), HistoryRoomDebugInfo.lInitSystemStorage - lSystemStorage);
			pListCtrlRoom->SetItemText(nItemInsertIndex, 5, strInfo);

			strInfo.Format(TEXT("%I64d"), HistoryRoomDebugInfo.lInitUserStorage - lUserStorage);
			pListCtrlRoom->SetItemText(nItemInsertIndex, 6, strInfo);

			strInfo = GetDebugStatus(nDebugStatus);
			pListCtrlRoom->SetItemText(nItemInsertIndex, 8, strInfo);
		}
	}

	//����ϵͳ������
	UpdateAllDebugInfo();
}

//����ϵͳ����
void CDialogAdvanced::UpdateSysDebugInfo(tagHistoryRoomDebugInfo *pHistoryRoomDebugInfo)
{	
	CString strInfo;

	CStatic *pStaticSystemInfo = (CStatic *)GetDlgItem(IDC_STATIC_SYSTEM_INFO);
	strInfo.Format(TEXT("ϵͳ��Ӯ������%I64d\t\t\t�����Ӯ������%I64d"),
		m_pClientDebugItemSinkDlg->m_lTotalSystemStorage, m_pClientDebugItemSinkDlg->m_lTotalUserStorage);
	pStaticSystemInfo->SetWindowText(strInfo);

	DWORD dwDebugIndexItem = 0;
	int nItemInsertIndex = 0;
	bool bAddItem = true;
	CListCtrl *pListCtrlSys = (CListCtrl *)GetDlgItem(IDC_LIST_SYS_HISTORY);
	for (int i = 0; i < pListCtrlSys->GetItemCount(); i++)
	{
		dwDebugIndexItem = _ttoi(pListCtrlSys->GetItemText(i, 0));
		if (pHistoryRoomDebugInfo->dwDebugIndex == dwDebugIndexItem)
		{
			nItemInsertIndex = i;
			bAddItem = false;
			break;
		}
		if (pHistoryRoomDebugInfo->dwDebugIndex < dwDebugIndexItem)
		{
			nItemInsertIndex = i + 1;
			break;
		}
	}

	if (bAddItem)
	{
		strInfo.Format(TEXT("%d"), pHistoryRoomDebugInfo->dwDebugIndex);
		pListCtrlSys->InsertItem(nItemInsertIndex, strInfo);
	}

	strInfo.Format(TEXT("%I64d"), pHistoryRoomDebugInfo->lSystemStorage);
	pListCtrlSys->SetItemText(nItemInsertIndex, 1, strInfo);

	strInfo.Format(TEXT("%I64d"), pHistoryRoomDebugInfo->lUserStorage);
	pListCtrlSys->SetItemText(nItemInsertIndex, 2, strInfo);

	strInfo.Format(TEXT("%d"), pHistoryRoomDebugInfo->nParameterK);
	pListCtrlSys->SetItemText(nItemInsertIndex, 3, strInfo);

	strInfo.Format(TEXT("%I64d"), (pHistoryRoomDebugInfo->bSystemStorageEnd && pHistoryRoomDebugInfo->nDebugStatus == FINISH_STATUS) ? pHistoryRoomDebugInfo->lUserStorage : 0);
	pListCtrlSys->SetItemText(nItemInsertIndex, 4, strInfo);

	strInfo.Format(TEXT("%I64d"), pHistoryRoomDebugInfo->lInitSystemStorage - pHistoryRoomDebugInfo->lSystemStorage);
	pListCtrlSys->SetItemText(nItemInsertIndex, 5, strInfo);

	strInfo.Format(TEXT("%I64d"), pHistoryRoomDebugInfo->lInitUserStorage - pHistoryRoomDebugInfo->lUserStorage);
	pListCtrlSys->SetItemText(nItemInsertIndex, 6, strInfo);

	CTime time = CTime(pHistoryRoomDebugInfo->lDebugTime);
	strInfo.Format(TEXT("%02d-%02d %02d:%02d"), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute());
	pListCtrlSys->SetItemText(nItemInsertIndex, 7, strInfo);

	//����ϵͳ������
	UpdateAllDebugInfo();
}

//����ϵͳ����
void CDialogAdvanced::UpdateSysDebugInfo(DWORD dwDebugIndex, LONGLONG lSystemStorage, LONGLONG lUserStorage, int nDebugStatus, LONGLONG lUpdateTime)
{
	CString strInfo;

	CStatic *pStaticSystemInfo = (CStatic *)GetDlgItem(IDC_STATIC_SYSTEM_INFO);
	strInfo.Format(TEXT("ϵͳ��Ӯ������%I64d\t\t\t�����Ӯ������%I64d"),
		m_pClientDebugItemSinkDlg->m_lTotalSystemStorage, m_pClientDebugItemSinkDlg->m_lTotalUserStorage);
	pStaticSystemInfo->SetWindowText(strInfo);

	int nItemInsertIndex = -1;
	CListCtrl *pListCtrlSys = (CListCtrl *)GetDlgItem(IDC_LIST_SYS_HISTORY);
	strInfo.Format(TEXT("%d"), dwDebugIndex);
	for (int i = 0; i < pListCtrlSys->GetItemCount(); i++)
	{
		if (strInfo == pListCtrlSys->GetItemText(i, 0))
		{
			nItemInsertIndex = i;
			break;
		}

	}
	if (nItemInsertIndex != -1)
	{
		//���µ���
		tagHistoryRoomDebugInfo HistoryRoomDebugInfo;

		//ȡ��ϵͳ����
		POSITION posCur = m_pClientDebugItemSinkDlg->GetSysDebugInfo(dwDebugIndex, HistoryRoomDebugInfo);
		if (posCur)
		{
			strInfo.Format(TEXT("%I64d"), lSystemStorage);
			pListCtrlSys->SetItemText(nItemInsertIndex, 1, strInfo);

			strInfo.Format(TEXT("%I64d"), lUserStorage);
			pListCtrlSys->SetItemText(nItemInsertIndex, 2, strInfo);

			strInfo.Format(TEXT("%I64d"), (HistoryRoomDebugInfo.bSystemStorageEnd && HistoryRoomDebugInfo.nDebugStatus == FINISH_STATUS) ? HistoryRoomDebugInfo.lUserStorage : 0);
			pListCtrlSys->SetItemText(nItemInsertIndex, 4, strInfo);

			strInfo.Format(TEXT("%I64d"), HistoryRoomDebugInfo.lInitSystemStorage - lSystemStorage);
			pListCtrlSys->SetItemText(nItemInsertIndex, 5, strInfo);

			strInfo.Format(TEXT("%I64d"), HistoryRoomDebugInfo.lInitUserStorage - lUserStorage);
			pListCtrlSys->SetItemText(nItemInsertIndex, 6, strInfo);
		}
	}

	//����ϵͳ������
	UpdateAllDebugInfo();
}

//����ϵͳ������
void CDialogAdvanced::UpdateAllDebugInfo()
{
	CString strInfo;

	CStatic *pStaticDebugStartTime = (CStatic *)GetDlgItem(IDC_STATIC_DEBUG_START_TIME);

	CTime time = CTime(m_pClientDebugItemSinkDlg->m_tStartDebugTime);
	strInfo.Format(TEXT("��ʼ��¼ʱ�䣺%02d-%02d %02d:%02d"),
		time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute());
	pStaticDebugStartTime->SetWindowText(strInfo);

	CStatic *pStaticAllSystemInfo = (CStatic *)GetDlgItem(IDC_STATIC_ALL_SYSTEM_INFO);
	strInfo.Format(TEXT("ϵͳ�����棺%I64d"),
		m_pClientDebugItemSinkDlg->m_lTotalSystemStorage - m_pClientDebugItemSinkDlg->m_lTotalUserStorage +
		m_pClientDebugItemSinkDlg->m_lRoomTotalSystemStorage - m_pClientDebugItemSinkDlg->m_lRoomTotalUserStorage +
		m_pClientDebugItemSinkDlg->m_lUserTotalSystemStorage - m_pClientDebugItemSinkDlg->m_lUserTotalUserStorage);
	pStaticAllSystemInfo->SetWindowText(strInfo);

	CStatic *pStaticAllUserInfo = (CStatic *)GetDlgItem(IDC_STATIC_ALL_USER_INFO);
	strInfo.Format(TEXT("��������棺%I64d"),
		-(
		m_pClientDebugItemSinkDlg->m_lTotalSystemStorage - m_pClientDebugItemSinkDlg->m_lTotalUserStorage +
				m_pClientDebugItemSinkDlg->m_lRoomTotalSystemStorage - m_pClientDebugItemSinkDlg->m_lRoomTotalUserStorage +
				m_pClientDebugItemSinkDlg->m_lUserTotalSystemStorage - m_pClientDebugItemSinkDlg->m_lUserTotalUserStorage)
		);
	pStaticAllUserInfo->SetWindowText(strInfo);


}


// ��ʼ������
BOOL CDialogAdvanced::OnInitDialog()
{
	// ��ʼ������
	__super::OnInitDialog();

	//��������б�
	INT nColumnCount = 0;
	CListCtrl *pListCtrlSys = (CListCtrl *)GetDlgItem(IDC_LIST_SYS_HISTORY);
	CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM_HISTORY);
	CListCtrl *pListCtrlUser = (CListCtrl *)GetDlgItem(IDC_LIST_USER_HISTORY);

	pListCtrlSys->SetExtendedStyle(pListCtrlUser->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	nColumnCount = 0;
	pListCtrlSys->InsertColumn(nColumnCount++, TEXT("���ID"), LVCFMT_CENTER, 60);
	pListCtrlSys->InsertColumn(nColumnCount++, TEXT("ϵͳ���"), LVCFMT_CENTER, 100);
	pListCtrlSys->InsertColumn(nColumnCount++, TEXT("��ҿ��"), LVCFMT_CENTER, 100);
	pListCtrlSys->InsertColumn(nColumnCount++, TEXT("����ϵ��%"), LVCFMT_CENTER, 100);
	pListCtrlSys->InsertColumn(nColumnCount++, TEXT("��ֵֹ"), LVCFMT_CENTER, 100);
	pListCtrlSys->InsertColumn(nColumnCount++, TEXT("ϵͳ��Ӯ"), LVCFMT_CENTER, 100);
	pListCtrlSys->InsertColumn(nColumnCount++, TEXT("�����Ӯ"), LVCFMT_CENTER, 100);
	pListCtrlSys->InsertColumn(nColumnCount++, TEXT("ʱ��"), LVCFMT_CENTER, 100);

	pListCtrlRoom->SetExtendedStyle(pListCtrlUser->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	nColumnCount = 0;
	pListCtrlRoom->InsertColumn(nColumnCount++, TEXT("����ID"), LVCFMT_CENTER, 60);
	pListCtrlRoom->InsertColumn(nColumnCount++, TEXT("ϵͳ���"), LVCFMT_CENTER, 100);
	pListCtrlRoom->InsertColumn(nColumnCount++, TEXT("��ҿ��"), LVCFMT_CENTER, 100);
	pListCtrlRoom->InsertColumn(nColumnCount++, TEXT("����ϵ��%"), LVCFMT_CENTER, 100);
	pListCtrlRoom->InsertColumn(nColumnCount++, TEXT("��ֵֹ"), LVCFMT_CENTER, 100);
	pListCtrlRoom->InsertColumn(nColumnCount++, TEXT("ϵͳ��Ӯ"), LVCFMT_CENTER, 100);
	pListCtrlRoom->InsertColumn(nColumnCount++, TEXT("�����Ӯ"), LVCFMT_CENTER, 100);
	pListCtrlRoom->InsertColumn(nColumnCount++, TEXT("ʱ��"), LVCFMT_CENTER, 100);
	pListCtrlRoom->InsertColumn(nColumnCount++, TEXT("��ǰ״̬"), LVCFMT_CENTER, 100);

	pListCtrlUser->SetExtendedStyle(pListCtrlUser->GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	nColumnCount = 0;
	pListCtrlUser->InsertColumn(nColumnCount++, TEXT("����ID"), LVCFMT_CENTER, 60);
	pListCtrlUser->InsertColumn(nColumnCount++, TEXT("�������ID"), LVCFMT_CENTER, 100);
	pListCtrlUser->InsertColumn(nColumnCount++, TEXT("ϵͳ���"), LVCFMT_CENTER, 100);
	pListCtrlUser->InsertColumn(nColumnCount++, TEXT("��ҿ��"), LVCFMT_CENTER, 100);
	pListCtrlUser->InsertColumn(nColumnCount++, TEXT("����ϵ��%"), LVCFMT_CENTER, 100);
	pListCtrlUser->InsertColumn(nColumnCount++, TEXT("��ֵֹ"), LVCFMT_CENTER, 100);
	pListCtrlUser->InsertColumn(nColumnCount++, TEXT("ϵͳ��Ӯ"), LVCFMT_CENTER, 100);
	pListCtrlUser->InsertColumn(nColumnCount++, TEXT("�����Ӯ"), LVCFMT_CENTER, 100);
	pListCtrlUser->InsertColumn(nColumnCount++, TEXT("ʱ��"), LVCFMT_CENTER, 100);
	pListCtrlUser->InsertColumn(nColumnCount++, TEXT("��ǰ״̬"), LVCFMT_CENTER, 100);
	return TRUE;
}

void CDialogAdvanced::OnBnClickedButtonRefreshHistory()
{
	SendDebugMessage(SUB_C_REFRESH_HISTORY_RECORD);

	// ������Ϣ
	CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM_HISTORY);
	pListCtrlRoom->DeleteAllItems();

	// ������Ϣ
	CListCtrl *pListCtrlUser = (CListCtrl *)GetDlgItem(IDC_LIST_USER_HISTORY);
	pListCtrlUser->DeleteAllItems();
}


void CDialogAdvanced::OnBnClickedButtonRoomCancel()
{
	CListCtrl *pListCtrlRoom = (CListCtrl *)GetDlgItem(IDC_LIST_ROOM_HISTORY);
	POSITION pos = pListCtrlRoom->GetFirstSelectedItemPosition();
	if (!pos)
	{
		MessageBox(_T("��ѡ��ĳ��"), _T("��ܰ��ʾ"), MB_OK);
		return;
	}

	int nItem = pListCtrlRoom->GetNextSelectedItem(pos);
	if (pListCtrlRoom->GetItemText(nItem, 8) == GetDebugStatus(FINISH_STATUS) ||
		pListCtrlRoom->GetItemText(nItem, 8) == GetDebugStatus(FINISH_STATUS))
	{
		MessageBox(_T("��ѡ��δ��ɺ�δȡ���ĵ�����"), _T("��ܰ��ʾ"), MB_OK);
		return;
	}


	//�������
	CMD_C_CancelRoomStorage CancelRoomStorage;
	CancelRoomStorage.dwDebugIndex = _ttoi(pListCtrlRoom->GetItemText(nItem, 0));

	// ������Ϣ
	SendDebugMessage(SUB_C_CANCEL_ROOM_STORAGE, &CancelRoomStorage, sizeof(CancelRoomStorage));
}

void CDialogAdvanced::OnBnClickedButtonUserCancel()
{
	CListCtrl *pListCtrlUser = (CListCtrl *)GetDlgItem(IDC_LIST_USER_HISTORY);
	POSITION pos = pListCtrlUser->GetFirstSelectedItemPosition();
	if (!pos)
	{
		MessageBox(_T("��ѡ��ĳ��"), _T("��ܰ��ʾ"), MB_OK);
		return;
	}

	int nItem = pListCtrlUser->GetNextSelectedItem(pos);
	if (pListCtrlUser->GetItemText(nItem, 9) == GetDebugStatus(FINISH_STATUS))
	{
		MessageBox(_T("��ѡ��δ��ɵĵ�����"), _T("��ܰ��ʾ"), MB_OK);
		return;
	}


	//�������
	CMD_C_CancelUserStorage CancelUserStorage;
	CancelUserStorage.dwDebugIndex = _ttoi(pListCtrlUser->GetItemText(nItem, 0));
	CancelUserStorage.dwGameID = _ttoi(pListCtrlUser->GetItemText(nItem, 1));

	// ������Ϣ
	SendDebugMessage(SUB_C_CANCEL_USER_STORAGE, &CancelUserStorage, sizeof(CancelUserStorage));
}
