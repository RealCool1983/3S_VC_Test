////////////////////////////////////////////////////////////////////////////////
//
//	Title:		Cryptography Object
//	File:		TSDocCrypto.h
//	Author:		Chad Slater
//	Date:		January 2001
//
//	Abstract:	This object exposes a simple interface for encrypting and
//				decrypting files and in-memory data using the CryptoAPI.
//				The encrypted data is then digitally signed and the signature
//				is stored as a property of the object.
//
//
//	Encrypted File Binary Format:
//-------------------------------
//
//	+-----------+
//	|  4 BYTES	| <-- Binary format version (reserved)
//	|-----------|
//	|  4 BYTES	| <-- Size of the private key
//	|-----------|
//	|			|
//	| VARIABLE	| <-- Private key (typically ~76 bytes)
//	|   SIZE	|
//	|___________|
//	|			|
//	| VARIABLE	|
//	|	SIZE	| <-- Encrypted data
//	|			|
//	|-----------|
//	|	 EOF	|
//	|___________|
//
//
//	Current String Binary Format (In-memory Encryption):
//-----------------------------------------------------
//		
// +---------------+
// | BASE64 ENCODED|<---- Base64 Encoded BSTR
// |    WRAPPER	   |
// | +-----------+ |
// | |  4 BYTES	 |<---- Binary format version (reserved)
// | |-----------| |
// | |  4 BYTES	 |<---- Size of the private key
// | |-----------| |
// | |			 | |
// | | VARIABLE	 |<---- Private key (typically ~76 bytes)
// | |   SIZE	 | |
// | |-----------| |
// | |  4 BYTES	 |<---- Size of the encrypted data
// | |-----------| |
// | |			 | |
// | | VARIABLE	 | |
// | |	SIZE	 |<---- Encrypted data
// | |___________| |
// |_______________|
//
//
//
//	Revision History:
//	---------------------------------------------------------------------
//
//	cslater		01/26/2001	Initial revision
//	cslater		01/29/2001	Implemented EncryptDoc and DecryptDoc
//	cslater		02/02/2001	Added Base64EncodeFile and Base64DecodeFile
//	cslater		02/06/2001	Added Base64EncodeFileToString and
//								  Base64DecodeStringToFile
//
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <wincrypt.h>


//Signature
#define     ENCRYPT_SIGNATURE               "3SYSTEM_ENCRYPT_BINFILE_SIGNATURE"

//Types
typedef		BYTE*							LPBYTE;

//Algorithm to use for encryption
#define		FILE_ENCRYPT_ALGORITHM			CALG_RC2 
#define		INMEMORY_ENCRYPT_ALGORITHM		CALG_RC4
#define		HASHING_ALGORITHM				CALG_MD5

//Key container name - Unique per application
#define		CRYPTO_KEY_CONTAINER_NAME		TEXT("ICRYPTO_VERSION_1_0")

//Binary file format version (encrypted files)
#define		INVALID_FORMAT_VERSION			0xFFFFFFFF
#define		BINARY_FORMAT_MAJOR_VERSION		((WORD)0x0001)
#define		BINARY_FORMAT_MINOR_VERSION		((WORD)0x0000)
#define		FILE_BINARY_FORMAT_VERSION		MAKELONG(BINARY_FORMAT_MINOR_VERSION,\
													 BINARY_FORMAT_MAJOR_VERSION)
#define		INMEMORY_FORMAT_VERSION			MAKELONG(BINARY_FORMAT_MINOR_VERSION,\
													 BINARY_FORMAT_MAJOR_VERSION)

//General defines
#define		FILE_INTERMEDIATE_FILENAME		"INTERMEDIATE.CRP"
#define		FILE_READBYTES					0x00000200
#define		SIZE_VERSION					sizeof(DWORD)
#define		SIZE_PRIVATEKEY_SIZE			sizeof(DWORD)
#define		SIZE_ENCRYPTED_DATA				sizeof(DWORD)

//General macros
#define		VERIFY_MALLOC(lpData)			{\
											if(!lpData)\
												nStringID=IDS_ERROR_OUTOFMEMORY,\
												THROW_HRESULT( E_OUTOFMEMORY );\
											}

#define		CRYPTO_OUTPUT_DEBUGSTRING(nStringID) {\
													{\
													TCHAR	szMessage[ MAX_MSG ] = { 0 }; \
													if(::LoadString( _Module.GetResourceInstance(),\
																	 nStringID,\
																	 szMessage,\
																	 MAX_MSG ) ) \
														::OutputDebugString( szMessage ); \
													}\
												}

#define	SAFE_MALLOC_FREE(lpData)		{\
										if(lpData)\
											{\
											free(lpData);\
											lpData = NULL;\
											}\
										}

//Private functions
int WriteFileFormatVersion(FILE* pDestination);
void ClearDigitalSignature(void);
BOOL Initialize(void);
BOOL InitializeContainer(void);
BOOL InitializeDigitalSignatureEngine(void);
BOOL InitializeEncryptionEngine(void);
BOOL WritePrivateKey(FILE* pDestination, HCRYPTKEY&	hPrivateKey);
HCRYPTKEY GeneratePrivateKey( LPBYTE*	lpPrivateKeyBlob, LPDWORD lpdwBlobSize, int enumAlgorithm);

//Export functions
#ifndef __CRYPTO_H__
#define __CRYPTO_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef int (__stdcall *FEncryptDoc)(IN TCHAR* Source, IN TCHAR* Target);
int __stdcall EncryptDoc(TCHAR *Source, TCHAR *Target);

typedef int (__stdcall *FDecryptDoc)(IN TCHAR* Source, IN TCHAR* Target);
int __stdcall DecryptDoc(TCHAR *Source, TCHAR *Target);

typedef int (__stdcall *FExportKeyPair)(OUT LPBYTE* lpPrivateKeyBlob, OUT LPDWORD PrivateKeyBlobSize, 
                                                            OUT LPBYTE* lpPublicKeyBlob, OUT LPDWORD PublicKeyBlobSize);
int __stdcall ExportKeyPair( LPBYTE* lpPrivateKeyBlob, LPDWORD PrivateKeyBlobSize,
                             LPBYTE* lpPublicKeyBlob, LPDWORD PublicKeyBlobSize);

typedef int (__stdcall *FEncryptFileByPrivateKey)(IN TCHAR *PlainTextFile, IN TCHAR *PrivateKeyFile, TCHAR* Target);
int __stdcall EncryptFileByPrivateKey(TCHAR *PlainTextFile, TCHAR *PrivateKeyFile, TCHAR* Target);
                        
typedef int (__stdcall *FDecryptFilecByPublicKey)(TCHAR *CipherTextFile, TCHAR *PublicKeyFile, TCHAR* Target);
int __stdcall DecryptFileByPublicKey(TCHAR *CipherTextFile, TCHAR *PublicKeyFile, TCHAR* Target);
     
typedef int(__stdcall *FEncryptBufferByPublicKey)(IN BYTE* PlainBuffer, IN DWORD PlainBufferSize,
													IN BYTE* PublicKeyBuffer, IN DWORD PublicKeySize,
													OUT BYTE* EncryptBuffer, OUT DWORD *EncryptBufferSize);
int __stdcall EncryptBufferByPublicKey(IN BYTE* PlainBuffer, IN DWORD PlainBufferSize,
										IN BYTE* PublicKeyBuffer, IN DWORD PublicKeySize,
										OUT BYTE* EncryptBuffer, OUT DWORD *EncryptBufferSize);

typedef int(__stdcall *FDecryptBufferByPrivateKey)(IN BYTE* CipherBuffer, IN DWORD CipherBufferSize,
													IN BYTE* PrivateKeyBuffer, IN DWORD PrivateKeySize,
													OUT BYTE* DecryptBuffer, OUT DWORD *DecryptBufferSize);
int __stdcall DecryptBufferByPrivateKey(IN BYTE* CipherBuffer, IN DWORD CipherBufferSize,
										IN BYTE* PrivateKeyBuffer, IN DWORD PrivateKeySize,
										OUT BYTE* DecryptBuffer, OUT DWORD *DecryptBufferSize);

#ifdef __cplusplus
}
#endif

#endif