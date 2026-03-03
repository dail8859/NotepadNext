/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Universal charset detector code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 2001
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *          Shy Shalom <shooshX@gmail.com>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include <stdio.h>
#include "prmem.h"

#include "nsSBCharSetProber.h"
#include "nsSBCSGroupProber.h"

#include "nsHebrewProber.h"

nsSBCSGroupProber::nsSBCSGroupProber()
{
  mProbers[0] = new nsSingleByteCharSetProber(&Win1251RussianModel);
  mProbers[1] = new nsSingleByteCharSetProber(&Koi8rRussianModel);
  mProbers[2] = new nsSingleByteCharSetProber(&Latin5RussianModel);
  mProbers[3] = new nsSingleByteCharSetProber(&MacCyrillicRussianModel);
  mProbers[4] = new nsSingleByteCharSetProber(&Ibm866RussianModel);
  mProbers[5] = new nsSingleByteCharSetProber(&Ibm855RussianModel);

  mProbers[6] = new nsSingleByteCharSetProber(&Iso_8859_7GreekModel);
  mProbers[7] = new nsSingleByteCharSetProber(&Windows_1253GreekModel);

  mProbers[8] = new nsSingleByteCharSetProber(&Latin5BulgarianModel);
  mProbers[9] = new nsSingleByteCharSetProber(&Win1251BulgarianModel);

  nsHebrewProber *hebprober = new nsHebrewProber();
  // Notice: Any change in these indexes - 10,11,12 must be reflected
  // in the code below as well.
  mProbers[10] = hebprober;
  mProbers[11] = new nsSingleByteCharSetProber(&Win1255Model, PR_FALSE, hebprober); // Logical Hebrew
  mProbers[12] = new nsSingleByteCharSetProber(&Win1255Model, PR_TRUE, hebprober); // Visual Hebrew
  // Tell the Hebrew prober about the logical and visual probers
  if (mProbers[10] && mProbers[11] && mProbers[12]) // all are not null
  {
    hebprober->SetModelProbers(mProbers[11], mProbers[12]);
  }
  else // One or more is null. avoid any Hebrew probing, null them all
  {
    for (PRUint32 i = 10; i <= 12; ++i)
    {
      delete mProbers[i];
      mProbers[i] = 0;
    }
  }

  mProbers[13] = new nsSingleByteCharSetProber(&Tis_620ThaiModel);
  mProbers[14] = new nsSingleByteCharSetProber(&Iso_8859_11ThaiModel);

  mProbers[15] = new nsSingleByteCharSetProber(&Iso_8859_1FrenchModel);
  mProbers[16] = new nsSingleByteCharSetProber(&Iso_8859_15FrenchModel);
  mProbers[17] = new nsSingleByteCharSetProber(&Windows_1252FrenchModel);

  mProbers[18] = new nsSingleByteCharSetProber(&Iso_8859_1SpanishModel);
  mProbers[19] = new nsSingleByteCharSetProber(&Iso_8859_15SpanishModel);
  mProbers[20] = new nsSingleByteCharSetProber(&Windows_1252SpanishModel);

  mProbers[21] = new nsSingleByteCharSetProber(&Iso_8859_2HungarianModel);
  mProbers[22] = new nsSingleByteCharSetProber(&Windows_1250HungarianModel);

  mProbers[23] = new nsSingleByteCharSetProber(&Iso_8859_1GermanModel);
  mProbers[24] = new nsSingleByteCharSetProber(&Windows_1252GermanModel);

  mProbers[25] = new nsSingleByteCharSetProber(&Iso_8859_3EsperantoModel);

  mProbers[26] = new nsSingleByteCharSetProber(&Iso_8859_3TurkishModel);
  mProbers[27] = new nsSingleByteCharSetProber(&Iso_8859_9TurkishModel);

  mProbers[28] = new nsSingleByteCharSetProber(&Iso_8859_6ArabicModel);
  mProbers[29] = new nsSingleByteCharSetProber(&Windows_1256ArabicModel);

  mProbers[30] = new nsSingleByteCharSetProber(&VisciiVietnameseModel);
  mProbers[31] = new nsSingleByteCharSetProber(&Windows_1258VietnameseModel);

  mProbers[32] = new nsSingleByteCharSetProber(&Iso_8859_15DanishModel);
  mProbers[33] = new nsSingleByteCharSetProber(&Iso_8859_1DanishModel);
  mProbers[34] = new nsSingleByteCharSetProber(&Windows_1252DanishModel);
  mProbers[35] = new nsSingleByteCharSetProber(&Ibm865DanishModel);

  mProbers[36] = new nsSingleByteCharSetProber(&Iso_8859_13LithuanianModel);
  mProbers[37] = new nsSingleByteCharSetProber(&Iso_8859_10LithuanianModel);
  mProbers[38] = new nsSingleByteCharSetProber(&Iso_8859_4LithuanianModel);

  mProbers[39] = new nsSingleByteCharSetProber(&Iso_8859_13LatvianModel);
  mProbers[40] = new nsSingleByteCharSetProber(&Iso_8859_10LatvianModel);
  mProbers[41] = new nsSingleByteCharSetProber(&Iso_8859_4LatvianModel);

  mProbers[42] = new nsSingleByteCharSetProber(&Iso_8859_1PortugueseModel);
  mProbers[43] = new nsSingleByteCharSetProber(&Iso_8859_9PortugueseModel);
  mProbers[44] = new nsSingleByteCharSetProber(&Iso_8859_15PortugueseModel);
  mProbers[45] = new nsSingleByteCharSetProber(&Windows_1252PortugueseModel);

  mProbers[46] = new nsSingleByteCharSetProber(&Iso_8859_3MalteseModel);

  mProbers[47] = new nsSingleByteCharSetProber(&Windows_1250CzechModel);
  mProbers[48] = new nsSingleByteCharSetProber(&Iso_8859_2CzechModel);
  mProbers[49] = new nsSingleByteCharSetProber(&Mac_CentraleuropeCzechModel);
  mProbers[50] = new nsSingleByteCharSetProber(&Ibm852CzechModel);

  mProbers[51] = new nsSingleByteCharSetProber(&Windows_1250SlovakModel);
  mProbers[52] = new nsSingleByteCharSetProber(&Iso_8859_2SlovakModel);
  mProbers[53] = new nsSingleByteCharSetProber(&Mac_CentraleuropeSlovakModel);
  mProbers[54] = new nsSingleByteCharSetProber(&Ibm852SlovakModel);

  mProbers[55] = new nsSingleByteCharSetProber(&Windows_1250PolishModel);
  mProbers[56] = new nsSingleByteCharSetProber(&Iso_8859_2PolishModel);
  mProbers[57] = new nsSingleByteCharSetProber(&Iso_8859_13PolishModel);
  mProbers[58] = new nsSingleByteCharSetProber(&Iso_8859_16PolishModel);
  mProbers[59] = new nsSingleByteCharSetProber(&Mac_CentraleuropePolishModel);
  mProbers[60] = new nsSingleByteCharSetProber(&Ibm852PolishModel);

  mProbers[61] = new nsSingleByteCharSetProber(&Iso_8859_1FinnishModel);
  mProbers[62] = new nsSingleByteCharSetProber(&Iso_8859_4FinnishModel);
  mProbers[63] = new nsSingleByteCharSetProber(&Iso_8859_9FinnishModel);
  mProbers[64] = new nsSingleByteCharSetProber(&Iso_8859_13FinnishModel);
  mProbers[65] = new nsSingleByteCharSetProber(&Iso_8859_15FinnishModel);
  mProbers[66] = new nsSingleByteCharSetProber(&Windows_1252FinnishModel);

  mProbers[67] = new nsSingleByteCharSetProber(&Iso_8859_1ItalianModel);
  mProbers[68] = new nsSingleByteCharSetProber(&Iso_8859_3ItalianModel);
  mProbers[69] = new nsSingleByteCharSetProber(&Iso_8859_9ItalianModel);
  mProbers[70] = new nsSingleByteCharSetProber(&Iso_8859_15ItalianModel);
  mProbers[71] = new nsSingleByteCharSetProber(&Windows_1252ItalianModel);

  mProbers[72] = new nsSingleByteCharSetProber(&Windows_1250CroatianModel);
  mProbers[73] = new nsSingleByteCharSetProber(&Iso_8859_2CroatianModel);
  mProbers[74] = new nsSingleByteCharSetProber(&Iso_8859_13CroatianModel);
  mProbers[75] = new nsSingleByteCharSetProber(&Iso_8859_16CroatianModel);
  mProbers[76] = new nsSingleByteCharSetProber(&Mac_CentraleuropeCroatianModel);
  mProbers[77] = new nsSingleByteCharSetProber(&Ibm852CroatianModel);

  mProbers[78] = new nsSingleByteCharSetProber(&Windows_1252EstonianModel);
  mProbers[79] = new nsSingleByteCharSetProber(&Windows_1257EstonianModel);
  mProbers[80] = new nsSingleByteCharSetProber(&Iso_8859_4EstonianModel);
  mProbers[81] = new nsSingleByteCharSetProber(&Iso_8859_13EstonianModel);
  mProbers[82] = new nsSingleByteCharSetProber(&Iso_8859_15EstonianModel);

  mProbers[83] = new nsSingleByteCharSetProber(&Iso_8859_1IrishModel);
  mProbers[84] = new nsSingleByteCharSetProber(&Iso_8859_9IrishModel);
  mProbers[85] = new nsSingleByteCharSetProber(&Iso_8859_15IrishModel);
  mProbers[86] = new nsSingleByteCharSetProber(&Windows_1252IrishModel);

  mProbers[87] = new nsSingleByteCharSetProber(&Windows_1250RomanianModel);
  mProbers[88] = new nsSingleByteCharSetProber(&Iso_8859_2RomanianModel);
  mProbers[89] = new nsSingleByteCharSetProber(&Iso_8859_16RomanianModel);
  mProbers[90] = new nsSingleByteCharSetProber(&Ibm852RomanianModel);

  mProbers[91] = new nsSingleByteCharSetProber(&Windows_1250SloveneModel);
  mProbers[92] = new nsSingleByteCharSetProber(&Iso_8859_2SloveneModel);
  mProbers[93] = new nsSingleByteCharSetProber(&Iso_8859_16SloveneModel);
  mProbers[94] = new nsSingleByteCharSetProber(&Mac_CentraleuropeSloveneModel);
  mProbers[95] = new nsSingleByteCharSetProber(&Ibm852SloveneModel);

  mProbers[96] = new nsSingleByteCharSetProber(&Iso_8859_1SwedishModel);
  mProbers[97] = new nsSingleByteCharSetProber(&Iso_8859_4SwedishModel);
  mProbers[98] = new nsSingleByteCharSetProber(&Iso_8859_9SwedishModel);
  mProbers[99] = new nsSingleByteCharSetProber(&Iso_8859_15SwedishModel);
  mProbers[100] = new nsSingleByteCharSetProber(&Windows_1252SwedishModel);

  mProbers[101] = new nsSingleByteCharSetProber(&Iso_8859_15NorwegianModel);
  mProbers[102] = new nsSingleByteCharSetProber(&Iso_8859_1NorwegianModel);
  mProbers[103] = new nsSingleByteCharSetProber(&Windows_1252NorwegianModel);
  mProbers[104] = new nsSingleByteCharSetProber(&Ibm865NorwegianModel);

  Reset();
}

nsSBCSGroupProber::~nsSBCSGroupProber()
{
  for (PRUint32 i = 0; i < NUM_OF_SBCS_PROBERS; i++)
  {
    delete mProbers[i];
  }
}


const char* nsSBCSGroupProber::GetCharSetName()
{
  //if we have no answer yet
  if (mBestGuess == -1)
  {
    GetConfidence();
    //no charset seems positive
    if (mBestGuess == -1)
      //we will use default.
      mBestGuess = 0;
  }
  return mProbers[mBestGuess]->GetCharSetName();
}

void  nsSBCSGroupProber::Reset(void)
{
  mActiveNum = 0;
  for (PRUint32 i = 0; i < NUM_OF_SBCS_PROBERS; i++)
  {
    if (mProbers[i]) // not null
    {
      mProbers[i]->Reset();
      mIsActive[i] = PR_TRUE;
      ++mActiveNum;
    }
    else
      mIsActive[i] = PR_FALSE;
  }
  mBestGuess = -1;
  mState = eDetecting;
}


nsProbingState nsSBCSGroupProber::HandleData(const char* aBuf, PRUint32 aLen)
{
  nsProbingState st;
  PRUint32 i;
  char *newBuf1 = 0;
  PRUint32 newLen1 = 0;

  //apply filter to original buffer, and we got new buffer back
  //depend on what script it is, we will feed them the new buffer 
  //we got after applying proper filter
  //this is done without any consideration to KeepEnglishLetters
  //of each prober since as of now, there are no probers here which
  //recognize languages with English characters.
  if (!FilterWithoutEnglishLetters(aBuf, aLen, &newBuf1, newLen1))
    goto done;
  
  if (newLen1 == 0)
    goto done; // Nothing to see here, move on.

  for (i = 0; i < NUM_OF_SBCS_PROBERS; i++)
  {
     if (!mIsActive[i])
       continue;
     st = mProbers[i]->HandleData(newBuf1, newLen1);
     if (st == eFoundIt)
     {
       mBestGuess = i;
       mState = eFoundIt;
       break;
     }
     else if (st == eNotMe)
     {
       mIsActive[i] = PR_FALSE;
       mActiveNum--;
       if (mActiveNum <= 0)
       {
         mState = eNotMe;
         break;
       }
     }
  }

done:
  PR_FREEIF(newBuf1);

  return mState;
}

float nsSBCSGroupProber::GetConfidence(void)
{
  PRUint32 i;
  float bestConf = 0.0, cf;

  switch (mState)
  {
  case eFoundIt:
    return (float)0.99; //sure yes
  case eNotMe:
    return (float)0.01;  //sure no
  default:
    for (i = 0; i < NUM_OF_SBCS_PROBERS; i++)
    {
      if (!mIsActive[i])
        continue;
      cf = mProbers[i]->GetConfidence();
      if (bestConf < cf)
      {
        bestConf = cf;
        mBestGuess = i;
      }
    }
  }
  return bestConf;
}

#ifdef DEBUG_chardet
void nsSBCSGroupProber::DumpStatus()
{
  PRUint32 i;
  float cf;
  
  cf = GetConfidence();
  printf(" SBCS Group Prober --------begin status \r\n");
  for (i = 0; i < NUM_OF_SBCS_PROBERS; i++)
  {
    if (!mIsActive[i])
      printf("  inactive: [%s] (i.e. confidence is too low).\r\n", mProbers[i]->GetCharSetName());
    else
      mProbers[i]->DumpStatus();
  }
  printf(" SBCS Group found best match [%s] confidence %f.\r\n",  
         mProbers[mBestGuess]->GetCharSetName(), cf);
}
#endif
