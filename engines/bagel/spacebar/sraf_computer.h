
/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef BAGEL_SPACEBAR_SRAF_COMPUTER_H
#define BAGEL_SPACEBAR_SRAF_COMPUTER_H

#include "bagel/boflib/gui/button.h"
#include "bagel/boflib/gui/list_box.h"
#include "bagel/boflib/gui/text_box.h"
#include "bagel/boflib/gui/dialog.h"
#include "bagel/boflib/file.h"
#include "bagel/baglib/storage_dev_win.h"

namespace Bagel {
namespace SpaceBar {

class SrafCompItem {
public:
	const char *m_pItem;
};

enum SELLERS {
	IRK4 = 0, YEEF8, QUOSH23
};

class DealSummarySellerItem {
public:
	SELLERS m_eSellerID;
	int m_nSellerOffer;
	const char *m_pSellerName;
};

enum BUYERS {
	PYLON3 = 0, DIPPIK10, VEBBIL18, GUNG14,
	REYES24, GILD13, HEM20, DORK44, RAWLEY23,
	JELLA37, SINJIN11, HUNDEY42, CHANDRA15, CLANG2,
	MINMARTZN, MINMARTBA, MINMARTRG, MINMARTUT,
	MINMARTPN, MINMARTSZ
};

enum STAFFERS {
	NORG72 = 0, PNURTH81, ZIG64, LENTIL24, VARGAS20, CHURG53, DURTEEN97
};

class DealSummaryBuyerItem {
public:
	BUYERS m_eBuyerID;
	int m_nBuyerOffer;
	const char *m_pBuyerName;
};

#define mOtherParty     0x0001
#define mBuyer          0x0002
#define mSeller         0x0004

// The ordering of these flags must match the ordering of these same
// characters in the array in which they are defined.
#define mNorg72         0x0008
#define mPnurth81       0x0010
#define mZig64          0x0020
#define mLentil24       0x0040
#define mVargas20       0x0080
#define mChurg53        0x0100

#define mStafferMale        0x0200
#define mStafferFemale      0x0400

class DispatchedTeamItem {
public:
	int m_nFlags = 0;
	int m_nMeetWithID = 0;
	int m_nDispatchTime = 0;
	int m_nTeamCaptain = 0;
	int m_nMeetingTime = 0;
};

class SrafTextScreen : public CBofDialog {
private:
	static CBofWindow *m_pSaveActiveWin;

	CBofTextBox *m_pTextBox = nullptr;          // Bof text box for all kinds of cool info
	CBofBmpButton *m_pOKButton = nullptr;
	CBofString _text;

	CBofRect gCompDisplay;
	CBofRect gTextWindow;
	CBofRect gStafferDisplay;
	CBofRect gSrafTextWindow;

public:
	SrafTextScreen(const CBofString &str, bool isText = false);
	virtual ~SrafTextScreen();
	int createTextScreen(CBofWindow *pParent = nullptr);
	void displayTextScreen();

	void onBofButton(CBofObject *, int) override;
	void onClose() override;
	void onPaint(CBofRect * pRect) override;
};

// Number of constants...
#define NUM_MAIL_MESSAGES    8
#define NUM_MINERALS        14
#define NUM_STAFFERS        6
#define NUM_MAIN_ITEMS      11
#define NUM_SELLERS         3
#define NUM_BUYERS          20
#define NUM_OTHER_PARTYS    3
#define NUM_MUSICAL_SCORES  7
#define NUM_CODE_WORDS      10
#define NUM_SRAFCOMPBUTT    11
#define NUM_OFFERINGS       2

// Constants for get meet member

#define kOthersList     1
#define kSellersList    2
#define kBuyersList     3
#define kStaffersList   4

//	Sraffin text only screens (for stuff like biographies)

class SrafComputer : public CBagStorageDevWnd {
public:
	SrafComputer();
	virtual ~SrafComputer();
	virtual void onKeyHit(uint32 lKey, uint32 nRepCount);
	virtual ErrorCode attach();
	virtual ErrorCode detach();
	void onLButtonDown(uint32 nFlags, CBofPoint *xPoint, void * = nullptr);
	void onLButtonUp(uint32 nFlags, CBofPoint *xPoint, void * = nullptr);
	void onMainLoop();
	void onPaint(CBofRect *pRect);
	void onBofButton(CBofObject *, int);
	void onBofListBox(CBofObject *pListBox, int nItemIndex);
	void setOn();
	void setOff();
	void SetQuit();
	void alignAtColumn(CBofString &sStr, const char *szRightText, int nAlignAt);
	CBofString &buildBidString(int index);

	void deleteListBox();

	ErrorCode createListBox();

	void FillMain();

	void HideAllButtons();

	void ActivateDealSummary();
	void ActivateBuyerBids();
	void ActivateDealBackground();
	void ActivateSellerBios();
	void ActivateOtherBios();
	void ActivateStaffBios();
	void ActivateDispatchTeam();
	void ActivateCurrentEMail();
	void ActivateAudioSettings();
	void ActivateRoboButler();
	void ActivateCheckTeams();
	void ActivateCodeWords();

	void DeactivateMainScreen();
	void ActivateMainScreen();

	void InitDealSummary();
	void InitBuyerBids();
	void InitDealBackground();
	void InitSellerBios();
	void InitOtherBios();
	void InitStaffBios();
	void InitDispatchTeam();
	void InitCurrentEMail();
	void InitAudioSettings();
	void InitRoboButler();

	void OnListMainScreen();
	void OnListDealSummary();
	void OnListBuyerBids();
	void OnListDealBackground();
	void OnListSellerBios();
	void OnListOtherBios();
	void OnListStaffBios();
	void OnListDispatchTeam();
	void OnListCurrentEMail();
	void OnListAudioSettings();
	void OnListRoboButler();
	void OnListCheckTeams();
	void OnListCodeWords();

	void OnButtonMainScreen(CBofButton *pButton, int nState);
	void OnButtonDealSummary(CBofButton *pButton, int nState);
	void OnButtonBuyerBids(CBofButton *pButton, int nState);
	void OnButtonDealBackground(CBofButton *pButton, int nState);
	void OnButtonSellerBios(CBofButton *pButton, int nState);
	void OnButtonOtherBios(CBofButton *pButton, int nState);
	void OnButtonStaffBios(CBofButton *pButton, int nState);
	void OnButtonDispatchTeam(CBofButton *pButton, int nState);
	void OnButtonCurrentEMail(CBofButton *pButton, int nState);
	void OnButtonAudioSettings(CBofButton *pButton, int nState);
	void OnButtonRoboButler(CBofButton *pButton, int nState);
	void OnButtonCheckTeams(CBofButton *pButton, int nState);
	void OnButtonCodeWords(CBofButton *pButton, int nState);

	// biography support routines
	void displayTextScreen(CBofString &);

	void RecalcSellerSummaryList();
	void RecalcBuyerSummaryList();
	void RecalcDispatchList(int);

	bool OnButtonSubmitOffer();

	//  support routines to return info about a list
	int GetMeetMember(int);

	/**
	 * Search the specified list and return the index of a true member, if there
	 * is one, return the element in the list that has "meet with" set to true.
	 */
	int GetAdjustedIndex(int, int, bool);

	// Dispatch support routines
	bool VerifyDispatchTeam();
	void NotifyBoss(CBofString &, int);
	int CalculateMeetingTime(int nFlags);
	bool ReportMeetingStatus(int);

	/**
	 * Search the specified list and return the index of a true member, if there
	 * is one, return the element in the list that has "meet with" set to true.
	 */
	void SetMeetingResult(int, int, bool);

	int GetTeamCaptain(int nFlags);
	int GetTeamGender(int nFlags);

	// Robobutler service methods

	void DoOrderBeverage();
	void DoOrderSnack();
	void DoShowChowButtons();

	// To activate the main screen from outside the sraffan computer

	static void SetMainScreen();
	static CBofBitmap *GetComputerBackdrop();

	// User interaction

	void DisplayMessage(const char *szMsg);

	// Finally, all done!
	void IncrementTurnCount();


	/**
	 * This dynamically updates the turncount on the screen,
	 * must know which line to update though.
	 */
	void DisplayTurnCount(int);
	void OnButtonFinished(bool);

	// Save/Restore

	void SaveSraffanVars();
	void RestoreSraffanVars();

protected:
	enum SRAFCOMPMODE {
		SCON, SCOFF, SCDONE
	} m_eMode;
	enum SRAFCURSCREEN {
		SCMAIN, SCDEAL, SCBIDS, SCBACKGROUNDDATA,
		SCSELLER_BIOS, SCOTHER_BIOS, SCSTAFF_BIOS,
		SCDISPATCH, SCEMAIL, SCAUDIO, SCORDER,
		SCCHECK_TEAMS, SCCODE_WORDS
	} m_eCurScreen;
	CBofBmpButton *m_pButtons[NUM_SRAFCOMPBUTT];
	RGBCOLOR m_cTextColor;
	RGBCOLOR m_cTextHiliteColor;
	RGBCOLOR m_cTextLineColor;

	CBofListBox *m_pLBox;               // Main screen list
	CBofList<SrafCompItem> *m_pMainList;
	CBofList<DealSummarySellerItem> *m_pSellerSummaryList;
	CBofList<DealSummaryBuyerItem> *m_pBuyerSummaryList;
	CBofList<DispatchedTeamItem> *m_pTeamList;

	CBofBitmap *m_pStafferBmp[NUM_STAFFERS + 1];    //  (one extra staffer, durteen)

	int m_nSelection;
	int m_nListPointSize;
	int m_nListItemHeight;
	SrafTextScreen *m_pTextOnlyScreen;

	// Code words

	CBofString *m_pszGroup1Word;
	CBofString *m_pszGroup2Word;
	bool m_bSrafAttached;

	static bool m_bSwonzaEnlightened;
	static int m_nStartingTime;

	static SrafComputer *m_pHead;               // Unique pointer to head of list

	static bool m_bRandomAudio;
	static int m_nRandomTime;

	static bool m_bFailureNotified;

private:
	CBofRect gCompDisplay;
	CBofRect gTextWindow;
	CBofRect gStafferDisplay;
	CBofRect gSrafTextWindow;
	CBofString gBidStr;
};

} // namespace SpaceBar
} // namespace Bagel

#endif
