/*****************************************************************************
*  Copyright 2005 Alt-N Technologies, Ltd. 
*
*  Licensed under the Apache License, Version 2.0 (the "License"); 
*  you may not use this file except in compliance with the License. 
*  You may obtain a copy of the License at 
*
*      http://www.apache.org/licenses/LICENSE-2.0 
*
*  This code incorporates intellectual property owned by Yahoo! and licensed 
*  pursuant to the Yahoo! DomainKeys Patent License Agreement.
*
*  Unless required by applicable law or agreed to in writing, software 
*  distributed under the License is distributed on an "AS IS" BASIS, 
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
*  See the License for the specific language governing permissions and 
*  limitations under the License.
*
*****************************************************************************/

#ifndef DKIMSIGN_H
#define DKIMSIGN_H

#include "dkimbase.h"

class CDKIMSign : public CDKIMBase
{
public:

	CDKIMSign();
	~CDKIMSign();

	int Init( DKIMSignOptions* pOptions );

	int GetSig( char* szPrivKey, char* szSignature, int nSigLength );
	int GetSig2( char* szPrivKey, char** pszSignature );

	virtual int ProcessHeaders(void);
	virtual int ProcessBody( char* szBuffer, int nBufLength, bool bEOF );

	enum CKDKIMConstants { OptimalHeaderLineLength = 65 };

protected:

	void Hash( const char* szBuffer, int nBufLength, bool bHdr, bool bAllmanOnly = false );

	bool SignThisTag( const string& sTag );
	void GetHeaderParams( const string& sHdr );
	void ProcessHeader( const string& sHdr );
	bool ParseFromAddress( void );

	void InitSig(void);
	void AddTagToSig( char* Tag, const string &sValue, char cbrk, bool bFold );
	void AddTagToSig( char* Tag, unsigned long nValue );
	void AddInterTagSpace( int nSizeOfNextTag );
	void AddFoldedValueToSig( const string &sValue, char cbrk );

	bool IsRequiredHeader( const string& sTag );
	int ConstructSignature( char* szPrivKey, bool bUseIetfBodyHash, bool bUseSha256 );

	int AssembleReturnedSig( char* szPrivKey );

	EVP_MD_CTX * m_Hdr_ietf_sha1ctx;		/* the header hash for ietf sha1  */
	EVP_MD_CTX * m_Hdr_ietf_sha256ctx;	/* the header hash for ietf sha256 */

	EVP_MD_CTX* m_Bdy_ietf_sha1ctx;		/* the body hash for ietf sha1  */
	EVP_MD_CTX * m_Bdy_ietf_sha256ctx;	/* the body hash for ietf sha256 */

	EVP_MD_CTX* m_allman_sha1ctx;		/* the hash for allman sha1  */

	int m_Canon;				// canonization method

	int m_EmptyLineCount;

	string hParam;
	string sFrom;
	string sSender;
	string sSelector;
	string sDomain;
	string sIdentity;					// for i= tag, if empty tag will not be included in sig
	string sRequiredHeaders;

	bool m_IncludeBodyLengthTag;
	int m_nBodyLength;
	time_t m_ExpireTime;
	int m_nIncludeTimeStamp;				// 0 = don't include t= tag, 1 = include t= tag
	int m_nIncludeQueryMethod;				// 0 = don't include q= tag, 1 = include q= tag
	int m_nHash;							// use one of the DKIM_HASH_xx constants here
	int m_nIncludeCopiedHeaders;			// 0 = don't include z= tag, 1 = include z= tag
	int m_nIncludeBodyHash;					// 0 = calculate sig using draft 0, 1 = include bh= tag and 
											// use new signature computation algorithm


	DKIMHEADERCALLBACK m_pfnHdrCallback;

	string m_sSig;
	int m_nSigPos;

	string m_sReturnedSig;
	bool m_bReturnedSigAssembled;

	string m_sCopiedHeaders;

};



#endif // DKIMSIGN_H
