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

#include "bagel/baglib/expression.h"
#include "bagel/baglib/master_win.h"
#include "bagel/baglib/storage_dev_win.h"
#include "bagel/baglib/sound_object.h"
#include "bagel/boflib/sound.h"
#include "bagel/bagel.h"

namespace Bagel {

CBagVar CBagExpression::m_xTempVar("CBagExpr::TempVar", "", FALSE);            // used as a default param


CBagExpression::CBagExpression(CBagExpression *pPrevExpr, BOOL bPrevNeg) {
	m_bNegative = FALSE;
	m_xPrevExpression = pPrevExpr;

	m_bPrevNegative = (UBYTE)bPrevNeg;
}

CBagExpression::~CBagExpression() {
}

BOOL CBagExpression::Evaluate(CBagVar *xLHOper, CBagVar *xRHOper, OPERATION xOper, CBagVar &xResult) {
	BOOL bRClocal = FALSE;

	// If the variable is named "RANDOM", generate a random number for its value
	if (xLHOper->GetName() == "RANDOM")
		xLHOper->SetValue(g_engine->getRandomNumber());
	if (xRHOper->GetName() == "RANDOM")
		xRHOper->SetValue(g_engine->getRandomNumber());

	switch (xOper) {
	case NONE:
		break;

	case ASSIGN:
		bRClocal = OnAssign(xLHOper, xRHOper, xResult);
		break;

	case EQUAL:
		bRClocal = OnEqual(xLHOper, xRHOper, xResult);
		break;

	case NOTEQUAL:
		bRClocal = OnNotEqual(xLHOper, xRHOper, xResult);
		break;

	case LESSTHAN:
		bRClocal = OnLessThan(xLHOper, xRHOper, xResult);
		break;

	case LESSTHANEQUAL:
		bRClocal = OnLessThanEqual(xLHOper, xRHOper, xResult);
		break;

	case GREATERTHAN:
		bRClocal = OnGreaterThan(xLHOper, xRHOper, xResult);
		break;

	case GREATERTHANEQUAL:
		bRClocal = OnGreaterThanEqual(xLHOper, xRHOper, xResult);
		break;

	case PLUSASSIGN:
		bRClocal = OnPlusAssign(xLHOper, xRHOper, xResult);
		break;

	case MINUSASSIGN:
		bRClocal = OnMinusAssign(xLHOper, xRHOper, xResult);
		break;

	case PLUS:
		bRClocal = OnPlus(xLHOper, xRHOper, xResult);
		break;

	case MINUS:
		bRClocal = OnMinus(xLHOper, xRHOper, xResult);
		break;

	case MULTIPLY:
		bRClocal = OnMultiply(xLHOper, xRHOper, xResult);
		break;

	case DIVIDE:
		bRClocal = OnDivide(xLHOper, xRHOper, xResult);
		break;

	case MOD:
		bRClocal = OnMod(xLHOper, xRHOper, xResult);
		break;

	case CONTAINS:
		bRClocal = OnContains(xLHOper, xRHOper, xResult);
		break;

	case HAS:
		bRClocal = OnHas(xLHOper, xRHOper, xResult);
		break;

	case STATUS:
		bRClocal = OnStatus(xLHOper, xRHOper, xResult);
		break;

	default:
		break;
	}

	return bRClocal;
}


CBagVar *CBagExpression::GetVariable(INT nPos) {
	CBagVar *pVar = m_cVarList.GetNodeItem(nPos);

	// Re-wrote because Left(), and Mid() were causing many allocations

	// If the variable is a reference (OBJ.PROPERTY)
	if (pVar->IsReference()) {
		CHAR *p, szFront[256], szBack[256];
		Common::strcpy_s(szFront, pVar->GetName());

		if ((p = strstr(szFront, "~~")) != nullptr) {
			Common::strcpy_s(szBack, p + 2);
			*p = '\0';

			CBofString sObject(szFront, 256);
			CBofString sProperty(szBack, 256);

			INT nVal = SDEVMNGR->GetObjectValue(sObject, sProperty);
			pVar->SetValue(nVal);
		}
	}

	return pVar;
}


CBagExpression::OPERATION CBagExpression::GetOperation(int nPos) {
	Assert(FALSE);
	return m_cOperList.GetNodeItem(nPos);
}


BOOL CBagExpression::Evaluate(BOOL bNeg, CBagVar &xResult) {
	BOOL bRCparent;
	BOOL bRClocal = FALSE;
	BOOL bVal = FALSE;
	CBagVar *xLHOper;
	CBagVar *xRHOper;
	CBagVar *xRHOper2;
	OPERATION xOper;

	INT nVCount, nECount;

	// There must be an expression for every variable after the first
	Assert(m_cVarList.GetCount() - 1 == m_cOperList.GetCount());

	nVCount = 0;
	nECount = 0;

	xLHOper = GetVariable(nVCount++);
	xResult = *xLHOper;

	bRCparent = TRUE;
	if (m_xPrevExpression) {
		bRCparent = m_xPrevExpression->Evaluate(m_bPrevNegative, xResult);
	}

	if (bRCparent) {
		while (nVCount < m_cVarList.GetCount()) {
			xRHOper = GetVariable(nVCount++);
			xOper = m_cOperList.GetNodeItem(nECount++);

			switch (xOper) {
			case AND:
				xRHOper2 = GetVariable(nVCount++);
				xOper = m_cOperList.GetNodeItem(nECount++);
				bVal = Evaluate(xRHOper, xRHOper2, xOper, xResult);

				bRClocal &= bVal;
				break;

			case OR:
				xRHOper2 = GetVariable(nVCount++);
				xOper = m_cOperList.GetNodeItem(nECount++);
				bVal = Evaluate(xRHOper, xRHOper2, xOper, xResult);

				bRClocal |= bVal;
				break;

			default:
				bRClocal = Evaluate(xLHOper, xRHOper, xOper, xResult);
				break;
			}
		}

		if (bNeg)
			// Evaluate before and with parent
			bRClocal = !bRClocal;

		bRClocal &= bRCparent;
	}

	return bRClocal;
}

BOOL CBagExpression::EvalLeftToRight(BOOL bNeg, CBagVar &xResult) {
	BOOL bRCparent;
	BOOL bRClocal = FALSE;
	BOOL bVal = FALSE;
	CBagVar *xLHOper;
	CBagVar *xRHOper;
	CBagVar *xRHOper2;
	CBagVar stLHOper;
	OPERATION xOper = NONE, xPrevOper;
	BOOL bFirstTime = TRUE;

	INT nVCount, nECount;

	// There must be an expression for every variable after the first
	Assert(m_cVarList.GetCount() - 1 == m_cOperList.GetCount());

	nVCount = 0;
	nECount = 0;

	xLHOper = GetVariable(nVCount++);
	xResult = *xLHOper;

	bRCparent = TRUE;
	if (m_xPrevExpression) {
		bRCparent = m_xPrevExpression->Evaluate(m_bPrevNegative, xResult);
	}

	if (bRCparent) {
		while (nVCount < m_cVarList.GetCount()) {
			xRHOper = GetVariable(nVCount++);
			xPrevOper = xOper; 		// save previous operator
			xOper = m_cOperList.GetNodeItem(nECount++);

			if (bFirstTime) {
				stLHOper = *xLHOper;
				bFirstTime = FALSE;
			} else {
				// Based on what we have for a previous operator, either use 
				// the left hand expression or the result of the previous expression. 
				switch (xPrevOper) {
				case MINUS:
				case MULTIPLY:
				case DIVIDE:
				case MOD:
				case PLUS:
					stLHOper = xResult;
					break;
				case NONE:
				case ASSIGN:
				case EQUAL:
				case NOTEQUAL:
				case LESSTHAN:
				case LESSTHANEQUAL:
				case GREATERTHAN:
				case GREATERTHANEQUAL:
				case PLUSASSIGN:
				case MINUSASSIGN:
				case CONTAINS:
				case HAS:
				case STATUS:
				default:
					stLHOper = *xLHOper;
					break;
				}
			}

			switch (xOper) {

			case AND:
				xRHOper2 = GetVariable(nVCount++);
				xOper = m_cOperList.GetNodeItem(nECount++);
				bVal = Evaluate(xRHOper, xRHOper2, xOper, xResult);

				bRClocal &= bVal;
				break;

			case OR:
				xRHOper2 = GetVariable(nVCount++);
				xOper = m_cOperList.GetNodeItem(nECount++);
				bVal = Evaluate(xRHOper, xRHOper2, xOper, xResult);

				// jwl 11.22.96 or this don't not it!!!
				bRClocal |= bVal;
				break;

			default:
				bRClocal = Evaluate(&stLHOper, xRHOper, xOper, xResult);
				break;
			}
		}

		if (bNeg)
			// Evaluate before and with parent
			bRClocal = !bRClocal;

		bRClocal &= bRCparent;
	}

	return bRClocal;
}


BOOL CBagExpression::NegEvaluate(CBagVar &xResult) {
	return Evaluate(FALSE, xResult);
}


BOOL CBagExpression::OnAssign(CBagVar *xLHOper, CBagVar *xRHOper, CBagVar & /*xResultOper*/) {
	//int nIndex;
	Assert((xLHOper != nullptr) && (xRHOper != nullptr));

	CHAR szBuf[256];
	Common::strcpy_s(szBuf, xRHOper->GetValue());
	Assert(strlen(szBuf) < 256);
	CBofString sBuf(szBuf, 256);

	xLHOper->SetValue(sBuf);

	return TRUE;
}


BOOL CBagExpression::OnEqual(CBagVar *xLHOper, CBagVar *xRHOper, CBagVar &xResultOper) {
	Assert((xLHOper != nullptr) && (xRHOper != nullptr));

	BOOL bVal = xLHOper->GetValue() == xRHOper->GetValue();
	xResultOper.SetBoolValue(bVal);

	return bVal;
}


BOOL CBagExpression::OnNotEqual(CBagVar *xLHOper, CBagVar *xRHOper, CBagVar &xResultOper) {
	Assert((xLHOper != nullptr) && (xRHOper != nullptr));
	BOOL bVal = xLHOper->GetValue() != xRHOper->GetValue();
	xResultOper.SetBoolValue(bVal);

	return bVal;
}


BOOL CBagExpression::OnLessThan(CBagVar *xLHOper, CBagVar *xRHOper, CBagVar &xResultOper) {
	Assert((xLHOper != nullptr) && (xRHOper != nullptr));
	BOOL bVal = xLHOper->GetNumValue() < xRHOper->GetNumValue();
	xResultOper.SetBoolValue(bVal);
	return bVal;
}


BOOL CBagExpression::OnGreaterThan(CBagVar *xLHOper, CBagVar *xRHOper, CBagVar &xResultOper) {
	Assert((xLHOper != nullptr) && (xRHOper != nullptr));
	BOOL bVal = xLHOper->GetNumValue() > xRHOper->GetNumValue();
	xResultOper.SetBoolValue(bVal);
	return bVal;
}


BOOL CBagExpression::OnLessThanEqual(CBagVar *xLHOper, CBagVar *xRHOper, CBagVar &xResultOper) {
	Assert((xLHOper != nullptr) && (xRHOper != nullptr));
	BOOL bVal = xLHOper->GetNumValue() <= xRHOper->GetNumValue();
	xResultOper.SetBoolValue(bVal);
	return bVal;
}


BOOL CBagExpression::OnGreaterThanEqual(CBagVar *xLHOper, CBagVar *xRHOper, CBagVar &xResultOper) {
	Assert((xLHOper != nullptr) && (xRHOper != nullptr));

	BOOL bVal = xLHOper->GetNumValue() >= xRHOper->GetNumValue();
	xResultOper.SetBoolValue(bVal);
	return bVal;
}


BOOL CBagExpression::OnPlusAssign(CBagVar *xLHOper, CBagVar *xRHOper, CBagVar &xResultOper) {
	Assert((xLHOper != nullptr) && (xRHOper != nullptr));

	if (xLHOper->IsNumeric() && xRHOper->IsNumeric()) {
		int nLHO = xLHOper->GetNumValue();
		int nRHO = xRHOper->GetNumValue();
		xLHOper->SetValue(nLHO + nRHO);
		xResultOper.SetValue(xLHOper->GetNumValue());
	}

	return TRUE;
}


BOOL CBagExpression::OnMinusAssign(CBagVar *xLHOper, CBagVar *xRHOper, CBagVar &xResultOper) {
	Assert((xLHOper != nullptr) && (xRHOper != nullptr));

	if (xLHOper->IsNumeric() && xRHOper->IsNumeric()) {
		int nLHO = xLHOper->GetNumValue();
		int nRHO = xRHOper->GetNumValue();
		xLHOper->SetValue(nLHO - nRHO);
		xResultOper.SetValue(xLHOper->GetNumValue());
	}

	return TRUE;
}


BOOL CBagExpression::OnContains(CBagVar *xLHOper, CBagVar *xRHOper, CBagVar & /*xResultOper*/) {
	Assert((xLHOper != nullptr) && (xRHOper != nullptr));
	CBagStorageDev *pSDev;
	CBagObject *pObj;

	if ((pSDev = SDEVMNGR->GetStorageDevice(xLHOper->GetValue())) == nullptr)
		return FALSE;

	if ((pObj = pSDev->GetObject(xRHOper->GetValue())) == nullptr)
		return FALSE;

	if (pObj->IsActive())
		return TRUE;

	return FALSE;
}

BOOL CBagExpression::OnHas(CBagVar *xLHOper, CBagVar *xRHOper, CBagVar & /*xResultOper*/) {
	Assert((xLHOper != nullptr) && (xRHOper != nullptr));
	CBagStorageDev *pSDev;
	CBagObject *pObj;

	if ((pSDev = SDEVMNGR->GetStorageDevice(xLHOper->GetValue())) == nullptr)
		return FALSE;

	if ((pObj = pSDev->GetObjectByType(xRHOper->GetValue(), TRUE)) == nullptr)
		return FALSE;

	return TRUE;
}

BOOL CBagExpression::OnStatus(CBagVar *pLHOper, CBagVar * /*pRHOper*/, CBagVar & /*xResultOper*/) {
	Assert(pLHOper != nullptr);

	CBagStorageDev *pSDev;
	CBagObject *pObj;

	if ((pSDev = SDEVMNGR->GetStorageDeviceContaining(pLHOper->GetValue())) == nullptr)
		return FALSE;

	if ((pObj = pSDev->GetObject(pLHOper->GetValue())) == nullptr)
		return FALSE;

	return FALSE;

}
BOOL CBagExpression::OnCurrSDev(CBagVar * /*xLHOper*/, CBagVar * /*xRHOper*/, CBagVar & /*xResultOper*/) {
	return TRUE;
}


BOOL CBagExpression::OnPlus(CBagVar *xLHOper, CBagVar *xRHOper, CBagVar &xResultOper) {
	Assert((xLHOper != nullptr) && (xRHOper != nullptr));

	if (xLHOper->IsNumeric() && xRHOper->IsNumeric()) {
		int nLHO = xLHOper->GetNumValue();
		int nRHO = xRHOper->GetNumValue();
		xResultOper.SetValue(nLHO + nRHO);
	}

	return TRUE;
}


BOOL CBagExpression::OnMinus(CBagVar *xLHOper, CBagVar *xRHOper, CBagVar &xResultOper) {
	Assert((xLHOper != nullptr) && (xRHOper != nullptr));

	if (xLHOper->IsNumeric() && xRHOper->IsNumeric()) {
		int nLHO = xLHOper->GetNumValue();
		int nRHO = xRHOper->GetNumValue();
		xResultOper.SetValue(nLHO - nRHO);
	}

	return TRUE;
}


BOOL CBagExpression::OnMultiply(CBagVar *xLHOper, CBagVar *xRHOper, CBagVar &xResultOper) {
	Assert((xLHOper != nullptr) && (xRHOper != nullptr));

	if (xLHOper->IsNumeric() && xRHOper->IsNumeric()) {
		INT nLHO = xLHOper->GetNumValue();
		INT nRHO = xRHOper->GetNumValue();

		xResultOper.SetValue(nLHO * nRHO);
	}

	return TRUE;
}


BOOL CBagExpression::OnDivide(CBagVar *xLHOper, CBagVar *xRHOper, CBagVar &xResultOper) {
	Assert((xLHOper != nullptr) && (xRHOper != nullptr));

	if (xLHOper->IsNumeric() && xRHOper->IsNumeric()) {
		INT nLHO = xLHOper->GetNumValue();
		INT nRHO = xRHOper->GetNumValue();

		// Divide by Zero error?
		Assert(nRHO != 0);

		xResultOper.SetValue(nLHO / nRHO);
	}

	return TRUE;
}


BOOL CBagExpression::OnMod(CBagVar *xLHOper, CBagVar *xRHOper, CBagVar &xResultOper) {
	Assert((xLHOper != nullptr) && (xRHOper != nullptr));

	if (xLHOper->IsNumeric() && xRHOper->IsNumeric()) {
		INT nLHO = xLHOper->GetNumValue();
		INT nRHO = xRHOper->GetNumValue();

		// Divide by Zero error?
		Assert(nRHO != 0);

		xResultOper.SetValue(nLHO % nRHO);
	}

	return TRUE;
}


BOOL CBagExpression::OnAnd(CBagVar *xLHOper, CBagVar *xRHOper, CBagVar & /*xResultOper*/) {
	Assert((xLHOper != nullptr) && (xRHOper != nullptr));

	return(!xLHOper->GetValue().Find("TRUE") && !xRHOper->GetValue().Find("TRUE"));
}


BOOL CBagExpression::OnOr(CBagVar *xLHOper, CBagVar *xRHOper, CBagVar & /*xResultOper*/) {
	Assert((xLHOper != nullptr) && (xRHOper != nullptr));

	return(!xLHOper->GetValue().Find("TRUE") || !xRHOper->GetValue().Find("TRUE"));
}


PARSE_CODES CBagExpression::SetInfo(bof_ifstream &istr) {
	int ch;
	CHAR szBuf[256];
	szBuf[0] = 0;
	CBofString sStr(szBuf, 256);

	CHAR szErrStr[256];
	Common::strcpy_s(szErrStr, "Error in expression ");
	CBofString errStr(szErrStr, 256);

	PARSE_CODES rc = PARSING_DONE;
	BOOL bDone = FALSE;
	CBagVar *pVar;
	OPERATION xOper;

	while (!bDone && rc == ERR_NONE) {
		istr.EatWhite();
		switch (ch = istr.peek()) {
		case '(': {
			istr.Get();
			istr.EatWhite();

			GetAlphaNumFromStream(istr, sStr);
			pVar = VARMNGR->GetVariable(sStr);
			if (!pVar) {                             // this must be a reference, make a new variable
				if (sStr.Find("~~") > 0) {
					pVar = new CBagVar;
					pVar->SetName(sStr);
					pVar->SetReference();
				} else {                             // this is an error condition, constants can only be rhopers
					//CBofString strName("Constant");
					//strName += sStr;
					pVar = new CBagVar;
					pVar->SetName(sStr);
					pVar->SetValue(sStr);
					pVar->SetConstant();
				}
			}
			m_cVarList.AddToTail(pVar);

			istr.EatWhite();
			while (((ch = istr.peek()) != ')') && rc == ERR_NONE) {
				GetOperatorFromStream(istr, xOper);
				if (xOper == NONE) {
					rc = UNKNOWN_TOKEN;
					errStr = "Bad operator:";
					break;
				}
				m_cOperList.AddToTail(xOper);

				istr.EatWhite();
				GetAlphaNumFromStream(istr, sStr);
				pVar = VARMNGR->GetVariable(sStr);
				if (!pVar) {
					if (sStr.Find("~~") > 0) {             // this must be a reference, make a new variable
						pVar = new CBagVar;
						pVar->SetName(sStr);
						pVar->SetReference();
					} else {                            // this must be a constant, make a new variable
						pVar = new CBagVar;
						pVar->SetName(sStr);
						pVar->SetValue(sStr);
						pVar->SetConstant();
					}
				}
				m_cVarList.AddToTail(pVar);

				istr.EatWhite();
			} // while parsing inner circle

			if (ch == ')') {
				istr.Get();
				bDone = TRUE;
			}
			break;
		}

		case 'N':
			GetAlphaNumFromStream(istr, sStr);
			if (!sStr.Find("NOT")) {
				m_bNegative = (UBYTE)!m_bNegative;
				istr.EatWhite();
				break;
			}
		default:
			rc = UNKNOWN_TOKEN;
			break;
		}
	}

	if (rc != PARSING_DONE) {
		ParseAlertBox(istr, "Error in expression:", __FILE__, __LINE__);
	}

	istr.EatWhite();

	return rc;
}


ERROR_CODE CBagExpression::GetOperatorFromStream(bof_ifstream &istr, OPERATION &xOper) {
	ERROR_CODE rc = ERR_NONE;

	CHAR szLocalBuff[256];
	szLocalBuff[0] = 0;

	CBofString sStr(szLocalBuff, 256);

	xOper = NONE;

	istr.EatWhite();
	GetOperStrFromStream(istr, sStr);

	if (sStr.IsEmpty()) {
		GetAlphaNumFromStream(istr, sStr);
		istr.EatWhite();
	}

	if (!sStr.Find("-=")) {
		xOper = MINUSASSIGN;

	} else if (!sStr.Find("+=")) {
		xOper = PLUSASSIGN;

	} else if (!sStr.Find(">=")) {
		xOper = GREATERTHANEQUAL;

	} else if (!sStr.Find("<=")) {
		xOper = LESSTHANEQUAL;

	} else if (!sStr.Find("!=")) {
		xOper = NOTEQUAL;

	} else if (!sStr.Find("==")) {
		xOper = EQUAL;

	} else if (!sStr.Find(">")) {
		xOper = GREATERTHAN;

	} else if (!sStr.Find("<")) {
		xOper = LESSTHAN;

	} else if (!sStr.Find("=")) {
		xOper = ASSIGN;

	} else if (!sStr.Find("+")) {
		xOper = PLUS;

	} else if (!sStr.Find("-")) {
		xOper = MINUS;

	} else if (!sStr.Find("*")) {
		xOper = MULTIPLY;

	} else if (!sStr.Find("/")) {
		xOper = DIVIDE;

	} else if (!sStr.Find("%")) {
		xOper = MOD;

	} else if (!sStr.Find("CONTAINS")) {
		// Sdev contains object
		xOper = CONTAINS;

	} else if (!sStr.Find("HAS")) {
		// Sdev has type of object
		xOper = HAS;

	} else if (!sStr.Find("OR")) {
		xOper = OR;

	} else if (!sStr.Find("STATUS")) {
		xOper = STATUS;

	} else if (!sStr.Find("AND")) {
		xOper = AND;
	}

	if (xOper == NONE)
		rc = ERR_UNKNOWN;

	return rc;
}

} // namespace Bagel
