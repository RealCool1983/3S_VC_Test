////////////////////////////////////////////////////////////////////////////////
//
//	Title:		Cryptography Object
//	File:		TSDocCrypto.cpp
//	Author:		Chad Slater
//	Date:		January 2001
//
//	Current Encrypted File Binary Format:
//---------------------------------------
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

#include "stdafx.h"
#include "Crypto.h"
#include <stdlib.h>
#include <stdio.h>

//global variables
HCRYPTPROV	m_hContext;
LPBYTE		m_lpDigitalSignature;
DWORD		m_dwDigitalSignatureLen;
CHAR        m_bstrContainerName[256];

#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)

int WriteFileFormatVersion( FILE* pDestination ){

    DWORD	dwFileFormat = FILE_BINARY_FORMAT_VERSION;

    if( pDestination == NULL ){
        return 1;
    }

    //	Stamp the file with the version header
    fwrite( &dwFileFormat, sizeof(DWORD), 1, pDestination );

    return 0;
}

void ClearDigitalSignature(void){
    SAFE_MALLOC_FREE( m_lpDigitalSignature );
    m_dwDigitalSignatureLen = 0;
}

BOOL Initialize(void){
	BOOL	bSuccess	= TRUE;
    m_hContext = NULL;
	m_lpDigitalSignature = NULL,
	m_dwDigitalSignatureLen = 0,	
    memset(m_bstrContainerName, 0, 256);
    strcpy(m_bstrContainerName, (char *)CRYPTO_KEY_CONTAINER_NAME);

	if( !m_hContext ){
		bSuccess = InitializeContainer();
		if( bSuccess ){
			bSuccess  = InitializeDigitalSignatureEngine();

            if( bSuccess ){
				bSuccess = 	InitializeEncryptionEngine();
            }

			//	If we failed to initialize digital signatures or encryption, then reset
			if( !bSuccess ){
				CryptReleaseContext(m_hContext, 0 );
				m_hContext = NULL;
			}
		}
	}

	return bSuccess;
}

BOOL InitializeContainer(void){
	BOOL	bSuccess = FALSE;

	bSuccess = CryptAcquireContext( &m_hContext,
									(LPCWSTR)m_bstrContainerName,
									MS_DEF_PROV,
									PROV_RSA_FULL, 0 );

	//	Container doesn't exist - Possibly try to create a new container
	if( !bSuccess ){
		int nLastError = GetLastError();
		if( nLastError == NTE_BAD_KEYSET ){
			//	Creating the key container for the first time
			bSuccess = CryptAcquireContext( &m_hContext,
											(LPCWSTR)m_bstrContainerName,
											MS_DEF_PROV,
											PROV_RSA_FULL, CRYPT_NEWKEYSET );
            if( !bSuccess ){
                return bSuccess;
            }
		}
        else{
            return bSuccess;
        }
	}

    if( !bSuccess ){
        return bSuccess;
    }

	return bSuccess;
}


BOOL InitializeDigitalSignatureEngine(void){
	BOOL		bSuccess = FALSE;
	HCRYPTKEY	hKey	 = NULL;

	//	Check the key pair for digital signatures
	if( !CryptGetUserKey( m_hContext, AT_SIGNATURE, &hKey ) ){
		if( GetLastError() == NTE_NO_KEY ) {
			// There is a container but no key - Create signature key pair
            if( !CryptGenKey( m_hContext, AT_SIGNATURE, 0, &hKey ) ){
                return FALSE;
            }else{
				bSuccess = TRUE;
			}
        }
        else{
			return FALSE;
		}
    }
    else{
        //	Key pair already available
		bSuccess = TRUE;
    }

    if(hKey){
	    CryptDestroyKey( hKey );
    }

	return bSuccess;
}


BOOL InitializeEncryptionEngine(void){
	BOOL		bSuccess = FALSE;
	HCRYPTKEY	hKey	 = NULL;

	//	Check the key pair for encryption
	if( !CryptGetUserKey( m_hContext, AT_KEYEXCHANGE, &hKey ) ){
		if( GetLastError() == NTE_NO_KEY ) {
			// There is a container but no key - Create an encryption key pair
            if( !CryptGenKey( m_hContext, AT_KEYEXCHANGE, 0, &hKey ) ){
                return FALSE;
            }
            else{
				bSuccess = TRUE;
		    }
        }else{
            return FALSE;
		}
    }else{
        //	Key pair already available
		bSuccess = TRUE;
    }

    if( hKey ){
		CryptDestroyKey( hKey );
    }

	return bSuccess;
}

BOOL DigitallySignHash( HCRYPTHASH hHash ){
	BOOL	bSuccess	= FALSE;

	//Clear existing signature
	ClearDigitalSignature();

	//Determine the length of the digital signature
	if( CryptSignHash( hHash, AT_SIGNATURE, NULL, 0, NULL, &m_dwDigitalSignatureLen ) ){
		m_lpDigitalSignature = (LPBYTE)malloc( m_dwDigitalSignatureLen + 1 );
        if( m_lpDigitalSignature ){
			//Sign the original (decrypted) data
			bSuccess = CryptSignHash(	hHash,
										AT_SIGNATURE, NULL, 0,
										m_lpDigitalSignature,
										&m_dwDigitalSignatureLen );
		}
    }

	return bSuccess;
}

BOOL ReadFileFormatVersion( FILE* pSrc ){
	BOOL	bSuccess	 = FALSE;
	DWORD	dwFileFormat = INVALID_FORMAT_VERSION;

	if( fread( &dwFileFormat, sizeof(DWORD), 1, pSrc ) ){
        if( dwFileFormat == FILE_BINARY_FORMAT_VERSION ){
			bSuccess = TRUE;
		}
    }
	return bSuccess;
}

BOOL ReadPrivateKey( FILE* pSrcFile, HCRYPTKEY& hPrivateKey){
	BOOL		bSuccess		= FALSE;
	DWORD		dwKeySize		= 0;
	LPBYTE		lpPrivateKey	= NULL;

	//	Read in the key size
	if( fread( &dwKeySize, sizeof(DWORD), 1, pSrcFile ) ){
		//	Allocate room to hold the private key
		lpPrivateKey = (LPBYTE)malloc( sizeof(BYTE)*dwKeySize );
        if( lpPrivateKey == NULL ){
            return FALSE;
        }

		if( lpPrivateKey ){
			//	Read in the private key
			if( fread(	lpPrivateKey,
						sizeof(BYTE),
						dwKeySize,
						pSrcFile ) == dwKeySize )
			{
				//	Import the private key into the CSP
				bSuccess = CryptImportKey(  m_hContext,
											lpPrivateKey,
											dwKeySize, 0, 0,
											&hPrivateKey);
			}
		}
	}

	SAFE_MALLOC_FREE( lpPrivateKey );

	return bSuccess;
}

BOOL WritePrivateKey(FILE* pDestination, HCRYPTKEY&	hPrivateKey	){
	BOOL		bSuccess	= FALSE;
	DWORD		dwBlobLen	= 0;
	LPBYTE		lpPrivateKey= NULL;

    if( pDestination == NULL){
        return FALSE;
    }

    //	Get a private key
    hPrivateKey = GeneratePrivateKey( &lpPrivateKey, &dwBlobLen, FILE_ENCRYPT_ALGORITHM );
    
    //	If we succeeded in getting a private key...
    if( lpPrivateKey ){
    	//	Write the size of the private key to the destination file
    	if( fwrite(  &dwBlobLen, sizeof(dwBlobLen), 1, pDestination ) ){
    		//	Write the private key (encrypted) to the destination file
            if( fwrite( lpPrivateKey, sizeof(BYTE), dwBlobLen, pDestination ) == dwBlobLen ){
    			bSuccess = TRUE;
            }
    	}
    }
    
    SAFE_MALLOC_FREE( lpPrivateKey );

    return bSuccess;
}

HCRYPTKEY GeneratePrivateKey( LPBYTE* lpPrivateKeyBlob, LPDWORD lpdwBlobSize, int enumAlgorithm) {
	HCRYPTKEY	hPrivateKey = NULL;
	HCRYPTKEY	hPublicKey  = NULL;

	//	Generate the private key
	ClearDigitalSignature();

    if( Initialize() && CryptGenKey(m_hContext, enumAlgorithm, CRYPT_EXPORTABLE, &hPrivateKey ) ){
		// Get handle to the encrypter's exchange public key
		if( CryptGetUserKey( m_hContext, AT_KEYEXCHANGE, &hPublicKey ) ){
			// Determine size of the key blob
			if( CryptExportKey(hPrivateKey, hPublicKey, SIMPLEBLOB, 0, NULL, lpdwBlobSize) ){
				(*lpPrivateKeyBlob) = (LPBYTE)malloc( sizeof(BYTE) * (*lpdwBlobSize) );
                if( lpPrivateKeyBlob == NULL ){
                    return hPrivateKey;
                }
            }

             if( lpPrivateKeyBlob ){
			    // Encrypt and export private key
				CryptExportKey( hPrivateKey,
					 			hPublicKey,
								SIMPLEBLOB,
								0,
								(*lpPrivateKeyBlob),
								lpdwBlobSize);
             }
		}
	}

    if( hPublicKey ){
		CryptDestroyKey( hPublicKey );
    }

	return hPrivateKey;
}

extern "C"
int __stdcall ExportKeyPair( LPBYTE* lpPrivateKeyBlob, LPDWORD PrivateKeyBlobSize,
                             LPBYTE* lpPublicKeyBlob, LPDWORD PublicKeyBlobSize)
{
  HCRYPTPROV hCryptProv = NULL;
  HCRYPTKEY hKey = NULL;
  int ret = 0;
      
  memset(m_bstrContainerName, 0, 256);
  strcpy(m_bstrContainerName, (char *)CRYPTO_KEY_CONTAINER_NAME);

    // Acquire access to key container
  if (!CryptAcquireContext(&hCryptProv, (LPCWSTR)m_bstrContainerName, NULL, PROV_RSA_FULL, 0))
    {
          // Try to create a new key container
		if (!CryptAcquireContext(&hCryptProv, (LPCWSTR)m_bstrContainerName, MS_DEF_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET))
          {
                ret = 1;
                goto CLEAN_UP;
          }
    }

    // Generate new key pair
    if (!CryptGenKey(hCryptProv, AT_KEYEXCHANGE, CRYPT_EXPORTABLE, &hKey))    
    {
        ret = 2;
        goto CLEAN_UP;
    }

    // Get public key size
    if (!CryptExportKey(hKey, NULL, PUBLICKEYBLOB, 0, NULL, PublicKeyBlobSize))
    {
        ret = 3;
        goto CLEAN_UP;
    }

    // Create a buffer for the public key
    (*lpPublicKeyBlob) = (LPBYTE)malloc( sizeof(BYTE) * (*PublicKeyBlobSize) );
    if( lpPublicKeyBlob == NULL ){
        ret = 4;
        goto CLEAN_UP;
    }
    memset(*lpPublicKeyBlob, 0, sizeof(BYTE) * (*PublicKeyBlobSize));

    // Get public key        
    if (!CryptExportKey(hKey, NULL, PUBLICKEYBLOB, 0, (*lpPublicKeyBlob), PublicKeyBlobSize))
    {
        ret = 5;
        goto CLEAN_UP;
    }

    // Get private key size
    if (!CryptExportKey(hKey, NULL, PRIVATEKEYBLOB, 0, NULL, PrivateKeyBlobSize))
    {
        ret = 6;
        goto CLEAN_UP;
    }

    (*lpPrivateKeyBlob) = (LPBYTE)malloc( sizeof(BYTE) * (*PrivateKeyBlobSize) );
    if( lpPrivateKeyBlob == NULL ){
        ret = 7;
        goto CLEAN_UP;
    }

    // Get private key        
    if (!CryptExportKey(hKey, NULL, PRIVATEKEYBLOB, 0, (*lpPrivateKeyBlob), PrivateKeyBlobSize))
    {
        ret = 8;
        goto CLEAN_UP;
    }

 CLEAN_UP:
        
    if (hKey) {
        CryptDestroyKey(hKey);
    }           
    if (hCryptProv) {
        CryptReleaseContext(hCryptProv, 0 );
        hCryptProv = NULL;
    }

    return ret;
}

BYTE *SpcEncrypt(HCRYPTKEY hKey, BOOL bFinal, BYTE *pbData, DWORD *cbData) {
  BYTE   *pbResult;
  DWORD  dwBlockLen, dwDataLen;
  ALG_ID Algid;
   
  dwDataLen = sizeof(ALG_ID);
  if (!CryptGetKeyParam(hKey, KP_ALGID, (BYTE *)&Algid, &dwDataLen, 0)) 
      return 0;
  if (GET_ALG_TYPE(Algid) != ALG_TYPE_STREAM) {
    dwDataLen = sizeof(DWORD);
    if (!CryptGetKeyParam(hKey, KP_BLOCKLEN, (BYTE *)&dwBlockLen, &dwDataLen, 0))
      return 0;
    dwDataLen = ((*cbData + (dwBlockLen * 2) - 1) / dwBlockLen) * dwBlockLen;
    if (!(pbResult = (BYTE *)LocalAlloc(LMEM_FIXED, dwDataLen))) 
        return 0;
    CopyMemory(pbResult, pbData, *cbData);
    if (!CryptEncrypt(hKey, 0, bFinal, 0, pbResult, &dwDataLen, *cbData)) {
      LocalFree(pbResult);
      return 0;
    }
    *cbData = dwDataLen;
    return pbResult;
  }
   
  if (!(pbResult = (BYTE *)LocalAlloc(LMEM_FIXED, *cbData))) 
      return 0;
  CopyMemory(pbResult, pbData, *cbData);
  if (!CryptEncrypt(hKey, 0, bFinal, 0, pbResult, cbData, *cbData)) {
    LocalFree(pbResult);
    return 0;
  }

  return pbResult;
}

extern "C"
int __stdcall EncryptFileByPrivateKey(TCHAR* strPlainFile, TCHAR* strPublicKeyFile, TCHAR* strEncryptedFile)
{
    // Variables
    HCRYPTPROV hCryptProv = NULL;
    HCRYPTKEY hKey = NULL;
    DWORD dwPublicKeyLen = 0;
    DWORD dwDataLen = 0;
    DWORD dwEncryptedLen = 0;
    BYTE* pbPublicKey = NULL;
    BYTE* pbData = NULL;
    HANDLE hPublicKeyFile = NULL;
    HANDLE hEncryptedFile = NULL;
    HANDLE hPlainFile = NULL;
    DWORD lpNumberOfBytesWritten = 0;
    int ret = 0;

    // Acquire access to key container
    if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))       
    {
          ret = 1;
          goto CLEAN_UP;
    }

    // Open public key file
    if ((hPublicKeyFile = CreateFile( strPublicKeyFile,
                                      GENERIC_READ,
                                      FILE_SHARE_READ,
                                      NULL,
                                      OPEN_EXISTING,
                                      FILE_FLAG_SEQUENTIAL_SCAN,
                                      NULL )) == INVALID_HANDLE_VALUE)
    {
          ret = 1;
          goto CLEAN_UP;
    }

    // Get file size
    if ((dwPublicKeyLen = GetFileSize(hPublicKeyFile, NULL)) == INVALID_FILE_SIZE)
    {
          ret = 1;
          goto CLEAN_UP;
    }

    // Create a buffer for the public key
    if (!(pbPublicKey = (BYTE *)malloc(dwPublicKeyLen)))
    {
          ret = 1;
          goto CLEAN_UP;
    }

    // Read public key
    if (!ReadFile(hPublicKeyFile, pbPublicKey, dwPublicKeyLen, &dwPublicKeyLen, NULL))
    {
          ret = 1;
          goto CLEAN_UP;
    }

    // Import public key
    if (!CryptImportKey(hCryptProv, pbPublicKey, dwPublicKeyLen, 0, 0, &hKey))
    {
          ret = 1;
          goto CLEAN_UP;
    }

    // Open plain text file
    if ((hPlainFile = CreateFile( strPlainFile,
                                  GENERIC_READ,
                                  FILE_SHARE_READ,
                                  NULL,
                                  OPEN_EXISTING,
                                  FILE_FLAG_SEQUENTIAL_SCAN,
                                  NULL )) == INVALID_HANDLE_VALUE)
    {
          ret = 1;
          goto CLEAN_UP;
    }

    // Get file size
    if ((dwDataLen = GetFileSize(hPlainFile, NULL)) == INVALID_FILE_SIZE)
    {
          ret = 1;
          goto CLEAN_UP;
    }

    // Create a buffer for the plain text
    if (!(pbData = (BYTE *)malloc(dwDataLen)))
    {
          ret = 1;
          goto CLEAN_UP;
    }

    // Read plain text
    if (!ReadFile(hPlainFile, pbData, dwDataLen, &dwDataLen, NULL))
    {
          ret = 1;
          goto CLEAN_UP;
    }

    // Get lenght for encrypted data
    if (!CryptEncrypt(hKey, NULL, TRUE, 0, NULL, &dwEncryptedLen, 0))
    {
          ret = 1;
          goto CLEAN_UP;
    }

    // Create a buffer for encrypted data
    if (!(pbData = (BYTE *)realloc(pbData, dwEncryptedLen)))
    {
          ret = 1;
          goto CLEAN_UP;
    }
    
    // Create a file to save the encrypted data
    if ((hEncryptedFile = CreateFile( strEncryptedFile,
                                      GENERIC_WRITE,
                                      0,
                                      NULL,
                                      CREATE_ALWAYS,
                                      FILE_ATTRIBUTE_NORMAL,
                                      NULL )) == INVALID_HANDLE_VALUE)
    {
          ret = 1;
          goto CLEAN_UP;
    }

    SetFilePointer(hPlainFile, 0, 0, FILE_BEGIN);

    DWORD dwError;

    do{
        memset(pbData, 0, dwEncryptedLen);
        
        dwDataLen = 64;
        if (!ReadFile(hPlainFile, pbData, dwDataLen, &dwDataLen, NULL))
        {
              ret = 1;
              goto CLEAN_UP;
        }

        dwError = GetLastError();
        if( dwError == ERROR_HANDLE_EOF || dwDataLen == 0){
            break;
        }

        // Encrypt data
        if (!CryptEncrypt(hKey, NULL, FALSE, 0, pbData, &dwDataLen, dwEncryptedLen))
        {
              ret = 1;
              goto CLEAN_UP;
        }    

        // Write the public key to the file
        if (!WriteFile( hEncryptedFile, 
                        (LPCVOID)pbData, 
                        dwDataLen, 
                        &lpNumberOfBytesWritten, 
                        NULL)) 
        {
              ret = 1;
              goto CLEAN_UP;
        }
    }while(dwError != ERROR_HANDLE_EOF);

 CLEAN_UP:       

    if (!pbPublicKey) {
          free(pbPublicKey);
    }
    if (!pbData) {
          free(pbData);
    }
    if (hPublicKeyFile) {
          CloseHandle(hPublicKeyFile);
    }
    if (hPlainFile) {
          CloseHandle(hPlainFile);
    }
    if (hEncryptedFile) {
          CloseHandle(hEncryptedFile);
    }
    if (hKey) {
          CryptDestroyKey(hKey);
    }           
    if (hCryptProv) {
          CryptReleaseContext(hCryptProv, 0);
    }

    return ret;
}

extern "C"
int __stdcall DecryptFileByPublicKey(TCHAR* strEncryptedFile, TCHAR* strPrivateKeyFile, TCHAR* strPlainFile)
{
    // Variables
    HCRYPTPROV hCryptProv = NULL;
    HCRYPTKEY hKey = NULL;
    DWORD dwPrivateKeyLen = 0;
    DWORD dwDataLen = 0;
    BYTE* pbPrivateKey = NULL;
    BYTE* pbData = NULL;
    HANDLE hPrivateKeyFile = NULL;
    HANDLE hEncryptedFile = NULL;
    HANDLE hPlainFile = NULL;
    DWORD lpNumberOfBytesWritten = 0;
    int ret = 0;
    DWORD dwFileSize = 0;

    // Acquire access to key container
    if (!CryptAcquireContext(&hCryptProv, TEXT("AlejaCMa.EncryptDecrypt"), NULL, PROV_RSA_FULL, 0)) 
    {
          // Try to create a new key container
          if (!CryptAcquireContext(&hCryptProv, TEXT("AlejaCMa.EncryptDecrypt"), NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
          {
                ret = 1;
                goto CLEAN_UP;
          }
    }

    // Open private key file
    if ((hPrivateKeyFile = CreateFile( strPrivateKeyFile,
                                       GENERIC_READ,
                                       FILE_SHARE_READ,
                                       NULL,
                                       OPEN_EXISTING,
                                       FILE_FLAG_SEQUENTIAL_SCAN,
                                       NULL
                                       )) == INVALID_HANDLE_VALUE)
    {
          ret = 1;
          goto CLEAN_UP;
    }

    // Get file size
    if ((dwPrivateKeyLen = GetFileSize(hPrivateKeyFile, NULL)) == INVALID_FILE_SIZE)
    {
          ret = 1;
          goto CLEAN_UP;
    }

    // Create a buffer for the private key
    if (!(pbPrivateKey = (BYTE *)malloc(dwPrivateKeyLen)))
    {
          ret = 1;
          goto CLEAN_UP;
    }

    // Read private key
    if (!ReadFile(hPrivateKeyFile, pbPrivateKey, dwPrivateKeyLen, &dwPrivateKeyLen, NULL))
    {
          ret = 1;
          goto CLEAN_UP;
    }

    // Import private key
    if (!CryptImportKey(hCryptProv, pbPrivateKey, dwPrivateKeyLen, 0, 0, &hKey))
    {
          ret = 1;
          goto CLEAN_UP;
    }

    // Open encrypted file
    if ((hEncryptedFile = CreateFile( strEncryptedFile,
                                      GENERIC_READ,
                                      FILE_SHARE_READ,
                                      NULL,
                                      OPEN_EXISTING,
                                      FILE_FLAG_SEQUENTIAL_SCAN,
                                      NULL )) == INVALID_HANDLE_VALUE)
    {
          ret = 1;
          goto CLEAN_UP;
    }

    // Get file size
    if ((dwFileSize = GetFileSize(hEncryptedFile, NULL)) == INVALID_FILE_SIZE)
    {
          ret = 1;
          goto CLEAN_UP;
    }

    // Create a buffer for the encrypted data
    dwFileSize = 512;
    if (!(pbData = (BYTE *)malloc(dwFileSize)))
    {
          ret = 1;
          goto CLEAN_UP;
    }

    // Create a file to save the plain text
    if ((hPlainFile = CreateFile( strPlainFile,
                                  GENERIC_WRITE,
                                  0,
                                  NULL,
                                  CREATE_ALWAYS,
                                  FILE_ATTRIBUTE_NORMAL,
                                  NULL )) == INVALID_HANDLE_VALUE)
    {
          ret = 1;
          goto CLEAN_UP;
    }

    SetFilePointer(hEncryptedFile, 0, 0, FILE_BEGIN);

    DWORD dwError;

    do{        
        memset(pbData, 0, dwFileSize);

        dwDataLen = 128;
                
        if (!ReadFile(hEncryptedFile, pbData, dwDataLen, &dwDataLen, NULL))
        {
              ret = 1;
              goto CLEAN_UP;
        }

        dwError = GetLastError();
        if( dwError == ERROR_HANDLE_EOF || dwDataLen == 0){
            break;
        }
        
        if (!CryptDecrypt(hKey, NULL, FALSE, 0, pbData, &dwDataLen))
        {
            dwError = GetLastError();
            if( dwError == NTE_BAD_LEN ){
                ret = 2;
            }
            ret = 1;
            goto CLEAN_UP;
        }    

        // Write the public key to the file
        if (!WriteFile( hPlainFile, 
                        (LPCVOID)pbData, 
                        dwDataLen, 
                        &lpNumberOfBytesWritten, 
                        NULL)) 
        {
              ret = 1;
              goto CLEAN_UP;
        }
    }while(dwError != ERROR_HANDLE_EOF);
    
 CLEAN_UP:

    if (!pbPrivateKey) {
          free(pbPrivateKey);
    }                       
    if (!pbData) {
          free(pbData);
    }           
    if (hPrivateKeyFile) {
          CloseHandle(hPrivateKeyFile);
    }
    if (hEncryptedFile) {
          CloseHandle(hEncryptedFile);
    }
    if (hPlainFile) {
          CloseHandle(hPlainFile);
    }
    if (hKey) {
          CryptDestroyKey(hKey);
    }           
    if (hCryptProv) {
          CryptReleaseContext(hCryptProv, 0);
    }

    return ret;
}

extern "C"
int __stdcall EncryptBufferByPublicKey(IN BYTE* PlainBuffer, IN DWORD PlainBufferSize,
										IN BYTE* PublicKeyBuffer, IN DWORD PublicKeySize,
                                        OUT BYTE* EncryptBuffer, OUT DWORD *EncryptBufferSize)
{
    // Variables
    HCRYPTPROV  hCryptProv = NULL;
    HCRYPTKEY   hKey = NULL;        
    BYTE*       pbData = NULL;
    DWORD       dwEncryptedLen = 0;
    DWORD       TotalEncryptLen = 0;
    DWORD       Count = 0;
    DWORD       dwDataLen = 0;   
    char        buf[256] = {0};
    int         ret = 0;

    // Acquire access to key container
    if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)){
        ret = 1;
        goto CLEAN_UP;
    }    

    // Import Public key
	if (!CryptImportKey(hCryptProv, PublicKeyBuffer, PublicKeySize, 0, 0, &hKey)){
        ret = 2;
        goto CLEAN_UP;
    }   

    // Get lenght for encrypted data
    if (!CryptEncrypt(hKey, NULL, TRUE, 0, NULL, &dwEncryptedLen, 0)){
        ret = 3;
        goto CLEAN_UP;
    }

    // Create a buffer for encrypted data
    if( !(pbData = (BYTE*)malloc(dwEncryptedLen)) ){
        ret = 4;
        goto CLEAN_UP;
    }

    TotalEncryptLen = 0;
    Count = 0;
    do{
        memset(pbData, 0, dwEncryptedLen);
        
        dwDataLen = 64; //20160606
		//dwDataLen = PlainBufferSize;//20160810 Karen Tsai
        memcpy(pbData, PlainBuffer + (Count * dwDataLen), dwDataLen);        

        // Encrypt data
        if( !CryptEncrypt(hKey, NULL, FALSE, 0, pbData, &dwDataLen, dwEncryptedLen) ){            
            DWORD result = GetLastError();
            sprintf(buf, "CryptEncrypt failed(0x%X)", result);
			MessageBox(NULL, (LPCWSTR)buf, (LPCWSTR)"ERROR", MB_OK);
            ret = 5;
            goto CLEAN_UP;
        }    
		
        if( EncryptBuffer != NULL ){
            memcpy(EncryptBuffer + TotalEncryptLen, pbData, dwDataLen);
            //20160619
		/*	FILE *pFile;
            char filepath[256] = {0};
			BYTE *temp;
			temp = (BYTE *)malloc(PlainBufferSize);
            ::sprintf(filepath, "G:\\PlainBuffer.bin");
            pFile = fopen(filepath, "w+b");
			memcpy(temp, PlainBuffer, PlainBufferSize);
            fwrite(temp, PlainBufferSize, 1, pFile);
            fclose(pFile);

			//FILE *pFile2;
            //char filepath[256] = {0};
			//BYTE *temp;
			temp = (BYTE *)malloc(*EncryptBufferSize);
            ::sprintf(filepath, "G:\\EncryptBuffer.bin");
            pFile = fopen(filepath, "w+b");
			memcpy(temp, EncryptBuffer, *EncryptBufferSize);
            fwrite(temp, *EncryptBufferSize, 1, pFile);
            fclose(pFile);
			*/
        }

        Count++;
        TotalEncryptLen += dwDataLen;
    }while( (Count*64)< PlainBufferSize);

    (*EncryptBufferSize) = TotalEncryptLen;

 CLEAN_UP:       

    if (!pbData) {
          free(pbData);
          pbData = NULL;
    }
    if (hKey) {
          CryptDestroyKey(hKey);
    }           
    if (hCryptProv) {
          CryptReleaseContext(hCryptProv, 0);
    }

    return ret;
}

extern "C"
int __stdcall DecryptBufferByPrivateKey(IN BYTE* CipherBuffer, IN DWORD CipherBufferSize,
										IN BYTE* PrivateKeyBuffer, IN DWORD PrivateKeySize,
                                        OUT BYTE* DecryptBuffer, OUT DWORD *DecryptBufferSize)
{
    // Variables
    HCRYPTPROV hCryptProv = NULL;
    HCRYPTKEY hKey = NULL;        
    BYTE*   pbData = NULL;
    DWORD   dwDecryptSize = 0;    
    DWORD   dwDataLen = 0;    
    DWORD   TotalDecryptLen = 0;
    DWORD   Count = 0;
    int     ret = 0;  

    // Acquire access to key container
    if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, 0)) 
    {
          // Try to create a new key container
          if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
          {
                ret = 1;
                goto CLEAN_UP;
          }
    }

    // Import Private key
	if (!CryptImportKey(hCryptProv, PrivateKeyBuffer, PrivateKeySize, 0, 0, &hKey))
    {
          ret = 2;
          goto CLEAN_UP;
    }

    // Create a buffer for the decrypted data
    dwDecryptSize = 512;
    if (!(pbData = (BYTE *)malloc(dwDecryptSize)))
    {
          ret = 3;
          goto CLEAN_UP;
    }

    TotalDecryptLen = 0;
    Count = 0;
    do{        
        memset(pbData, 0, dwDecryptSize);

        dwDataLen = 128;                
        memcpy(pbData, CipherBuffer + (Count * dwDataLen) , dwDataLen);

        if( !CryptDecrypt(hKey, NULL, FALSE, 0, pbData, &dwDataLen) ){
            ret = 4;
            goto CLEAN_UP;
        }    

        if( DecryptBuffer != NULL ){
            memcpy(DecryptBuffer + TotalDecryptLen, pbData, dwDataLen);
			//20160619
/*
			FILE *pFile;	
            //pFile = fopen("E:\\Work\\Kingston_KeyGen\\KeyProGen\\CipherBuffer.bin", "w+b");
			pFile = fopen("D:\\test.bin", "w+b");
			if(pFile = NULL){
			    MessageBox(NULL, "open failed", "ERROR", MB_OK);
			}
            fwrite(CipherBuffer, CipherBufferSize, 1, pFile);
            fclose(pFile);

			FILE *pFile;
            char filepath[256] = {0};
			::sprintf(filepath, "D:\\Log\\MP\\0x%.8x.bin", cycleList);
			pFile = fopen(filepath, "w+b");
			fwrite(tempbuf, 9216, 1, pFile);
			fclose(pFile);

			FILE *pFile;
            char filepath[256] = {0};
			BYTE *temp;
			temp = (BYTE *)malloc(CipherBufferSize);
            ::sprintf(filepath, "G:\\CipherBuffer.bin");
            pFile = fopen(filepath, "w+b");
			memcpy(temp, CipherBuffer, CipherBufferSize);
            fwrite(temp, CipherBufferSize, 1, pFile);
            fclose(pFile);

			FILE *pFile2;
            //char filepath[256] = {0};
			//BYTE *temp;
			temp = (BYTE *)malloc(*DecryptBufferSize);
            ::sprintf(filepath, "G:\\DecryptBuffer.bin");
            pFile = fopen(filepath, "w+b");
			memcpy(temp, DecryptBuffer, *DecryptBufferSize);
            fwrite(temp, *DecryptBufferSize, 1, pFile);
            fclose(pFile);

			FILE *pFile2;	
            pFile2 = fopen("E:\\Work\\Kingston_KeyGen\\KeyProGen\\DecryptBuffer.bin", "w+b");
            fwrite(DecryptBuffer, *DecryptBufferSize, 1, pFile2);
            fclose(pFile2);*/
        }

        Count++;
        TotalDecryptLen += dwDataLen;        
    }while( (Count*128)< CipherBufferSize);

    (*DecryptBufferSize) = TotalDecryptLen;
    
 CLEAN_UP:
                
    if (!pbData) {
          free(pbData);
          pbData = NULL;
    }           
    if (hKey) {
          CryptDestroyKey(hKey);
    }           
    if (hCryptProv) {
          CryptReleaseContext(hCryptProv, 0);
    }

    return ret;
}

extern "C"
int __stdcall EncryptDoc(TCHAR *Source, TCHAR *Target){
    HRESULT		hr					= S_OK;
    HCRYPTKEY	hPrivateKey			= NULL;
    HCRYPTHASH	hDigSigHash			= NULL;    
    LPBYTE		lpData				= NULL;
    FILE*		pSrcFile			= NULL;
    FILE*		pDestinationFile	= NULL;
    BOOL		bSrcIsDestination	= (Target == NULL)?TRUE:FALSE;
    int         ret = 0;

	pSrcFile = fopen((char *)Source, "rb");

    //	Open destination with intentions of destroying existing contents
    if( pSrcFile ){
		pDestinationFile = bSrcIsDestination ? fopen(FILE_INTERMEDIATE_FILENAME, "w+b") : fopen((char *)Target, "w+b");
    }

    //	Make sure the files opened
    if( pSrcFile == NULL || pDestinationFile == NULL){
        return 1;
    }

    ret = WriteFileFormatVersion(pDestinationFile);
    if( ret != 0 ){
        return 2;
    }

    //Write the private key to the destination file
	ClearDigitalSignature();

    if(	Initialize() && WritePrivateKey( pDestinationFile, hPrivateKey) ){
        lpData = (LPBYTE)malloc( FILE_READBYTES * sizeof( BYTE ) );
        if( lpData == NULL ){
            return 3;
        }

        //Create the hash for digital signature
        if( !CryptCreateHash( m_hContext, HASHING_ALGORITHM, 0, 0, &hDigSigHash ) ){
            return 4;
        }

        //	Read in the data from the source file, encrypt it and write to the destination
		do{
			DWORD dwCount = fread(lpData, sizeof(BYTE), FILE_READBYTES, pSrcFile );
            if( ferror( pSrcFile ) ){
                return 5;
            }

            // If encryption fails, give some useful information back to caller
            if( !CryptEncrypt(hPrivateKey, hDigSigHash, feof(pSrcFile), 0, lpData, &dwCount, FILE_READBYTES ) ){
                return 6;
            }

			// Write encrypted data to the destination file
			fwrite(lpData, sizeof(BYTE), dwCount, pDestinationFile );
            if( ferror( pDestinationFile ) ){
				return 7;
            }
		} while(!feof( pSrcFile ));
		fflush( pDestinationFile );

        if( !DigitallySignHash( hDigSigHash ) ){
				return 8;
		}
    }else{
        return 9;
    }

    if( hPrivateKey ){
	    CryptDestroyKey( hPrivateKey );
    }    

    if( pSrcFile ){
		fclose( pSrcFile );
    }

    if( pDestinationFile ){
		fclose( pDestinationFile );
    }

	SAFE_MALLOC_FREE( lpData );

    if( hDigSigHash ){
		CryptDestroyHash( hDigSigHash );
    }

    //	Caller wants to overwrite src - Copy Destination to Source
	if( bSrcIsDestination ){
		//	CopyFile fails if file is readonly
		SetFileAttributes( Source, FILE_ATTRIBUTE_NORMAL );
		if( !CopyFile( (LPCWSTR)FILE_INTERMEDIATE_FILENAME, Source, FALSE ) )
			return 10;
	}

	//	Delete the intermediate file - if it exists
	_unlink( FILE_INTERMEDIATE_FILENAME );

    return 0;
}

extern "C"
int __stdcall DecryptDoc(TCHAR *Source, TCHAR *Target){
	HRESULT		hr					= S_OK;
	HCRYPTKEY	hPrivateKey			= NULL;
	HCRYPTHASH	hDigSigHash			= NULL;
	LPBYTE		lpData				= NULL;
	FILE*		pSrcFile			= NULL;
	FILE*		pDestinationFile	= NULL;
	BOOL		bSrcIsDestination	= (Target == NULL)?TRUE:FALSE;

	pSrcFile = fopen((char *)Source, "rb");

    //	Open destination with intentions of destroying existing contents
    if( pSrcFile ){
		pDestinationFile = bSrcIsDestination ? fopen(FILE_INTERMEDIATE_FILENAME, "w+b") : fopen((char *)Target, "w+b");
    }

	//	Make sure the files opened
    if( pSrcFile == NULL || pDestinationFile == NULL){
        return 1;
    }

	//	Read the version stamp and make sure its a version that we can read
    if( !ReadFileFormatVersion( pSrcFile ) ){
        return 2;
    }

	//	Read the private key and import into CSP
	ClearDigitalSignature();
	if(	Initialize() && ReadPrivateKey( pSrcFile, hPrivateKey ) ){
		lpData	   = (LPBYTE)malloc( FILE_READBYTES * sizeof( BYTE ) );
        if( lpData == NULL ){
            return 3;
        }

		//	Create the hash for digital signature
		if( !CryptCreateHash( m_hContext, HASHING_ALGORITHM, 0, 0, &hDigSigHash ) ){
            return 4;
        }

		//	Read in the data from the source file, encrypt it and write to the destination
		do {
			DWORD dwCount = fread(	lpData, sizeof(BYTE), FILE_READBYTES, pSrcFile );
			if( ferror( pSrcFile ) ){
                return 5;
            }

			// If encryption fails, give some useful information back to caller
			if( !CryptDecrypt(hPrivateKey, hDigSigHash, feof(pSrcFile), 0, lpData, &dwCount) ){
                return 6;
            }

			// Write decrypted data to the destination file
			fwrite( lpData, sizeof(BYTE), dwCount, pDestinationFile );
			if( ferror( pDestinationFile ) )
				return 7;
			}
		while( !feof( pSrcFile ) );
		fflush( pDestinationFile );

		//	Determine the length of the digital signature
        if( !DigitallySignHash( hDigSigHash ) ){
			return 8;
        }
	}


	///////////////////////////////////////////////////////////////////////
	//	Cleanup
    if( hPrivateKey ){
		CryptDestroyKey( hPrivateKey );
    }

    if( pSrcFile ){
		fclose( pSrcFile );
    }

    if( pDestinationFile ){
		fclose( pDestinationFile );
    }

	SAFE_MALLOC_FREE( lpData );

    if( hDigSigHash ){
		CryptDestroyHash( hDigSigHash );
    }

    //	Caller wants to overwrite src - Copy Destination to Source
	if( bSrcIsDestination ){
		//	CopyFile fails if file is readonly
		SetFileAttributes( Source, FILE_ATTRIBUTE_NORMAL );
        if( !CopyFile( (LPCWSTR)FILE_INTERMEDIATE_FILENAME, Source, FALSE ) ){
			return 9;
        }
	}

	//	Delete the intermediate file - if it exists
	_unlink( FILE_INTERMEDIATE_FILENAME );

	return 0;
}

