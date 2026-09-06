#include "StdAfx.h"
#include "PythonPrivateShop.h"
#include "PythonNetworkStream.h"

CPythonPrivateShop::CPythonPrivateShop()
{
	Clear();
	ClearMyPrivateShop();

	m_pSelectedInstance = nullptr;
	m_bResultPage = 0;
}

CPythonPrivateShop::~CPythonPrivateShop()
{
}

struct ItemStockSortFunc
{
	bool operator() (TPrivateShopItem& rkLeft, TPrivateShopItem& rkRight)
	{
		return rkLeft.wDisplayPos < rkRight.wDisplayPos;
	}
};

void CPythonPrivateShop::ClearMyPrivateShop()
{
	m_llGold = 0;
	m_dwCheque = 0;
	m_lX = 0;
	m_lY = 0;
	m_strMyTitle = "";
	m_bMyState = STATE_CLOSED;
	m_bMyPageCount = 0;

	memset(m_aMyPrivateShopItem, 0, sizeof(m_aMyPrivateShopItem));
	memset(m_aPrivateShopItem, 0, sizeof(m_aPrivateShopItem));
	m_map_privateShopItemStock.clear();
}

void CPythonPrivateShop::Clear()
{
	m_strTitle = "";
	m_bState = STATE_CLOSED;
	m_bPageCount = 0;
	m_dwActiveVID = 0;

	memset(m_aPrivateShopItem, 0, sizeof(m_aPrivateShopItem));
	memset(m_aMyPrivateShopItem, 0, sizeof(m_aMyPrivateShopItem));

	m_bIsMainPlayerPrivateShop = false;
}

void CPythonPrivateShop::BuildPrivateShop(const char* c_szName, DWORD dwPolyVnum, BYTE bTitleType, BYTE bPageCount)
{
	TraceError("PRIVATESHOP_CLIENT: build_begin title=%s poly=%u title_type=%u page_count=%u stock_count=%u",
		c_szName ? c_szName : "", dwPolyVnum, bTitleType, bPageCount, static_cast<unsigned>(m_map_privateShopItemStock.size()));
	std::vector<TPrivateShopItem> vec_itemStock;
	vec_itemStock.reserve(m_map_privateShopItemStock.size());

	for (auto it = m_map_privateShopItemStock.begin(); it != m_map_privateShopItemStock.end(); ++it)
	{
		TraceError("PRIVATESHOP_CLIENT: build_stock_collect window=%u cell=%u display=%u gold=%lld cheque=%u",
			it->second.TPos.window_type, it->second.TPos.cell, it->second.wDisplayPos, it->second.TPrice.llGold, it->second.TPrice.dwCheque);
		vec_itemStock.push_back(it->second);
	}

	std::sort(vec_itemStock.begin(), vec_itemStock.end(), ItemStockSortFunc());
	TraceError("PRIVATESHOP_CLIENT: build_stock_sorted count=%u", static_cast<unsigned>(vec_itemStock.size()));

	const bool bSent = CPythonNetworkStream::Instance().SendBuildPrivateShopPacket(c_szName, dwPolyVnum, bTitleType, bPageCount, vec_itemStock);
	TraceError("PRIVATESHOP_CLIENT: build_send_done title=%s success=%d", c_szName ? c_szName : "", bSent ? 1 : 0);

	// Do not clear the stock until we receive positive response from server
	// ClearPrivateShopStock();
}

void CPythonPrivateShop::ClearPrivateShopStock()
{
	TraceError("PRIVATESHOP_CLIENT: clear_stock count=%u", static_cast<unsigned>(m_map_privateShopItemStock.size()));
	m_map_privateShopItemStock.clear();
}

void CPythonPrivateShop::AddPrivateShopItemStock(TItemPos ItemPos, WORD wDisplayPos, long long llPrice, DWORD dwCheque)
{
	if (llPrice <= 0)
	{
		TraceError("PRIVATESHOP_CLIENT: add_stock_fail reason=invalid_price window=%u cell=%u display=%u gold=%lld cheque=%u",
			ItemPos.window_type, ItemPos.cell, wDisplayPos, llPrice, dwCheque);
		return;
	}

	TraceError("PRIVATESHOP_CLIENT: add_stock_begin window=%u cell=%u display=%u gold=%lld cheque=%u", ItemPos.window_type, ItemPos.cell, wDisplayPos, llPrice, dwCheque);
	DelPrivateShopItemStock(ItemPos);

	TPrivateShopItem sellingItem;
	sellingItem.TPos = ItemPos;
	sellingItem.TPrice.llGold = llPrice;
	sellingItem.TPrice.dwCheque = 0;
	sellingItem.wDisplayPos = wDisplayPos;

	m_map_privateShopItemStock.emplace(ItemPos, sellingItem);
	TraceError("PRIVATESHOP_CLIENT: add_stock_done window=%u cell=%u display=%u stock_count=%u",
		ItemPos.window_type, ItemPos.cell, wDisplayPos, static_cast<unsigned>(m_map_privateShopItemStock.size()));
}

void CPythonPrivateShop::DelPrivateShopItemStock(TItemPos ItemPos)
{
	if (m_map_privateShopItemStock.end() == m_map_privateShopItemStock.find(ItemPos))
	{
		TraceError("PRIVATESHOP_CLIENT: del_stock_skip reason=not_found window=%u cell=%u", ItemPos.window_type, ItemPos.cell);
		return;
	}

	m_map_privateShopItemStock.erase(ItemPos);
	TraceError("PRIVATESHOP_CLIENT: del_stock_done window=%u cell=%u stock_count=%u", ItemPos.window_type, ItemPos.cell, static_cast<unsigned>(m_map_privateShopItemStock.size()));
}

void CPythonPrivateShop::GetPrivateShopItemPrice(TItemPos ItemPos, long long& r_llGold, DWORD& r_dwCheque)
{
	auto it = m_map_privateShopItemStock.find(ItemPos);

	if (m_map_privateShopItemStock.end() == it)
	{
		r_llGold = 0;
		r_dwCheque = 0;
		return;
	}

	const TPrivateShopItem& c_rShopItem = it->second;

	r_llGold = c_rShopItem.TPrice.llGold;
	r_dwCheque = c_rShopItem.TPrice.dwCheque;
}

long long CPythonPrivateShop::GetTotalStockGold()
{
	long long llTotalGold = 0;

	for (const auto& kv : m_map_privateShopItemStock)
		llTotalGold += kv.second.TPrice.llGold;

	return llTotalGold;
}

DWORD CPythonPrivateShop::GetTotalStockCheque()
{
	return 0;
}

void CPythonPrivateShop::SetItemData(const TPrivateShopItemData& c_rShopItemData, bool bIsMainPlayerPrivateShop /* = false */)
{
	if (c_rShopItemData.wPos >= PRIVATE_SHOP_HOST_ITEM_MAX_NUM)
		return;

	// Set item data based on shop type
	if (bIsMainPlayerPrivateShop)
		m_aMyPrivateShopItem[c_rShopItemData.wPos] = c_rShopItemData;
	else
		m_aPrivateShopItem[c_rShopItemData.wPos] = c_rShopItemData; 
}

bool CPythonPrivateShop::GetItemData(WORD wPos, const TPrivateShopItemData** c_ppItemData, bool bIsMainPlayerPrivateShop)
{
	if (wPos >= PRIVATE_SHOP_HOST_ITEM_MAX_NUM)
		return false;

	// Select item data based on shop type
	if(bIsMainPlayerPrivateShop)
		*c_ppItemData = &m_aMyPrivateShopItem[wPos];
	else
		*c_ppItemData = &m_aPrivateShopItem[wPos];

	return true;
}

void CPythonPrivateShop::RemoveItemData(WORD wPos, bool bIsMainPlayerPrivateShop)
{
	if (wPos >= PRIVATE_SHOP_HOST_ITEM_MAX_NUM)
		return;

	if (bIsMainPlayerPrivateShop)
		memset(&m_aMyPrivateShopItem[wPos], 0, sizeof(TPrivateShopItemData));
	else
		memset(&m_aPrivateShopItem[wPos], 0, sizeof(TPrivateShopItemData));
}

void CPythonPrivateShop::ChangeItemPrice(WORD wPos, long long llGold, DWORD dwCheque)
{
	TPrivateShopItemData* pItemData;

	if (IsMainPlayerPrivateShop())
		pItemData = &m_aMyPrivateShopItem[wPos];
	else
		pItemData = &m_aPrivateShopItem[wPos];

	if (pItemData)
	{
		pItemData->TPrice.llGold = llGold;
		pItemData->TPrice.dwCheque = 0;
	}
}

void CPythonPrivateShop::MoveItem(WORD wPos, WORD wChangePos)
{
	bool bIsMainPlayerPrivateShop = IsMainPlayerPrivateShop();

	if (bIsMainPlayerPrivateShop)
	{
		memcpy(&m_aMyPrivateShopItem[wChangePos], &m_aMyPrivateShopItem[wPos], sizeof(TPrivateShopItemData));
		RemoveItemData(wPos, bIsMainPlayerPrivateShop);
	}
	else
	{
		memcpy(&m_aPrivateShopItem[wChangePos], &m_aPrivateShopItem[wPos], sizeof(TPrivateShopItemData));
		RemoveItemData(wPos, bIsMainPlayerPrivateShop);
	}
}

long long CPythonPrivateShop::GetTotalGold()
{
	long long llTotalGold = GetGold();	// Gold in the stash

	for (const auto& rShopItem : m_aMyPrivateShopItem)
		llTotalGold += rShopItem.TPrice.llGold;

	return llTotalGold;
}

DWORD CPythonPrivateShop::GetTotalCheque()
{
	return 0;
}

void CPythonPrivateShop::ClearSearchResult()
{
	m_vec_searchItem.clear();
	m_bResultPage = 0;
}

void CPythonPrivateShop::SortSearchResult()
{
#ifdef ENABLE_PRIVATE_SHOP_PRICE_SORT
	static auto currency_sort_func = [](TPrivateShopSearchData& rkLeft, TPrivateShopSearchData& rkRight)
	{
		if (rkLeft.TPrice.dwCheque != rkRight.TPrice.dwCheque)
			return rkLeft.TPrice.dwCheque < rkRight.TPrice.dwCheque;

		return rkLeft.TPrice.llGold < rkRight.TPrice.llGold;
	};

	std::sort(m_vec_searchItem.begin(), m_vec_searchItem.end(), currency_sort_func);
#endif

	static auto time_sort_func = [](TPrivateShopSearchData& rkLeft, TPrivateShopSearchData& rkRight)
	{
		return rkLeft.tCheckin < rkRight.tCheckin;
	};

	std::sort(m_vec_searchItem.begin(), m_vec_searchItem.end(), time_sort_func);
}

void CPythonPrivateShop::SetSearchItemData(TPrivateShopSearchData& rSearchItem)
{
	m_vec_searchItem.push_back(rSearchItem);
}

bool CPythonPrivateShop::GetSearchItemData(WORD wIndex, const TPrivateShopSearchData** c_pSearchItem)
{
	if (wIndex >= m_vec_searchItem.size())
		return false;

	*c_pSearchItem = &m_vec_searchItem.at(wIndex);

	return true;
}

