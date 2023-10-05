#include "StdAfx.h"
#include "servercontrolitemsink.h"

//////////////////////////////////////////////////////////////////////////

//扑克数据
const CPoint	CServerControlItemSink::m_ptXian[ITEM_COUNT][ITEM_X_COUNT]=
{
	CPoint(1, 0), CPoint(1, 1), CPoint(1, 2), CPoint(1, 3), CPoint(1, 4), 
		CPoint(0, 0), CPoint(0, 1), CPoint(0, 2), CPoint(0, 3), CPoint(0, 4),
		CPoint(2, 0), CPoint(2, 1), CPoint(2, 2), CPoint(2, 3), CPoint(2, 4),
		CPoint(0, 0), CPoint(1, 1), CPoint(2, 2), CPoint(1, 3), CPoint(0, 4),
		CPoint(2, 0), CPoint(1, 1), CPoint(0, 2), CPoint(1, 3), CPoint(2, 4),
		CPoint(0, 0), CPoint(0, 1), CPoint(1, 2), CPoint(0, 3), CPoint(0, 4),
		CPoint(2, 0), CPoint(2, 1), CPoint(1, 2), CPoint(2, 3), CPoint(2, 4),
		CPoint(1, 0), CPoint(2, 1), CPoint(2, 2), CPoint(2, 3), CPoint(1, 4),
		CPoint(1, 0), CPoint(0, 1), CPoint(0, 2), CPoint(0, 3), CPoint(1, 4)
};
//
CServerControlItemSink::CServerControlItemSink(void)
{
}

CServerControlItemSink::~CServerControlItemSink( void )
{

}

//服务器控制
bool __cdecl CServerControlItemSink::ServerControl(USERCONTROL &UserContorl, BYTE cbCardData[][ITEM_X_COUNT])
{
	BYTE cbCardDataTemp[ITEM_X_COUNT * ITEM_Y_COUNT] = {0};
	for (int i = 0; i< ITEM_Y_COUNT; i++)
	{
		for (int j = 0; j< ITEM_X_COUNT; j++)
		{
			cbCardData[i][j] = 0xFF;
		}
	}
	BYTE cbCardCount = GetCardType(UserContorl, cbCardDataTemp);
	if(cbCardCount > 0)
	{
		if(cbCardCount == ITEM_X_COUNT * ITEM_Y_COUNT)
		{
			for (int i = 0; i< ITEM_Y_COUNT; i++)
			{
				for (int j = 0; j< ITEM_X_COUNT; j++)
				{
					cbCardData[i][j] = cbCardDataTemp[i * ITEM_Y_COUNT + j];
				}
			}
		}
		else if(cbCardCount >= 3 && cbCardCount <= 5)
		{
			int nXian = rand() % 9;
			bool bLeft = (rand() % 2) == 0;
			if(bLeft)
			{
				for (int i = 0; i < cbCardCount; i++)
				{
					cbCardData[m_ptXian[nXian][i].x][m_ptXian[nXian][i].y] = cbCardDataTemp[i];
				}
			}
			else
			{
				for (int i = 0; i < cbCardCount; i++)
				{
					cbCardData[m_ptXian[nXian][ITEM_X_COUNT - i - 1].x][m_ptXian[nXian][ITEM_X_COUNT - i - 1].y] = cbCardDataTemp[i];
				}
			}

			//补空
			BYTE cbType = 0;
			for (int y = 0; y< ITEM_Y_COUNT; y++)
			{
				for (int x = 0; x< ITEM_X_COUNT; x++)
				{
					if(cbCardData[y][x] == 0xFF)
					{		
						cbType = rand() % 8;
						for (int i = 0; i < 8; i++)
						{
							cbType++;
							if(cbType == 8)
							{
								cbType = 0;
							}
							//左边
							if(x > 0)
							{
								//左上边
								if(y > 0)
								{
									if(cbCardData[y - 1][x - 1] == cbType)
									{
										continue;
									}
								}

								//左边
								if(cbCardData[y][x - 1] == cbType)
								{
									continue;
								}

								//左下边
								if(y < ITEM_Y_COUNT - 1)
								{
									if(cbCardData[y + 1][x - 1] == cbType)
									{
										continue;
									}
								}
							}

							//右边
							if(x < ITEM_X_COUNT - 1)
							{
								//左上边
								if(y > 0)
								{
									if(cbCardData[y - 1][x + 1] == cbType)
									{
										continue;
									}
								}

								//左边
								if(cbCardData[y][x + 1] == cbType)
								{
									continue;
								}

								//左下边
								if(y < ITEM_Y_COUNT - 1)
								{
									if(cbCardData[y + 1][x + 1] == cbType)
									{
										continue;
									}
								}
							}
							break;
						}
						cbCardData[y][x] = cbType;
					}
				}
			}
		}
		else
		{
			ASSERT(NULL);
			return false;
		}
		return true;
	}
	ASSERT(NULL);
	return false;
}

int CServerControlItemSink::GetCardType(USERCONTROL &UserContorl, BYTE cbCardData[ITEM_X_COUNT * ITEM_Y_COUNT])
{
	int nCardCount = 0;
	switch(UserContorl.controlType)
	{
	case CONTINUE_3:
		{
			nCardCount = 3;
			break;
		}
	case CONTINUE_4:
		{
			nCardCount = 4;
			break;
		}
	case CONTINUE_5:
		{
			nCardCount = 5;
			break;
		}
	case CONTINUE_ALL:
	case CONTINUE_LOST:
		{
			nCardCount = ITEM_X_COUNT * ITEM_Y_COUNT;
			break;
		}
	}
	switch(UserContorl.cbControlData)
	{
	case 0:
		{
			//str = TEXT("斧头");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_FUTOU;
			}
			break;
		}
	case 1:
		{
			//str = TEXT("银枪");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_YINGQIANG;
			}
			break;
		}
	case 2:
		{
			//str = TEXT("大刀");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_DADAO;
			}
			break;
		}
	case 3:
		{
			//str = TEXT("鲁");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_LU;
			}
			break;
		}
	case 4:
		{
			//str = TEXT("林");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_LIN;
			}
			break;
		}
	case 5:
		{
			//str = TEXT("宋");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_SONG;
			}
			break;
		}
	case 6:
		{
			//str = TEXT("替天行道");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_TITIANXINGDAO;
			}
			break;
		}
	case 7:
		{
			//str = TEXT("忠义堂");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_ZHONGYITANG;
			}
			break;
		}
	case 8:
		{
			//str = TEXT("水浒传");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_SHUIHUZHUAN;
			}
			break;
		}
	case 9:
		{
			//str = TEXT("武器");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_FUTOU + rand() % 3;
			}
			break;
		}
	case 10:
		{
			//str = TEXT("人物");
			for (int i = 0; i < nCardCount; i++)
			{
				cbCardData[i] = CT_LU + rand() % 3;
			}
			break;
		}
	}

	return nCardCount;
}