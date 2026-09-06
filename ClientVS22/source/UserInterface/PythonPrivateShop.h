#pragma once

#include "Packet.h"
#include <list>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

class CInstanceBase;

enum EPrivateShopState
{
	STATE_NONE,
	STATE_CLOSED,
	STATE_OPEN,
	STATE_MODIFY
};

enum EMode
{
	MODE_NONE,
	MODE_LOOK,
	MODE_TRADE,
};

enum EFilterTypes
{
	FILTER_TYPE_ITEM_VNUM,
	FILTER_TYPE_ITEM_TYPE,
	FILTER_TYPE_ITEM_SUBTYPE,
	FILTER_TYPE_CLASS,
	FILTER_TYPE_MIN_LEVEL,
	FILTER_TYPE_MAX_LEVEL,
	FILTER_TYPE_MIN_REFINEMENT,
	FILTER_TYPE_MAX_REFINEMENT,

	FILTER_TYPE_MIN_CHEQUE,
	FILTER_TYPE_MAX_CHEQUE,

	FILTER_TYPE_MIN_GOLD,
	FILTER_TYPE_MAX_GOLD,

	FILTER_TYPE_MAX_NUM
};

enum EShopSearch
{
	RESULT_MAX_NUM = 10,
};

enum EConfig
{
	MAX_VIEW_DISTANCE = 10000,
};

class CPythonPrivateShop : public CSingleton<CPythonPrivateShop>
{
	public:
		enum EAttachEffect
		{
			EFFECT_LIFE_NORMAL,
			EFFECT_LIFE_INFINITE,
			EFFECT_LIFE_WITH_MOTION,
		};

		struct TAttachingEffect
		{
			DWORD		dwEffectIndex;
			int			iBoneIndex;
			DWORD		dwModelIndex;
			D3DXMATRIX	matTranslation;
			BOOL		isAttaching;

			int			iLifeType;
			DWORD		dwEndTime;
		};

		typedef struct SPrivateShopInstance
		{
			DWORD								m_dwVID;
			std::string							m_strName;
			CGraphicThingInstance*				m_pGraphicThingInstance;
			std::list<TAttachingEffect>			m_list_attachingEffect;
			DWORD								m_dwCurrentSelectEffect;

			SPrivateShopInstance()
			{
				m_dwVID = 0;
				m_strName = "NoName";
				m_pGraphicThingInstance = nullptr;
				m_list_attachingEffect.clear();
				m_dwCurrentSelectEffect = 0;
			};

			virtual ~SPrivateShopInstance()
			{
				ClearAttachingEffect();
			};

			bool		LoadRace(DWORD dwVirtualNumber, const TPixelPosition& c_rPixelPosition);
			DWORD		AttachEffectByName(DWORD dwParentPartIndex, const char* c_pszBoneName, const char* c_pszEffectFileName);
			DWORD		AttachEffectByID(DWORD dwParentPartIndex, const char* c_pszBoneName, DWORD dwEffectID, const D3DXVECTOR3* c_pv3Position = NULL);
			void		DetachEffect(DWORD dwEffectID);

			void		AttachSelectEffect();
			void		DetachSelectEffect();

			void		ShowAllAttachingEffect();
			void		HideAllAttachingEffect();
			void		ClearAttachingEffect();
			void		UpdateAttachingEffect();

			bool		IntersectBoundingBox();

			void		SetVID(DWORD dwVID) { m_dwVID = dwVID; }
			DWORD		GetVID() { return m_dwVID; }

			void		SetName(const char* c_szName) { m_strName = c_szName; }
			const char*	GetName() { return m_strName.c_str(); }

			CGraphicThingInstance&	GetGraphicThingInstanceRef();
			CGraphicThingInstance*	GetGraphicThingInstancePtr();
		} TPrivateShopInstance;

		enum EDeco
		{
			DECO_TYPE_APPEARANCE	= 1,
			DECO_TYPE_TITLE			= 2,

			APPEARANCE_TOKEN_NAME	= 1,
			APPEARANCE_TOKEN_VNUM	= 2,
			APPEARANCE_MAX_TOKEN,

			TITLE_TOKEN_NAME		= 1,
			TITLE_TOKEN_PATH		= 2,
			TITLE_TOKEN_TEXTCOLOR	= 3,
			TITLE_MAX_TOKEN,
		};

		typedef struct SAppearanceDeco
		{
			std::string		stName;
			DWORD			dwVnum;
		} TAppearanceDeco;

		typedef struct STitleDeco
		{
			std::string stName;
			std::string	stPath;
			std::string	stTextColor;
		} TTitleDeco;

		CPythonPrivateShop();
		~CPythonPrivateShop();

		void		ClearMyPrivateShop();
		void		Clear();

		// Private Shop Instance
		TPrivateShopInstance*	CreatePrivateShopInstance(DWORD dwVirtualID, DWORD dwVirtualNumber, const char* c_szName, long lX, long lY, long lZ);
		TPrivateShopInstance*	GetPrivateShopInstance(DWORD dwVirtualID);
		DWORD					GetPrivateShopInstanceVID(const char* c_szOwnerName);
		bool					DeletePrivateShopInstance(DWORD dwVirtualID);
		TPrivateShopInstance*	GetSelectedInstance() { return m_pSelectedInstance; }
		bool					GetPickedInstanceVID(DWORD* p_dwPrivateShopVID);
		bool					GetPrivateShopPosition(DWORD dwVirtualID, TPixelPosition* pPosition);
		void					SetSelectedInstance(TPrivateShopInstance* pSelectedInstance) { m_pSelectedInstance = pSelectedInstance; }
		void					SelectSearchPrivateShop(DWORD dwVirtualID);
		void					UnselectSearchPrivateShop(DWORD dwVirtualID);
		void					ClearSelectedSearchPrivateShop();
		void					Render();
		void					Deform();
		void					Update();
		void					Destroy();

		// Private Shop
		void		BuildPrivateShop(const char* c_szName, DWORD dwPolyVnum, BYTE bTitleType, BYTE bPageCount);
		void		ClearPrivateShopStock();
		void		AddPrivateShopItemStock(TItemPos ItemPos, WORD wDisplayPos, long long llPrice, DWORD dwCheque);
		void		DelPrivateShopItemStock(TItemPos ItemPos);
		void		GetPrivateShopItemPrice(TItemPos ItemPos, long long& r_llGold, DWORD& r_dwCheque);
		long long	GetTotalStockGold();
		DWORD		GetTotalStockCheque();
		void	SetItemData(const TPrivateShopItemData& c_rShopItemData, bool bIsMainPlayerPrivateShop);
		bool	GetItemData(WORD wPos, const TPrivateShopItemData** c_ppItemData, bool bIsMainPlayerPrivateShop);
		void	RemoveItemData(WORD wPos, bool bIsMainPlayerPrivateShop);
		void	ChangeItemPrice(WORD wPos, long long llGold, DWORD dwCheque);
		void	MoveItem(WORD wPos, WORD wChangePos);

		/* 'My' prefix refers to the main instance's private shop */
		void	SetMyState(BYTE bState) { m_bMyState = bState; }
		void	SetGold(long long llGold) { m_llGold = llGold; }
		void	SetCheque(DWORD dwCheque) { m_dwCheque = dwCheque; }
		void	ChangeGold(long long llGold) { m_llGold += llGold; }
		void	ChangeCheque(DWORD dwCheque) { m_dwCheque += dwCheque; }
		void	SetLocation(long lX, long lY, BYTE bChannel) { m_lX = lX, m_lY = lY, m_bChannel = bChannel; }
		void	SetMyTitle(std::string strTitle) { m_strMyTitle = strTitle; }
		void	SetPremiumTime(DWORD dwPremiumTime) { m_dwPremiumTime = dwPremiumTime; }
		void	SetMyPageCount(BYTE bPageCount) { m_bMyPageCount = bPageCount; }

		BYTE		GetMyState() { return m_bMyState; }
		long long	GetGold() { return m_llGold; }
		DWORD		GetCheque() { return m_dwCheque; }
		void		GetLocation(long& lX, long& lY, BYTE& bChannel) { lX = m_lX, lY = m_lY, bChannel = m_bChannel; }
		std::string	GetMyTitle() { return m_strMyTitle; }
		DWORD		GetPremiumTime() { return m_dwPremiumTime; }
		BYTE		GetMyPageCount() { return m_bMyPageCount; }
		long long	GetTotalGold();
		DWORD		GetTotalCheque();

		void		SetTitle(std::string strTitle) { m_strTitle = strTitle; }
		std::string	GeTitle() { return m_strTitle; }
		void		SetState(BYTE bState) { m_bState = bState; }
		BYTE		GetState() { return m_bState; }
		void		SetPageCount(BYTE bPageCount) { m_bPageCount = bPageCount; }
		BYTE		GetPageCount() { return m_bPageCount; }

		void		SetMainPlayerPrivateShop(bool bIsMainPlayerPrivateShop) { m_bIsMainPlayerPrivateShop = bIsMainPlayerPrivateShop; }
		bool		IsMainPlayerPrivateShop() { return m_bIsMainPlayerPrivateShop; }

		void		ClearSearchResult();
		void		SortSearchResult();
		void		SetSearchItemData(TPrivateShopSearchData& rSearchItem);
		bool		GetSearchItemData(WORD wIndex, const TPrivateShopSearchData** c_pSearchItem);

		void		SetResultPage(BYTE bPage) { m_bResultPage = bPage; }
		BYTE		GetResultPage() { return m_bResultPage; }

		void		SetActiveVID(DWORD dwVID) { m_dwActiveVID = dwVID; }
		DWORD		GetActiveVID() { return m_dwActiveVID; }

		size_t		GetResultMaxCount() { return m_vec_searchItem.size(); }

	private:
		typedef std::map<TItemPos, TPrivateShopItem> TPrivateShopItemStock;
		TPrivateShopItemStock	m_map_privateShopItemStock;

		TPrivateShopItemData	m_aPrivateShopItem[PRIVATE_SHOP_HOST_ITEM_MAX_NUM];
		TPrivateShopItemData	m_aMyPrivateShopItem[PRIVATE_SHOP_HOST_ITEM_MAX_NUM];

		long long		m_llGold;
		DWORD			m_dwCheque;
		long			m_lX;
		long			m_lY;
		long			m_lMapIndex;
		BYTE			m_bChannel;
		DWORD			m_dwPremiumTime;
		std::string		m_strMyTitle;
		BYTE			m_bMyState;
		BYTE			m_bMyPageCount;

		std::string		m_strTitle;
		BYTE			m_bState;
		bool			m_bIsMainPlayerPrivateShop;
		BYTE			m_bPageCount;
		DWORD			m_dwActiveVID;

		std::unordered_map<DWORD, std::unique_ptr<TPrivateShopInstance> >	m_map_privateShopInstance;
		std::unordered_map<DWORD, TPrivateShopInstance*>					m_map_selectedPrivateShop;
		TPrivateShopInstance* m_pSelectedInstance;

		std::vector<TPrivateShopSearchData>	m_vec_searchItem;
		BYTE	m_bResultPage;

		std::vector<TAppearanceDeco>			m_vec_appearanceDeco;
		std::vector<TTitleDeco>			m_vec_titleDeco;
};

