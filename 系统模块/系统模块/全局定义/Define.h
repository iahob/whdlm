#ifndef DEFINE_HEAD_FILE
#define DEFINE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//��ֵ����

//ͷ���С
#define FACE_CX						48									//ͷ����
#define FACE_CY						48									//ͷ��߶�

#define BIGFACE_CX					96									//ͷ����
#define BIGFACE_CY					96									//ͷ��߶�

//���ȶ���
#define LEN_LESS_ACCOUNTS			6									//����ʺ�
#define LEN_LESS_NICKNAME			6									//����ǳ�
#define LEN_LESS_PASSWORD			6									//�������
#define LEN_LESS_COMPELLATION		4									//��С����

//��������
#define MAX_CHAIR					200									//�������
#define MAX_ROOMCHAIR				512									//��󷿼�����
#define MAX_TABLE					512									//�������
#define MAX_COLUMN					64									//����б�
#define MAX_BATCH					1									//�������
#define MAX_ANDROID					2048								//������
#define MAX_PROPERTY				128									//������
#define MAX_WHISPER_USER			16									//���˽��
#define MAX_BUFF					10									//���Buff
#define MAX_GAMESERVER_LEVEL		32									//��󷿼�ȼ�
#define MAX_GAMECHAT				64									//�������
#define MAX_CHATSTRINGLEN			128									//��������ַ�������
#define MAX_SHARESTORAGEITEM		256									//�����������

//�б���
#define MAX_KIND					256									//�������
#define MAX_SERVER					1024								//��󷿼�

//��������
#define INVALID_CHAIR				0xFFFF								//��Ч����
#define INVALID_TABLE				0xFFFF								//��Ч����
#define INVALID_SERVER				0xFFFF								//��Ч����
#define INVALID_KIND				0xFFFF								//��Ч��Ϸ

//˰�ն���
#define REVENUE_BENCHMARK			0L								    //˰�����
#define REVENUE_DENOMINATOR			1000L								//˰�շ�ĸ

//////////////////////////////////////////////////////////////////////////////////
//ϵͳ����
#define	MAX_ACCOUNTS				5									//����˺�

//��������
#define SCORE						LONGLONG							//��������
#define SCORE_STRING				TEXT("%I64d")						//��������

//��Ϸ״̬
#define GAME_STATUS_FREE			0									//����״̬
#define GAME_STATUS_PLAY			100									//��Ϸ״̬
#define GAME_STATUS_WAIT			200									//�ȴ�״̬

//ϵͳ����
#define LEN_USER_CHAT				128									//���쳤��
#define TIME_USER_CHAT				1L									//������
#define TRUMPET_MAX_CHAR            128									//���ȳ���

//////////////////////////////////////////////////////////////////////////////////
//��������

//�б�����
#define PRIME_TYPE					11L									//������Ŀ
#define PRIME_KIND					53L									//������Ŀ
#define PRIME_NODE					101L								//�ڵ���Ŀ
#define PRIME_PAGE					53L									//�Զ���Ŀ
#define PRIME_SERVER				1009L								//������Ŀ

//��������
#define PRIME_SERVER_USER			503L								//��������
#define PRIME_ANDROID_USER			503L								//AI��
#define PRIME_PLATFORM_USER			100003L								//ƽ̨����

//��������
#define PRIME_ANDROID_PARAMETER		65									//������Ŀ

//////////////////////////////////////////////////////////////////////////////////
//���ݳ���

//��������
#define LEN_MD5						33									//��������
#define LEN_USERNOTE				32									//��ע����
#define LEN_ACCOUNTS				32									//�ʺų���
#define LEN_NICKNAME				32									//�ǳƳ���
#define LEN_PASSWORD				33									//���볤��
#define LEN_GROUP_NAME				32									//��������
#define LEN_UNDER_WRITE				32									//����ǩ��
#define LEN_REMARKS					32									//��ע��Ϣ
#define LEN_DATETIME				20									//���ڳ���
#define LEN_GROUP_INTRODUCE			64									//Ⱥ�����

//���ݳ���
#define LEN_QQ						16									//Q Q ����
#define LEN_EMAIL					33									//�����ʼ�
#define LEN_USER_NOTE				256									//�û���ע
#define LEN_SEAT_PHONE				33									//�̶��绰
#define LEN_MOBILE_PHONE			12									//�ƶ��绰
#define LEN_PASS_PORT_ID			19									//֤������
#define LEN_COMPELLATION			16									//��ʵ����
#define LEN_DWELLING_PLACE			128									//��ϵ��ַ
#define LEN_USER_UIN				33									//UIN����
#define LEN_WEEK					7									//���ڳ���

#define LEN_TASK_NAME				20									//��������
#define LEN_TRANS_REMARK			32									//ת�˱�ע	
#define LEN_VERIFY_CODE				5									//��֤����
#define LEN_POSTALCODE				8									//��������
#define LEN_BIRTHDAY				16									//�û�����
#define LEN_BLOODTYPE				6									//�û�Ѫ��
#define LEN_CONSTELLATION			6									//�û�����
#define LEN_PHONE_MODE				21									//�ֻ��ͺ�
#define LEN_DEVICE_TOKEN			65									//�豸��ʶ����
#define LEN_FACE_URL				500									//ͷ��URL
#define LEN_PLACE_NAME				33									//��������
#define LEN_CHECK_CODE				7									//��֤�볤��
#define LEN_BANKEROP_BT				5									//���д�ȡ������ť
#define LEN_SUB_RESERVED_BUFFER		256									//����Ԥ��

//������ʶ
#define LEN_NETWORK_ID				13									//��������
#define LEN_MACHINE_ID				33									//���г���

//�б�����
#define LEN_TYPE					32									//���೤��
#define LEN_KIND					32									//���ͳ���
#define LEN_NODE					32									//�ڵ㳤��
#define LEN_PAGE					32									//���Ƴ���
#define LEN_SERVER					32									//���䳤��
#define LEN_PROCESS					32									//���̳���

//////////////////////////////////////////////////////////////////////////
//���Ѷ���
#define MAX_FRIEND_COUNT			1000								//������
#define MIN_FRIEND_CONTENT			20									//��С����	
#define MAX_FRIEND_TRANSFER			5									//�����	

#define FRIEND_OPERATE_REMOVE		1									//�Ƴ�����
#define FRIEND_OPERATE_MDGROUPID	2									//�޸����

//��¼����
#define LOGON_COMMAND_USERINFO      0x0001								//�û���Ϣ
#define LOGON_COMMAND_GROUPINFO		0x0002								//������Ϣ
#define LOGON_COMMAND_FACE			0x0004								//�û�ͷ��
#define LOGON_COMMAND_FRIENDS       0x0008								//�û�����
#define LOGON_COMMAND_REMARKS		0x0010								//��ע��Ϣ
#define LOGON_TIME_SPACE			1000								//ʱ����

//���鶨��
#define GROUP_MAX_COUNT				32									//������
#define GROUP_LEN_NAME				32									//��������	
#define GROUP_LEN_INFO				1021								//��Ϣ����

#define GROUP_OPERATE_ADD			1									//��ӷ���
#define GROUP_OPERATE_MODIFY		2									//�༭����
#define GROUP_OPERATE_REMOVE		3									//�Ƴ�����


//��������
#define FRIEND_INVALID_SERVERID			WORD(0xffff)						//��Ч����
#define FRIEND_INVALID_TABLEID			WORD(0xffff)						//��Ч����
#define FRIEND_INVALID_CHAIR			WORD(0xffff)						//��Ч����

#define	DAY_SECONDS						(24*60*60)							//ÿ������
#define	HOUR_SECONDS					(60*60)								//Сʱ����

//�û�״̬
#define FRIEND_US_OFFLINE					0x00								//����״̬
#define FRIEND_US_ONLINE					0x01								//����״̬
#define FRIEND_US_LEAVE						0x02								//�뿪״̬
#define FRIEND_US_BUSY						0x04								//æµ״̬
#define FRIEND_US_UNDISTRUB					0x08								//�����״̬
#define FRIEND_US_CORBET					0x10								//����״̬


//��������
#define LEN_FONT_NAME				16									//��������
#define LEN_MESSAGE_CONTENT			512									//��Ϣ����		
#define MAX_EXPRESSION_COUNT		100									//������	

//////////////////////////////////////////////////////////////////////////////////

//�û���ϵ
#define	CP_NORMAL					0									//δ֪��ϵ
#define	CP_FRIEND					1									//���ѹ�ϵ
#define	CP_DETEST					2									//����ϵ
#define CP_SHIELD					3									//��������

//////////////////////////////////////////////////////////////////////////////////

//�Ա���
#define GENDER_FEMALE				0									//Ů���Ա�
#define GENDER_MANKIND				1									//�����Ա�

//////////////////////////////////////////////////////////////////////////////////

//��Ϸģʽ
#define GAME_GENRE_GOLD				0x0001								//�������
#define GAME_GENRE_SCORE			0x0002								//��ֵ����
#define GAME_GENRE_MATCH			0x0004								//��������
#define GAME_GENRE_EDUCATE			0x0008								//ѵ������
#define GAME_GENRE_PERSONAL			0x0010								//Լս����

//��������
#define SERVER_GENRE_NORMAL         0x0001                              //��ͨ����
#define SERVER_GENRE_PASSWD         0x0002                              //��������

//����ģʽ
#define SCORE_GENRE_NORMAL			0x0100								//��ͨģʽ
#define SCORE_GENRE_POSITIVE		0x0200								//�Ǹ�ģʽ

//////////////////////////////////////////////////////////////////////////////////

//��������
#define TASK_TYPE_WIN_INNINGS		0x01								//Ӯ������
#define TASK_TYPE_SUM_INNINGS		0x02								//�ܾ�����
#define TASK_TYPE_FIRST_WIN			0x04								//��ʤ����
#define TASK_TYPE_KEEP_WIN			0x08								//��Ӯ����

#define TASK_TYPE_CREATE_ROOM		0x100								//��������
#define TASK_TYPE_CONSUME_ROOM		0x101								//���ķ���
#define TASK_TYPE_PLAY_INNINGS		0x102								//Լս����

//����״̬
#define TASK_STATUS_UNFINISH		0									//����״̬
#define TASK_STATUS_SUCCESS			1									//����ɹ�
#define TASK_STATUS_FAILED			2									//����ʧ��	
#define TASK_STATUS_REWARD			3									//��ȡ����

//��������
#define TASK_MAX_COUNT				128									//��������

//////////////////////////////////////////////////////////////////////////////////
//�齱����
#define MAX_LOTTERY					16									//��ཱ��

//��Ϸ����
#define LEN_GAME_DATA				1024								//��Ϸ����
#define LEN_GAME_ITEM				64									//��������
#define MAX_DATA_ITEM				16									//�������
	
//////////////////////////////////////////////////////////////////////////////////

//�û�״̬
#define US_NULL						0x00								//û��״̬
#define US_FREE						0x01								//վ��״̬
#define US_SIT						0x02								//����״̬
#define US_READY					0x03								//ͬ��״̬
#define US_LOOKON					0x04								//�Թ�״̬
#define US_PLAYING					0x05								//��Ϸ״̬
#define US_OFFLINE					0x06								//����״̬


//�û�״̬
#define US_GP_OFFLINE				0x00								//����״̬	
#define US_GP_ONLINE				0x01								//����״̬

//////////////////////////////////////////////////////////////////////////////////
//��������

//�����û�״̬
#define MUS_NULL					0x00								//û��״̬
#define MUS_OUT						0x01								//��̭״̬
#define MUS_SIGNUPED     			0x02								//����״̬
#define MUS_PLAYING					0x04								//��������

//����״̬
#define MS_FREE						0x00								//����״̬
#define MS_WAITPLAY					0x01								//�ȴ���ʼ
#define MS_MATCHING					0x02								//��������
#define MS_WAITEND					0x04								//�ȴ�����
#define MS_MATCHEND					0x08								//��������

//�ɷ�����
#define DEDUCT_AREA_SERVER			0									//����ɷ�
#define DEDUCT_AREA_WEBSITE			1									//��վ�ɷ�

//��������
#define FEE_TYPE_GOLD				0									//���ɽ��
#define FEE_TYPE_MEDAL				1									//������ʯ	

//��������
#define MATCH_JOINCD_MEMBER_ORDER	0x01								//��Ա�ȼ�	
#define MATCH_JOINCD_EXPERIENCE		0x02								//����ȼ�

//ɸѡ��ʽ
#define FILTER_TYPE_SINGLE_TURN		0									//��������
#define FILTER_TYPE_TOTAL_RANKING	1									//��������

//����ģʽ
#define SIGNUP_MODE_SIGNUP_FEE		0x01								//��������
#define SIGNUP_MODE_MATCH_USER		0x02								//�ض�����

//������ʽ
#define RANKING_MODE_TOTAL_GRADES   0									//������ʽ
#define RANKING_MODE_SPECIAL_GRADES 1									//������ʽ 

//ɸѡ�ɼ�
#define FILTER_GRADES_MODE_BEST		0									//���ųɼ�
#define FILTER_GRADES_MODE_AVERAGE	1									//ƽ���ɼ�
#define FILTER_GRADES_MODE_AVERAGEEX 2									//ƽ���ɼ�

//����ģʽ
#define MATCH_MODE_NORMAL			0x00								//���濪��
#define MATCH_MODE_ROUND			0x01								//ѭ������

//���ȶ���
#define MAX_MATCH_ROUNDS			10									//�������
#define MAX_MATCH_SECTION			10									//���׶�
//////////////////////////////////////////////////////////////////////////////////

//�������
#define SRL_LOOKON					0x00000001							//�Թ۱�־
#define SRL_OFFLINE					0x00000002							//���߱�־
#define SRL_SAME_IP					0x00000004							//ͬ����־

//�������
#define SRL_ROOM_CHAT				0x00000100							//�����־
#define SRL_GAME_CHAT				0x00000200							//�����־
#define SRL_WISPER_CHAT				0x00000400							//˽�ı�־
#define SRL_HIDE_USER_INFO			0x00000800							//���ر�־

//////////////////////////////////////////////////////////////////////////////////
//�б�����

//��Ч����
#define UD_NULL						0									//��Ч����
#define UD_IMAGE					100									//ͼ������
#define UD_CUSTOM					200									//�Զ�����

//��������
#define UD_GAME_ID					1									//��Ϸ��ʶ
#define UD_USER_ID					2									//�û���ʶ
#define	UD_NICKNAME					3									//�û��ǳ�

//��չ����
#define UD_BANKER					4									//����
#define UD_WIN_LOST					5									//��Ӯ���
#define UD_IS_ANDROID				6									//AI��
#define UD_IS_CONTROL				7									//�Ƿ����

#define UD_GENDER					10									//�û��Ա�
#define UD_GROUP_NAME				11									//��������
#define UD_UNDER_WRITE				12									//����ǩ��

//״̬��Ϣ
#define UD_TABLE					20									//��Ϸ����
#define UD_CHAIR					21									//���Ӻ���

//������Ϣ
#define UD_SCORE					30									//�û�����
#define UD_GRADE					31									//�û��ɼ�
#define UD_USER_MEDAL				32									//�û�����
#define UD_EXPERIENCE				33									//�û�����
#define UD_LOVELINESS				34									//�û�����
#define UD_WIN_COUNT				35									//ʤ������
#define UD_LOST_COUNT				36									//�������
#define UD_DRAW_COUNT				37									//�;�����
#define UD_FLEE_COUNT				38									//�Ӿ�����
#define UD_PLAY_COUNT				39									//�ܾ�����

//���ֱ���
#define UD_WIN_RATE					40									//�û�ʤ��
#define UD_LOST_RATE				41									//�û�����
#define UD_DRAW_RATE				42									//�û�����
#define UD_FLEE_RATE				43									//�û�����
#define UD_GAME_LEVEL				44									//��Ϸ�ȼ�

//��չ��Ϣ
#define UD_NOTE_INFO				50									//�û���ע
#define UD_LOOKON_USER				51									//�Թ��û�
#define UD_ONLINE_TIME				52									//AI����ʱ��
#define UD_CARRY_GRADE				53									//AIЯ���ȼ�
#define UD_LEAVE_GRADE				54									//AI�뿪�ȼ�

//ͼ���б�
#define UD_IMAGE_FLAG				(UD_IMAGE+1)						//�û���־
#define UD_IMAGE_GENDER				(UD_IMAGE+2)						//�û��Ա�
#define UD_IMAGE_STATUS				(UD_IMAGE+3)						//�û�״̬

//////////////////////////////////////////////////////////////////////////////////
//���ݿⶨ��

#define DB_ERROR 					-1  								//����ʧ��
#define DB_SUCCESS 					0  									//����ɹ�
#define DB_NEEDMB 					18 									//����ʧ��
#define DB_PASSPORT					19									//����ʧ��

//////////////////////////////////////////////////////////////////////////////////
//���߱�ʾ
#define PT_USE_MARK_DOUBLE_SCORE    0x0001								//˫������
#define PT_USE_MARK_FOURE_SCORE     0x0002								//�ı�����
#define PT_USE_MARK_GUARDKICK_CARD  0x0010								//���ߵ���
#define PT_USE_MARK_POSSESS         0x0020								//������� 

#define MAX_PT_MARK                 4                                   //��ʶ��Ŀ

//��Ч��Χ
#define VALID_TIME_DOUBLE_SCORE     3600                                //��Чʱ��
#define VALID_TIME_FOUR_SCORE       3600                                //��Чʱ��
#define VALID_TIME_GUARDKICK_CARD   3600                                //����ʱ��
#define VALID_TIME_POSSESS          3600                                //����ʱ��
#define VALID_TIME_KICK_BY_MANAGER  3600                                //��Ϸʱ�� 

#define PROPERTY_LEN_NAME			32									//��������
#define PROPERTY_LEN_INFO			128									//������Ϣ
//////////////////////////////////////////////////////////////////////////////////
//�豸����
#define DEVICE_TYPE_PC              0x00                                //PC
#define DEVICE_TYPE_ANDROID         0x10                                //Android
#define DEVICE_TYPE_ANDROID_COCOS2D	0x11								//Android COCOS2D
#define DEVICE_TYPE_ANDROID_U3D		0x12								//Android U3D
#define DEVICE_TYPE_ITOUCH          0x20                                //iTouch
#define DEVICE_TYPE_IPHONE          0x30                                //iPhone
#define DEVICE_TYPE_IPHONE_COCOS2D	0x31								//Android COCOS2D
#define DEVICE_TYPE_IPHONE_U3D		0x32								//Android U3D
#define DEVICE_TYPE_IPAD            0x40                                //iPad
#define DEVICE_TYPE_IPAD_COCOS2D	0x41								//Android COCOS2D
#define DEVICE_TYPE_IPAD_U3D		0x42								//Android U3D

/////////////////////////////////////////////////////////////////////////////////
//�ֻ�����

//��ͼģʽ
#define	VIEW_MODE_ALL				0x0001								//ȫ������
#define	VIEW_MODE_PART				0x0002								//���ֿ���

//��Ϣģʽ
#define VIEW_INFO_LEVEL_1			0x0010								//������Ϣ
#define VIEW_INFO_LEVEL_2			0x0020								//������Ϣ
#define VIEW_INFO_LEVEL_3			0x0040								//������Ϣ
#define VIEW_INFO_LEVEL_4			0x0080								//������Ϣ

//��������
#define RECVICE_GAME_CHAT			0x0100								//��������
#define RECVICE_ROOM_CHAT			0x0200								//��������
#define RECVICE_ROOM_WHISPER		0x0400								//����˽��

//��Ϊ��ʶ
#define BEHAVIOR_LOGON_NORMAL       0x0000                              //��ͨ��¼
#define BEHAVIOR_LOGON_IMMEDIATELY  0x1000                              //������¼

/////////////////////////////////////////////////////////////////////////////////
//������
#define RESULT_ERROR 					-1  								//�������
#define RESULT_SUCCESS 					0  									//����ɹ�
#define RESULT_FAIL 					1  									//����ʧ��

/////////////////////////////////////////////////////////////////////////////////
//�仯ԭ��
#define SCORE_REASON_WRITE              0                                   //д�ֱ仯
#define SCORE_REASON_INSURE             1                                   //���б仯
#define SCORE_REASON_PROPERTY           2                                   //���߱仯
#define SCORE_REASON_MATCH_FEE          3                                   //��������
#define SCORE_REASON_MATCH_QUIT         4                                   //��������

/////////////////////////////////////////////////////////////////////////////////

//��¼����ʧ��ԭ��
#define LOGON_FAIL_SERVER_INVALIDATION  200   

/////////////////////////////////////////////////////////////////////////////////

//��������
#define ANDROID_SIMULATE				0x01								//�໥ģ��
#define ANDROID_PASSIVITY				0x02								//�������
#define ANDROID_INITIATIVE				0x04								//�������

/////////////////////////////////////////////////////////////////////////////////
//��������

//�۷�����
#define MATCH_FEE_TYPE_GOLD				0x00								//�۷�����
#define MATCH_FEE_TYPE_MEDAL			0x01								//�۷�����

//��������
#define MATCH_TYPE_LOCKTIME				0x00								//��ʱ����
#define MATCH_TYPE_IMMEDIATE			0x01								//��ʱ����

////////////////////////////////////////////////////////////////////////////////

//��������
#define CONSUME_TYPE_GOLD				0x01								//��Ϸ��
#define CONSUME_TYPE_USEER_MADEL		0x02								//Ԫ��
#define CONSUME_TYPE_CASH				0x03								//��Ϸ��
#define CONSUME_TYPE_LOVELINESS			0x04								//����ֵ

//�ȼ�����
#define IDI_HOT_KEY_BOSS			0x0100								//�ϰ��ȼ�
#define IDI_HOT_KEY_WHISPER			0x0200								//˽���ȼ�

//˽�˷����
const int RULE_LEN = 100;												//������򳤶�
const int SPECIAL_INFO_LEN = 1000;										//��Է������ʱ����ʱ��һЩ����Ҫ��
const int MAX_CREATE_COUNT = 5;
const int PERSONAL_ROOM_GUID = 32;										//Լս����Ψһ��ʶ

//Լս��Ϸͨ������
const int PERSONAL_OPTION_COUNT = 5;									//Լս������Ŀ
const int CELLSCORE_COUNT = 5;											//�׷�������Ŀ
const int GOLD_TURN_COUNT = 10000000;									//Լս��Ҿ���

//�������
#define MAXT_VOICE_LENGTH				15000							//��������
#define MAXT_H5_VOICE_LENGTH			256								//��������
//��Ƶ���
#define LEN_VIDEO_NUMBER	22											//��ƵID
#define LEN_GAMEOVER_DATA	50											//���ݳ���
#define MAX_PASSWORD_COUNT  10											//�������


//���ֲ����
#define GROUP_ID_ARRAY_LEN	512		//���ֲ�id����
#define GROUP_MAX_CHAIR_GAME_TABLE_COUNT	10												//�����˰�����Ϸ������
#define GROUP_MAX_CHAIR_GAME_CHAIR_COUNT	MAX_CHAIR / GROUP_MAX_CHAIR_GAME_TABLE_COUNT	//�����˰�����Ϸ������(�����Ϸ������)
const int PERSONAL_ROOM_CHAIR = GROUP_MAX_CHAIR_GAME_CHAIR_COUNT;		//˽�˷������������ӵ������Ŀ

//////////////////////////////////////////////////////////////////////////
//���ֲ�����
#define GP_SOCKET_PACKET			8192								//���ݳ���
#define GS_GROUP_COUNT				20000								//Ⱥ����Ŀ

//��������
#define UPGROUP_KIND_WEALTH			0x01								//�Ƹ�����
#define UPGROUP_KIND_PROPERTY		0x02								//��������
#define UPGROUP_KIND_BATTLERULE		0x04								//��������

//��������
#define UPMEMBER_KIND_BATTLE		1									//Լս����

//��Ϣ����
#define APPLY_MSG_TYPE_REQUEST		1									//������Ϣ
#define APPLY_MSG_TYPE_RESPOND		2									//��Ӧ��Ϣ

//Ⱥ��״̬
#define GROUP_STATUS_ENABLE			1									//����״̬
#define GROUP_STATUS_DISABLE		2									//ͣ��״̬

//Լս����
#define	GROUP_BATTLE_MASK_GYKF		0x01								//��Ա����

//����״̬
#define APPLY_STATUS_NONE			0									//���״̬
#define APPLY_STATUS_AGREE			1									//ͬ��״̬
#define APPLY_STATUS_REFUSE			2									//�ܾ�״̬

//�������
#define LOGON_ECODE_FAILED			1									//ʧ�ܴ���
#define LOGON_ECODE_REENTRY			2									//�������

//��ϷKindID
#define ZZMJ_KIND_ID  386
#define HZMJ_KIND_ID  389
#define ZJH_KIND_ID	  6
#define NN_KIND_ID	  27
#define TBZ_KIND_ID	  47
#define OXSIXX_KIND_ID	  50
#define SET_RULE		1
#define DZ_KIND_ID		3
#define FISH_3D		523
#define FISH_LK		510
#define FISH_DR		527
#define GO_BANG_KIND_ID	  401
#define OxEight_KIND_ID	57
#define FishPrawnCrab_KIND_ID	  529
#define CANDY_PARTY_KIND_ID	  31

//
#define MAKEULONGLONG(a, b)  ((ULONGLONG)(((DWORD)(((ULONGLONG)(a)) & 0xffffffff)) | ((ULONGLONG)((DWORD)(((ULONGLONG)(b)) & 0xffffffff))) << 32))

//////////////////////////////////////////////////////////////////////////


#endif