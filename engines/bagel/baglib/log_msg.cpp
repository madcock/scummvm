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

#include "bagel/baglib/log_msg.h"
#include "bagel/baglib/button_object.h"
#include "bagel/baglib/menu_dlg.h"
#include "bagel/boflib/sound.h"
#include "bagel/boflib/gui/movie.h"
#include "bagel/baglib/master_win.h"
#include "bagel/baglib/pda.h"
#include "bagel/boflib/gfx/text.h"
#include "bagel/boflib/log.h"

namespace Bagel {

CBagLog *CBagLog::m_bLastFloatPage;

#define LOGBORDER (42)
#define LOGZBORDER (84)

#define OVERRIDESMK             "$SBARDIR\\BAR\\LOG\\OVERRIDE.SMK"
#define OVERRIDEMOVIE           "OVERRIDE_MOVIE"

CBagLog::CBagLog() : CBagStorageDevBmp() {
	m_pQueued_Msgs = new CBofList<CBagObject *>;
	SetCurFltPage(1);
}

CBagLog::~CBagLog() {
	if (m_pQueued_Msgs != nullptr) {
		ReleaseMsg();       // Delete all master sprite objects
		delete m_pQueued_Msgs;
		m_pQueued_Msgs = nullptr;
	}
}

CBofPoint CBagLog::ArrangeFloater(CBofPoint nPos, CBagObject *pObj) {
	CBofPoint   NextPos = nPos;

	// Things are so convoluted now, it is entirely  possible that this method
	// will get called on a storage device that is not the current one,
	// so we will not require a backdrop.

	if (getBackground() != nullptr) {
		CBofString sDevName = GetName();
		int nBorderSize = 0;

		// Get this from script, allows individual log states to set border.
		CBagVar *pVar = VARMNGR->GetVariable("LOGBORDER");
		if (pVar != nullptr) {
			nBorderSize = pVar->GetNumValue();
		}

		if (nBorderSize == 0) {
			if (sDevName == "LOG_WLD") {
				nBorderSize = LOGBORDER;
			} else if (sDevName == "LOGZ_WLD") {
				nBorderSize = LOGZBORDER;
			}
		}

		// The log window has instructional text at the top and bottom, so
		// create a floater rect to stay in the middle area
		CBofRect xFloatRect = getBackground()->getRect();
		xFloatRect.top += nBorderSize;
		xFloatRect.bottom -= (nBorderSize / 2);

		// calculate what page the whole object belongs on
		int nPageNum = ((NextPos.y + pObj->getRect().height()) / xFloatRect.height());
		// page numbering is 1-N
		nPageNum++;
		SetNumFloatPages(nPageNum);

		int nTotalPages = GetCurFltPage();
		// Now position this object int the sdev
		// if it fell on this page, show it
		if (nPageNum == nTotalPages) {
			CBofPoint xPagePos = NextPos;
			// Bring the current page into view
			xPagePos.y = xPagePos.y - ((nPageNum - 1) * xFloatRect.height());
			// Add in the border
			xPagePos.y += nBorderSize;
			pObj->setPosition(xPagePos);
		} else {
			// Set the position to be off the sdev, so it won't show
			pObj->setPosition(CBofPoint(NextPos.x, getBackground()->height() + 1));
		}

		// Calculate the position for the next floater
		// This will get sent back to the calling func
		NextPos.x += pObj->getRect().width();

		// Check to see if the whole object can fit in width, if it can't wrap
		if (NextPos.x > (xFloatRect.width() - pObj->getRect().width())) {
			NextPos.x = 0;
			NextPos.y += pObj->getRect().height();
		}
	}

	m_bLastFloatPage = this;
	return NextPos;
}

void CBagLog::ArrangePages() {
	// Don't bother if we don't have a floater worth arranging...
	if (m_bLastFloatPage == nullptr) {
		return;
	}

	CBagObject *pUpObj = nullptr;
	CBagObject *pDownObj = nullptr;

	CBagLog *pLastFloat = m_bLastFloatPage;

	// Get the up button and the down button...
	pUpObj = pLastFloat->GetObject("LOGPAGUP");
	pDownObj = pLastFloat->GetObject("LOGPAGDOWN");

	if (pUpObj == nullptr || pDownObj == nullptr) {
		return;
	}

	// Get current page number and last page number
	int nLastPage = pLastFloat->GetNumFloatPages();
	int nCurPage = pLastFloat->GetCurFltPage();
	int nFirstPage = 1;

	if (nCurPage > nFirstPage && nCurPage < nLastPage) {
		if (pUpObj->isAttached() == false) {
			pUpObj->setActive();
			pUpObj->attach();
		}
		if (pDownObj->isAttached() == false) {
			pDownObj->setActive();
			pDownObj->attach();
		}
	} else if (nCurPage == nFirstPage && nCurPage == nLastPage) {
		if (pUpObj->isAttached()) {
			pUpObj->setActive(false);
			pUpObj->detach();
		}
		if (pDownObj->isAttached()) {
			pDownObj->setActive(false);
			pDownObj->detach();
		}
	} else if (nCurPage <= nFirstPage) {
		if (pUpObj->isAttached()) {
			pUpObj->setActive(false);
			pUpObj->detach();
		}
		if (pDownObj->isAttached() == false) {
			pDownObj->setActive();
			pDownObj->attach();
		}
	} else if (nCurPage >= nLastPage) {
		if (pUpObj->isAttached() == false) {
			pUpObj->setActive();
			pUpObj->attach();
		}

		if (pDownObj->isAttached()) {
			pDownObj->setActive(false);
			pDownObj->detach();
		}
	}

	// Reinitialize
	m_bLastFloatPage = nullptr;
}

int CBagLog::GetCurFltPage() {
	int nCurFltPage = 0;

	// Read in their total nuggets from game
	CBagVar *pVar = VARMNGR->GetVariable("CUR_BAR_LOG_PAGE");

	if (pVar) {
		nCurFltPage = pVar->GetNumValue();
	}

	return nCurFltPage;
}

void CBagLog::SetCurFltPage(int fltPage) {
	// Read in their total nuggets from game
	CBagVar *pVar = VARMNGR->GetVariable("CUR_BAR_LOG_PAGE");

	if (pVar)
		pVar->SetValue(fltPage);
}

ErrorCode CBagLog::ReleaseMsg() {
	ErrorCode errCode = ERR_NONE;
	int nCount = m_pQueued_Msgs->GetCount();

	if (nCount) {
		for (int i = 0; i < nCount; ++i) {
			CBagObject *pObj = m_pQueued_Msgs->RemoveHead();

			// This is waiting to be played, mark it in memory as such, the fixes
			// get uglier and uglier... since zoomed pda doesn't have a message light,
			// only set this thing as waiting if we are in the regular PDA,
			// otherwise, we get superfluous blinking of the PDA light.
			CBofString  sDevName = GetName();
			if (sDevName == "LOG_WLD") {
				pObj->SetMsgWaiting(true);
			}
		}
	}

	m_pQueued_Msgs->RemoveAll();
	return errCode;
}

CBagObject *CBagLog::OnNewUserObject(const CBofString &initStr) {
	CBagTextObject *LogObj = nullptr;
	CBofRect cSDevRect = getRect();
	CBofString  sDevName = GetName();
	int     nPntSize = 10;

	if (sDevName == "LOG_WLD")
		nPntSize = FONT_8POINT;
	else if (sDevName == "LOGZ_WLD")
		nPntSize = FONT_18POINT;

	if (initStr == "MSG") {
		LogObj = (CBagTextObject *)new CBagLogMsg(cSDevRect.width());
		LogObj->SetInitInfo(initStr);
		LogObj->setPointSize(nPntSize);
		LogObj->setColor(7);
		LogObj->SetFloating();
	} else if (initStr == "SUS") {
		LogObj = (CBagTextObject *)new CBagLogSuspect(cSDevRect.width());
		LogObj->SetInitInfo(initStr);

		// Reduce point size on zoompda suspect list, make it
		// all fit in the zoompda window.
		if (nPntSize == FONT_18POINT) {
			nPntSize -= 2;
		}
		LogObj->setPointSize(nPntSize);
		LogObj->setColor(7);
		LogObj->SetFloating();
	} else if (initStr == "CLU") {
		LogObj = (CBagTextObject *)new CBagLogClue(initStr, cSDevRect.width(), nPntSize);
	} else if (initStr == "RES") {
		LogObj = (CBagTextObject *)new CBagLogResidue(cSDevRect.width());
		LogObj->SetInitInfo(initStr);
		LogObj->setPointSize(nPntSize);
		LogObj->setColor(7);
		LogObj->SetFloating();
	}

	return LogObj;
}

bool CBagLog::RemoveFromMsgQueue(CBagObject *deletedObj) {
	bool bRemoved = false;
	int nCount = m_pQueued_Msgs->GetCount();

	for (int i = 0; i < nCount; i++) {
		CBagObject *pObj = m_pQueued_Msgs->GetNodeItem(i);

		if (pObj == deletedObj) {
			m_pQueued_Msgs->Remove(i);
			bRemoved = true;
			break;
		}
	}

	return bRemoved;
}

ErrorCode CBagLog::ActivateLocalObject(CBagObject *bagObj) {
	ErrorCode errCode = ERR_NONE;

	if (bagObj != nullptr) {

		if (bagObj->IsMsgWaiting() ||
		        (bagObj->GetType() == USEROBJ && (bagObj->GetInitInfo() != nullptr) && (*bagObj->GetInitInfo() == "MSG"))) {
			m_pQueued_Msgs->addToTail(bagObj);

			// Since zoomed pda doesn't  have a message light, only set this thing
			// as waiting if we are in the  regular PDA, otherwise, we get superfluous
			// blinking of the PDA light.
			CBofString  sDevName = GetName();
			if (sDevName == "LOG_WLD") {
				bagObj->SetMsgWaiting(true);
			}

			CBagButtonObject *pMsgLight = nullptr;
			CBagStorageDev *pPda = nullptr;

			pPda = SDEVMNGR->GetStorageDevice("BPDA_WLD");

			if (pPda) {
				pMsgLight = (CBagButtonObject *)pPda->GetObject("MSGLIGHT");

				if (pMsgLight) {
					if (!pMsgLight->isAttached()) {
						pMsgLight->setActive();
						pMsgLight->attach();
					}

					// Make sure this guy always gets updated regardless of its
					// dirty bit.
					pMsgLight->SetAlwaysUpdate(true);
					pMsgLight->setAnimated(true);
				}
			}
		} else {
			errCode = CBagStorageDev::ActivateLocalObject(bagObj);
		}
	}

	return errCode;
}

ErrorCode CBagLog::PlayMsgQue() {
	ErrorCode errCode = ERR_NONE;
	int nCount = m_pQueued_Msgs->GetCount();

	// Walk through the message queue and play all the messages
	// Only play one message per click on the pda message light.
	if (nCount) {

		CBagStorageDev *pPda = nullptr;
		pPda = SDEVMNGR->GetStorageDevice("BPDA_WLD");

		// If we're in a closeup, then don't play the message!
		CBagStorageDev *pSDev = CBagel::getBagApp()->getMasterWnd()->GetCurrentStorageDev();
		bool bPlayMsg = true;

		if ((pSDev != nullptr) && pSDev->IsCIC()) {
			bPlayMsg = false;

			char szLocalBuff[256];
			CBofString mStr(szLocalBuff, 256);

			mStr = OVERRIDESMK;
			MACROREPLACE(mStr);

			CBagMovieObject *pMovie = (CBagMovieObject *)GetObject(OVERRIDEMOVIE);
			if (pMovie) {
				if (pMovie->isAttached() == false) {
					pMovie->attach();
					pMovie->SetVisible();
				}
				pMovie->runObject();
			}
		}

		// If we're playing a valid message (not the override message) then make sure
		// we remove it from the queue.
		if (bPlayMsg) {
			CBagObject *pObj = m_pQueued_Msgs->RemoveHead();

			if (pObj) {
				CRect  r = getRect();

				errCode = CBagStorageDev::ActivateLocalObject(pObj);
				CBagMenu *pObjMenu = pObj->GetMenuPtr();
				if (pObjMenu)
					pObjMenu->TrackPopupMenu(0, 0, 0, CBofApp::GetApp()->GetMainWindow(), nullptr, &r);
				pObj->runObject();
				pObj->SetMsgWaiting(false);

				// Mark this guy as played...
				((CBagLogMsg *)pObj)->SetMsgPlayed(true);
			}

			// Although this might seem like a superfluous thing to do, but wait!
			// it is not!  the runobject call above can cause the number of objects in the
			// message queue to be decremented.
			nCount = m_pQueued_Msgs->GetCount();

			// Don't stop message light from blinking unless we're down to zero
			// messages in the queue.

			if (pPda) {
				CBagButtonObject *pMsgLight = (CBagButtonObject *)pPda->GetObject("MSGLIGHT");

				if (pMsgLight) {
					if (nCount) {
						pMsgLight->setAnimated(true);
					} else {
						pMsgLight->setAnimated(false);
					}
				}
			}
		}
	}

	return errCode;
}

CBagLogResidue::CBagLogResidue(int sdevWidth) : CBagTextObject() {
	m_xObjType = USEROBJ;
	m_nSdevWidth = sdevWidth;
	m_bTitle = true;
}

void CBagLogResidue::setSize(const CBofSize &size) {
	CBagTextObject::setSize(CBofSize(m_nSdevWidth, size.cy));
}

CBagLogMsg::CBagLogMsg(int sdevWidth) : CBagTextObject() {
	m_xObjType = USEROBJ;
	m_nSdevWidth = sdevWidth;
	m_bTitle = true;

	// Start all messages off as not played
	SetMsgPlayed(false);
}

void CBagLogMsg::setSize(const CBofSize &size) {
	CBagTextObject::setSize(CBofSize(m_nSdevWidth, size.cy));
}

PARSE_CODES CBagLogMsg::setInfo(CBagIfstream &istr) {
	bool nObjectUpdated = false;
	char szLocalBuff[256];
	CBofString sStr(szLocalBuff, 256);

	char szLocalBuff2[256];
	CBofString s(szLocalBuff2, 256);

	while (!istr.eof()) {
		istr.eatWhite();
		
		char ch = (char)istr.peek();
		switch (ch) {
		//
		//  SENDEE FRANK - Sets the sendee name of the message to FRANK
		//
		case 'S': {
			GetAlphaNumFromStream(istr, sStr);

			if (!sStr.Find("SENDEE")) {
				istr.eatWhite();
				GetAlphaNumFromStream(istr, s);

				// Replace any underscores with spaces
				s.ReplaceChar('_', ' ');

				SetMsgSendee(s);

				nObjectUpdated = true;
			} else {
				PutbackStringOnStream(istr, sStr);
			}
			break;
		}

		//
		//   TIME x- Sets the time of the message to xx:xx
		//
		case 'T': {
			GetAlphaNumFromStream(istr, sStr);

			if (!sStr.Find("TIME")) {
				istr.eatWhite();
				char cNext = (char)istr.peek();
				int     nMsgTime = 0;
				if (Common::isDigit(cNext)) {
					GetIntFromStream(istr, nMsgTime);
				} else {
					GetAlphaNumFromStream(istr, m_sMsgTimeStr);
				}

				SetMsgTime(nMsgTime);
				nObjectUpdated = true;
			} else {
				PutbackStringOnStream(istr, sStr);
			}
			break;
		}

		default: {
			PARSE_CODES rc = CBagObject::setInfo(istr);
			if (rc == PARSING_DONE) {
				return PARSING_DONE;
			}

			if (rc == UPDATED_OBJECT) {
				nObjectUpdated = true;
			} else { // rc==UNKNOWN_TOKEN
				if (nObjectUpdated)
					return UPDATED_OBJECT;

				return UNKNOWN_TOKEN;
			}
			break;
		}
		}
	}

	return PARSING_DONE;
}

void CBagLogMsg::setProperty(const CBofString &prop, int val) {
	if (!prop.Find("TIME")) {
		SetMsgTime(val);
	} else if (!prop.Find("PLAYED")) {
		SetMsgPlayed(val);
	}

	CBagObject::setProperty(prop, val);
}

int CBagLogMsg::getProperty(const CBofString &prop) {
	if (!prop.Find("TIME"))
		return GetMsgTime();

	// Played requires a 1 or a 0 (don't use true or false).
	if (!prop.Find("PLAYED")) {
		bool bPlayed = GetMsgPlayed();
		return (bPlayed ? 1 : 0);
	}

	return CBagObject::getProperty(prop);
}

ErrorCode CBagLogMsg::update(CBofBitmap *bmp, CBofPoint pt, CBofRect *srcRect, int maskColor) {
	// We could use a variable here, translate it's value if that's the case.
	if (GetMsgTime() == 0) {
		CBagVar *pVar = VARMNGR->GetVariable(m_sMsgTimeStr);
		int nMsgTime = pVar->GetNumValue();
		SetMsgTime(nMsgTime);
	}

	int nMsgTime = GetMsgTime();
	int nHr = nMsgTime / 100;
	int nMn = nMsgTime - (nHr * 100);

	setFont(FONT_MONO);
	setText(BuildString("%-30s%02d:%02d", m_sMsgSendee.GetBuffer(), nHr, nMn));

	return CBagTextObject::update(bmp, pt, srcRect, maskColor);
}

ErrorCode CBagLogResidue::update(CBofBitmap *bmp, CBofPoint pt, CBofRect *srcRect, int maskColor) {
	return CBagTextObject::update(bmp, pt, srcRect, maskColor);
}

CBagLogSuspect::CBagLogSuspect(int sdevWidth) : CBagTextObject() {
	m_xObjType = USEROBJ;
	m_nSdevWidth = sdevWidth;

	m_bTitle = true;

	// Need to save state info, set all to false.
	setState(0);
}

PARSE_CODES CBagLogSuspect::setInfo(CBagIfstream &istr) {
	bool nObjectUpdated = false;
	char szLocalBuff[256];
	CBofString sStr(szLocalBuff, 256);

	char szLocalBuff2[256];
	CBofString s(szLocalBuff2, 256);

	while (!istr.eof()) {
		istr.eatWhite();

		char ch = (char)istr.peek();
		switch (ch) {
		//
		//  NAME FRANK - Sets the sendee name of the message to FRANK
		//
		case 'N': {
			GetAlphaNumFromStream(istr, sStr);

			if (!sStr.Find("NAME")) {
				istr.eatWhite();
				GetAlphaNumFromStream(istr, s);

				// Replace any underscores with spaces
				s.ReplaceChar('_', ' ');
				SetSusName(s);

				nObjectUpdated = true;
			} else {
				PutbackStringOnStream(istr, sStr);
			}
			break;
		}

		case 'S': {
			GetAlphaNumFromStream(istr, sStr);

			if (!sStr.Find("SPECIES")) {
				istr.eatWhite();
				GetAlphaNumFromStream(istr, s);

				// Replace any underscores with spaces
				s.ReplaceChar('_', ' ');

				SetSusSpecies(s);

				nObjectUpdated = true;
			} else {
				PutbackStringOnStream(istr, sStr);
			}
			break;
		}

		case 'R': {
			GetAlphaNumFromStream(istr, sStr);

			if (!sStr.Find("ROOM")) {
				istr.eatWhite();
				GetAlphaNumFromStream(istr, s);

				// Replace any underscores with spaces
				s.ReplaceChar('_', ' ');
				SetSusRoom(s);

				nObjectUpdated = true;
			} else {
				PutbackStringOnStream(istr, sStr);
			}
			break;
		}

		default: {
			PARSE_CODES rc = CBagObject::setInfo(istr);
			if (rc == PARSING_DONE) {
				return PARSING_DONE;
			}

			if (rc == UPDATED_OBJECT) {
				nObjectUpdated = true;
			} else { // rc==UNKNOWN_TOKEN
				if (nObjectUpdated)
					return UPDATED_OBJECT;

				return UNKNOWN_TOKEN;
			}
			break;
		}
		}
	}

	return PARSING_DONE;
}
void CBagLogSuspect::setProperty(const CBofString &prop, int val) {
	if (!prop.Find("ROOM")) {
		switch (val) {
		case 1:  // BAP
			SetSusRoom("Entry Vestibule");
			break;
		case 2:  // BBP
			SetSusRoom("Howdy Saloon");
			break;
		case 4:  // BDP
			SetSusRoom("Bar Area");
			break;
		case 5:  // BEP
			SetSusRoom("Dance Floor");
			break;
		case 6:  // BFP
			SetSusRoom("Dining Area");
			break;
		case 7:  // BGP
			SetSusRoom("Gambling Hall");
			break;
		case 10: // BJP
			SetSusRoom("Kitchen");
			break;
		case 23:  // BWP
			SetSusRoom("Outside Howdy Saloon");
			break;
		default:
			break;
		}
	} else {
		// Hack alert!  If our value is 2, then this means toggle the boolean!!!
		int bVal = false;
		if (val == 1)
			bVal = true;
		if (val == 0)
			bVal = false;
		if (val == 2)
			bVal = 2;

		if (!prop.Find("CHECKED"))
			SetSusChecked(bVal == 2 ? !GetSusChecked() : bVal);
		else if (!prop.Find("VP"))
			SetSusVP(bVal == 2 ? !GetSusVP() : bVal);
		else if (!prop.Find("RP"))
			SetSusRP(bVal == 2 ? !GetSusRP() : bVal);
		else
			CBagObject::setProperty(prop, val);
	}
}


int CBagLogSuspect::getProperty(const CBofString &prop) {
	if (!prop.Find("CHECKED"))
		return GetSusChecked();

	if (!prop.Find("VP"))
		return GetSusVP();

	if (!prop.Find("RP"))
		return GetSusRP();

	return CBagObject::getProperty(prop);
}


void CBagLogSuspect::setSize(const CBofSize &size) {
	CBagTextObject::setSize(CBofSize(m_nSdevWidth, size.cy));

}


ErrorCode CBagLogSuspect::update(CBofBitmap *bmp, CBofPoint pt, CBofRect *srcRect, int maskColor) {
	char szSusChecked[256];
	CBofString sSusChecked(szSusChecked, 256);

	// Remove all the references to the jamming and voice printer state
	char szSusVP[256];
	char szSusRP[256];

	CBofString sSusVP(szSusVP, 256);
	CBofString sSusRP(szSusRP, 256);

	if (GetSusChecked())
		sSusChecked = "Y";
	else
		sSusChecked = "N";

	if (GetSusVP())
		sSusVP = "OK";
	else
		sSusVP = "?";

	if (GetSusRP())
		sSusRP = "Y";
	else
		sSusRP = "N";

	setFont(FONT_MONO);

	setText(BuildString(" %-5.5s %-17.17s %-12.12s %-20.20s %-4.4s %-4.4s",
	                    sSusChecked.GetBuffer(),
	                    m_sSusName.GetBuffer(),
	                    m_sSusSpecies.GetBuffer(),
	                    m_sSusRoom.GetBuffer(),
	                    sSusVP.GetBuffer(),
	                    sSusRP.GetBuffer()));

	return CBagTextObject::update(bmp, pt, srcRect, maskColor);
}

// Energy detector objects, this should be pretty straightforward.

CBagEnergyDetectorObject::CBagEnergyDetectorObject() {
	setFont(FONT_MONO);				// Correct for spacing
	setColor(7);					// Make it white
	SetFloating();					// Is definitely floating
	setHighlight();					// Is highlight
	SetTitle();						// As title
	m_bTextInitialized = false;     // Not initialized yet
}

CBagEnergyDetectorObject::~CBagEnergyDetectorObject() {
}

PARSE_CODES CBagEnergyDetectorObject::setInfo(CBagIfstream &istr) {
	bool nObjectUpdated = false;
	char szLocalBuff[256];
	CBofString sStr(szLocalBuff, 256);

	char szLocalBuff2[256];
	CBofString s(szLocalBuff2, 256);

	while (!istr.eof()) {
		istr.eatWhite();
		
		char ch = (char)istr.peek();
		switch (ch) {
		//
		//  ZHAPS - NUMBER OF ZHAPS (ENERGY UNITS)
		//
		case 'Z': {
			GetAlphaNumFromStream(istr, sStr);

			if (!sStr.Find("ZHAPS")) {
				istr.eatWhite();
				GetAlphaNumFromStream(istr, s);

				m_sZhapsStr = s;

				nObjectUpdated = true;
			} else {
				PutbackStringOnStream(istr, sStr);
			}
			break;
		}

		//
		//  CAUSE - REASON FOR ENERGY BURST
		//
		case 'C': {
			GetAlphaNumFromStream(istr, sStr);

			if (!sStr.Find("CAUSE")) {
				istr.eatWhite();
				GetAlphaNumFromStream(istr, s);

				m_sCauseStr = s;

				nObjectUpdated = true;
			} else {
				PutbackStringOnStream(istr, sStr);
			}
			break;
		}

		//
		//   TIME x- Sets the time of the message to xx:xx
		//
		case 'T': {
			GetAlphaNumFromStream(istr, sStr);

			if (!sStr.Find("TIME")) {
				istr.eatWhite();
				(void)istr.peek();

				GetAlphaNumFromStream(istr, m_sEnergyTimeStr);
				nObjectUpdated = true;
			} else {
				PutbackStringOnStream(istr, sStr);
			}
			break;
		}

		//
		//  SIZE n - n point size of the txt
		//
		case 'S': {
			char szLocalStr[256];
			szLocalStr[0] = 0;
			CBofString sStr2(szLocalStr, 256);

			GetAlphaNumFromStream(istr, sStr2);

			if (!sStr2.Find("SIZE")) {
				istr.eatWhite();
				int n;
				GetIntFromStream(istr, n);
				setPointSize(n);
				nObjectUpdated = true;
			} else {
				PutbackStringOnStream(istr, sStr2);
			}
			break;
		}

		default: {
			PARSE_CODES rc = CBagObject::setInfo(istr);
			if (rc == PARSING_DONE) {
				return PARSING_DONE;
			}

			if (rc == UPDATED_OBJECT) {
				nObjectUpdated = true;
			} else { // rc==UNKNOWN_TOKEN
				if (nObjectUpdated)
					return UPDATED_OBJECT;

				return UNKNOWN_TOKEN;
			}
			break;
		}
		}
	}

	return PARSING_DONE;
}

ErrorCode CBagEnergyDetectorObject::update(CBofBitmap *pBmp, CBofPoint pt, CBofRect *pSrcRect, int nMaskColor) {
	// Don't draw until we're attached
	if (isAttached() == false) {
		return ERR_NONE;
	}

	return CBagTextObject::update(pBmp, pt, pSrcRect, nMaskColor);
}

ErrorCode CBagEnergyDetectorObject::attach() {
	Assert(IsValidObject(this));

	int nMsgTime;
	char szLocalBuff[256];
	CBofString causeStr(szLocalBuff, 256);

	char szZhapsBuff[256];
	CBofString zStr(szZhapsBuff, 256);

	// We could use a variable here, translate it's value if that's the case.
	CBagVar *pVar = VARMNGR->GetVariable(m_sEnergyTimeStr);
	if (pVar) {
		nMsgTime = pVar->GetNumValue();
	} else {
		nMsgTime = atoi(m_sEnergyTimeStr.GetBuffer());
	}

	int nHr = nMsgTime / 100;
	int nMn = nMsgTime - (nHr * 100);

	// Get the number of zhaps.
	pVar = VARMNGR->GetVariable(m_sZhapsStr);
	if (pVar) {
		zStr = pVar->GetValue();
	} else {
		zStr = m_sZhapsStr;
	}

	// Get the cause
	pVar = VARMNGR->GetVariable(m_sCauseStr);
	if (pVar) {
		causeStr = pVar->GetValue();
	} else {
		causeStr = m_sCauseStr;
	}

	// Replace any underscores with spaces
	causeStr.ReplaceChar('_', ' ');

	CBofString cStr;

	SetPSText(&cStr);

	setText(BuildString("%02d:%02d %6.6s %s  %-35.35s", nHr, nMn, zStr.GetBuffer(), "zhaps", causeStr.GetBuffer()));
	RecalcTextRect(false);

	return CBagObject::attach();
}

CBagLogClue::CBagLogClue(const CBofString &sInit, int nSdevWidth, int nPointSize) : CBagTextObject() {
	m_xObjType = USEROBJ;
	m_nSdevWidth = nSdevWidth;

	m_bTitle = true;

	m_pStringVar1 = nullptr;
	m_pStringVar2 = nullptr;
	m_pStringVar3 = nullptr;
	m_pStringVar4 = nullptr;

	setFont(FONT_MONO);
	SetInitInfo(sInit);
	setPointSize(nPointSize);
	setColor(7);
	SetFloating();
}

ErrorCode CBagLogClue::attach() {
	char szFormatStr[256];
	char szClueStr[256];
	CBofString cFormat(szFormatStr, 256);

	Assert(IsValidObject(this));

	ErrorCode ec = CBagTextObject::attach();

	// Get what is defined in the script.
	cFormat = getFileName();

	// Replace '$' with '%' (% is an illegal character embedded in a clue string).
	cFormat.ReplaceChar('_', ' ');
	cFormat.ReplaceChar('$', '%');

	// Format the text appropriately.
	Common::sprintf_s(szClueStr, cFormat.GetBuffer(),
	                  (m_pStringVar1 ? (const char *)m_pStringVar1->GetValue() : (const char *)""),
	                  (m_pStringVar2 ? (const char *)m_pStringVar2->GetValue() : (const char *)""),
	                  (m_pStringVar3 ? (const char *)m_pStringVar3->GetValue() : (const char *)""),
	                  (m_pStringVar4 ? (const char *)m_pStringVar4->GetValue() : (const char *)""));

	CBofString cStr(szClueStr);
	SetPSText(&cStr);

	return ec;
}

PARSE_CODES CBagLogClue::setInfo(CBagIfstream &istr) {
	bool nObjectUpdated = false;

	char szLocalBuff[256];
	CBofString sStr(szLocalBuff, 256);

	while (!istr.eof()) {
		istr.eatWhite();
		char ch = (char)istr.peek();
		switch (ch) {
		//
		//  STRINGVAR - This will be a variable used to display some information that
		//  is contained in script in a clue statement.
		//
		case 'S': {
			GetAlphaNumFromStream(istr, sStr);

			if (!sStr.Find("STRINGVAR")) {
				istr.eatWhite();
				GetAlphaNumFromStream(istr, sStr);
				CBagVar *pVar = VARMNGR->GetVariable(sStr);
				// The variable must have been found, if it wasn't, then
				// complain violently.

				if (pVar == nullptr) {
					return UNKNOWN_TOKEN;
				}

				if (m_pStringVar1 == nullptr) {
					m_pStringVar1 = pVar;
				} else if (m_pStringVar2 == nullptr) {
					m_pStringVar2 = pVar;
				} else if (m_pStringVar3 == nullptr) {
					m_pStringVar3 = pVar;
				} else if (m_pStringVar4 == nullptr) {
					m_pStringVar4 = pVar;
				} else {
					return UNKNOWN_TOKEN;
				}
			} else {
				PutbackStringOnStream(istr, sStr);
			}
			break;
		}

		default: {
			PARSE_CODES rc = CBagObject::setInfo(istr);
			if (rc == PARSING_DONE) {
				return PARSING_DONE;
			}

			if (rc == UPDATED_OBJECT) {
				nObjectUpdated = true;
			} else { // rc==UNKNOWN_TOKEN
				if (nObjectUpdated)
					return UPDATED_OBJECT;

				return UNKNOWN_TOKEN;
			}
			break;
		}
		}

	}

	return PARSING_DONE;
}

ErrorCode CBagLogClue::update(CBofBitmap *pBmp, CBofPoint pt, CBofRect *pSrcRect, int nMaskColor) {
	return CBagTextObject::update(pBmp, pt, pSrcRect, nMaskColor);
}

} // namespace Bagel
