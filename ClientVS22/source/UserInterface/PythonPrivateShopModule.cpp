#include "StdAfx.h"
#include "PythonPrivateShop.h"
#include "PythonNetworkStream.h"
#include "PythonPlayer.h"

#include "../GameLib/ItemManager.h"

PyObject* privateShopBuild(PyObject* poSelf, PyObject* poArgs)
{
	TraceError("PRIVATESHOP_CLIENT: py_build_begin");
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
	{
		TraceError("PRIVATESHOP_CLIENT: py_build_fail reason=arg_title");
		return Py_BuildException();
	}

	DWORD dwPolyVnum;
	if (!PyTuple_GetUnsignedLong(poArgs, 1, &dwPolyVnum))
	{
		TraceError("PRIVATESHOP_CLIENT: py_build_fail reason=arg_poly title=%s", szName);
		return Py_BuildException();
	}

	BYTE bTitleType;
	if (!PyTuple_GetByte(poArgs, 2, &bTitleType))
	{
		TraceError("PRIVATESHOP_CLIENT: py_build_fail reason=arg_title_type title=%s poly=%u", szName, dwPolyVnum);
		return Py_BuildException();
	}

	BYTE bPageCount;
	if (!PyTuple_GetByte(poArgs, 3, &bPageCount))
	{
		TraceError("PRIVATESHOP_CLIENT: py_build_fail reason=arg_page_count title=%s poly=%u title_type=%u", szName, dwPolyVnum, bTitleType);
		return Py_BuildException();
	}

	TraceError("PRIVATESHOP_CLIENT: py_build_call title=%s poly=%u title_type=%u page_count=%u", szName, dwPolyVnum, bTitleType, bPageCount);
	CPythonPrivateShop::Instance().BuildPrivateShop(szName, dwPolyVnum, bTitleType, bPageCount);
	TraceError("PRIVATESHOP_CLIENT: py_build_done title=%s", szName);
	return Py_BuildNone();
}

PyObject* privateShopClearPrivateShopStock(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPrivateShop::Instance().ClearPrivateShopStock();
	return Py_BuildNone();
}

PyObject* privateShopAddItemStock(PyObject* poSelf, PyObject* poArgs)
{
	TraceError("PRIVATESHOP_CLIENT: py_add_stock_begin");
	BYTE bItemWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bItemWindowType))
	{
		TraceError("PRIVATESHOP_CLIENT: py_add_stock_fail reason=arg_window");
		return Py_BuildException();
	}

	WORD wItemSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wItemSlotIndex))
	{
		TraceError("PRIVATESHOP_CLIENT: py_add_stock_fail reason=arg_slot window=%u", bItemWindowType);
		return Py_BuildException();
	}

	int iDisplaySlotIndex;
	if (!PyTuple_GetInteger(poArgs, 2, &iDisplaySlotIndex))
	{
		TraceError("PRIVATESHOP_CLIENT: py_add_stock_fail reason=arg_display window=%u slot=%u", bItemWindowType, wItemSlotIndex);
		return Py_BuildException();
	}

	long long llPrice;
	if (!PyTuple_GetLongLong(poArgs, 3, &llPrice))
	{
		TraceError("PRIVATESHOP_CLIENT: py_add_stock_fail reason=arg_price window=%u slot=%u display=%d", bItemWindowType, wItemSlotIndex, iDisplaySlotIndex);
		return Py_BuildException();
	}

	DWORD dwCheque;
	if (!PyTuple_GetUnsignedLong(poArgs, 4, &dwCheque))
	{
		TraceError("PRIVATESHOP_CLIENT: py_add_stock_fail reason=arg_cheque window=%u slot=%u display=%d gold=%lld", bItemWindowType, wItemSlotIndex, iDisplaySlotIndex, llPrice);
		return Py_BuildException();
	}

	TraceError("PRIVATESHOP_CLIENT: py_add_stock_call window=%u slot=%u display=%d gold=%lld cheque=%u", bItemWindowType, wItemSlotIndex, iDisplaySlotIndex, llPrice, dwCheque);
	CPythonPrivateShop::Instance().AddPrivateShopItemStock(TItemPos(bItemWindowType, wItemSlotIndex), iDisplaySlotIndex, llPrice, 0);
	TraceError("PRIVATESHOP_CLIENT: py_add_stock_done window=%u slot=%u display=%d", bItemWindowType, wItemSlotIndex, iDisplaySlotIndex);
	return Py_BuildNone();
}

PyObject* privateShopDeleteItemStock(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bItemWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bItemWindowType))
		return Py_BuildException();

	WORD wItemSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wItemSlotIndex))
		return Py_BuildException();

	CPythonPrivateShop::Instance().DelPrivateShopItemStock(TItemPos(bItemWindowType, wItemSlotIndex));
	return Py_BuildNone();
}

PyObject* privateShopGetStockItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bItemWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bItemWindowType))
		return Py_BuildException();

	WORD wItemSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wItemSlotIndex))
		return Py_BuildException();

	long long llGold = 0;
	DWORD dwCheque = 0;

	CPythonPrivateShop::Instance().GetPrivateShopItemPrice(TItemPos(bItemWindowType, wItemSlotIndex), llGold, dwCheque);

	return Py_BuildValue("L", llGold);
}

PyObject* privateShopGetStockChequeItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bItemWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bItemWindowType))
		return Py_BuildException();

	WORD wItemSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wItemSlotIndex))
		return Py_BuildException();

	long long llGold = 0;
	DWORD dwCheque = 0;

	CPythonPrivateShop::Instance().GetPrivateShopItemPrice(TItemPos(bItemWindowType, wItemSlotIndex), llGold, dwCheque);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetTotalStockGold(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("L", CPythonPrivateShop::Instance().GetTotalStockGold());
}

PyObject* privateShopGetTotalStockCheque(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetLocation(PyObject* poSelf, PyObject* poArgs)
{
	long lX, lY;
	BYTE bChannel;

	CPythonPrivateShop::Instance().GetLocation(lX, lY, bChannel);

	return Py_BuildValue("iii", lX, lY, bChannel);
}

PyObject* privateShopGetTitle(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("s", CPythonPrivateShop::Instance().GeTitle().c_str());
}

PyObject* privateShopGetMyTitle(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("s", CPythonPrivateShop::Instance().GetMyTitle().c_str());
}

PyObject* privateShopGetGold(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("L", CPythonPrivateShop::Instance().GetGold());
}

PyObject* privateShopGetCheque(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetPremiumTime(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetPremiumTime());
}

PyObject* privateShopGetMyState(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetMyState());
}

PyObject* privateShopGetState(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetState());
}

PyObject* privateShopGetPageCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetPageCount());
}

PyObject* privateShopGetMyPageCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetMyPageCount());
}

PyObject* privateShopGetTotalGold(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("L", CPythonPrivateShop::Instance().GetTotalGold());
}

PyObject* privateShopGetTotalCheque(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetActiveVID(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetActiveVID());
}

PyObject* privateShopGetItemVnum(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	bool bIsMainPlayerPrivateShop = CPythonPrivateShop::Instance().IsMainPlayerPrivateShop();

	const TPrivateShopItemData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetItemData(iPos, &c_pItemData, bIsMainPlayerPrivateShop))
		return Py_BuildValue("i", c_pItemData->dwVnum);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetItemCount(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	bool bIsMainPlayerPrivateShop = CPythonPrivateShop::Instance().IsMainPlayerPrivateShop();

	const TPrivateShopItemData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetItemData(iPos, &c_pItemData, bIsMainPlayerPrivateShop))
		return Py_BuildValue("i", c_pItemData->dwCount);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	bool bIsMainPlayerPrivateShop = CPythonPrivateShop::Instance().IsMainPlayerPrivateShop();

	const TPrivateShopItemData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetItemData(iPos, &c_pItemData, bIsMainPlayerPrivateShop))
		return Py_BuildValue("L", c_pItemData->TPrice.llGold);

	return Py_BuildValue("L", 0);
}

PyObject* privateShopGetChequeItemPrice(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	bool bIsMainPlayerPrivateShop = CPythonPrivateShop::Instance().IsMainPlayerPrivateShop();

	const TPrivateShopItemData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetItemData(iPos, &c_pItemData, bIsMainPlayerPrivateShop))
		return Py_BuildValue("i", 0);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetItemMetinSocket(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	int iMetinSocketIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketIndex))
		return Py_BuildException();

	bool bIsMainPlayerPrivateShop = CPythonPrivateShop::Instance().IsMainPlayerPrivateShop();

	const TPrivateShopItemData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetItemData(iPos, &c_pItemData, bIsMainPlayerPrivateShop))
		return Py_BuildValue("i", c_pItemData->alSockets[iMetinSocketIndex]);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetItemAttribute(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	int iAttrSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iAttrSlotIndex))
		return Py_BuildException();

	if (iAttrSlotIndex >= 0 && iAttrSlotIndex < ITEM_ATTRIBUTE_SLOT_MAX_NUM)
	{

		bool bIsMainPlayerPrivateShop = CPythonPrivateShop::Instance().IsMainPlayerPrivateShop();

		const TPrivateShopItemData* c_pItemData;
		if (CPythonPrivateShop::Instance().GetItemData(iPos, &c_pItemData, bIsMainPlayerPrivateShop))
			return Py_BuildValue("ii", c_pItemData->aAttr[iAttrSlotIndex].bType, c_pItemData->aAttr[iAttrSlotIndex].sValue);
	}

	return Py_BuildValue("ii", 0, 0);
}

PyObject* privateShopClearSearchResult(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPrivateShop::Instance().ClearSearchResult();
	return Py_BuildNone();
}

PyObject* privateShopGetSearchResultMaxCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetResultMaxCount());
}

PyObject* privateShopGetSearchResultPage(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonPrivateShop::Instance().GetResultPage());
}

PyObject* privateShopSetSearchResultPage(PyObject* poSelf, PyObject* poArgs)
{
	int iPage;
	if (!PyTuple_GetInteger(poArgs, 0, &iPage))
		return Py_BuildException();

	CPythonPrivateShop::Instance().SetResultPage((BYTE)iPage);

	return Py_BuildNone();
}

PyObject* privateShopGetSearchResult(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	const TPrivateShopSearchData* c_pItemData;
	if (!CPythonPrivateShop::Instance().GetSearchItemData(iPos, &c_pItemData))
		return Py_BuildValue("isiLi", 0, "", 0, 0, 0);

	return Py_BuildValue("isiLi", c_pItemData->dwVnum, c_pItemData->szOwnerName, c_pItemData->dwCount, c_pItemData->TPrice.llGold, 0);
}

PyObject* privateShopGetSearchItemVnum(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	const TPrivateShopSearchData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetSearchItemData(iPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->dwVnum);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetSearchItemMetinSocket(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	int iMetinSocketIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iMetinSocketIndex))
		return Py_BuildException();

	const TPrivateShopSearchData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetSearchItemData(iPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->alSockets[iMetinSocketIndex]);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopGetSearchItemAttribute(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	int iAttrSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iAttrSlotIndex))
		return Py_BuildException();

	if (iAttrSlotIndex >= 0 && iAttrSlotIndex < ITEM_ATTRIBUTE_SLOT_MAX_NUM)
	{
		const TPrivateShopSearchData* c_pItemData;
		if (CPythonPrivateShop::Instance().GetSearchItemData(iPos, &c_pItemData))
			return Py_BuildValue("ii", c_pItemData->aAttr[iAttrSlotIndex].bType, c_pItemData->aAttr[iAttrSlotIndex].sValue);
	}

	return Py_BuildValue("ii", 0, 0);
}

PyObject* privateShopGetSeachItemVID(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
		return Py_BuildException();

	const TPrivateShopSearchData* c_pItemData;
	if (CPythonPrivateShop::Instance().GetSearchItemData(iPos, &c_pItemData))
		return Py_BuildValue("i", c_pItemData->dwShopID);

	return Py_BuildValue("i", 0);
}

PyObject* privateShopIsMainPlayerPrivateShop(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("b", CPythonPrivateShop::Instance().IsMainPlayerPrivateShop());
}

PyObject* privateShopGetName(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CPythonPrivateShop::SPrivateShopInstance* pPrivateShopInstance = CPythonPrivateShop::Instance().GetPrivateShopInstance(iVirtualID);
	if (!pPrivateShopInstance)
		return Py_BuildValue("s", "");

	return Py_BuildValue("s", pPrivateShopInstance->GetName());
}

PyObject* privateShopGetProjectPosition(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	int iHeight;
	if (!PyTuple_GetInteger(poArgs, 1, &iHeight))
		return Py_BuildException();

	CPythonPrivateShop::SPrivateShopInstance* pPrivateShopInstance = CPythonPrivateShop::Instance().GetPrivateShopInstance(iVirtualID);
	if (!pPrivateShopInstance)
		return Py_BuildValue("ii", -100, -100);

	CGraphicThingInstance* pInstance = pPrivateShopInstance->GetGraphicThingInstancePtr();
	if (!pInstance)
		return Py_BuildValue("ii", -100, -100);

	const TPixelPosition c_PixelPosition = pInstance->GetPosition();

	CPythonGraphic& rpyGraphic = CPythonGraphic::Instance();

	float fx, fy, fz;
	rpyGraphic.ProjectPosition(c_PixelPosition.x,
		c_PixelPosition.y, // Using different axis on the private shop instance -_-
		c_PixelPosition.z + float(iHeight),
		&fx, &fy, &fz);

	if (1 == int(fz))
		return Py_BuildValue("ii", -100, -100);

	return Py_BuildValue("ii", int(fx), int(fy));
}

PyObject* privateShopGetMainCharacterDistance(PyObject* poSelf, PyObject* poArgs)
{
	int iVirtualID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVirtualID))
		return Py_BuildException();

	CPythonPrivateShop::SPrivateShopInstance* pPrivateShopInstance = CPythonPrivateShop::Instance().GetPrivateShopInstance(iVirtualID);
	if (!pPrivateShopInstance)
		return Py_BuildValue("f", -1.0f);

	CGraphicThingInstance* pInstance = pPrivateShopInstance->GetGraphicThingInstancePtr();
	if (!pInstance)
		return Py_BuildValue("f", -1.0f);

	if (!pInstance->isShow())
		return Py_BuildValue("f", -1.0f);

	const TPixelPosition c_ShopPosition = pInstance->GetPosition();

	TPixelPosition MainActorPosition;
	CPythonPlayer::Instance().NEW_GetMainActorPosition(&MainActorPosition);

	float fdx = MainActorPosition.x - c_ShopPosition.x;
	float fdy = MainActorPosition.y - (-c_ShopPosition.y); // Using different axis on the private shop instance -_-

	float fDistance = sqrtf((fdx * fdx) + (fdy * fdy));

	return Py_BuildValue("f", fDistance);
}

PyObject* privateShopCreatePrivateShopSearchPos(PyObject* poSelf, PyObject* poArgs)
{
	char* c_szOwnerName;
	if (!PyTuple_GetString(poArgs, 0, &c_szOwnerName))
		return Py_BuildException();

	DWORD dwVID = CPythonPrivateShop::Instance().GetPrivateShopInstanceVID(c_szOwnerName);
	if (dwVID)
		CPythonPrivateShop::Instance().SelectSearchPrivateShop(dwVID);

	return Py_BuildNone();
}

PyObject* privateShopDeletePrivateShopSearchPos(PyObject* poSelf, PyObject* poArgs)
{
	switch (PyTuple_Size(poArgs))
	{
		case 0:
		{
			CPythonPrivateShop::Instance().ClearSelectedSearchPrivateShop();
		}
		break;

		case 1:
		{
			char* c_szOwnerName;
			if (!PyTuple_GetString(poArgs, 0, &c_szOwnerName))
				return Py_BuildException();

			DWORD dwVID = CPythonPrivateShop::Instance().GetPrivateShopInstanceVID(c_szOwnerName);
			if (dwVID)
				CPythonPrivateShop::Instance().UnselectSearchPrivateShop(dwVID);
		}
		break;
	}

	return Py_BuildNone();
}

void initPrivateShop()
{
	static PyMethodDef s_methods[] =
	{

		{ "BuildPrivateShop",				privateShopBuild,						METH_VARARGS },
		{ "ClearPrivateShopStock",			privateShopClearPrivateShopStock,		METH_VARARGS },
		{ "AddItemStock",					privateShopAddItemStock,				METH_VARARGS },
		{ "DeleteItemStock",				privateShopDeleteItemStock,				METH_VARARGS },
		{ "GetStockItemPrice",				privateShopGetStockItemPrice,			METH_VARARGS },
		{ "GetStockChequeItemPrice",		privateShopGetStockChequeItemPrice,		METH_VARARGS },
		{ "GetTotalStockGold",				privateShopGetTotalStockGold,			METH_VARARGS },
		{ "GetTotalStockCheque",			privateShopGetTotalStockCheque,			METH_VARARGS },
		{ "GetLocation",					privateShopGetLocation,					METH_VARARGS },
		{ "GetMyTitle",						privateShopGetMyTitle,					METH_VARARGS },
		{ "GetTitle",						privateShopGetTitle,					METH_VARARGS },
		{ "GetGold",						privateShopGetGold,						METH_VARARGS },
		{ "GetCheque",						privateShopGetCheque,					METH_VARARGS },
		{ "GetPremiumTime",					privateShopGetPremiumTime,				METH_VARARGS },
		{ "GetMyState",						privateShopGetMyState,					METH_VARARGS },
		{ "GetState",						privateShopGetState,					METH_VARARGS },
		{ "GetPageCount",					privateShopGetPageCount,				METH_VARARGS },
		{ "GetMyPageCount",					privateShopGetMyPageCount,				METH_VARARGS },
		{ "GetTotalGold",					privateShopGetTotalGold,				METH_VARARGS },
		{ "GetTotalCheque",					privateShopGetTotalCheque,				METH_VARARGS },
		{ "GetActiveVID",					privateShopGetActiveVID,				METH_VARARGS },

		{ "GetItemVnum",					privateShopGetItemVnum,					METH_VARARGS },
		{ "GetItemCount",					privateShopGetItemCount,				METH_VARARGS },
		{ "GetItemPrice",					privateShopGetItemPrice,				METH_VARARGS },
		{ "GetChequeItemPrice",				privateShopGetChequeItemPrice,			METH_VARARGS },
		{ "GetItemMetinSocket",				privateShopGetItemMetinSocket,			METH_VARARGS },
		{ "GetItemAttribute",				privateShopGetItemAttribute,			METH_VARARGS },

		{ "ClearSearchResult",				privateShopClearSearchResult,			METH_VARARGS },
		{ "GetSearchResultMaxCount",		privateShopGetSearchResultMaxCount,		METH_VARARGS },
		{ "GetSearchResultPage",			privateShopGetSearchResultPage,			METH_VARARGS },
		{ "SetSearchResultPage",			privateShopSetSearchResultPage,			METH_VARARGS },
		{ "GetSearchResult",				privateShopGetSearchResult,				METH_VARARGS },

		{ "GetSearchItemVnum",				privateShopGetSearchItemVnum,			METH_VARARGS },
		{ "GetSearchItemMetinSocket",		privateShopGetSearchItemMetinSocket,	METH_VARARGS },
		{ "GetSearchItemAttribute",			privateShopGetSearchItemAttribute,		METH_VARARGS },
		{ "GetSeachItemVID",				privateShopGetSeachItemVID,				METH_VARARGS },

		{ "IsMainPlayerPrivateShop",		privateShopIsMainPlayerPrivateShop,		METH_VARARGS },
		{ "GetName",						privateShopGetName,						METH_VARARGS },
		{ "GetProjectPosition",				privateShopGetProjectPosition,			METH_VARARGS },
		{ "GetMainCharacterDistance",		privateShopGetMainCharacterDistance,	METH_VARARGS },
		{ "CreatePrivateShopSearchPos",		privateShopCreatePrivateShopSearchPos,	METH_VARARGS },
		{ "DeletePrivateShopSearchPos",		privateShopDeletePrivateShopSearchPos,	METH_VARARGS },

		{ NULL,							NULL,										NULL },
	};

	PyObject* poModule = Py_InitModule("privateShop",						s_methods);

	PyModule_AddIntConstant(poModule, "PRIVATE_SHOP_PAGE_MAX_NUM",			PRIVATE_SHOP_PAGE_MAX_NUM);
	PyModule_AddIntConstant(poModule, "PRIVATE_SHOP_WIDTH",					PRIVATE_SHOP_WIDTH);
	PyModule_AddIntConstant(poModule, "PRIVATE_SHOP_HEIGHT",				PRIVATE_SHOP_HEIGHT);
	PyModule_AddIntConstant(poModule, "PRIVATE_SHOP_HOST_ITEM_MAX_NUM",		PRIVATE_SHOP_HOST_ITEM_MAX_NUM);
	PyModule_AddIntConstant(poModule, "PRIVATE_SHOP_PAGE_ITEM_MAX_NUM",		PRIVATE_SHOP_PAGE_ITEM_MAX_NUM);
	PyModule_AddIntConstant(poModule, "STATE_CLOSED",						STATE_CLOSED);
	PyModule_AddIntConstant(poModule, "STATE_OPEN",							STATE_OPEN);
	PyModule_AddIntConstant(poModule, "STATE_MODIFY",						STATE_MODIFY);
	PyModule_AddIntConstant(poModule, "TITLE_MAX_LEN",						TITLE_MAX_LEN);
	PyModule_AddIntConstant(poModule, "TITLE_MIN_LEN",						TITLE_MIN_LEN);
	PyModule_AddIntConstant(poModule, "MODE_NONE",							MODE_NONE);
	PyModule_AddIntConstant(poModule, "MODE_LOOK",							MODE_LOOK);
	PyModule_AddIntConstant(poModule, "MODE_TRADE",							MODE_TRADE);
	PyModule_AddIntConstant(poModule, "RESULT_MAX_NUM",						RESULT_MAX_NUM);
	PyModule_AddIntConstant(poModule, "MAX_VIEW_DISTANCE",					MAX_VIEW_DISTANCE);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_ITEM_VNUM",				FILTER_TYPE_ITEM_VNUM);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_ITEM_TYPE",				FILTER_TYPE_ITEM_TYPE);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_ITEM_SUBTYPE",			FILTER_TYPE_ITEM_SUBTYPE);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_CLASS",					FILTER_TYPE_CLASS);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_MIN_LEVEL",				FILTER_TYPE_MIN_LEVEL);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_MAX_LEVEL",				FILTER_TYPE_MAX_LEVEL);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_MIN_REFINEMENT",			FILTER_TYPE_MIN_REFINEMENT);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_MAX_REFINEMENT",			FILTER_TYPE_MAX_REFINEMENT);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_MIN_CHEQUE",				FILTER_TYPE_MIN_CHEQUE);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_MAX_CHEQUE",				FILTER_TYPE_MAX_CHEQUE);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_MIN_GOLD",				FILTER_TYPE_MIN_GOLD);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_MAX_GOLD",				FILTER_TYPE_MAX_GOLD);
	PyModule_AddIntConstant(poModule, "FILTER_TYPE_MAX_NUM",				FILTER_TYPE_MAX_NUM);
}

