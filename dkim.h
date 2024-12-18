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

#ifdef WIN32
#define DKIM_CALL	WINAPI
#else
#define DKIM_CALL
#define MAKELONG(a,b) ((long)(((unsigned)(a) & 0xffff) | (((unsigned)(b) & 0xffff) << 16)))
#endif


#ifdef __cplusplus
extern "C" {
#endif

// DKIM Body hash versions
#define DKIM_BODYHASH_ALLMAN_1	1
#define DKIM_BODYHASH_IETF_1	2
#define DKIM_BODYHASH_BOTH		DKIM_BODYHASH_ALLMAN_1 | DKIM_BODYHASH_IETF_1

// DKIM hash algorithms
#define DKIM_HASH_SHA1			1
#define DKIM_HASH_SHA256		2
#define DKIM_HASH_SHA1_AND_256  DKIM_HASH_SHA1 | DKIM_HASH_SHA256

// DKIM canonicalization methods
#define DKIM_CANON_SIMPLE		1
#define DKIM_CANON_NOWSP		2
#define DKIM_CANON_RELAXED		3

#define DKIM_SIGN_SIMPLE			MAKELONG(DKIM_CANON_SIMPLE,DKIM_CANON_SIMPLE)
#define DKIM_SIGN_SIMPLE_RELAXED	MAKELONG(DKIM_CANON_RELAXED,DKIM_CANON_SIMPLE)
#define DKIM_SIGN_RELAXED			MAKELONG(DKIM_CANON_RELAXED,DKIM_CANON_RELAXED)
#define DKIM_SIGN_RELAXED_SIMPLE	MAKELONG(DKIM_CANON_SIMPLE,DKIM_CANON_RELAXED)

// DKIM Error codes
#define DKIM_SUCCESS						0		// operation successful
#define DKIM_FAIL							-1		// verify error: message is suspicious
#define DKIM_BAD_SYNTAX						-2		// signature error: DKIM-Signature could not parse or has bad tags/values
#define DKIM_SIGNATURE_BAD					-3		// signature error: RSA verify failed
#define DKIM_SIGNATURE_BAD_BUT_TESTING		-4		// signature error: RSA verify failed but testing
#define DKIM_SIGNATURE_EXPIRED				-5		// signature error: x= is old
#define DKIM_SELECTOR_INVALID				-6		// signature error: selector doesn't parse or contains invalid values
#define DKIM_SELECTOR_GRANULARITY_MISMATCH	-7		// signature error: selector g= doesn't match i=
#define DKIM_SELECTOR_KEY_REVOKED			-8		// signature error: selector p= empty
#define DKIM_SELECTOR_DOMAIN_NAME_TOO_LONG	-9		// signature error: selector domain name too long to request
#define DKIM_SELECTOR_DNS_TEMP_FAILURE		-10		// signature error: temporary dns failure requesting selector
#define DKIM_SELECTOR_DNS_PERM_FAILURE		-11		// signature error: permanent dns failure requesting selector
#define DKIM_SELECTOR_PUBLIC_KEY_INVALID	-12		// signature error: selector p= value invalid or wrong format
#define DKIM_NO_SIGNATURES					-13		// process error, no sigs
#define DKIM_NO_VALID_SIGNATURES			-14		// process error, no valid sigs
#define DKIM_BODY_HASH_MISMATCH				-15		// sigature verify error: message body does not hash to bh value
#define DKIM_SELECTOR_ALGORITHM_MISMATCH	-16		// signature error: selector h= doesn't match signature a=
#define DKIM_STAT_INCOMPAT					-17		// signature error: incompatible v=
#define DKIM_UNSIGNED_FROM					-18		// signature error: not all message's From headers in signature
#define DKIM_OUT_OF_MEMORY					-20		// memory allocation failed
#define DKIM_INVALID_CONTEXT				-21		// DKIMContext structure invalid for this operation
#define DKIM_NO_SENDER						-22		// signing error: Could not find From: or Sender: header in message
#define DKIM_BAD_PRIVATE_KEY				-23		// signing error: Could not parse private key
#define DKIM_BUFFER_TOO_SMALL				-24		// signing error: Buffer passed in is not large enough
#define DKIM_MAX_ERROR						-25		// set this to 1 greater than the highest error code (but negative)

// DKIM_SUCCESS									// verify result: all signatures verified
												// signature result: signature verified
#define DKIM_FINISHED_BODY					1	// process result: no more message body is needed
#define DKIM_PARTIAL_SUCCESS				2	// verify result: at least one but not all signatures verified
#define DKIM_NEUTRAL						3	// verify result: no signatures verified but message is not suspicous
#define DKIM_SUCCESS_BUT_EXTRA				4	// signature result: signature verified but it did not include all of the body



// This function is called once for each header in the message
// return 1 to include this header in the signature and 0 to exclude.
typedef int (DKIM_CALL *DKIMHEADERCALLBACK)(const char* szHeader);

// This function is called to retrieve a TXT record from DNS
typedef int (DKIM_CALL *DKIMDNSCALLBACK)(const char* szFQDN, char* szBuffer, int nBufLen );

#ifdef _WIN32
#include <pshpack1.h>
#endif

typedef struct DKIMContext_t
{
	unsigned int reserved1;
	unsigned int reserved2;
	void*		 reserved3;
} DKIMContext;

typedef struct DKIMSignOptions_t
{
	int nCanon;								// canonization 
	int nIncludeBodyLengthTag;				// 0 = don't include l= tag, 1 = include l= tag
	int nIncludeTimeStamp;					// 0 = don't include t= tag, 1 = include t= tag
	int nIncludeQueryMethod;				// 0 = don't include q= tag, 1 = include q= tag
	char szSelector[80];					// selector - required
	char szDomain[256];						// domain - optional - if empty, domain is computed from sender
	char szIdentity[256];					// for i= tag, if empty tag will not be included in sig
	unsigned long expireTime;				// for x= tag, if 0 tag will not be included in sig
	DKIMHEADERCALLBACK pfnHeaderCallback;	// header callback
	char szRequiredHeaders[256];			// colon-separated list of headers that must be signed
	int nHash;								// use one of the DKIM_HASH_xx constants here
											// even if not present in the message
	int nIncludeCopiedHeaders;				// 0 = don't include z= tag, 1 = include z= tag
	int nIncludeBodyHash;					// use one of the DKIM_BODYHASH_xx constants here
} DKIMSignOptions;							

typedef struct DKIMVerifyOptions_t
{
	DKIMDNSCALLBACK pfnSelectorCallback;	// selector record callback
	DKIMDNSCALLBACK pfnPracticesCallback;	// ADSP record callback
	int nHonorBodyLengthTag;				// 0 = ignore l= tag, 1 = use l= tag to limit the amount of body verified
	int nCheckPractices;					// 0 = use default (unknown) practices, 1 = request and use author domain signing practices
	int nSubjectRequired;					// 0 = subject is required to be signed, 1 = not required
	int nSaveCanonicalizedData;				// 0 = canonicalized data is not saved, 1 = canonicalized data is saved
	int nAllowUnsignedFromHeaders;			// 0 = From headers not included in the signature are not allowed, 1 = allowed
} DKIMVerifyOptions;

typedef struct DKIMVerifyDetails_t
{
	char *szSignature;
	char *szSignatureDomain;
	char *szIdentityDomain;
	char *szCanonicalizedData;
	int nResult;
} DKIMVerifyDetails;


#ifdef _WIN32
#include <poppack.h>
#endif

int DKIM_CALL DKIMSignInit( DKIMContext* pSignContext, DKIMSignOptions* pOptions );
int DKIM_CALL DKIMSignProcess( DKIMContext* pSignContext, char* szBuffer, int nBufLength );
int DKIM_CALL DKIMSignGetSig( DKIMContext* pSignContext, char* szPrivKey, char* szSignature, int nSigLength );
int DKIM_CALL DKIMSignGetSig2( DKIMContext* pSignContext, char* szPrivKey, char** pszSignature );
void DKIM_CALL DKIMSignFree( DKIMContext* pSignContext );

int DKIM_CALL DKIMVerifyInit( DKIMContext* pVerifyContext, DKIMVerifyOptions* pOptions );
int DKIM_CALL DKIMVerifyProcess( DKIMContext* pVerifyContext, char* szBuffer, int nBufLength );
int DKIM_CALL DKIMVerifyResults( DKIMContext* pVerifyContext );
int DKIM_CALL DKIMVerifyGetDetails( DKIMContext* pVerifyContext, int* nSigCount, DKIMVerifyDetails** pDetails, char* szPractices );
void DKIM_CALL DKIMVerifyFree( DKIMContext* pVerifyContext );

char *DKIM_CALL DKIMVersion();

char *DKIM_CALL DKIMGetErrorString( int ErrorCode );

#ifdef __cplusplus
}
#endif
